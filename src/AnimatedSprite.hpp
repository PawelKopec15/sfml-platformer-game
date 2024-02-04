#pragma once

#include <SFML/Graphics.hpp>

class AnimatedSprite
{
public:
	AnimatedSprite()  = default;
	~AnimatedSprite() = default;

	void setTexture(const sf::Texture& val) { sprite.setTexture(val); }
	const sf::Sprite& getSprite() { return sprite; }

	void setPosition(const sf::Vector2f& val) { sprite.setPosition(val); }
	void move(const sf::Vector2f& offset) { sprite.move(offset); }

	void setRotation(float val) { sprite.setRotation(val); }
	void rotate(float val) { sprite.rotate(val); }

	void setScale(const sf::Vector2f& val) { sprite.setScale(val); }
	void setOrigin(const sf::Vector2f& val) { sprite.setOrigin(val); }

private:
	sf::Sprite sprite;
};