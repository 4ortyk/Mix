#pragma once
#include "SDL_main.h"
#include "Consumer.h"
#include <windows.h>

class CSDLMixer : public CConsumer{
public:
	CSDLMixer(double timeLenSec);
	CSDLMixer(const CSDLMixer& obj);

	virtual CSDLMixer& operator=(const CSDLMixer& obj);

	// mix audio data from different sourses into sign stream using SDL lib algorithm
	virtual bool MixData();
};