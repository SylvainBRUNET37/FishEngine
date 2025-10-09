#pragma once

class Horloge
{
public:
	Horloge();
	static std::int64_t GetTimeCount();
	[[nodiscard]] double GetSecPerCount() const { return m_SecPerCount; }
	// retourne le temps en millisecondes entre deux count.
	[[nodiscard]] double GetTimeBetweenCounts(int64_t start, int64_t stop) const;

private:
	double m_SecPerCount;
};
