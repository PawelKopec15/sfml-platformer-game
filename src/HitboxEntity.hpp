#pragma once

#include "CollisionBody.hpp"
#include "GlobalDefines.hpp"
#include "GravityEntity.hpp"

struct Hitbox
{
	std::string name = "";
	sf::RectangleShape box;
	sf::Vector2f offset = sf::Vector2f(0, 0);
};

class HitboxEntity : public GravityEntity
{
public:
	HitboxEntity(const sf::Vector2f& position, const sf::Vector2f& colliderSize = sf::Vector2f(14.f, 13.f),
				 const sf::Vector2f& colliderOffset = sf::Vector2f(0.f, 0.f), float gravityConstant = D_GRAV_CONSTANT,
				 float terminalVelocity = D_TERMINAL_VEL)
		: GravityEntity(position, gravityConstant, terminalVelocity),
		  collider(position + colliderOffset, colliderSize, sf::Color(0, 0, 255, 128)),
		  colliderOffset(colliderOffset)
	{}
	~HitboxEntity() override = default;

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

	void setNextFrameResetHor(bool val) { nextFrameResetHor = val; }
	void setNextFrameResetVer(bool val) { nextFrameResetVer = val; }
	bool getNextFrameResetHor() const { return nextFrameResetHor; }
	bool getNextFrameResetVer() const { return nextFrameResetVer; }

	void setOnFloor(bool val) { onFloor = val; }
	void setOnCeil(bool val) { onCeil = val; }
	void setOnWall(bool val) { onWall = val; }

protected:
	CollisionBody collider;
	sf::Vector2f colliderOffset;
	Hitbox hurtBox;
	Hitbox attackBox;

	bool nextFrameResetHor = false;
	bool nextFrameResetVer = false;

	bool onFloor = false;
	bool onCeil  = false;
	bool onWall  = false;
};