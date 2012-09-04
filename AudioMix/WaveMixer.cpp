#include "stdafx.h"
#include "windows.h"
#include "WaveMixer.h"
#include "DataProvider.h"
#include "SDL.h"


CWaveMixer::CWaveMixer(double timeLenSec) : CConsumer(AUDIO_SOURCE_COUNT, timeLenSec)
{
}

CWaveMixer::CWaveMixer(const CWaveMixer& obj) : CConsumer(obj)
{
}


CWaveMixer& CWaveMixer::operator=(const CWaveMixer& obj)
{
	if (this == &obj)
		return *this;

	CConsumer::operator=(obj);
	
	return *this;
}


bool CWaveMixer::MixData()
{
	if (m_Providers.empty()) {
		return false;
	}

	if (m_MixedStream) {
		delete [] m_MixedStream;
		m_MixedStream = NULL;
	}

	CDataProvider::ProviderIt providerIt = m_Providers.begin();
	for ( ; providerIt != m_Providers.end(); ++providerIt) {
		CDataProvider::ProviderPtr pProvider = *providerIt;
		if (pProvider == NULL)
			continue;

		Uint8* dataStream = new Uint8[m_PortionLen];
		unsigned dataLen = pProvider->getNextDataPortion(m_PortionLen, dataStream);
		if (dataLen < 1) {
			delete [] dataStream;
			continue;
		}

		if (m_MixedStream == NULL) {
			m_MixedStream = new Uint8 [m_PortionLen];
			if (m_MixedStream != NULL && dataStream != NULL) {
				memcpy_s(m_MixedStream, dataLen, dataStream, dataLen);
			}
		} else {
			MixStreams(m_MixedStream, dataStream, dataLen);
		}

		delete [] dataStream;
	}

	return m_MixedStream != NULL;
}


void CWaveMixer::MixStreams(Uint8* dest, Uint8* src, unsigned len)
{
	switch (m_BytesPerSec) 	{
		case 1:
			{
				float gain = log10(20.0f);
				for (unsigned i = 0; i < len; ++i) {
					*dest = (BYTE)(((*src + *dest) >> 1) * gain);
					++src;
					++dest;
				}
			}
			break;

		case 2:
			{
				LPWORD lpSrcData = (LPWORD)src;
				LPWORD lpDstData = (LPWORD)dest;
				for (unsigned i = 0; i < len / 2; ++i) {
					float sample1 = (*lpSrcData - 32768) / 32768.0f;
					float sample2 = (*lpDstData - 32768) / 32768.0f;
					
					if (fabs(sample1*sample2) > 0.25f) {
						*lpDstData = (WORD)(*lpSrcData + *lpDstData);
					} else {
						*lpDstData = (fabs(sample1) < fabs(sample2)) ? *lpSrcData : *lpDstData;
					}

					++lpSrcData;
					++lpDstData;
				}
			}
			break;
	}
}