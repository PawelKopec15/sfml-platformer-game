#pragma once

#include "GlobalDefines.hpp"

class Timer
{
public:
	Timer() = default;
	explicit Timer(int microseconds) : target(microseconds) {}
	explicit Timer(float seconds) : target((int)(seconds * 1000000.f)) {}
	~Timer() = default;

	void set(int microseconds) { target = microseconds; }
	void setSeconds(float seconds) { target = (int)(seconds * 1000000.f); }
	void reset() { elapsedMicroseconds = 0; }
	void block() { elapsedMicroseconds = target; }

	bool hasTimedOut() const { return elapsedMicroseconds >= target; }
	int getTimeLeft() const { return target - elapsedMicroseconds; }
	int getTargetTime() const { return target; }

	bool tickAutoReset(int delta, bool softReset = false)
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
	bool tick(int delta)
	{
		if (hasTimedOut())
			return true;
		elapsedMicroseconds += delta;
		return false;
	}

private:
	int elapsedMicroseconds = 0;
	int target              = 1000000;
};