#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

#include "KeyFrameAnimator.hpp"

class AnimatedSprite
{
public:
	enum class KeyType
	{
		RECT_X   = 0,
		RECT_Y   = 1,
		RECT_W   = 2,
		RECT_H   = 3,
		OFFSET_X = 4,
		OFFSET_Y = 5,
		ROTATION = 6,
		SCALE_X  = 7,
		SCALE_Y  = 8,
		ORIGIN_X = 9,
		ORIGIN_Y = 10,
	};

	explicit AnimatedSprite(const sf::Vector2f& position = sf::Vector2f(0.f, 0.f),
							const sf::Vector2f& offset   = sf::Vector2f(0.f, 0.f))
		: spriteOffset(offset)
	{
		setPosition(position);
	}
	AnimatedSprite(const sf::Vector2f& position, const sf::Vector2f& offset, const sf::Texture& texture,
				   const std::vector<std::pair<std::string, KeyFrameAnimator<KeyType>>>& animations)
	{
		AnimatedSprite(position, offset);
		setTexture(texture);
		for (const auto& anim : animations)
			addAnimation(anim.first, anim.second);
	}

	~AnimatedSprite() = default;

	const sf::Sprite& get() { return sprite; }

	void addAnimation(const std::string& name, const KeyFrameAnimator<KeyType>& animation)
	{
		animations[name] = animation;
		if (currentAnimation == "")
			currentAnimation = name;
	}
	bool setAnimation(const std::string& name, bool resetAnimation = true)
	{
		if (!animations.count(name))
			return false;

		currentAnimation = name;

		if (resetAnimation)
			resetCurrentAnimation(false);

		return false;
	}
	const std::string& getCurrentAnimation() { return currentAnimation; }

	void resetCurrentAnimation(bool soft = true)
	{
		_handle_animation_return_vector(animations[currentAnimation].reset(soft));
	}
	bool ended() { return animations[currentAnimation].ended(); }
	bool ended(const std::string& name)
	{
		if (!animations.count(name))
			return ended();

		return animations[name].ended();
	}

	void tick(int delta)
	{
		_handle_animation_return_vector(
			animations[currentAnimation].tick((int)((float)delta * animationSpeedMultiplier)));
	}

	void setTexture(const sf::Texture& val) { sprite.setTexture(val); }
	void setTextureRect(const sf::IntRect& val) { sprite.setTextureRect(val); }
	const sf::Sprite& getSprite() { return sprite; }

	void setPosition(const sf::Vector2f& val) { sprite.setPosition(val + spriteOffset); }
	void move(const sf::Vector2f& offset) { sprite.move(offset); }

	void setRotation(float val) { sprite.setRotation(val); }
	void rotate(float val) { sprite.rotate(val); }

	void setScale(const sf::Vector2f& val) { sprite.setScale(val); }
	void setOrigin(const sf::Vector2f& val) { sprite.setOrigin(val); }

	void setOffset(const sf::Vector2f& val)
	{
		spriteOffset = val;
		setPosition(sprite.getPosition());
	}
	const sf::Vector2f& getOffset() const { return spriteOffset; }

	float getAnimationSpeedMultiplier() const { return animationSpeedMultiplier; }
	void setAnimationSpeedMultiplier(float val) { animationSpeedMultiplier = val; }

private:
	sf::Sprite sprite;
	sf::Vector2f spriteOffset;

	std::map<std::string, KeyFrameAnimator<KeyType>, std::less<>> animations;
	std::string currentAnimation = "";

	float animationSpeedMultiplier = 1.f;

	void _handle_animation_return_vector(std::vector<std::pair<KeyType, float>> vector)
	{
		for (const auto& pair : vector)
		{
			switch (pair.first)
			{
				case KeyType::RECT_X:
					sprite.setTextureRect(sf::IntRect(pair.second, sprite.getTextureRect().top,
													  sprite.getTextureRect().width, sprite.getTextureRect().height));
					break;

				case KeyType::RECT_Y:
					sprite.setTextureRect(sf::IntRect(sprite.getTextureRect().left, pair.second,
													  sprite.getTextureRect().width, sprite.getTextureRect().height));
					break;

				case KeyType::RECT_W:
					sprite.setTextureRect(sf::IntRect(sprite.getTextureRect().left, sprite.getTextureRect().top,
													  pair.second, sprite.getTextureRect().height));
					break;

				case KeyType::RECT_H:
					sprite.setTextureRect(sf::IntRect(sprite.getTextureRect().left, sprite.getTextureRect().top,
													  sprite.getTextureRect().width, pair.second));
					break;

				case KeyType::OFFSET_X:
					spriteOffset.x = pair.second;
					break;

				case KeyType::OFFSET_Y:
					spriteOffset.y = pair.second;
					break;

				case KeyType::ROTATION:
					sprite.setRotation(pair.second);
					break;

				case KeyType::SCALE_X:
					sprite.setScale(pair.second, sprite.getScale().y);
					break;

				case KeyType::SCALE_Y:
					sprite.setScale(sprite.getScale().x, pair.second);
					break;

				case KeyType::ORIGIN_X:
					sprite.setOrigin(pair.second, sprite.getOrigin().y);
					break;

				case KeyType::ORIGIN_Y:
					sprite.setOrigin(sprite.getOrigin().x, pair.second);
					break;

				default:
					std::cerr << "Unexpected KeyType in instance of AnimatedSprite" << std::endl;
					break;
			}
		}
	}
};