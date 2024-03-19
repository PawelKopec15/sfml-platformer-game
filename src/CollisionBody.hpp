#pragma once

#include "Area2D.hpp"

class CollisionBody : public Area2D
{
public:
	CollisionBody(const sf::Vector2f& position = sf::Vector2f(0, 0), const sf::Vector2f& size = sf::Vector2f(16, 16),
				  const sf::Color& color = sf::Color(200, 200, 200, 96))
		: Area2D(position, size, color){};
	~CollisionBody() = default;

	sf::Vector2f getOverlapVector(CollisionBody& other)
	{
		if (!intersects(other))
			return sf::Vector2f(0, 0);

		const auto selfBounds  = area.getGlobalBounds();
		const auto otherBounds = other.getRectangleShape().getGlobalBounds();

		float amount_h = std::min(selfBounds.left + selfBounds.width, otherBounds.left + otherBounds.width) -
						 std::max(selfBounds.left, otherBounds.left);
		float amount_v = std::min(selfBounds.top + selfBounds.height, otherBounds.top + otherBounds.height) -
						 std::max(selfBounds.top, otherBounds.top);

		return {amount_h, amount_v};
	}

	sf::Vector2f getOverlapVectorOriented(CollisionBody& other)
	{
		auto overlapVector = getOverlapVector(other);

		if (getCenter().x < other.getCenter().x)
			overlapVector.x *= -1.f;
		if (getCenter().y < other.getCenter().y)
			overlapVector.y *= -1.f;

		return overlapVector;
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
};