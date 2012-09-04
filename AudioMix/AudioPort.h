#pragma once
#include <boost/thread/thread.hpp>
#include <string>
#include "DataProvider.h"
#include "Defineds.h"

class CPlayer;

class CAudioPort{
public:
	CAudioPort();
	~CAudioPort();

	void setOutputFileName(const String& fileName);

	void setPlayer(CPlayer *pPlayer);
	
	// Create and add new data provider with info from passed file
	void AddDataFile(const String& fileName);

	// Start single thread in which data will be mixed using provided info
	void openPort();

private:
	// Mix data - main logic
	void Mix();

private:
	CDataProvider::ProviderVec m_Providers;		// vector of data provider pointers
	bool m_Started;								// is mixing thread already runs flag
	
	String m_OutputFileName;					// file name with output mixed data
	CPlayer * m_pPlayer;						// plyer which should play mixed stream
};