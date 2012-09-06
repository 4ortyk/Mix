#include "stdafx.h"
#include "DataProvider.h"

CDataProvider::CDataProvider()
{
	m_Buffer = NULL;
	m_FrameLen = 0;
	m_Pos = 0;
	m_Width = 0;
	m_Height = 0;
}

CDataProvider::CDataProvider(const String& fileName, 
						 	 unsigned width,
							 unsigned height,
							 unsigned framesPerSec,
							 unsigned videoLen)
{
	m_Buffer = NULL;

	m_FramesPerSec = framesPerSec;
	m_Pos = 0;
	m_Width = width;
	m_Height = height;
	m_TimeLength = videoLen;
	CalculateFrameLen();

	setBufferFromEncodeFile(fileName);
}

CDataProvider::CDataProvider(Uint8* buff, 
							 const unsigned buffLen,
							 unsigned width,
							 unsigned height,
							 unsigned framesPerSec,
							 unsigned videoLen)
{
	m_Buffer = NULL;

	m_FramesPerSec = framesPerSec;
	m_Pos = 0;
	m_Width = width;
	m_Height = height;
	m_TimeLength = videoLen;
	CalculateFrameLen();

	setBuffer(buff, buffLen);
}

CDataProvider::CDataProvider(const CDataProvider& obj)
{
	*this = obj;
}

CDataProvider::~CDataProvider()
{
	cleanBuffer();
}

 CDataProvider& CDataProvider::operator=(const CDataProvider& obj)
 {
	if (this == &obj)
		return *this;

	m_Pos = obj.m_Pos;
	m_BuffLen = obj.m_BuffLen;

	cleanBuffer();

	if (obj.m_Buffer == NULL) {
		return *this;
	}

	m_Buffer = new Uint8[m_BuffLen];
	if (m_Buffer != NULL) {
		memcpy_s(m_Buffer, m_BuffLen, obj.m_Buffer, m_BuffLen);
	}

	return *this;
 }

// init provider data form stream
void CDataProvider::setBuffer(Uint8* buff, const unsigned buffLen)
{
	if (buff == NULL)
		return;

	cleanBuffer();
	
	unsigned len = (buffLen % 4) ? (buffLen / 4) * 4 : buffLen;
	len = ::std::min(len, m_FrameLen * m_FramesPerSec * m_TimeLength);

	m_Buffer = new Uint8[len];
	if (m_Buffer != NULL) {
		memcpy_s(m_Buffer, len, buff, len);
	}
}


void CDataProvider::setBufferFromEncodeFile(const String& fileName)
{
	::std::ifstream inputFile(fileName, ::std::ios::binary|::std::ios::in);
	if (!inputFile)
		return;

	inputFile.seekg (0, std::ios::end);
	std::streamoff end = inputFile.tellg();

	inputFile.seekg (0, std::ios::beg);
	std::streamoff begin = inputFile.tellg();
	m_BuffLen = ::std::min((unsigned) (end - begin),
		m_FrameLen * m_FramesPerSec * m_TimeLength);
	
	cleanBuffer();

	m_Buffer = new Uint8[m_BuffLen];
	if (m_Buffer != NULL) {
		inputFile.read((char *)m_Buffer, m_BuffLen);
	}

	inputFile.close();
}

unsigned CDataProvider::getNextFrame(Uint8* buff)
{
	if (buff == NULL || m_Buffer == NULL)
		return 0;

	if (m_Pos == m_BuffLen - 1)
		return 0;

	unsigned copySize = 
		((m_Pos + m_FrameLen) < m_BuffLen) ? m_FrameLen : m_BuffLen - m_Pos - 1;
	errno_t isError = memcpy_s(buff, copySize, m_Buffer + m_Pos, copySize);
	m_Pos += copySize;
	return (isError) ? 0 : copySize;
}


unsigned CDataProvider::getFrameLength() const
{
	return m_FrameLen;
}


unsigned CDataProvider::getWidth() const
{
	return m_Width;
}


void CDataProvider::setWidth(unsigned width)
{
	m_Width = width;
	CalculateFrameLen();
}


unsigned CDataProvider::getHeight() const
{
	return m_Height;
}


void CDataProvider::setHeight(unsigned height)
{
	m_Height = height;
	CalculateFrameLen();
}


void CDataProvider::CalculateFrameLen()
{
	m_FrameLen = static_cast<unsigned>(m_Width * m_Height * YUV_SIZE_KOEF);
}

void CDataProvider::cleanBuffer()
{
	if (m_Buffer != NULL) {
		delete [] m_Buffer;
		m_Buffer = NULL;
	}
}