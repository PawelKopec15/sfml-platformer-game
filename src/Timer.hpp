#pragma once

#include "GlobalDefines.hpp"

class Timer
{
public:
	Timer() = default;
	explicit Timer(signed long long microseconds) : target(microseconds) {}
	explicit Timer(float seconds) : target((int)(seconds * 1000000.f)) {}
	~Timer() = default;

	void set(signed long long microseconds) { target = microseconds; }
	void setSeconds(float seconds) { target = (int)(seconds * 1000000.f); }
	void reset() { elapsedMicroseconds = 0; }
	void block() { elapsedMicroseconds = target; }

	bool hasTimedOut() const { return elapsedMicroseconds >= target; }
	signed long long getTimeLeft() const { return target - elapsedMicroseconds; }

	bool tickAutoReset(signed long long delta, bool softReset = false)
	{
		elapsedMicroseconds += delta;
		if (!hasTimedOut())
			return false;

		if (softReset)
			elapsedMicroseconds -= target;
		else
			reset();

		return true;
	}
	bool tick(signed long long delta)
	{
		if (hasTimedOut())
			return true;
		elapsedMicroseconds += delta;
		return false;
	}

private:
	signed long long elapsedMicroseconds = 0;
	signed long long target              = 1000000;
};