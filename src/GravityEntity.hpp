#pragma once

#include <SFML/Graphics.hpp>

#include "AnimatedSprite.hpp"
#include "GlobalDefines.hpp"

class GravityEntity
{
public:
	GravityEntity(const sf::Vector2f& position, float gravityConstant = D_GRAV_CONSTANT,
				  float terminalVelocity = D_TERMINAL_VEL)
		: position(position), gravityConstant(gravityConstant), terminalVelocity(terminalVelocity)
	{}
	virtual ~GravityEntity() = default;

	virtual void process(sf::Int64 delta)
	{
		moveVector.y += gravityConstant * DELTA_CORRECTION;
		if (moveVector.y > terminalVelocity)
			moveVector.y = terminalVelocity;

		move(sf::Vector2f(moveVector.x * DELTA_CORRECTION, moveVector.y * DELTA_CORRECTION));
	}

	void setSpriteTexture(const sf::Texture& texture) { sprite.setTexture(texture); }
	void setSpriteOffset(const sf::Vector2f& offset) { spriteOffset = offset; }

	virtual void setPosition(const sf::Vector2f& val)
	{
		position = val;
		sprite.setPosition(val + spriteOffset);
	}
	const sf::Vector2f& getPosition() { return position; }

	const sf::Vector2f& getMoveVector() { return moveVector; }
	void setMoveVector(const sf::Vector2f& val) { moveVector = val; }

	virtual void move(const sf::Vector2f& offset)
	{
		position += offset;
		sprite.move(offset);
	}

protected:
	sf::Vector2f position;
	sf::Vector2f moveVector = sf::Vector2f(0, 0);

	float gravityConstant;
	float terminalVelocity;

	AnimatedSprite sprite;
	sf::Vector2f spriteOffset = sf::Vector2f(0, 0);
};