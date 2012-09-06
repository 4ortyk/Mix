#include "stdafx.h"
#include "Layout.h"

CLayout::CLayout()
{
	m_Width = 352;
	m_Height = 288;
	
	/*
	   ++=========+=========++
	   ||         |         ||
	   ||         | frame 2 ||
	   ||         |         ||
	   || frame 1 +---------++
	   ||         |         ||
	   ||         | frame 3 ||
	   ||         |         ||
	   ++=========+=========++
	*/

	Frame frame;
	// frame 1
	frame.paddingLeft = 0;
	frame.paddingTop = 0;
	frame.width = 176;
	frame.height = 288;
	frame.scaleType = eFillByHeight;
	m_Frames.push_back(frame);

	// frame 2
	frame.paddingLeft = 176;
	frame.paddingTop = 0;
	frame.width = 176;
	frame.height = 144;
	frame.scaleType = eFillByWidth | eFillByHeight;
	m_Frames.push_back(frame);

	// frame 3
	frame.paddingLeft = 176;
	frame.paddingTop = 144;
	frame.width = 176;
	frame.height = 144;
	frame.scaleType = eFillByWidth | eFillByHeight;
	m_Frames.push_back(frame);
}

CLayout::~CLayout()
{
	m_Frames.clear();
}


size_t CLayout::getFrameArea() const
{
	return m_Width * m_Height;
}

size_t CLayout::getFrameArea(unsigned frameNum) const
{
	if (frameNum < 0 || frameNum >= m_Frames.size())
		return 0;

	return m_Frames.at(frameNum).width * m_Frames.at(frameNum).height ;
}


size_t CLayout::getPartsCount() const
{
	return m_Frames.size();
}


size_t CLayout::getTopPadding(unsigned frameNum) const
{
	if (frameNum < 0 || frameNum >= m_Frames.size())
		return getHeight();

	return m_Frames.at(frameNum).paddingTop;
}


size_t CLayout::getLeftPadding(unsigned frameNum) const
{
	if (frameNum < 0 || frameNum >= m_Frames.size())
		return getWidth();

	return m_Frames.at(frameNum).paddingLeft;
}

size_t CLayout::getWidth() const
{
	return m_Width;
}

size_t CLayout::getWidth(unsigned frameNum) const
{
	if (frameNum < 0 || frameNum >= m_Frames.size())
		return 0U;

	return m_Frames.at(frameNum).width;
}

size_t CLayout::getHeight() const
{
	return m_Height;
}

size_t CLayout::getHeight(unsigned frameNum) const
{
	if (frameNum < 0 || frameNum >= m_Frames.size())
		return 0U;

	return m_Frames.at(frameNum).height;
}

int CLayout::getScaleFillType(unsigned frameNum) const
{
	if (frameNum < 0 || frameNum >= m_Frames.size())
		return eFillNone;

	return m_Frames.at(frameNum).scaleType; 
}

CLayout::Frame::Frame()
{
	paddingLeft = 0u;
	paddingTop = 0u;
	width = 0u;
	height = 0u;
	scaleType = eFillNone;
}