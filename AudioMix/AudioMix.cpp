// SDLTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/format.hpp>
#include <string>
#include <vector>
#include "AudioPort.h"
#include "SDLPlayer.h"

#include <windows.h>
#include <stdio.h>
#include <process.h>

using ::std::vector;

int _tmain(int argc, _TCHAR* argv[])
{
	vector<String> srcNames;
	srcNames.reserve(AUDIO_PORT_COUNT * AUDIO_SOURCE_COUNT);
	// for 1st provider
	srcNames.push_back(_T("../../data/sound2_Cut.wav"));
	srcNames.push_back(_T("../../data/sound3_Cut.wav"));
	srcNames.push_back(_T("../../data/sound4_Cut.wav"));
	srcNames.push_back(_T("../../data/sound5_Cut.wav"));
	// for 2nd provider
	srcNames.push_back(_T("../../data/sound6_Cut.wav"));
	srcNames.push_back(_T("../../data/sound7_Cut.wav"));
	srcNames.push_back(_T("../../data/sound8_Cut.wav"));
	srcNames.push_back(_T("../../data/sound9_Cut.wav"));
	// for 3rd provider
	srcNames.push_back(_T("../../data/sound10_Cut.wav"));
	srcNames.push_back(_T("../../data/sound11_Cut.wav"));
	srcNames.push_back(_T("../../data/sound12_Cut.wav"));
	srcNames.push_back(_T("../../data/sound2_Cut.wav"));
	// for 4th provider
	srcNames.push_back(_T("../../data/sound5_Cut.wav"));
	srcNames.push_back(_T("../../data/sound7_Cut.wav"));
	srcNames.push_back(_T("../../data/sound9_Cut.wav"));
	srcNames.push_back(_T("../../data/sound3_Cut.wav"));
	// for 5th provider
	srcNames.push_back(_T("../../data/sound11_Cut.wav"));
	srcNames.push_back(_T("../../data/sound3_Cut.wav"));
	srcNames.push_back(_T("../../data/sound12_Cut.wav"));
	srcNames.push_back(_T("../../data/sound6_Cut.wav"));
	// for 6th provider
	srcNames.push_back(_T("../../data/sound4_Cut.wav"));
	srcNames.push_back(_T("../../data/sound5_Cut.wav"));
	srcNames.push_back(_T("../../data/sound9_Cut.wav"));
	srcNames.push_back(_T("../../data/sound10_Cut.wav"));
	// for 7th provider
	srcNames.push_back(_T("../../data/sound2_Cut.wav"));
	srcNames.push_back(_T("../../data/sound6_Cut.wav"));
	srcNames.push_back(_T("../../data/sound10_Cut.wav"));
	srcNames.push_back(_T("../../data/sound5_Cut.wav"));
	// for 8th provider
	srcNames.push_back(_T("../../data/sound4_Cut.wav"));
	srcNames.push_back(_T("../../data/sound9_Cut.wav"));
	srcNames.push_back(_T("../../data/sound3_Cut.wav"));
	srcNames.push_back(_T("../../data/sound12_Cut.wav"));

	CSDLPlayer player(44100, 2);

	boost::thread_group group;
	vector<CAudioPort *> audioPortVec;

	::std::clock_t start = ::std::clock();
	for (unsigned i = 0; i < AUDIO_PORT_COUNT; ++i) {
		CAudioPort *pAudioPort = new CAudioPort();
		audioPortVec.push_back(pAudioPort);

		String fileName = _T("Mix_%d.wav");
		boost::wformat f(fileName);
		f%(i+1);
		pAudioPort->setOutputFileName(f.str());
	
		for (unsigned j = 0; j < AUDIO_SOURCE_COUNT; ++j) {
			pAudioPort->AddDataFile(srcNames.at(i * AUDIO_SOURCE_COUNT + j));
		}

		if (i == 0) {
			pAudioPort->setPlayer(&player);
		}
	
		group.create_thread(boost::bind(&CAudioPort::openPort, pAudioPort));
	}
	group.join_all();

	::std::cout << "General time: " << (::std::clock() - start) / (double) CLOCKS_PER_SEC;

	for (vector<CAudioPort *>::iterator it = audioPortVec.begin(); it != audioPortVec.end(); ++it) {
		if (*it) {
			delete *it;
			*it = NULL;
		}
	}

    return EXIT_SUCCESS;
}