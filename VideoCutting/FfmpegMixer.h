#pragma once
#include "Consumer.h"

struct AVFrame;
class CFfmpegMixer : public CConsumer {
public:
	CFfmpegMixer(const CLayout& layout);
	CFfmpegMixer(const CFfmpegMixer& obj);
	virtual ~CFfmpegMixer();
	virtual CFfmpegMixer& operator=(const CFfmpegMixer& obj);

protected:
	/*
	 * Scale video frame using layout frame params
	 */
	void ScaleFrame(Uint8 *srcBuff, size_t originalW, size_t originalH,
					Uint8 *destBuff, size_t scaledW, size_t scaledH);

	/**
	 *
	 */
	//void SplitYUVChanels(Uint8 *src, unsigned buffLen, 
	//	unsigned frameW, unsigned frameH);
};