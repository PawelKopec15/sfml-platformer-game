#pragma once

#include "GravityEntity.hpp"
#include "MaskArea2D.hpp"

class Collectable : public GravityEntity
{
public:
	explicit Collectable(const sf::Vector2f& position, uint32_t mask, const sf::Vector2f& size = {16.f, 16.f},
						 const AnimatedSprite& sprite = AnimatedSprite())
		: GravityEntity(position, 0.f, 0.f), collectArea(position, size, mask)
	{
		this->sprite = sprite;
		this->sprite.setPosition(position);
	}

	void process(sf::Int64 delta) override
	{
		move(sf::Vector2f(moveVector.x * DELTA_CORRECTION, moveVector.y * DELTA_CORRECTION));
	}

	void setPosition(const sf::Vector2f& val) override
	{
		this->GravityEntity::setPosition(val);
		collectArea.setPosition(val);
	}
	void move(const sf::Vector2f& offset) override
	{
		this->GravityEntity::move(offset);
		collectArea.move(offset);
	}

	MaskArea2D& accessCollectArea() { return collectArea; }

private:
	MaskArea2D collectArea;
};