#pragma once

#include <SFML/Graphics.hpp>
#include <algorithm>

class CollisionBody
{
public:
	CollisionBody(const sf::Vector2f& position = sf::Vector2f(0, 0), const sf::Vector2f& size = sf::Vector2f(16, 16),
				  const sf::Color& color = sf::Color(200, 200, 200, 96))
	{
		collisionBox.setPosition(position);
		collisionBox.setSize(size);
		collisionBox.setFillColor(color);
		collisionBox.setOutlineColor(sf::Color(255, 255, 255, 96));
		collisionBox.setOutlineThickness(-0.5f);
	}
	~CollisionBody() = default;

	sf::RectangleShape getCollisionBox() { return collisionBox; }
	sf::Vector2f getCenter()
	{
		return collisionBox.getPosition() + sf::Vector2f(collisionBox.getSize().x / 2, collisionBox.getSize().y / 2);
	}

	void setPosition(const sf::Vector2f& position) { collisionBox.setPosition(position); }
	void setSize(const sf::Vector2f& size) { collisionBox.setSize(size); }
	void setColor(const sf::Color& color) { collisionBox.setFillColor(color); }

	void move(const sf::Vector2f& offset) { collisionBox.move(offset); }

	const sf::Vector2f& getCacheVector() const { return cacheVector; }
	void setCacheVector(const sf::Vector2f& val) { cacheVector = val; }

	bool intersects(CollisionBody& other)
	{
		return other.getCollisionBox().getGlobalBounds().intersects(collisionBox.getGlobalBounds());
	}

	sf::Vector2f getOverlapVector(CollisionBody& other)
	{
		if (!intersects(other))
			return sf::Vector2f(0, 0);

		const auto selfBounds  = collisionBox.getGlobalBounds();
		const auto otherBounds = other.getCollisionBox().getGlobalBounds();

		float amount_h = std::min(selfBounds.left + selfBounds.width, otherBounds.left + otherBounds.width) -
						 std::max(selfBounds.left, otherBounds.left);
		float amount_v = std::min(selfBounds.top + selfBounds.height, otherBounds.top + otherBounds.height) -
						 std::max(selfBounds.top, otherBounds.top);

		return sf::Vector2f(amount_h, amount_v);
	}

	sf::Vector2f getEjectionVector(CollisionBody& other, const sf::Vector2f& overlapVector = sf::Vector2f(0, 0))
	{
		sf::Vector2f ejectionVector(0, 0);

		sf::Vector2f amount;

		if (overlapVector == sf::Vector2f(0, 0))
			amount = getOverlapVector(other);
		else
			amount = overlapVector;

		if (amount.y <= amount.x)
		{
			if (getCenter().y < other.getCenter().y)
				ejectionVector.y = -amount.y;
			else
				ejectionVector.y = amount.y;
		}
		else
		{
			if (getCenter().x < other.getCenter().x)
				ejectionVector.x = -amount.x;
			else
				ejectionVector.x = amount.x;
		}

		// std::cout << "----- " << amount_h << " " << amount_v << std::endl;

		return ejectionVector;
	}

private:
	sf::RectangleShape collisionBox;
	sf::Vector2f cacheVector;
};