#pragma once

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

struct KeyFrame
{
	float targetValue = 0.f;
	bool continuous   = false;

	KeyFrame(float targetValue, bool continuous = false) : targetValue(targetValue), continuous(continuous) {}
};

class KeyFrameTimeline
{
public:
	KeyFrameTimeline()  = default;
	~KeyFrameTimeline() = default;

	void addKeyFrame(int when, const std::shared_ptr<KeyFrame>& keyFrame) { timeline[when] = keyFrame; }

	void addKeyFrame(int when, float keyFrameTargetValue, bool keyFrameContinuous = false)
	{
		addKeyFrame(when, std::make_shared<KeyFrame>(KeyFrame(keyFrameTargetValue, keyFrameContinuous)));
	}

	const std::map<int, std::shared_ptr<KeyFrame>>& getTimeline() const { return timeline; }

	void setLastKeyFrame(int val) { lastKeyFrame = val; }
	int getLastKeyFrame() const { return lastKeyFrame; }
	float getLastKeyFrameTargetValue() const
	{
		try
		{
			return timeline.at(lastKeyFrame)->targetValue;
		} catch (const std::out_of_range&)
		{
			return 0.f;
		}
	}

private:
	std::map<int, std::shared_ptr<KeyFrame>> timeline;
	int lastKeyFrame = -1;
};

template <typename T>
class KeyFrameAnimator
{
public:
	KeyFrameAnimator(int duration = 1000000, bool loop = true) : duration(duration), loop(loop) {}
	~KeyFrameAnimator() = default;

	void setDuration(int val) { duration = val; }
	int getDuration() const { return duration; }

	void addKeyFrameTimeline(const T& name, const KeyFrameTimeline& timeline) { timelines[name] = timeline; }

	void addKeyToKeyFrameTimeline(const T& timeLineName, int when, const std::shared_ptr<KeyFrame>& keyFrame)
	{
		if (!timelines.count(timeLineName))
			addKeyFrameTimeline(timeLineName, KeyFrameTimeline());

		timelines[timeLineName].addKeyFrame(when, keyFrame);
	}
	void addKeyToKeyFrameTimeline(const T& timeLineName, int when, float keyFrameTargetValue,
								  bool keyFrameContinuous = false)
	{
		addKeyToKeyFrameTimeline(timeLineName, when,
								 std::make_shared<KeyFrame>(KeyFrame(keyFrameTargetValue, keyFrameContinuous)));
	}

	void clearAllTimelines() { timelines.clear(); }

	std::vector<std::pair<T, float>> reset(bool soft = false)
	{
		elapsedTime = soft ? (elapsedTime - duration) : 0;
		for (auto&& timeline : timelines)
			timeline.second.setLastKeyFrame(-1);

		return tick(0);
	}

	bool ended() const { return elapsedTime >= duration; }
	void lock() { elapsedTime = duration; }

	std::vector<std::pair<T, float>> tick(int delta)
	{
		std::vector<std::pair<T, float>> toRet;

		if (ended())
		{
			if (loop)
				return reset(true);

			return toRet;
		}

		elapsedTime += delta;

		for (auto&& timeline : timelines)
		{
			std::pair<T, float> p = {T(), 0};

			if (_handle_timeline(timeline, p))
				toRet.push_back(p);
		}

		return toRet;
	}

private:
	std::map<T, KeyFrameTimeline, std::less<>> timelines;

	int elapsedTime = 0;
	int duration;
	bool loop;

	bool _handle_timeline(std::pair<const T, KeyFrameTimeline>& outTimeline, std::pair<T, float>& outToRet) const
	{
		bool foundAnything      = false;
		bool foundPreviousPoint = false;

		for (const auto& keyFramePoint : outTimeline.second.getTimeline())
		{
			if (keyFramePoint.first <= elapsedTime)
			{
				foundPreviousPoint = true;

				if (keyFramePoint.first > outTimeline.second.getLastKeyFrame())
				{
					outToRet      = {outTimeline.first, keyFramePoint.second->targetValue};
					foundAnything = true;
					outTimeline.second.setLastKeyFrame(keyFramePoint.first);
				}
			}
			else if (foundPreviousPoint && keyFramePoint.first > elapsedTime)
			{
				if (!keyFramePoint.second->continuous)
					return foundAnything;

				auto keyFrameIntervalTime = (float)(keyFramePoint.first - outTimeline.second.getLastKeyFrame());
				if (keyFrameIntervalTime <= 0.f)
					continue;

				auto midwayVal =
					((float)(elapsedTime - outTimeline.second.getLastKeyFrame()) / keyFrameIntervalTime) *
						(keyFramePoint.second->targetValue - outTimeline.second.getLastKeyFrameTargetValue()) +
					outTimeline.second.getLastKeyFrameTargetValue();

				outToRet = {outTimeline.first, midwayVal};

				return true;
			}
		}

		return foundAnything;
	}
};
