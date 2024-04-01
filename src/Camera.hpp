#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "GravityEntity.hpp"
#include "KeyFrameAnimator.hpp"
#include "TMXParser.hpp"

class Camera
{
public:
	Camera() { transitionAnimator.lock(); };
	~Camera() = default;

	void findCameraZones(const TMXMap& map)
	{
		for (const auto& objectGroup : map.objectGroups)
		{
			if (objectGroup.second.name == "CameraZones")
				_handleCameraZoneObjects(objectGroup.second.objects);
		}
	}

	void setView(const sf::View val) { view = val; }
	const sf::View& getView() { return view; }

	bool isInTransitionAnimation() { return !transitionAnimator.ended(); }

	void transitionAnimationTick(sf::Int64 delta, GravityEntity& outEntity)
	{
		auto vec = transitionAnimator.tick(delta);
		for (const auto& pair : vec)
		{
			switch (pair.first)
			{
				case TransitionKeyType::CAMERA_X:
					view.setCenter(pair.second, view.getCenter().y);
					break;
				case TransitionKeyType::CAMERA_Y:
					view.setCenter(view.getCenter().x, pair.second);
					break;
				case TransitionKeyType::ENTITY_X:
					outEntity.setPosition({pair.second, outEntity.getPosition().y});
					break;
				case TransitionKeyType::ENTITY_Y:
					outEntity.setPosition({outEntity.getPosition().x, pair.second});
					break;
			}
		}
	}

	void followEntity(GravityEntity& outEntity, float stopMarginHor, float stopMarginVer, bool restrictView = true,
					  bool restrictEntity = true)
	{
		view.setCenter(outEntity.getPosition());

		std::vector<std::shared_ptr<CameraZone>> activeCameraZones;

		// Special handling for lastCameraZone to avoid duplicates of it.
		// Always handled first so if entity wasn't supposed to get out of
		// it he will be forced to stay inside.
		if (lastCameraZone != nullptr)
		{
			if (restrictView)
				handleRestrictView(outEntity, *lastCameraZone);
			if (restrictEntity)
				handleRestrictEntity(outEntity, *lastCameraZone, stopMarginHor, stopMarginVer);
		}

		for (const auto& cameraZone : cameraZones)
		{
			if (outEntity.getPosition().x > cameraZone.bounds.left &&
				outEntity.getPosition().x < cameraZone.bounds.left + cameraZone.bounds.width &&
				outEntity.getPosition().y > cameraZone.bounds.top &&
				outEntity.getPosition().y < cameraZone.bounds.top + cameraZone.bounds.height)

				activeCameraZones.push_back(std::make_shared<CameraZone>(cameraZone));
		}

		for (auto&& cameraZone : activeCameraZones)
		{
			cameraZone->canInitiateTransition = true;

			if (restrictView)
				handleRestrictView(outEntity, *cameraZone);
			if (restrictEntity)
				handleRestrictEntity(outEntity, *cameraZone, stopMarginHor, stopMarginVer);
		}

		// Setting current camera zone as LastCameraZone (to prevent entity from getting out when he shouldn't).
		if (activeCameraZones.size() == 1)
			lastCameraZone = activeCameraZones[0];
	}

private:
	sf::View view;

	enum class CameraZoneBound
	{
		KILL_ENTITY             = -3,
		BLOCK_ENTITY            = -2,
		DONT_FOLLOW_PAST        = -1,
		IGNORE_BOUND            = 0,
		ANIMATE_TRANSITION      = 1,
		ANIMATE_TRANSITION_SLOW = 2,
	};

	struct CameraZone
	{
		sf::FloatRect bounds;
		CameraZoneBound top;
		CameraZoneBound bottom;
		CameraZoneBound left;
		CameraZoneBound right;

		bool canInitiateTransition = true;
	};

	std::vector<CameraZone> cameraZones        = {};
	std::shared_ptr<CameraZone> lastCameraZone = nullptr;

	enum class TransitionKeyType
	{
		CAMERA_X = 0,
		CAMERA_Y = 1,
		ENTITY_X = 2,
		ENTITY_Y = 3,
	};

	const sf::Int64 transitionAnimationBeginPoint = 250000;
	const sf::Int64 transitionAnimationEndPoint   = 850000;
	const sf::Int64 defaultTransitionDuration     = 1000000;
	const int slowModifier                        = 3;

	KeyFrameAnimator<TransitionKeyType> transitionAnimator =
		KeyFrameAnimator<TransitionKeyType>(defaultTransitionDuration, false);

