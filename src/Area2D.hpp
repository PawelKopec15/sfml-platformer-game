#pragma once

#include <SFML/Graphics.hpp>
#include <algorithm>

class Area2D
{
public:
	Area2D(const sf::Vector2f& position = sf::Vector2f(0, 0), const sf::Vector2f& size = sf::Vector2f(16, 16),
		   const sf::Color& color = sf::Color(0, 0, 200, 96))
	{
		area.setPosition(position);
		area.setSize(size);
		area.setFillColor(color);
		area.setOutlineColor(sf::Color(255, 255, 255, 96));
		area.setOutlineThickness(-0.5f);
	}
	~Area2D() = default;

	const sf::RectangleShape& getRectangleShape() { return area; }
	sf::Vector2f getCenter() { return area.getPosition() + sf::Vector2f(area.getSize().x / 2, area.getSize().y / 2); }

	void setPosition(const sf::Vector2f& position) { area.setPosition(position); }
	void setSize(const sf::Vector2f& size) { area.setSize(size); }
	void setColor(const sf::Color& color) { area.setFillColor(color); }

	const sf::Vector2f& getPosition() { return area.getPosition(); }
	const sf::Vector2f& getSize() { return area.getSize(); }
	const sf::Color& getColor() { return area.getFillColor(); }
	sf::FloatRect getRect() { return {area.getPosition(), area.getSize()}; }

	void move(const sf::Vector2f& offset) { area.move(offset); }

	bool intersects(Area2D& other)
	{
		return other.getRectangleShape().getGlobalBounds().intersects(area.getGlobalBounds());
	}

protected:
	sf::RectangleShape area;
};