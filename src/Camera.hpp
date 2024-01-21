#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "GravityEntity.hpp"
#include "TMXParser.hpp"

class Camera
{
public:
	Camera()  = default;
	~Camera() = default;

	void findCameraZones(const TMXMap& map)
	{
		for (const auto& objectGroup : map.objectGroups)
		{
			if (objectGroup.second.name == "CameraZones")
				handleCameraZoneLayer(objectGroup.second.objects);
		}
	}

	void setView(const sf::View val) { view = val; }
	const sf::View getView() { return view; }

	void followEntity(GravityEntity& outEntity, float stopMarginHor, float stopMarginVer, bool restrictView = true,
					  bool restrictEntity = true)
	{
		view.setCenter(outEntity.getPosition());

		std::vector<std::shared_ptr<CameraZone>> activeCameraZones;

		for (const auto& cameraZone : cameraZones)
		{
			if (outEntity.getPosition().x > cameraZone.bounds.left &&
				outEntity.getPosition().x < cameraZone.bounds.left + cameraZone.bounds.width &&
				outEntity.getPosition().y > cameraZone.bounds.top &&
				outEntity.getPosition().y < cameraZone.bounds.top + cameraZone.bounds.height)

				activeCameraZones.push_back(std::make_shared<CameraZone>(cameraZone));
		}

		if (activeCameraZones.empty() && lastCameraZone != nullptr)
			activeCameraZones.push_back(lastCameraZone);
		else if (activeCameraZones.size() == 1)
			lastCameraZone = activeCameraZones[0];

		for (const auto& cameraZone : activeCameraZones)
		{
			if (restrictView)
				handleRestrictView(outEntity, *cameraZone);
			if (restrictEntity)
				handleRestrictEntity(outEntity, *cameraZone, stopMarginHor, stopMarginVer);
		}
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
	};

	std::vector<CameraZone> cameraZones        = {};
	std::shared_ptr<CameraZone> lastCameraZone = nullptr;

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

	void handleRestrictEntity(GravityEntity& outEntity, const CameraZone& zone, float stopMarginHor,
							  float stopMarginVer)
	{
		if (outEntity.getPosition().x < zone.bounds.left + stopMarginHor && zone.left == CameraZoneBound::BLOCK_ENTITY)
		{
			outEntity.setPosition(sf::Vector2f(zone.bounds.left + stopMarginHor, outEntity.getPosition().y));
			outEntity.setMoveVector(sf::Vector2f(0.f, outEntity.getMoveVector().y));
		}
		else if (outEntity.getPosition().x + stopMarginHor > zone.bounds.left + zone.bounds.width &&
				 zone.right == CameraZoneBound::BLOCK_ENTITY)
		{
			outEntity.setPosition(
				sf::Vector2f(zone.bounds.left + zone.bounds.width - stopMarginHor, outEntity.getPosition().y));
			outEntity.setMoveVector(sf::Vector2f(0.f, outEntity.getMoveVector().y));
		}

		if (outEntity.getPosition().y < zone.bounds.top + stopMarginVer && zone.top == CameraZoneBound::BLOCK_ENTITY)
		{
			outEntity.setPosition(sf::Vector2f(outEntity.getPosition().x, zone.bounds.top + stopMarginVer));
			outEntity.setMoveVector(sf::Vector2f(outEntity.getMoveVector().x, 0.f));
		}
		else if (outEntity.getPosition().y + stopMarginVer > zone.bounds.top + zone.bounds.height &&
				 zone.bottom == CameraZoneBound::BLOCK_ENTITY)
		{
			outEntity.setPosition(
				sf::Vector2f(outEntity.getPosition().x, zone.bounds.top + zone.bounds.height - stopMarginVer));
			outEntity.setMoveVector(sf::Vector2f(outEntity.getMoveVector().x, 0.f));
		}
	}

	CameraZoneBound intToCameraZoneBound(int intValue)
	{
		auto result = static_cast<CameraZoneBound>(intValue);

		if (static_cast<int>(result) != intValue)
			return CameraZoneBound::IGNORE_BOUND;

		return result;
	}

	void handleCameraZoneLayer(const std::map<int, TMXObject>& layer)
	{
		auto layerCopy = layer;

		for (auto& object : layerCopy)
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