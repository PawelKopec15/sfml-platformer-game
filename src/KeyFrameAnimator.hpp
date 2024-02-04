#pragma once

#include <iostream>
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

	void addKeyFrame(signed long long when, const std::shared_ptr<KeyFrame>& keyFrame) { timeline[when] = keyFrame; }

	void addKeyFrame(signed long long when, float keyFrameTargetValue, bool keyFrameContinuous = false)
	{
		addKeyFrame(when, std::make_shared<KeyFrame>(KeyFrame(keyFrameTargetValue, keyFrameContinuous)));
	}

	const std::map<signed long long, std::shared_ptr<KeyFrame>>& getTimeline() const { return timeline; }

	void setLastKeyFrame(signed long long val) { lastKeyFrame = val; }
	signed long long getLastKeyFrame() const { return lastKeyFrame; }
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
	std::map<signed long long, std::shared_ptr<KeyFrame>> timeline;
	signed long long lastKeyFrame = -1;
};

class KeyFrameAnimator
{
public:
	KeyFrameAnimator(signed long long duration = 1000000, bool loop = true) : duration(duration), loop(loop) {}
	~KeyFrameAnimator() = default;

	void addKeyFrameTimeline(const std::string& name, const KeyFrameTimeline& timeline) { timelines[name] = timeline; }

	std::vector<std::pair<std::string, float>> reset(bool soft = false)
	{
		elapsedTime = soft ? (elapsedTime - duration) : 0;
		for (auto&& timeline : timelines)
			timeline.second.setLastKeyFrame(-1);

		return tick(0);
	}

	bool ended() const { return elapsedTime >= duration; }

	std::vector<std::pair<std::string, float>> tick(signed long long delta)
	{
		std::vector<std::pair<std::string, float>> toRet;

		if (ended())
		{
			if (loop)
				return reset(true);

			return toRet;
		}

		elapsedTime += delta;

		for (auto&& timeline : timelines)
		{
			std::pair<std::string, float> p = {"", 0};

			if (_handle_timeline(timeline, p))
				toRet.push_back(p);
		}

		return toRet;
	}

private:
	std::map<std::string, KeyFrameTimeline, std::less<>> timelines;

	bool _handle_timeline(std::pair<const std::string, KeyFrameTimeline>& outTimeline,
						  std::pair<std::string, float>& outToRet) const
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

	signed long long elapsedTime = 0;
	signed long long duration;
	bool loop;
};