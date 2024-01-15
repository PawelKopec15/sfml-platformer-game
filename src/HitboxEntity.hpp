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
	HitboxEntity(const sf::Vector2f& position, const sf::Vector2f& collisionBodySize = sf::Vector2f(16, 16),
				 float gravityConstant = D_GRAV_CONSTANT, float terminalVelocity = D_TERMINAL_VEL)
		: GravityEntity(position, gravityConstant, terminalVelocity),
		  collider(position, collisionBodySize, sf::Color(0, 0, 255, 128))
	{}
	~HitboxEntity() override = default;

	void process(sf::Int64 delta) override { this->GravityEntity::process(delta); }

	void setPosition(const sf::Vector2f& position) override
	{
		this->GravityEntity::setPosition(position);
		collider.setPosition(position);
	}
	void setCollisionBodySize(const sf::Vector2f& size) { collider.setSize(size); }

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

protected:
	CollisionBody collider;
	Hitbox hurtBox;
	Hitbox attackBox;

	bool nextFrameResetHor = false;
	bool nextFrameResetVer = false;
};