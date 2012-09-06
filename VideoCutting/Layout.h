#pragma once
#include <vector>

/**
 *
 */
class CLayout {
public:
	enum eScaleFillType { eFillNone = 0, eFillByWidth = 1, eFillByHeight = 2 };

public:
	CLayout();
	~CLayout();

	/**
	 * Get output frame area
	 */
	size_t getFrameArea() const;

	/**
	 * Get area for passed frame
	 * @param frameNum		number of interested frame
	 * @return				if frameNum is valid - return frame area,
	 *						else - return 0
	 */
	size_t getFrameArea(unsigned frameNum) const;

	/**
	 * Get layout parts count
	 */
	size_t getPartsCount() const;

	/**
	 * Get top offset for passed frame
	 * @param frameNum		number of interested frame
	 * @return				if frameNum is valide - return frame top offset,
	 *						else - return output frame height
	 */
	size_t getTopPadding(unsigned frameNum) const;

	/**
	 * Get left offset for passed frame
	 * @param frameNum		number of interested frame
	 * @return				if frameNum is valid - return frame left offset,
	 *						else - return output frame height
	 */
	size_t getLeftPadding(unsigned frameNum) const;

	/**
	 * Get output layout width
	 */
	size_t getWidth() const;

	/**
	 * Get frame width
	 * @param frameNum	layout frame index
	 * @return			if frameNum is valid - return frame width,
	 *					else - return 0
	 */
	size_t getWidth(unsigned frameNum) const;

	/**
	 * Get output layout height
	 */
	size_t getHeight() const;

	/**
	 * Get frame height
	 * @param frameNum	layout frame index
	 * @return			if frameNum is valid - return frame height,
	 *					else - return 0
	 */
	size_t getHeight(unsigned frameNum) const;

	/**
	 * Get frame scale fill type
	 * @param frameNum	layout frame index
	 * @return			if frameNum is valid - return frame scale fill type,
	 *					else - return 0
	 */
	int getScaleFillType(unsigned frameNum) const;
protected:
	struct Frame {
		Frame();

		size_t paddingTop;
		size_t paddingLeft;
		size_t width;
		size_t height;
		int scaleType;
	};
	typedef ::std::vector<Frame> FrameVec;
	typedef FrameVec::iterator FrameIt;

protected:
	FrameVec m_Frames;	// list output frame parts descrioptions

	size_t m_Width;		// output layout width

	size_t m_Height;	// output laout height
};