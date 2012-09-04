#pragma once
#include "Consumer.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#else
{
#endif
	
	#include "include\sox.h"
}


class CSoxMixer : public CConsumer {
public:
	CSoxMixer(double timeLenSec);
	CSoxMixer(const CSoxMixer& obj);

	virtual CSoxMixer& operator=(const CSoxMixer& obj);

	// mix audio data from different sourses into sign stream using sox lib algorithm
	// CONCEPTUAL ISSUE: not use sox API
	virtual bool MixData();

protected:
	typedef boost::shared_ptr<Uint8> SoxSamplePtr;
	typedef ::std::vector<SoxSamplePtr> SoxSampleVec;
	typedef SoxSampleVec::iterator SoxSampleIt;

	// contains all data providers' stream that should be mixed
	typedef struct {
		SoxSampleVec			ibuf;
		::std::vector<size_t>   ilen;
	} InputMixerInfo;

	// contains information about singl data provider in sox format
	typedef boost::shared_ptr<sox_format_t> SoxFormatPtr;
	typedef struct {
		double volume;
		uint64_t volume_clips;
		SoxFormatPtr ft;		// libSoX file descriptor
	} DataInfo;

	typedef boost::shared_ptr<DataInfo> DataPtr;
	typedef ::std::vector<DataPtr> DataVec;
	typedef DataVec::iterator DataIt;

protected:
	// converts audio data from providers into sox format
	bool addData(const CDataProvider::ProviderPtr& pProvider);
	// balances volume
	void balanceInput(SoxSamplePtr buf, size_t ws, DataPtr f);
	// cleara buffers and streams data
	void cleanData();

protected:
	DataVec m_DataVec;				// list of providers params in sox format
	InputMixerInfo m_MixerInfo;		// duffer with data for mixing
	size_t m_MaxUnoChanelSampleLen;	// audio data bytes count in singl chanel
};