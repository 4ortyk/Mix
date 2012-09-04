#pragma once
#include "Player.h"


void PlayAudio(void *unused, Uint8 *stream, int len);

class CSDLPlayer : public CPlayer {
public:
	CSDLPlayer(unsigned freq, unsigned channels);
	CSDLPlayer(const CSDLPlayer& obj);
	~CSDLPlayer();

	virtual CSDLPlayer& operator=(const CSDLPlayer& obj);

	virtual void Play(unsigned trackTimeLenMs);

private:
	// callback funcktion which provides part
	// by part audio stream to audio device
	friend void PlayAudio(void *unused, Uint8 *stream, int len);

private:
	bool m_bOpenAudio;
};