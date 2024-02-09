#pragma once

#include <SFML/Graphics.hpp>

#include "ColliderEntity.hpp"
#include "Timer.hpp"

class Controls
{
public:
	struct Key
	{
		sf::Keyboard::Key keyBind;
		bool isPressed    = false;
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

			if (now && !key.isPressed)
				key.justPressed = true;
			else
				key.justPressed = false;

			if (!now && key.isPressed)
				key.justReleased = true;
			else
				key.justReleased = false;

			key.isPressed = now;
		}
	}

	Key getKey(uint32_t index) { return arr[index]; }

private:
	void _updateKeyStatus()
	{
		for (auto&& key : arr)
		{
			bool now = sf::Keyboard::isKeyPressed(key.keyBind);

			if (now && !key.isPressed)
				key.justPressed = true;
			else
				key.justPressed = false;

			if (!now && key.isPressed)
				key.justReleased = true;
			else
				key.justReleased = false;

			key.isPressed = now;
		}
	}

	std::array<Key, 8> arr = {Key(sf::Keyboard::Up),    Key(sf::Keyboard::Down), Key(sf::Keyboard::Left),
							  Key(sf::Keyboard::Right), Key(sf::Keyboard::Z),    Key(sf::Keyboard::LShift),
							  Key(sf::Keyboard::Enter), Key(sf::Keyboard::Space)};
};

class Player : public ColliderEntity
{
public:
	Player(const sf::Vector2f& position, Controls controls)
		: ColliderEntity(position, sf::Vector2f(14.f, 14.f), sf::Vector2f(-7.f, -7.f)), controls(controls)
	{}
	~Player() override = default;

	void process(sf::Int64 delta) override
	{
		controls.update();

		float horizontalInput = (float)controls.getKey(3).isPressed - (float)controls.getKey(2).isPressed;
		const bool running    = controls.getKey(5).isPressed;

		lookDir = horizontalInput == 0.f ? lookDir : (int)horizontalInput;
		sprite.setScale({(float)lookDir, 1});

		// running
		if (running && horizontalInput != 0.f)
		{
			if (fabs(moveVector.x) < walkSpeed)
				moveVector.x = horizontalInput * walkSpeed;

			if ((moveVector.x > 0 && horizontalInput > 0) || (moveVector.x < 0 && horizontalInput < 0))
				moveVector.x += horizontalInput * acc * DELTA_CORRECTION;
			else
			{
				moveVector.x += horizontalInput * turnAroundDeAcc * DELTA_CORRECTION;
			}

			if (moveVector.x > maxRunSpeed)
				moveVector.x = maxRunSpeed;
			else if (moveVector.x < -maxRunSpeed)
				moveVector.x = -maxRunSpeed;
		}
		// walking
		else
		{
			if (std::fabs(moveVector.x) > walkSpeed)
				moveVector.x -= std::fabs(moveVector.x) / moveVector.x * deAcc * DELTA_CORRECTION;
			else
				moveVector.x = horizontalInput * walkSpeed;
		}

		// jumping
		coyoteTimer.tick(delta);
		if (getCanJump() && controls.getKey(4).justPressed)
		{
			const float secretCrustyCrabFormula =
				(std::max(std::fabs(moveVector.x), walkSpeed) / maxRunSpeed) * maxJumpSpeed * 0.4f +
				maxJumpSpeed * 0.65f;
			moveVector.y = -secretCrustyCrabFormula;
			setCanJump(false);
			bigJump = true;
		}
		if (onFloor)
			setCanJump(true);
		if (bigJump && moveVector.y < 0 && controls.getKey(4).justReleased)
		{
			moveVector.y /= 2.f;
			bigJump = false;
		}

		// animations

		sprite.setAnimationSpeedMultiplier(1.f);
		if (coyoteTimer.hasTimedOut())
		{
			if (moveVector.y <= 0.f)
				sprite.setAnimation("Jump");
			else
				sprite.setAnimation("Fall");
		}
		else
		{
			if (moveVector.x == 0.f)
				sprite.setAnimation("Stand");
			else if (horizontalInput == 0.f || (moveVector.x > 0.f && horizontalInput > 0.f) ||
					 (moveVector.x < 0.f && horizontalInput < 0.f))
			{
				if (onRightWall || onLeftWall)
					sprite.setAnimation("Push", sprite.getCurrentAnimation() != "Push");
				else
				{
					sprite.setAnimation("Run", sprite.getCurrentAnimation() != "Run");
					sprite.setAnimationSpeedMultiplier(1.f + 2.f * (std::fabs(moveVector.x) - walkSpeed));
				}
			}

			else
				sprite.setAnimation("Turn");
		}

		this->ColliderEntity::process(delta);
	}

	void setCanJump(bool val)
	{
		if (val)
			coyoteTimer.reset();
		else
			coyoteTimer.block();
	}
	// sf::Int64 getOnGroundTimerTimeLeft() const { return onGroundTimer.getTimeLeft(); }
	bool getCanJump() const { return !coyoteTimer.hasTimedOut(); }

private:
	Controls controls;

	Timer coyoteTimer = Timer(0.5f);

	int lookDir = 1;

	const float maxJumpSpeed = 3.f;
	bool bigJump             = false;

	const float walkSpeed       = 0.7f;
	const float maxRunSpeed     = 1.45f;
	const float acc             = 0.005f;
	const float deAcc           = 0.015f;
	const float turnAroundDeAcc = 0.03f;
};