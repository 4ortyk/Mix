#include "stdafx.h"
#include "VideoPort.h"
#include "Layout.h"
#include <boost/thread/thread.hpp>
#include <Windows.h>

#ifdef MODE_FFMPEG
#include "FfmpegMixer.h"
#endif

boost::recursive_mutex CVideoPort::m_Mutex;

CVideoPort::CVideoPort()
{
	m_Started = false;
	m_OutputFileName = _T("");
}


CVideoPort::~CVideoPort()
{
}


void CVideoPort::setOutputFileName(const String& fileName)
{
	m_OutputFileName = fileName;
}


void CVideoPort::AddDataFile(const String& fileName, unsigned width, unsigned height)
{
	if (fileName.empty())
		return;

	CDataProvider* pPr = 
		new CDataProvider(fileName, width, height, FRAMES_PER_SEC, VIDEO_LEN_SEC);
	CDataProvider::ProviderPtr pProvider(pPr);
	
	m_Providers.push_back(pProvider);
}


void CVideoPort::openPort()
{
	if (m_Started) {
		::std::cout << "\nThread is already running. Can be only one thread in time.\n";
	} else {
		m_Started = true;
		Mix();
	}
}


void CVideoPort::Mix()
{
	// check whether we need to mix data:
	// if we don't have where set output data - no reasons to mix it
	if (m_OutputFileName.empty())
		return;

	::std::ofstream outputFile(m_OutputFileName, ::std::ios::binary|::std::ios::out);
	if (!outputFile)
		return;

	//should be defined one of mode
	CLayout layout;
#ifdef MODE_FFMPEG
	CFfmpegMixer mixer(layout);
#endif
	
	/////////////////////////////////////////////////////////////////////////////
	for (CDataProvider::ProviderIt it = m_Providers.begin(); it != m_Providers.end(); ++it) {
		if (!mixer.AddProvider(*it)) {
			std::cout << "\nCan't add data provider";
		}
	}
	/////////////////////////////////////////////////////////////////////////////
	::std::clock_t start = ::std::clock();
	int i = 1;

	bool bMixed = false;
	do {
		::std::clock_t beforeMixT = ::std::clock();
		
		{
			boost::lock_guard<boost::recursive_mutex> lock(m_Mutex);
			bMixed = mixer.MixVideo();
		}

		if (!bMixed)
			continue;

		const Uint8 *mixBuff = mixer.getMixedVideo();
		unsigned buffLen = mixer.getFrameLength();
		if (mixBuff == NULL || buffLen < 1)
			continue;
		fprintf(stderr, "i: %d\n", i++);
		
		if (!m_OutputFileName.empty()) {
			outputFile.write((const char *) mixBuff, buffLen);
		}

		::std::clock_t afterMixT = ::std::clock();
		int sleepTime = 1000 / FRAMES_PER_SEC - (afterMixT - beforeMixT);
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