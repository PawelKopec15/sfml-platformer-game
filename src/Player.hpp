#pragma once

#include <SFML/Graphics.hpp>

#include "HitboxEntity.hpp"
#include "Timer.hpp"

class Controls
{
public:
	struct Key
	{
		sf::Keyboard::Key keyBind;
		bool pressed      = false;
		bool justPressed  = false;
		bool justReleased = false;

		explicit Key(sf::Keyboard::Key keyBind) : keyBind(keyBind) {}
	};

	Controls()  = default;
	~Controls() = default;

	void update()
	{
		for (auto&& key : arr)
		{
			bool now = sf::Keyboard::isKeyPressed(key.keyBind);

			if (now && !key.pressed)
				key.justPressed = true;
			else
				key.justPressed = false;

			if (!now && key.pressed)
				key.justReleased = true;
			else
				key.justReleased = false;

			key.pressed = now;
		}
	}

	Key getKey(uint32_t index) { return arr[index]; }

private:
	void _updateKeyStatus()
	{
		for (auto&& key : arr)
		{
			bool now = sf::Keyboard::isKeyPressed(key.keyBind);

			if (now && !key.pressed)
				key.justPressed = true;
			else
				key.justPressed = false;

			if (!now && key.pressed)
				key.justReleased = true;
			else
				key.justReleased = false;

			key.pressed = now;
		}
	}

	std::array<Key, 8> arr = {Key(sf::Keyboard::Up),    Key(sf::Keyboard::Down), Key(sf::Keyboard::Left),
							  Key(sf::Keyboard::Right), Key(sf::Keyboard::Z),    Key(sf::Keyboard::LShift),
							  Key(sf::Keyboard::Enter), Key(sf::Keyboard::Space)};
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
		controls.update();

		float horizontalInput = (float)controls.getKey(3).pressed - (float)controls.getKey(2).pressed;
		const bool running    = controls.getKey(5).pressed;

		// running
		if (running && horizontalInput != 0.f)
		{
			if (fabs(moveVector.x) < walkSpeed)
				moveVector.x = horizontalInput * walkSpeed;

			if ((moveVector.x > 0 && horizontalInput > 0) || (moveVector.x < 0 && horizontalInput < 0))
				moveVector.x += horizontalInput * acc * DELTA_CORRECTION;
			else
				moveVector.x += horizontalInput * turnAroundDeAcc * DELTA_CORRECTION;

			if (moveVector.x > maxRunSpeed)
				moveVector.x = maxRunSpeed;
			else if (moveVector.x < -maxRunSpeed)
				moveVector.x = -maxRunSpeed;
		}
		else
		{
			if (std::fabs(moveVector.x) > walkSpeed)
				moveVector.x -= std::fabs(moveVector.x) / moveVector.x * deAcc * DELTA_CORRECTION;
			else
				moveVector.x = horizontalInput * walkSpeed;
		}

		// jumping
		if (!canJumpTimer.tick(delta) && controls.getKey(4).justPressed)
		{
			const float secretCrustyCrabFormula =
				(std::max(std::fabs(moveVector.x), walkSpeed) / maxRunSpeed) * maxJumpSpeed * 0.45f +
				maxJumpSpeed * 0.55;
			moveVector.y = -secretCrustyCrabFormula;
			canJumpTimer.block();
			bigJump = true;
		}
		if (onFloor)
			canJumpTimer.reset();
		if (bigJump && moveVector.y < 0 && controls.getKey(4).justReleased)
		{
			moveVector.y /= 2.f;
			bigJump = false;
		}

		this->HitboxEntity::process(delta);
	}

	void setCanJump(bool val)
	{
		if (val)
			canJumpTimer.reset();
		else
			canJumpTimer.block();
	}
	sf::Int64 getCanJump() const { return canJumpTimer.getTimeLeft(); }

private:
	Controls controls;

	Timer canJumpTimer = Timer(0.05f);

	const float maxJumpSpeed = 3.2f;
	bool bigJump             = false;

	const float walkSpeed       = 0.6f;
	const float maxRunSpeed     = 1.4f;
	const float acc             = 0.01f;
	const float deAcc           = 0.08f;
	const float turnAroundDeAcc = 0.06f;
};