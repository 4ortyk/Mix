#pragma once
#include "Defineds.h"

class CPlayer {
public:
	CPlayer();
	CPlayer(const CPlayer& obj);
	virtual ~CPlayer();

	virtual CPlayer& operator=(const CPlayer& obj);

	// Set stream which should be played
	virtual void setStream(const Uint8 *stream, const unsigned streamLen);
	
	// Play audio from stream
	// @trackTimeLenMs - how long play track, ms
	virtual void Play(unsigned trackTimeLenMs) = 0;

protected:
	// additional method cleans audio stream data and set length 0
	void cleanStream();

protected:
	static Uint8 *m_Stream;
	static unsigned long m_StreamLen;
};