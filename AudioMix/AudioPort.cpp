#include "stdafx.h"
#include "AudioPort.h"

#ifdef MODE_SDL
#include "SDLMixer.h"
#elif defined MODE_WAVE
#include "WaveMixer.h"
#elif defined MODE_SOX
#include "SoxMixer.h"
#endif

#include "Player.h"
#include <boost/bind.hpp>

#define FREQ 44100UL
#define CHANELS_COUNT 2U
#define WAV_HEADER_SIZE 44
#define PART_TIME_MS 200U

CAudioPort::CAudioPort()
{
	m_Started = false;
	m_OutputFileName = _T("");
	m_pPlayer = NULL;
}


CAudioPort::~CAudioPort()
{

}


void CAudioPort::setOutputFileName(const String& fileName)
{
	m_OutputFileName = fileName;
}


void CAudioPort::setPlayer(CPlayer *pPlayer)
{
	m_pPlayer = pPlayer;
}


void CAudioPort::AddDataFile(const String& fileName)
{
	if (fileName.empty())
		return;

	CDataProvider* pPr = new CDataProvider(fileName, WAV_HEADER_SIZE, CDataProvider::maxVolume);
	CDataProvider::ProviderPtr pProvider(pPr);
	
	m_Providers.push_back(pProvider);
}


void CAudioPort::openPort()
{
	if (m_Started) {
		::std::cout << "\nThread is already running. Can be only one thread in time.\n";
	} else {
		m_Started = true;
		Mix();
	}
}


void CAudioPort::Mix()
{
	// check whether we need to mix data:
	// if we don't have where set output data - no reasons to mix it
	if (m_OutputFileName.empty())
		return;

	::std::ofstream outputFile(m_OutputFileName, ::std::ios::binary|::std::ios::out);
	if (!outputFile)
		return;

	//should be defined one of mode
#ifdef MODE_SDL
	CSDLMixer mixer(PART_TIME_MS / 1000.0);
#elif defined (MODE_WAVE)
	CWaveMixer mixer(PART_TIME_MS / 1000.0);
#elif defined (MODE_SOX)
	CSoxMixer mixer(PART_TIME_MS / 1000.0);
#endif
	mixer.setFreq(FREQ);
	mixer.setNumChanels(CHANELS_COUNT);
	mixer.setBytesPerSec(BYTES_PER_SEC);
	
	/////////////////////////////////////////////////////////////////////////////
	for (CDataProvider::ProviderIt it = m_Providers.begin(); it != m_Providers.end(); ++it) {
		if (!mixer.AddProvider(*it)) {
			std::cout << "\nCan't add data provider";
		}
	}
	/////////////////////////////////////////////////////////////////////////////
	std::clock_t start = std::clock();
	int i = 1;

	bool bMixed = false;
	do {
		std::clock_t beforeMixT = std::clock();
		bMixed = mixer.MixData();
		if (!bMixed)
			continue;

		const Uint8 *mixBuff = mixer.getMixedData();
		unsigned buffLen = mixer.getPortionLength();
		if (mixBuff == NULL || buffLen < 1)
			continue;
		fprintf(stderr, "i: %d\n", i++);
		
		if (!m_OutputFileName.empty()) {
			outputFile.write((const char *) mixBuff, buffLen);
		}

		if (m_pPlayer != NULL) {
			m_pPlayer->setStream(mixBuff, buffLen);
			m_pPlayer->Play(PART_TIME_MS);
		}

		std::clock_t afterMixT = ::std::clock();
		int sleepTime = PART_TIME_MS - (afterMixT - beforeMixT);
		if (sleepTime > 0) {
			boost::posix_time::milliseconds ptMs(sleepTime);
			boost::this_thread::sleep(ptMs);
		}
	} while (bMixed);
	fprintf(stderr, "\nTime: %0.4fs\n", (std::clock() - start) / (double) CLOCKS_PER_SEC);
	/////////////////////////////////////////////////////////////////////////////

	outputFile.close();

	m_Started = false;
}