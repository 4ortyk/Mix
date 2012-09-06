#pragma once

#ifndef UNICODE  
typedef ::std::string String;
#else
typedef ::std::wstring String;
#endif

#define VIDEO_PORT_COUNT 8
#define VIDEO_SOURCE_COUNT 3

#define YUV_SIZE_KOEF 1.5f

#define VIDEO_LEN_SEC 1
#define FRAMES_PER_SEC 25

typedef unsigned __int8 Uint8;