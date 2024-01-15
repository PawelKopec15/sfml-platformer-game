#pragma once

#include <SFML/Graphics.hpp>

#include "HitboxEntity.hpp"

struct Controls
{
	sf::Keyboard::Key keyUp    = sf::Keyboard::Up;
	sf::Keyboard::Key keyDown  = sf::Keyboard::Down;
	sf::Keyboard::Key keyLeft  = sf::Keyboard::Left;
	sf::Keyboard::Key keyRight = sf::Keyboard::Right;

	sf::Keyboard::Key keyJump   = sf::Keyboard::Z;
	sf::Keyboard::Key keyRun    = sf::Keyboard::X;
	sf::Keyboard::Key keyStart  = sf::Keyboard::Enter;
	sf::Keyboard::Key keySelect = sf::Keyboard::Space;
};

class Player : public HitboxEntity
{
public:
	Player(const sf::Vector2f& position, Controls controls)
		: HitboxEntity(position, sf::Vector2f(14, 14)), controls(controls)
	{}
	~Player() override = default;

	void process(sf::Int64 delta) override
	{
		float horizontalInput =
			sf::Keyboard::isKeyPressed(controls.keyRight) - sf::Keyboard::isKeyPressed(controls.keyLeft);

		moveVector.x = horizontalInput;

		if (canJump && sf::Keyboard::isKeyPressed(controls.keyJump))
		{
			moveVector.y = -3.5f;
			canJump      = false;
		}

		this->HitboxEntity::process(delta);
	}

	void setCanJump(bool val) { canJump = val; }
	bool getCanJump() const { return canJump; }

private:
	Controls controls;

	bool canJump = false;
};