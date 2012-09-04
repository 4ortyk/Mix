#pragma once
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "Defineds.h"

class CDataProvider {
public:
	static Uint8 minVolume;
	static Uint8 maxVolume;

	typedef boost::shared_ptr<CDataProvider> ProviderPtr;
	typedef ::std::vector<ProviderPtr> ProviderVec;
	typedef ::std::vector<ProviderPtr>::iterator ProviderIt;

public:
	CDataProvider();
	CDataProvider(const String& fileName, unsigned headerSize, Uint8 volume = maxVolume);
	CDataProvider(Uint8* buff, const unsigned buffLen, Uint8 volume = maxVolume);
	CDataProvider(const CDataProvider& obj);
	~CDataProvider();

	virtual CDataProvider& operator=(const CDataProvider& obj);

	void setVolume(Uint8 volume);
	Uint8 getVolume() const;


	// init provider data form buffer
	void setBuffer(Uint8* buff, const unsigned buffLen);

	// inti provider data from file
	void setBufferFromFile(const String& fileName, unsigned headerSize);

	// copy next data portion from provider buffer
	// @param len - portion size
	// @buff - output buffer with coppied data
	// return count of copied data
	unsigned getNextDataPortion(unsigned len, Uint8* buff);

protected:
	Uint8 *m_Buffer;	// buffer with audio data content (without header, only samples)
	unsigned m_BuffLen; // buffer with audio data size in byte
	unsigned m_Pos;		// current read data position
	
	Uint8 m_Volume;		// volume (0-127) with which data will be mixed into output stream
};