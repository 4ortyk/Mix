#pragma once
#include "DataProvider.h"
#include "Defineds.h"
#include <boost/thread/recursive_mutex.hpp>

class CPlayer;

class CVideoPort{
public:
	CVideoPort();
	~CVideoPort();

	void setOutputFileName(const String& fileName);
	
	/**
	 * Create and add new data provider with info from passed file
	 * @param fileName		video source file path
	 * @param width			source video width
	 * @param height		source video height
	 */
	void AddDataFile(const String& fileName, unsigned width, unsigned height);

	/**
	 * Start single thread in which data will be mixed using provided info
	 */
	void openPort();

private:
	/**
	 * Mix data - main logic
	 */
	void Mix();

private:
	CDataProvider::ProviderVec m_Providers;		// vector of data provider pointers

	bool m_Started;								// is mixing thread already runs flag
	
	String m_OutputFileName;					// file name with output mixed data

	static boost::recursive_mutex m_Mutex;
};