#pragma once

#ifndef UNICODE  
typedef ::std::string String;
#else
typedef ::std::wstring String;
#endif

#define AUDIO_PORT_COUNT 1
#define AUDIO_SOURCE_COUNT 4
#define BYTES_PER_SEC 2

typedef unsigned __int8 Uint8;