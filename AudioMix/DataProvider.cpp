#include "stdafx.h"
#include "DataProvider.h"

Uint8 CDataProvider::minVolume = 0;
Uint8 CDataProvider::maxVolume = 127;

CDataProvider::CDataProvider()
{
	m_Buffer = NULL;
	m_Pos = 0;
	m_Volume = 127;
}

CDataProvider::CDataProvider(const String& fileName, unsigned headerSize, Uint8 volume)
{
	m_Buffer = NULL;
	setBufferFromFile(fileName, headerSize);

	m_Pos = 0;
	m_Volume = volume;
}

CDataProvider::CDataProvider(Uint8* buff, const unsigned buffLen, Uint8 volume)
{
	m_Buffer = NULL;
	setBuffer(buff, buffLen);

	m_Pos = 0;
	m_Volume = volume;
}

CDataProvider::CDataProvider(const CDataProvider& obj)
{
	*this = obj;
}

CDataProvider::~CDataProvider()
{
	if (m_Buffer != NULL) {
		delete [] m_Buffer;
		m_Buffer = NULL;
	}	
}

 CDataProvider& CDataProvider::operator=(const CDataProvider& obj)
 {
	if (this == &obj)
		return *this;

	m_Pos = obj.m_Pos;
	m_Volume = obj.m_Volume;
	m_BuffLen = obj.m_BuffLen;

	if (m_Buffer != NULL) {
		delete [] m_Buffer;
		m_Buffer = NULL;
	}

	if (obj.m_Buffer == NULL) {
		return *this;
	}

	m_Buffer = new Uint8[m_BuffLen];
	if (m_Buffer != NULL) {
		memcpy_s(m_Buffer, m_BuffLen, obj.m_Buffer, m_BuffLen);
	}

	return *this;
 }

void CDataProvider::setVolume(Uint8 volume)
{
	m_Volume = (volume > maxVolume) ? maxVolume : volume;
}

Uint8 CDataProvider::getVolume() const
{
	return m_Volume;
}

// init provider data form stream
void CDataProvider::setBuffer(unsigned __int8* buff, const unsigned buffLen)
{
	if (buff == NULL)
		return;

	m_BuffLen = buffLen;
	if (m_Buffer)
		delete [] m_Buffer;

	m_Buffer = new unsigned __int8[m_BuffLen];
	memcpy(buff, m_Buffer, buffLen);
}

// inti provider data from file
void CDataProvider::setBufferFromFile(const String& fileName, unsigned headerSize)
{
	::std::ifstream inputFile(fileName, ::std::ios::binary|::std::ios::in);
	if (!inputFile)
		return;

	inputFile.seekg (0, std::ios::end);
	std::streamoff end = inputFile.tellg();

	inputFile.seekg (headerSize, std::ios::beg);
	std::streamoff begin = inputFile.tellg();
	m_BuffLen = (unsigned) (end - begin);

	if (m_Buffer)
		delete [] m_Buffer;

	m_Buffer = new Uint8[m_BuffLen];
	inputFile.read((char *)m_Buffer, m_BuffLen);
	inputFile.close();
}

////////////////////////////////////////////////////////////////
// copy next data portion from provider buffer
// @param len - portion size
// @buff - output buffer with coppied data
// return count of copied data
unsigned CDataProvider::getNextDataPortion(unsigned len, unsigned __int8* buff)
{
	if (buff == NULL)
		return 0;

	if (m_Pos == m_BuffLen - 1)
		return 0;

	unsigned copySize = ((m_Pos + len) < m_BuffLen) ? len : m_BuffLen - m_Pos - 1;
	memcpy(buff, m_Buffer + m_Pos , copySize);
	m_Pos += copySize;
	return copySize;
}