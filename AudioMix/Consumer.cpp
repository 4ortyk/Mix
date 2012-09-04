#include "stdafx.h"
#include "Consumer.h"
#include "DataProvider.h"

CConsumer::CConsumer(unsigned providerCount, double timeLenSec) : m_DataProviderMaxCount(providerCount)
{
	m_MixedStream = NULL;
	m_PortionLen = 2048;
	m_Providers.reserve(m_DataProviderMaxCount);
	m_PartTimeSec = timeLenSec;
	m_NumChanels = 0;
	m_Freq = 0;
	m_BytesPerSec = 0;
}

CConsumer::CConsumer(const CConsumer& obj) : m_DataProviderMaxCount(obj.m_DataProviderMaxCount)
{
	*this = obj;
}

CConsumer::~CConsumer()
{
	if (m_MixedStream != NULL) {
		delete [] m_MixedStream;
		m_MixedStream = NULL;
	}

	m_Providers.clear();
}

 CConsumer& CConsumer::operator=(const CConsumer& obj)
 {
	 if (this == &obj)
		 return *this;

	m_NumChanels = obj.m_NumChanels;
	m_Freq = obj.m_Freq;
	m_PartTimeSec = obj.m_PartTimeSec;
	m_BytesPerSec = obj.m_BytesPerSec;
	m_PortionLen = obj.m_PortionLen;

	if (m_MixedStream != NULL) {
		delete [] m_MixedStream;
		m_MixedStream = NULL;
	}

	m_MixedStream = new Uint8[m_PortionLen];
	memcpy_s(m_MixedStream, m_PortionLen, obj.m_MixedStream, m_PortionLen);

	m_Providers.clear();
	unsigned providersCount = ::std::min(obj.m_Providers.size(), m_DataProviderMaxCount);
	for (unsigned i = 0; i < providersCount; ++i) {
		m_Providers.push_back(obj.m_Providers.at(i));
	}

	return *this;
 }

bool CConsumer::AddProvider(CDataProvider::ProviderPtr pProvider)
{
	if (m_Providers.size() == m_DataProviderMaxCount) {
		return false;
	}

	m_Providers.push_back(pProvider);
	return true;
}

const Uint8* CConsumer::getMixedData() const
{
	return m_MixedStream;
}

unsigned CConsumer::getPortionLength() const
{
	return m_PortionLen;
}

void CConsumer::setPortionLength(unsigned len)
{
	m_PortionLen = len;
}

void CConsumer::setNumChanels(unsigned numChanels)
{
	m_NumChanels = numChanels;
	ReCalculatePartLength();
}


unsigned CConsumer::getNumChanels() const
{
	return m_NumChanels;
}


void CConsumer::setFreq(unsigned long freq)
{
	m_Freq = freq;
	ReCalculatePartLength();
}


unsigned long CConsumer::getFreq() const
{
	return m_Freq;
}


void CConsumer::setBytesPerSec(unsigned bytes)
{
	m_BytesPerSec = bytes;
	ReCalculatePartLength();
}


unsigned CConsumer::getBytesPerSec() const
{
	return m_BytesPerSec;
}

void CConsumer::ReCalculatePartLength()
{
	m_PortionLen = 
		static_cast<unsigned>(m_Freq * m_NumChanels * m_BytesPerSec * m_PartTimeSec);
}