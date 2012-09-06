#include "stdafx.h"
#include "SDLPlayer.h"
#include "SDL_timer.h"
#include "SDL_audio.h"

unsigned audio_pos = 0;

void PlayAudio(void *unused, Uint8 *stream, int len)
{
	if (CSDLPlayer::m_StreamLen == 0 || CSDLPlayer::m_Stream == NULL)
            return;

    /* Mix as much data as possible */
    len = ( (unsigned)len > CSDLPlayer::m_StreamLen ? CSDLPlayer::m_StreamLen : len );
	SDL_MixAudio(stream, CSDLPlayer::m_Stream + audio_pos, len, SDL_MIX_MAXVOLUME);
    audio_pos += len;
    CSDLPlayer::m_StreamLen -= len;
}


CSDLPlayer::CSDLPlayer(unsigned freq,  unsigned channels)
{
	SDL_AudioSpec fmt;

	fmt.freq = freq;
    fmt.format = AUDIO_S16;
    fmt.channels = (channels < 1) ? 1 : channels;
    fmt.samples = 512;
    fmt.callback = ::PlayAudio;
    fmt.userdata = NULL;

	m_bOpenAudio = SDL_OpenAudio(&fmt, NULL) == 0;

    /* Open the audio device and start playing sound! */
    if ( !m_bOpenAudio ) {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
    }
}

CSDLPlayer::CSDLPlayer(const CSDLPlayer& obj) : CPlayer(obj)
{
	m_bOpenAudio = obj.m_bOpenAudio;
}

CSDLPlayer::~CSDLPlayer()
{
	SDL_CloseAudio();
}

CSDLPlayer& CSDLPlayer::operator=(const CPlayer& obj)
{
	if (this == &obj)
		return *this;

	CPlayer::operator=(obj);

	return *this;
}

CSDLPlayer& CSDLPlayer::operator=(const CSDLPlayer& obj)
{
	if (this == &obj)
		return *this;

	CPlayer::operator=(obj);
	m_bOpenAudio = obj.m_bOpenAudio;

	return *this;
}

void CSDLPlayer::Play(unsigned trackTimeLenMs)
{
	if (!m_bOpenAudio || m_Stream == NULL) {
		fprintf(stderr, "Audio stream isn't initialized\n");
		return;
	}

	audio_pos = 0;
    SDL_PauseAudio(0);

	//while (CSDLPlayer::m_StreamLen > 0) {
	//	SDL_Delay(5000);
	//}
}