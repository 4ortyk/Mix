#pragma once
#include <stdint.h>
#include "DataProvider.h"
#include "Layout.h"

/**
 * CConsumer [class] implements gathering video sources in single frame
 */
class CConsumer {
public:
	CConsumer(const CLayout& layout);
	CConsumer(const CConsumer& obj);
	virtual ~CConsumer();
	virtual CConsumer& operator=(const CConsumer& obj);

	CLayout getLayout() const;

	const Uint8* getMixedVideo() const;

	/**
	 *
	 */
	size_t getFrameLength() const;

	/** 
	 * Add video provider
	 * @param pProvider		pointer on data provider which should be added
	 * @return				false if consumare's providers count is equal
	 *						layout parts count, else true
	 */
	bool AddProvider(const CDataProvider::ProviderPtr& pProvider);

	/**
	 * Convert input video frames from provider in order to
	 * display it with correct params inside output video frame
	 */
	virtual bool MixVideo();

protected:
	/**
	 * Calculate mixed buffer size
	 */
	size_t CalculateBufferSize() const;

	/**
	 *
	 */
	virtual bool CalculateScaledFrameSize(const unsigned frameNum,
		const float origFrameProportion,
		size_t& scaledW, size_t& scaledH);

	/*
	 * Scale video frame using layout frame params
	 */
	virtual void ScaleFrame(Uint8 *srcBuff, size_t originalW, size_t originalH,
							Uint8 *destBuff, size_t scaledW, size_t scaledH) = 0;

	/**
	 * Crop video frame using layout frame params
	 */
	virtual void CropFrame(Uint8 *srcBuff,
						  size_t originalW, size_t originalH,
						  Uint8 *destBuff,
						  size_t resultedW, size_t resultedH);

	/**
	 * Add video frame into correct position in output layout
	 * @param ovrFrameBuffer	- overlayer buffer with Y chanel which will be put over underlayer
	 * @param ovrUbuffer		- overlayer buffer with U chanel which will be put over underlayer
	 * @param ovrVbuffer		- overlayer buffer with V chanel which will be put over underlayer
	 * @result					- overwritting into m_MixedFrame
	 */
	virtual void CombineFrame(const Uint8 *ovrFrameBuffer, const Uint8 *ovrUbuffer, 
			const Uint8 *ovrVbuffer, unsigned frameNum);

protected:
	CDataProvider::ProviderVec m_Providers;	// list of video data providers

	CLayout m_Layout;							// mixed video layout
												// (how input video frames will be placed in output frame)
	Uint8 * m_MixedFrame;						// frame with resized input frames pleaced in layout parts
};