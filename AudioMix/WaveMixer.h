#pragma once
#include "Consumer.h"

class CWaveMixer : public CConsumer {
public:
	CWaveMixer(double timeLenSec);
	CWaveMixer(const CWaveMixer& obj);

	virtual CWaveMixer& operator=(const CConsumer& obj);
	virtual CWaveMixer& operator=(const CWaveMixer& obj);

	// mix audio data from different sourses into sign stream using byte mixing
	virtual bool MixData();

private:
	// mixs data from dest and src and puts result into dest
	void MixStreams(Uint8* dest, Uint8* src, unsigned len);
};