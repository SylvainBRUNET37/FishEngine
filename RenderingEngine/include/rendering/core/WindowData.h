#ifndef WINDOW_DATA_H
#define WINDOW_DATA_H

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