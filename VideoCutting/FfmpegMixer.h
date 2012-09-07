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
	 * @param srcBuff		buffer with video frame that have to be scaled
	 * @param originalW		video frame width before scaling
	 * @param originalH		video frame height before scaling
	 * @param destBuff		buffer into which result scaled video 
	 *						frame have to be saved
	 * @param scaledW		video frame width after scaling
	 * @param scaledH		video frame height after scaling
	 */
	void ScaleFrame(Uint8 *srcBuff, size_t originalW, size_t originalH,
					Uint8 *destBuff, size_t scaledW, size_t scaledH);

};