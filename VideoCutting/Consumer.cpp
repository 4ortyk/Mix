#include "stdafx.h"
#include "Consumer.h"

CConsumer::CConsumer(const CLayout& layout) : m_Layout(layout)
{
	m_MixedFrame = NULL;
}


CConsumer::CConsumer(const CConsumer& obj)
{
	*this = obj;
}
	
CConsumer::~CConsumer()
{
	if (m_MixedFrame != NULL) {
		delete [] m_MixedFrame;
		m_MixedFrame = NULL;
	}

	m_Providers.clear();
}

CConsumer& CConsumer::operator=(const CConsumer& obj)
{
	if (this == &obj)
		return *this;

	size_t oldframeSize = CalculateBufferSize();
	m_Layout = obj.m_Layout;
	
	size_t newFrameSize = CalculateBufferSize();
	if ((oldframeSize != newFrameSize) && (m_MixedFrame != NULL)) {
		delete [] m_MixedFrame;
		m_MixedFrame = new Uint8 [newFrameSize];
	}

	if (m_MixedFrame != NULL) {
		memcpy_s(m_MixedFrame, newFrameSize, obj.m_MixedFrame, newFrameSize);
	}

	m_Providers.clear();
	m_Providers.reserve(obj.m_Providers.size());
	CDataProvider::ProviderCIt it = obj.m_Providers.begin();
	for (; it != obj.m_Providers.end(); ++it) {
		m_Providers.push_back(*it);
	}

	return *this;
}

CLayout CConsumer::getLayout() const
{
	return m_Layout;
}


const Uint8 * CConsumer::getMixedVideo() const
{
	return m_MixedFrame;
}


size_t CConsumer::getFrameLength() const
{
	return CalculateBufferSize();
}


bool CConsumer::AddProvider(const CDataProvider::ProviderPtr& pProvider)
{
	if (m_Providers.size() == m_Layout.getPartsCount())
		return false;

	m_Providers.push_back(pProvider);
	return true;
}


bool CConsumer::MixVideo()
{
	// init output buffer
	if (m_Layout.getFrameArea() == 0 || m_Layout.getPartsCount() == 0)
		return false;
	
	if (m_MixedFrame == NULL) {
		m_MixedFrame = new Uint8[CalculateBufferSize()];
		if (m_MixedFrame == NULL)
			return false;
	}

	bool bMixedData = false;
	CDataProvider::ProviderIt it = m_Providers.begin();
	for (; it != m_Providers.end(); ++it) {
		CDataProvider::ProviderPtr& pProvider = *it;
		if (pProvider == NULL)
			continue;

		const size_t frameLen = pProvider->getFrameLength();
		Uint8 * frameBuff = new Uint8 [frameLen];
		if (frameBuff == NULL)
			continue;

		////////////////////////////////////////////////////////////////////////////////
		// take next frame
		size_t copyLen = pProvider->getNextFrame(frameBuff);
		if (copyLen == 0) {
			delete [] frameBuff;
			frameBuff = NULL;
			continue;
		}

		////////////////////////////////////////////////////////////////////////////////
		// scale frame
		unsigned frameNum = it - m_Providers.begin();
		size_t originH = pProvider->getHeight();
		size_t originW = pProvider->getWidth();
		size_t scaledW = originW;
		size_t scaledH = originH;
		bool bNeedScale = CalculateScaledFrameSize(frameNum, 
			(float)originW / originH, scaledW, scaledH);

		unsigned scaledBuffSize = 
			static_cast<unsigned>(scaledW * scaledH * YUV_SIZE_KOEF);
		Uint8 * scaledBuff = NULL;

		if (scaledW == originW && scaledH == originH) {
			scaledBuff = frameBuff;
		} else {
			scaledBuff = new Uint8 [scaledBuffSize];
			if (scaledBuff == NULL) {
				delete [] frameBuff;
				frameBuff = NULL;
				continue;
			}

			ScaleFrame(frameBuff, pProvider->getWidth(), pProvider->getHeight(),
				scaledBuff, scaledW, scaledH);

			delete [] frameBuff;
			frameBuff = NULL;
		}

		////////////////////////////////////////////////////////////////////////////////
		// crop too large frame
		size_t frameW = m_Layout.getWidth(frameNum);
		size_t frameH = m_Layout.getHeight(frameNum);
		Uint8 *croppedBuff = NULL;
		if (scaledW <= frameW && scaledH <= frameH) {
			croppedBuff = scaledBuff;
		} else {
			unsigned croppedBuffSize = 
			static_cast<unsigned>(frameW * frameH * YUV_SIZE_KOEF);
			croppedBuff = new Uint8 [croppedBuffSize];
			if (croppedBuff == NULL) {
				delete [] scaledBuff;
				scaledBuff = NULL;
				continue;
			}

			CropFrame(scaledBuff, scaledW, scaledH, croppedBuff, frameW, frameH);

			delete [] scaledBuff;
			scaledBuff = NULL;
		}
		
		////////////////////////////////////////////////////////////////////////////////
		// add frame into layout
		unsigned uBlockPos = static_cast<unsigned>(m_Layout.getFrameArea(frameNum));
		unsigned vBlockPos = uBlockPos + uBlockPos / 4;
		CombineFrame(croppedBuff, croppedBuff + uBlockPos, 
			croppedBuff + vBlockPos, frameNum);
		bMixedData = true;

		delete [] croppedBuff;
		croppedBuff = NULL;
	}

	return bMixedData;
}


