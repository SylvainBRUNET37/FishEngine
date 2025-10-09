#include "pch.h"
#include "rendering/utils/Horloge.h"

namespace PM3D
{
	Horloge::Horloge()
	{
		LARGE_INTEGER counterFrequency;
		QueryPerformanceFrequency(&counterFrequency);
		m_SecPerCount = 1.0 / static_cast<double>(counterFrequency.QuadPart);
	}

	std::int64_t Horloge::GetTimeCount()
	{
		LARGE_INTEGER countNumber;
		QueryPerformanceCounter(&countNumber);
		return countNumber.QuadPart;
	}

	double Horloge::GetTimeBetweenCounts(const int64_t start, const int64_t stop) const
	{
		return static_cast<double>(stop - start) * m_SecPerCount;
	}
} // namespace PM3D
