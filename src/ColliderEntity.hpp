#pragma once

#include <utility>

#include "CollisionBody.hpp"
#include "GlobalDefines.hpp"
#include "GravityEntity.hpp"

class ColliderEntity : public GravityEntity
{
public:
	template <typename... Args>
	ColliderEntity(const sf::Vector2f& position, const sf::Vector2f& colliderSize, const sf::Vector2f& colliderOffset,
				   Args&&... args)
		: GravityEntity(position, std::forward<Args>(args)...),
		  collider(position + colliderOffset, colliderSize, sf::Color(0, 0, 255, 128)),
		  colliderOffset(colliderOffset)
	{}

	~ColliderEntity() override = default;

	void process(sf::Int64 delta) override { this->GravityEntity::process(delta); }

	void setPosition(const sf::Vector2f& position) override
	{
		this->GravityEntity::setPosition(position);
		collider.setPosition(position + colliderOffset);
	}
	void setColliderSize(const sf::Vector2f& size) { collider.setSize(size); }
	void setColliderOffset(const sf::Vector2f& val) { colliderOffset = val; }

	void move(const sf::Vector2f& offset) override
	{
		this->GravityEntity::move(offset);
		collider.move(offset);
	}

	sf::Vector2f getCenter() { return collider.getCenter(); }

	CollisionBody& accessCollider() { return collider; }

	void setOnFloor(bool val) { onFloor = val; }
	void setOnCeil(bool val) { onCeil = val; }
	void setOnLeftWall(bool val) { onLeftWall = val; }
	void setOnRightWall(bool val) { onRightWall = val; }

	void resetOnEverything()
	{
		setOnFloor(false);
		setOnCeil(false);
		setOnLeftWall(false);
		setOnRightWall(false);
	}

protected:
	CollisionBody collider;
	sf::Vector2f colliderOffset = sf::Vector2f(0.f, 0.f);

	bool onFloor     = false;
	bool onCeil      = false;
	bool onLeftWall  = false;
	bool onRightWall = false;
};