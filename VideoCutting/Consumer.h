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
	 * Get video frame buffer length, bytes
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
	 * Calculate video frame width and height in order with layout frame
	 * into which video should be put
	 * @param frameNum				layout frame number into which video should be put
	 * @param origFrameProportion	ratio of video width to video length
	 * @param scaledW				reference into scaled width value
	 * @param scaledH				reference into scaled height value
	 * @result						saved into scaledW and scaledH
	 */
	virtual bool CalculateScaledFrameSize(const unsigned frameNum,
		const float origFrameProportion,
		size_t& scaledW, size_t& scaledH);

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
	virtual void ScaleFrame(Uint8 *srcBuff,
							size_t originalW, size_t originalH,
							Uint8 *destBuff,
							size_t scaledW, size_t scaledH) = 0;

	/*
	 * Crop video frame using layout frame params
	 * @param srcBuff		buffer with video frame that have to be croped
	 * @param originalW		video frame width before cropping
	 * @param originalH		video frame height before cropping
	 * @param destBuff		buffer into which result croped video 
	 *						frame have to be saved
	 * @param resultedW		video frame width after cropping
	 * @param resultedH		video frame height after cropping
	 */
	virtual void CropFrame(Uint8 *srcBuff,
						  size_t originalW, size_t originalH,
						  Uint8 *destBuff,
						  size_t resultedW, size_t resultedH);

	/**
	 * Add video frame into correct position in output layout
	 * @param ovrFrameBuffer	overlayer buffer with Y chanel which will be put over underlayer
	 * @param ovrUbuffer		overlayer buffer with U chanel which will be put over underlayer
	 * @param ovrVbuffer		overlayer buffer with V chanel which will be put over underlayer
	 * @frameNum				number of layoutr frame into which overFrame should be placed
	 * @result					overwritting into m_MixedFrame
	 */
	virtual void CombineFrame(const Uint8 *ovrFrameBuffer, const Uint8 *ovrUbuffer, 
			const Uint8 *ovrVbuffer, unsigned frameNum);

protected:
	CDataProvider::ProviderVec m_Providers;	// list of video data providers

	CLayout m_Layout;						// mixed video layout
											// (how input video frames will be placed in output frame)
	Uint8 * m_MixedFrame;					// frame with resized input frames pleaced in layout parts
};