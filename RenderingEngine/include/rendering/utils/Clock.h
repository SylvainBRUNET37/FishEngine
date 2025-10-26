#ifndef CLOCK_H
#define CLOCK_H
#include <cstdint>

class Clock
{
public:
	Clock() = delete;

	[[nodiscard]] static std::int64_t GetTimeCount();
	[[nodiscard]] static double GetSecPerCount();

	[[nodiscard]] static double GetTimeBetweenCounts(const std::int64_t start, const std::int64_t stop)
	{
		return static_cast<double>(stop - start) * GetSecPerCount();
	}
private:
	static double secPerCount;
};

#endif