	void setTransition(bool verticalNotHorizontal, float entityCurrentCoord, float entityTargetCoord,
					   float cameraTargetCoord, bool slow = false)
	{
		transitionAnimator.reset();
		transitionAnimator.clearAllTimelines();

		transitionAnimator.setDuration(slow ? slowModifier * defaultTransitionDuration : defaultTransitionDuration);

		KeyFrameTimeline entityTimeline;
		KeyFrameTimeline cameraTimeline;

		entityTimeline.addKeyFrame(transitionAnimationBeginPoint, entityCurrentCoord, false);

		entityTimeline.addKeyFrame(slow ? transitionAnimationEndPoint * slowModifier : transitionAnimationEndPoint,
								   entityTargetCoord, true);

		cameraTimeline.addKeyFrame(transitionAnimationBeginPoint,
								   verticalNotHorizontal ? view.getCenter().y : view.getCenter().x, false);

		cameraTimeline.addKeyFrame(slow ? transitionAnimationEndPoint * slowModifier : transitionAnimationEndPoint,
								   cameraTargetCoord, true);

		transitionAnimator.addKeyFrameTimeline(
			verticalNotHorizontal ? TransitionKeyType::ENTITY_Y : TransitionKeyType::ENTITY_X, entityTimeline);

		transitionAnimator.addKeyFrameTimeline(
			verticalNotHorizontal ? TransitionKeyType::CAMERA_Y : TransitionKeyType::CAMERA_X, cameraTimeline);
	}

	void handleRestrictView(GravityEntity& outEntity, const CameraZone& zone)
	{
		const sf::FloatRect viewRect(view.getCenter() - view.getSize() / 2.f, view.getCenter() + view.getSize() / 2.f);

		if (viewRect.height > zone.bounds.height + zone.bounds.top && zone.bottom != CameraZoneBound::IGNORE_BOUND)
			view.move(0, zone.bounds.height + zone.bounds.top - viewRect.height);
		else if (viewRect.top < zone.bounds.top && zone.top != CameraZoneBound::IGNORE_BOUND)
			view.move(0, zone.bounds.top - viewRect.top);

		if (viewRect.width > zone.bounds.width + zone.bounds.left && zone.right != CameraZoneBound::IGNORE_BOUND)
			view.move(zone.bounds.width + zone.bounds.left - viewRect.width, 0);
		else if (viewRect.left < zone.bounds.left && zone.left != CameraZoneBound::IGNORE_BOUND)
			view.move(zone.bounds.left - viewRect.left, 0);
	}

