#include "stdafx.h"
#include "SDLMixer.h"
#include "DataProvider.h"
#include "SDL.h"


CSDLMixer::CSDLMixer(double timeLenSec) : CConsumer(AUDIO_SOURCE_COUNT, timeLenSec)
{
}

CSDLMixer::CSDLMixer(const CSDLMixer& obj) : CConsumer(obj)
{
}


CSDLMixer& CSDLMixer::operator=(const CSDLMixer& obj)
{
	if (this == &obj)
		return *this;

	CConsumer::operator=(obj);
	
	return *this;
}


bool CSDLMixer::MixData()
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
			SDL_MixAudio(m_MixedStream, dataStream, dataLen, pProvider->getVolume());
		}

		delete [] dataStream;
	}

	return m_MixedStream != NULL;
}