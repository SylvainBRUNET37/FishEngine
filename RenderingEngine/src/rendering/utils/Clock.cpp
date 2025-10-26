#include "pch.h"
#include "rendering/utils/Clock.h"

#include "rendering/device/BlendState.h"

double Clock::secPerCount = []
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	return 1.0 / static_cast<double>(frequency.QuadPart);
}();

double Clock::GetSecPerCount()
{
	return secPerCount;
}

int64_t Clock::GetTimeCount()
{
	LARGE_INTEGER countNumber;
	QueryPerformanceCounter(&countNumber);
	return countNumber.QuadPart;
}
