#pragma once
#include <vector>
#include "DataProvider.h"
#include "Defineds.h"

class CConsumer {
public:
	CConsumer(unsigned providerCount, double timeLenSec);
	CConsumer(const CConsumer& obj);
	virtual ~CConsumer();

	virtual CConsumer& operator=(const CConsumer& obj);

	// add audio data sourse
	virtual bool AddProvider(CDataProvider::ProviderPtr pProvider);

	// mix audio data from different sourses into sign stream
	virtual bool MixData() = 0;

	// get result stream with mixed audio data
	virtual const Uint8 * getMixedData() const;
	
	virtual unsigned getPortionLength() const;
	virtual void setPortionLength(unsigned len);

	virtual void setNumChanels(unsigned numChanel);
	virtual unsigned getNumChanels() const;

	virtual void setFreq(unsigned long freq);
	virtual unsigned long getFreq() const;

	virtual void setBytesPerSec(unsigned bytes);
	virtual unsigned getBytesPerSec() const;

protected:
	/** 
	 * Calculate data sample length using audio stream params
	 */
	virtual void ReCalculatePartLength();

protected:
	CDataProvider::ProviderVec m_Providers;	// list of sourses which audio data should be mixed
	
	const unsigned m_DataProviderMaxCount;
	
	unsigned m_PortionLen;					// audio data bytes count which should be taked 
											// from provider in order to mix it

	Uint8 *m_MixedStream;					// result stream with mixed data

	unsigned m_NumChanels;

	unsigned m_Freq;

	unsigned m_BytesPerSec;

	double m_PartTimeSec;					// sample time length in sec
};