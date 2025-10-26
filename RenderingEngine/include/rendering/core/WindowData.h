#ifndef WINDOW_DATA_H
#define WINDOW_DATA_H

#include <cstdint>

enum class DisplayMode : uint8_t
{
	WINDOWED,
	FULLSCREEN
};

struct WindowData
{
	size_t screenWidth;
	size_t screenHeight;
	DisplayMode displayMode;
};

#endif