	void handleRestrictEntity(GravityEntity& outEntity, CameraZone& zone, float stopMarginHor, float stopMarginVer)
	{
		if (outEntity.getPosition().x < zone.bounds.left + stopMarginHor)
		{
			switch (zone.left)
			{
				case CameraZoneBound::BLOCK_ENTITY:
					outEntity.setPosition(sf::Vector2f(zone.bounds.left + stopMarginHor, outEntity.getPosition().y));
					outEntity.setMoveVector(sf::Vector2f(0.f, outEntity.getMoveVector().y));
					break;

				case CameraZoneBound::KILL_ENTITY:
					outEntity.die();
					break;

				case CameraZoneBound::ANIMATE_TRANSITION:
					if (zone.canInitiateTransition)
					{
						setTransition(false, outEntity.getPosition().x, outEntity.getPosition().x - 2.f * stopMarginHor,
									  view.getCenter().x - view.getSize().x, false);
						zone.canInitiateTransition = false;
					}
					break;

				case CameraZoneBound::ANIMATE_TRANSITION_SLOW:
					if (zone.canInitiateTransition)
					{
						setTransition(false, outEntity.getPosition().x, outEntity.getPosition().x - 2.f * stopMarginHor,
									  view.getCenter().x - view.getSize().x, true);
						zone.canInitiateTransition = false;
					}
					break;

				default:
					break;
			}
		}
		else if (outEntity.getPosition().x + stopMarginHor > zone.bounds.left + zone.bounds.width)
		{
			switch (zone.right)
			{
				case CameraZoneBound::BLOCK_ENTITY:
					outEntity.setPosition(
						sf::Vector2f(zone.bounds.left + zone.bounds.width - stopMarginHor, outEntity.getPosition().y));
					outEntity.setMoveVector(sf::Vector2f(0.f, outEntity.getMoveVector().y));
					break;

				case CameraZoneBound::KILL_ENTITY:
					outEntity.die();
					break;

				case CameraZoneBound::ANIMATE_TRANSITION:
					if (zone.canInitiateTransition)
					{
						setTransition(false, outEntity.getPosition().x, outEntity.getPosition().x + 2.f * stopMarginHor,
									  view.getCenter().x + view.getSize().x, false);
						zone.canInitiateTransition = false;
					}
					break;

				case CameraZoneBound::ANIMATE_TRANSITION_SLOW:
					if (zone.canInitiateTransition)
					{
						setTransition(false, outEntity.getPosition().x, outEntity.getPosition().x + 2.f * stopMarginHor,
									  view.getCenter().x + view.getSize().x, true);
						zone.canInitiateTransition = false;
					}
					break;

				default:
					break;
			}
		}

		if (outEntity.getPosition().y < zone.bounds.top + stopMarginVer)
		{
			switch (zone.top)
			{
				case CameraZoneBound::BLOCK_ENTITY:
					outEntity.setPosition(sf::Vector2f(outEntity.getPosition().x, zone.bounds.top + stopMarginVer));
					outEntity.setMoveVector(sf::Vector2f(outEntity.getMoveVector().x, 0.f));
					break;

				case CameraZoneBound::KILL_ENTITY:
					outEntity.die();
					break;

				case CameraZoneBound::ANIMATE_TRANSITION:
					if (zone.canInitiateTransition)
					{
						setTransition(true, outEntity.getPosition().y, outEntity.getPosition().y - 2.f * stopMarginVer,
									  view.getCenter().y - view.getSize().y, false);
						zone.canInitiateTransition = false;
					}
					break;

				case CameraZoneBound::ANIMATE_TRANSITION_SLOW:
					if (zone.canInitiateTransition)
					{
						setTransition(true, outEntity.getPosition().y, outEntity.getPosition().y - 2.f * stopMarginVer,
									  view.getCenter().y - view.getSize().y, true);
						zone.canInitiateTransition = false;
					}
					break;

				default:
					break;
			}
		}
		else if (outEntity.getPosition().y + stopMarginVer > zone.bounds.top + zone.bounds.height)
		{
			switch (zone.bottom)
			{
				case CameraZoneBound::BLOCK_ENTITY:
					outEntity.setPosition(
						sf::Vector2f(outEntity.getPosition().x, zone.bounds.top + zone.bounds.height - stopMarginVer));
					outEntity.setMoveVector(sf::Vector2f(outEntity.getMoveVector().x, 0.f));
					break;

				case CameraZoneBound::KILL_ENTITY:
					outEntity.die();
					break;

				case CameraZoneBound::ANIMATE_TRANSITION:
					if (zone.canInitiateTransition)
					{
						setTransition(true, outEntity.getPosition().y, outEntity.getPosition().y + 2.f * stopMarginVer,
									  view.getCenter().y + view.getSize().y, false);
						zone.canInitiateTransition = false;
					}
					break;

				case CameraZoneBound::ANIMATE_TRANSITION_SLOW:
					if (zone.canInitiateTransition)
					{
						setTransition(true, outEntity.getPosition().y, outEntity.getPosition().y + 2.f * stopMarginVer,
									  view.getCenter().y + view.getSize().y, true);
						zone.canInitiateTransition = false;
					}
					break;

				default:
					break;
			}
		}
	}

	CameraZoneBound intToCameraZoneBound(int intValue)
	{
		auto result = static_cast<CameraZoneBound>(intValue);

		if (static_cast<int>(result) != intValue)
			return CameraZoneBound::IGNORE_BOUND;

		return result;
	}

	void _handleCameraZoneObjects(std::map<int, TMXObject> objects)
	{
		for (auto& object : objects)
		{
			if (object.second.type != "CameraZone")
				continue;

			CameraZone newCameraZone;

			newCameraZone.bounds =
				sf::FloatRect(object.second.x, object.second.y, object.second.width, object.second.height);

			newCameraZone.top    = intToCameraZoneBound(std::stoi(object.second.properties["Top"].value));
			newCameraZone.bottom = intToCameraZoneBound(std::stoi(object.second.properties["Bottom"].value));
			newCameraZone.left   = intToCameraZoneBound(std::stoi(object.second.properties["Left"].value));
			newCameraZone.right  = intToCameraZoneBound(std::stoi(object.second.properties["Right"].value));

			cameraZones.push_back(newCameraZone);
		}
	}
};