size_t CConsumer::CalculateBufferSize() const
{
	return static_cast<size_t>(m_Layout.getFrameArea() * YUV_SIZE_KOEF);
}


bool CConsumer::CalculateScaledFrameSize(const unsigned frameNum, 
										 const float origFrameProportion,
										 size_t& scaledW,
										 size_t& scaledH)
{
	const size_t frameW = m_Layout.getWidth(frameNum);
	const size_t frameH = m_Layout.getHeight(frameNum);
	if (frameW == 0 || frameH == 0)
		return false;

	int scaleType = m_Layout.getScaleFillType(frameNum);
	bool bScaleByW = (scaleType & CLayout::eFillByWidth) != 0;
	bool bScaleByH = (scaleType & CLayout::eFillByHeight) != 0;
	if (bScaleByW && bScaleByH) {
		scaledW = (frameW % 2) ? frameW / 2 * 2 : frameW;
		scaledH = (frameH % 2) ? frameH / 2 * 2 : frameH;
	} else if(bScaleByW) {
		scaledW = (frameW % 2) ? frameW / 2 * 2 : frameW;
		scaledH = static_cast<size_t>(scaledW / origFrameProportion);
		scaledH = (scaledH % 2) ? scaledH / 2 * 2 : scaledH;
	} else if (bScaleByH) {
		scaledH = (frameH % 2) ? frameH / 2 * 2 : frameH;
		scaledW = static_cast<size_t>(scaledH * origFrameProportion);
		scaledW = (scaledW % 2) ? scaledW / 2 * 2 : scaledW;
	} else {
		return false;
	}

	return true;
}

void CConsumer::CropFrame(Uint8 *srcBuff,
						  size_t originalW, size_t originalH,
						  Uint8 *destBuff,
						  size_t resultedW, size_t resultedH)
{
	if (srcBuff == NULL || destBuff == NULL)
		return;

	if (resultedW % 2) ++resultedW;
	if (resultedH % 2) ++resultedH;
	unsigned paddingByWidth = (originalW - resultedW) / 2;

	// allo the only possibilities to cut insode and error in case of outside
	if (resultedW > originalW || resultedH > originalH)
		return; // say error

	unsigned long long originalFrameSize = originalW * originalH;
	unsigned long long originalComponentSize = originalFrameSize / 4;
	unsigned long long resultedFrameSize = resultedW * resultedH;
	unsigned long long resultedComponentSize = resultedFrameSize / 4;

	// define the position of U V components
	unsigned uBlockPos = originalW * originalH;
	unsigned vBlockPos = uBlockPos + uBlockPos / 4;
	uint8_t *originalFrameBuffer = srcBuff;
	uint8_t *originalUbuffer = srcBuff + uBlockPos;
	uint8_t *originalVbuffer = srcBuff + vBlockPos;

	uBlockPos = resultedW * resultedH;
	vBlockPos = uBlockPos + uBlockPos / 4;
	uint8_t *resultedFrameBuffer = destBuff;
	uint8_t *resultedUbuffer = destBuff + uBlockPos;
	uint8_t *resultedVbuffer = destBuff + vBlockPos;

	unsigned resultedHHalf = resultedH / 2;
	for (unsigned i = 0; i < resultedHHalf; ++i)
	{
		// first and second lines of buffers
		for (unsigned int j = 0; j < 2; ++j)
		{
			// skip the left part
			originalFrameBuffer += paddingByWidth;
			// copy the needed space from original to resulted buffer
			memcpy(resultedFrameBuffer, originalFrameBuffer, resultedW);
			// move over middle part and skip the right part
			originalFrameBuffer += resultedW + paddingByWidth;
			// move resulted buffer to the next line
			resultedFrameBuffer += resultedW;
		}

		// modify the U and V components
		// skip the left part
		originalUbuffer += paddingByWidth / 2;
		originalVbuffer += paddingByWidth / 2;
		// copy the needed chunk of components
		memcpy(resultedUbuffer, originalUbuffer, resultedW / 2);
		memcpy(resultedVbuffer, originalVbuffer, resultedW / 2);
		// move over middle part and skip the right part
		originalUbuffer += (resultedW + paddingByWidth) / 2;
		originalVbuffer += (resultedW + paddingByWidth) / 2;
		// move resultedU and resultedV over 1 frame's line
		resultedUbuffer += resultedW / 2;
		resultedVbuffer += resultedW / 2;
	}
}


