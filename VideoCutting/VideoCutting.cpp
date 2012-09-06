// VideoCutting.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <process.h>
#include "Defineds.h"
#include "VideoPort.h"

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

using ::std::vector;

struct FileDesc {
	String fileName;
	size_t width;
	size_t height;
};

int _tmain(int argc, _TCHAR* argv[])
{
	vector<FileDesc> src;
	src.reserve(VIDEO_PORT_COUNT * VIDEO_SOURCE_COUNT);
	// for 1st provider
	FileDesc file;
	file.fileName = _T("../../data/bus_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);
	file.fileName = _T("../../data/akiyo_qcif.yuv");
	file.width = 176;
	file.height = 144;
	src.push_back(file);
	file.fileName = _T("../../data/bus_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);
	// for 2nd provider
	file.fileName = _T("../../data/coastguard_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);
	file.fileName = _T("../../data/container_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);
	file.fileName = _T("../../data/bridge-close_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);	
	// for 3rd provider
	file.fileName = _T("../../data/bridge-far_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);
	file.fileName = _T("../../data/container_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);
	file.fileName = _T("../../data/bridge-close_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);	
	// for 4th provider
	file.fileName = _T("../../data/bridge-close_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);
	file.fileName = _T("../../data/bridge-far_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);
	file.fileName = _T("../../data/flower_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);
	// for 5th provider
	file.fileName = _T("../../data/bus_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);
	file.fileName = _T("../../data/akiyo_qcif.yuv");
	file.width = 176;
	file.height = 144;
	src.push_back(file);
	file.fileName = _T("../../data/flower_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);
	// for 6th provider
	file.fileName = _T("../../data/coastguard_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);
	file.fileName = _T("../../data/container_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);
	file.fileName = _T("../../data/bridge-close_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);	
	// for 7rd provider
	file.fileName = _T("../../data/bridge-far_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);
	file.fileName = _T("../../data/container_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);
	file.fileName = _T("../../data/bridge-close_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);	
	// for 8th provider
	file.fileName = _T("../../data/bridge-close_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);
	file.fileName = _T("../../data/bridge-far_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);
	file.fileName = _T("../../data/flower_cif.yuv");
	file.width = 352;
	file.height = 288;
	src.push_back(file);

	boost::thread_group group;
	vector<CVideoPort *> videoPortVec;

	::std::clock_t start = ::std::clock();
	for (unsigned i = 0; i < VIDEO_PORT_COUNT; ++i) {
		CVideoPort *pVideoPort = new CVideoPort();
		videoPortVec.push_back(pVideoPort);

		String fileName = _T("D:\\Mix_");
		TCHAR num[5];
		_itow_s(i + 1, num, 10);
		fileName += num;
		fileName += _T(".yuv");
		pVideoPort->setOutputFileName(fileName);
	
		for (unsigned j = 0; j < VIDEO_SOURCE_COUNT; ++j) {
			const FileDesc& fileDesc = src.at(i * VIDEO_SOURCE_COUNT + j);
			pVideoPort->AddDataFile(fileDesc.fileName, fileDesc.width, fileDesc.height);
		}

		group.create_thread(boost::bind(&CVideoPort::openPort, pVideoPort));
	}
	group.join_all();

	::std::cout << "General time: " << (::std::clock() - start) / (double) CLOCKS_PER_SEC;

	for (vector<CVideoPort *>::iterator it = videoPortVec.begin(); it != videoPortVec.end(); ++it) {
		if (*it) {
			delete *it;
			*it = NULL;
		}
	}

    return EXIT_SUCCESS;
}