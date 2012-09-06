#include "stdafx.h"
#include "DataProvider.h"

CDataProvider::CDataProvider()
{
	m_Buffer = NULL;
	m_FrameLen = 0;
	m_BuffPos = 0;
	m_FilePos = 0;
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
	m_FilePos = 0;
	m_BuffPos = 0;
	m_Width = width;
	m_Height = height;
	m_BuffTimeLen = videoLen;
	CalculateFrameLen();

	m_SrcFileName = fileName;
	setBufferFromEncodeFile();
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

	m_BuffPos = obj.m_BuffPos;
	m_BuffLen = obj.m_BuffLen;
	m_FilePos = obj.m_FilePos;
	m_SrcFileName = obj.m_SrcFileName;
	m_BuffTimeLen = obj.m_BuffTimeLen;

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

void CDataProvider::setBufferFromEncodeFile()
{
	::std::ifstream inputFile(m_SrcFileName, ::std::ios::binary|::std::ios::in);
	if (!inputFile)
		return;

	inputFile.seekg (0, std::ios::end);
	std::streamoff end = inputFile.tellg();

	int oldBuffLen = m_BuffLen;
	m_BuffLen = ::std::min((unsigned) (end - m_FilePos),
		m_FrameLen * m_FramesPerSec * m_BuffTimeLen);

	if (m_BuffLen == 0) {
		cleanBuffer();
		inputFile.close();
		return;
	}

	if (m_BuffLen != oldBuffLen) {
		cleanBuffer();
		m_Buffer = new Uint8[m_BuffLen];
	}

	if (m_Buffer != NULL) {
		inputFile.seekg (m_FilePos, std::ios::beg);
		inputFile.read((char *)m_Buffer, m_BuffLen);
	}

	m_FilePos += m_BuffLen;
	inputFile.close();
}

unsigned CDataProvider::getNextFrame(Uint8* buff)
{
	if (buff == NULL)
		return 0;

	if (m_BuffPos == m_BuffLen - 1) {
		m_BuffPos = 0;
		setBufferFromEncodeFile();
	}

	if (m_BuffLen == 0)
		return 0;

	unsigned copySize = 
		((m_BuffPos + m_FrameLen) < m_BuffLen) ? m_FrameLen : m_BuffLen - m_BuffPos - 1;
	errno_t isError = memcpy_s(buff, copySize, m_Buffer + m_BuffPos, copySize);
	m_BuffPos += copySize;
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
		printf("%p\n", m_Buffer);
		m_Buffer = NULL;
	}
}