/**
							m_Layout left padding (ovr_left)
				            |   |
		         ----------++===+==============================++-----------
	m_Layout top padding   ||   |                              || 1 stage
		         ----------++---+------------+                 ++-----------
	            (ovr_top)  ||   |            |                 || 2 stage
				           ||   | overlayer  |                 ||
				           ||   +------------+                 ++-----------
				           ||                                  || 3 stage
				           ||                   m_MixedFrame   ||
				           ++==================================++

 */
void CConsumer::CombineFrame(const Uint8 *ovrFrameBuffer,
							 const Uint8 *ovrUbuffer,
							 const Uint8 *ovrVbuffer,
							 unsigned frameNum)
{
	// check the possibility to put overlayer inside underlayer
	int ovrTop = m_Layout.getTopPadding(frameNum);
	int ovrLeft = m_Layout.getLeftPadding(frameNum);
	if (ovrTop < 0 || ovrLeft < 0)
		return; // say error

	// alling the top and left by even numbers (0, 2, 4, 6)
	if (ovrTop % 2) --ovrTop;
	if (ovrLeft % 2) --ovrLeft;

	int undWidth = m_Layout.getWidth();
	int undHeight = m_Layout.getHeight();
	int ovrWidth = m_Layout.getWidth(frameNum);
	int ovrHeight = m_Layout.getHeight(frameNum);
	int ovrRight = undWidth - ovrLeft - ovrWidth;

	if (ovrWidth > undWidth || ovrHeight > undHeight)
		return; // say error

	if ((ovrLeft + ovrWidth) > undWidth || (ovrTop + ovrHeight) > undHeight)
		return; // say error

	unsigned long long undFrameSize = undWidth * undHeight;
	unsigned long long undComponentSize = undFrameSize / 4;
	unsigned long long ovrFrameSize = ovrWidth * ovrHeight;
	unsigned long long ovrComponentSize = ovrFrameSize / 4;

	// define the position of U V components
	Uint8 *undFrameBuffer = m_MixedFrame;
	const unsigned uBlockPos = m_Layout.getFrameArea();
	const unsigned vBlockPos = uBlockPos + uBlockPos / 4;
	Uint8 *undUbuffer = m_MixedFrame + uBlockPos;
	Uint8 *undVbuffer = m_MixedFrame + vBlockPos;

	// 1 stage
	// skipp the top lines
	unsigned long long topShift = ovrTop * undWidth;
	unsigned long long topComponentShift = topShift / 4;
	undFrameBuffer += topShift;
	undUbuffer += topComponentShift; 
	undVbuffer += topComponentShift;
	
	// 2 stage
	// combine under and over frame
	int ovrHeightHalf = ovrHeight / 2;
	for (unsigned long long i = 0; i < ovrHeightHalf; ++i)
	{
		// first and second lines of buffers
		for (unsigned int j = 0; j < 2; ++j)
		{
			// skip the left part
			undFrameBuffer += ovrLeft;
			// overwrite by overlayer picture
			memcpy(undFrameBuffer, ovrFrameBuffer, ovrWidth);
			// move over the middle part and skip the right part
			undFrameBuffer += ovrWidth + ovrRight;
			// move ovrBuffer to the next line
			ovrFrameBuffer += ovrWidth;
		}

		// modify the U and V components
		// skip the left part
		undUbuffer += ovrLeft / 2;
		undVbuffer += ovrLeft / 2;
		// overwrite the overlay component
		memcpy(undUbuffer, ovrUbuffer, ovrWidth / 2);
		memcpy(undVbuffer, ovrVbuffer, ovrWidth / 2);
		// move over middle part and skip the right part
		undUbuffer += (ovrWidth + ovrRight) / 2;
		undVbuffer += (ovrWidth + ovrRight) / 2;
		// move ovrU and ovrV over 1 frame's line
		ovrUbuffer += ovrWidth / 2;
		ovrVbuffer += ovrWidth / 2;
	}

	// 3 stage
	// skip the rest - do nothing
}