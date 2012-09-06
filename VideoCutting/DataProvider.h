#pragma once
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "Defineds.h"

/**
 * CDataProvider	[class] contains video data buffer and provides getting
 *					video frame by frame method
 */
class CDataProvider {
public:
	typedef boost::shared_ptr<CDataProvider> ProviderPtr;
	typedef ::std::vector<ProviderPtr> ProviderVec;
	typedef ProviderVec::iterator ProviderIt;
	typedef ProviderVec::const_iterator ProviderCIt;

public:
	CDataProvider();
	/**
	 * Create provider using data from file
	 * @param fileName		encoded video file path
	 * @param width			video frame width
	 * @param height		video frame height
	 * @param framesPerSec	video frequency
	 * @param videoLen		video length in sec
	 */
	CDataProvider(const String& fileName, unsigned width,
		unsigned height, unsigned framesPerSec, unsigned videoLen);

	/**
	 * Create provider using buffered data
	 * @param buff			buffer with encoded video data
	 * @param buffLen		bytes count that should be copied into provider buffer from src
	 * @param width			video frame width
	 * @param height		video frame height
	 * @param framesPerSec	video frequency
	 * @param videoLen		video length in sec
	 */
	CDataProvider(Uint8* buff, const unsigned buffLen, unsigned width,
		unsigned height, unsigned framesPerSec, unsigned videoLen);

	CDataProvider(const CDataProvider& obj);
	virtual ~CDataProvider();

	virtual CDataProvider& operator=(const CDataProvider& obj);

	/**
	 * Init provider data form buffer
	 */
	virtual void setBuffer(Uint8* buff, const unsigned buffLen);

	/**
	 * Inti provider data from encoded video file
	 */
	virtual void setBufferFromEncodeFile(const String& fileName);

	/**
	 * Copy next frame from provider buffer
	 * @param buff	output buffer with coppied data
	 * @return		count of copied data
	 */
	virtual unsigned getNextFrame(Uint8* buff);

	/**
	 * @return frame length in bytes
	 */
	virtual unsigned getFrameLength() const;

	/**
	 * Get video frame width
	 */
	virtual unsigned getWidth() const;

	/**
	 * Set video frame width
	 */
	virtual void setWidth(unsigned width);

	/**
	 * Get video frame heigth
	 */
	virtual unsigned getHeight() const;

	/**
	 * Set video frame heigth
	 */
	virtual void setHeight(unsigned height);

protected:
	/**
	 * Calculate singl frame length
	 * @return frame length in bytes
	 */
	virtual void CalculateFrameLen();

	/**
	 * Clean all video chanels buffers
	 */
	void cleanBuffer();

protected:
	Uint8 *m_Buffer;			// buffer with video data content (without header, only samples)

	unsigned m_BuffLen;			// buffer with audio data size in byte

	unsigned m_Pos;				// current read data position

	unsigned m_FramesPerSec;	// video frequency

	unsigned m_FrameLen;		// singl frame length, byte count

	unsigned m_Width;			// video frame width

	unsigned m_Height;			// video frame height

	unsigned m_TimeLength;		// video trake length, s
};