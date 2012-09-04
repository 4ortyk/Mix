#include "stdafx.h"
#include "SoxMixer.h"
#include "DataProvider.h"
#include <boost/thread/thread.hpp>
#ifdef __cplusplus
extern "C"
#endif
{
	#include "include/xmalloc.h"
}

#undef SOX_SAMPLE_MIN
#define SOX_SAMPLE_MIN (Uint8)0
#undef SOX_SAMPLE_MAX
#define SOX_SAMPLE_MAX ((1 << (sizeof(Uint8) * 8)) - 1)

CSoxMixer::CSoxMixer(double timeLenSec) : CConsumer(AUDIO_SOURCE_COUNT, timeLenSec)
{
	m_MaxUnoChanelSampleLen = 0;
}

CSoxMixer::CSoxMixer(const CSoxMixer& obj) : CConsumer(obj)
{
	m_DataVec.clear();
	for (unsigned i = 0; i < obj.m_DataVec.size(); ++i) {
		m_DataVec.push_back(obj.m_DataVec.at(i));
	}

	m_MixerInfo.ilen.clear();
	m_MixerInfo.ibuf.clear();
	for (unsigned i = 0; i < obj.m_MixerInfo.ibuf.size(); ++i) {
		m_MixerInfo.ibuf.push_back(obj.m_MixerInfo.ibuf.at(i));
		m_MixerInfo.ilen.push_back(obj.m_MixerInfo.ilen.at(i));
	}

	m_MaxUnoChanelSampleLen = obj.m_MaxUnoChanelSampleLen;
}


CSoxMixer& CSoxMixer::operator=(const CSoxMixer& obj)
{
	if (this == &obj)
		return *this;

	CConsumer::operator=(obj);
	
	return *this;
}

bool CSoxMixer::addData(const CDataProvider::ProviderPtr& pProvider)
{
	if (pProvider.get() == NULL)
	return false;

	// check whether we can take next audio data portion
	Uint8* dataStream = new Uint8[m_PortionLen];
	unsigned dataLen = pProvider->getNextDataPortion(m_PortionLen, dataStream);
	if (dataLen < 1) {
		delete [] dataStream;
		return false;
	}
	
	// fill header data
	DataPtr pData(new DataInfo);
	pData->volume = pProvider->getVolume() / CDataProvider::maxVolume;
	pData->volume_clips = 0;
	
	pData->ft = SoxFormatPtr(new sox_format_t);
	pData->ft->filename = NULL;
	pData->ft->signal.channels = m_NumChanels;
	pData->ft->signal.rate = static_cast<sox_rate_t>(m_Freq);
	pData->ft->signal.precision = m_BytesPerSec * 8;
	pData->ft->signal.length = m_PortionLen;
	pData->ft->signal.mult = NULL;
	m_DataVec.push_back(pData);

	// fill audio sample data
	Uint8* pBuffU = new Uint8[dataLen];
	if (pBuffU != NULL) {
		memcpy_s(pBuffU, dataLen, dataStream, dataLen);
		SoxSamplePtr pBuff  = SoxSamplePtr(pBuffU);
		balanceInput(pBuff, dataLen, pData);

		m_MixerInfo.ilen.push_back(dataLen);
		m_MixerInfo.ibuf.push_back(pBuff);

		m_MaxUnoChanelSampleLen = ::std::max(m_MaxUnoChanelSampleLen, dataLen / m_NumChanels);
	}
	delete [] dataStream;

	return true;
}


bool CSoxMixer::MixData()
{
	cleanData();

	if (m_Providers.empty()) {
		return false;
	}	

	// Convert providers into file data
	::std::for_each(m_Providers.begin(), m_Providers.end(), boost::bind(&CSoxMixer::addData, this, _1));
	if (m_DataVec.empty())
		return false;

	m_MixedStream = new Uint8[m_MaxUnoChanelSampleLen * m_NumChanels * m_DataVec.size()];
	if (m_MixedStream == NULL)
		return false;
	Uint8 *p = m_MixedStream;

	// Mix data
	uint64_t mixing_clips = 0;
	const unsigned dataCount = m_DataVec.size();
	for (size_t ws = 0; ws < m_MaxUnoChanelSampleLen; ++ws) {
		for(unsigned i = 1; i < dataCount; ++i) {
			for (unsigned chanel = 0; chanel < m_NumChanels; ++chanel) {
				Uint8* pBuf = m_MixerInfo.ibuf.at(i).get();
				if (ws < m_MixerInfo.ilen.at(i)) {
					unsigned byte = ws * m_DataVec.at(i)->ft->signal.channels + chanel;
					*p++ = (ws < m_MixerInfo.ilen.at(i)) * pBuf[byte];
				}
			}
		}
	}

	return m_MixedStream != NULL;
}

void CSoxMixer::cleanData()
{
	if (m_MixedStream) {
		delete [] m_MixedStream;
		m_MixedStream = NULL;
	}

	m_MixerInfo.ibuf.clear();
	m_MixerInfo.ilen.clear();
	m_DataVec.clear();
}

void CSoxMixer::balanceInput(SoxSamplePtr buf, size_t ws, DataPtr pData)
{
  size_t s = ws;
  Uint8 *pBuf = buf.get();

  if (pData->volume != 1) while (s--) {
    double d = pData->volume * *buf;
    *pBuf++ =  static_cast<int>(SOX_ROUND_CLIP_COUNT(d, pData->volume_clips));
  }
}