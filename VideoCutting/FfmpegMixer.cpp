#include "stdafx.h"
#include "FfmpegMixer.h"

#ifdef __cplusplus
extern "C"
#endif
{
	#include <libavformat\avformat.h>
	#include <libavutil\avutil.h>
	#include <libswscale\swscale.h>
}

CFfmpegMixer::CFfmpegMixer(const CLayout& layout) : CConsumer(layout)
{
}


CFfmpegMixer::CFfmpegMixer(const CFfmpegMixer& obj) : CConsumer(obj)
{
}


CFfmpegMixer::~CFfmpegMixer()
{
}


CFfmpegMixer& CFfmpegMixer::operator=(const CFfmpegMixer& obj)
{
	if (this == &obj)
		return *this;

	CConsumer::operator=(obj);

	return *this;
}


void CFfmpegMixer::ScaleFrame(Uint8 *srcBuff, size_t originalW, size_t originalH, 
							  Uint8 *destBuff, size_t scaledW, size_t scaledH)
{
	if (srcBuff == NULL || destBuff == NULL)
		return;

	AVFrame* originalFrame = avcodec_alloc_frame();
	AVFrame* rescaledFrame = avcodec_alloc_frame();

	avpicture_fill(reinterpret_cast<AVPicture*>(originalFrame), srcBuff, PIX_FMT_YUV420P, originalW, originalH);
	avpicture_fill(reinterpret_cast<AVPicture*>(rescaledFrame), destBuff, PIX_FMT_YUV420P, scaledW, scaledH);

	SwsContext* encoderSwsContext = sws_getContext(originalW, originalH, 
		PIX_FMT_YUV420P, scaledW, scaledH,
		PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	sws_scale(encoderSwsContext, originalFrame->data, originalFrame->linesize,
		0, originalH, rescaledFrame->data, rescaledFrame->linesize); 

	av_free(originalFrame);
	av_free(rescaledFrame);
}