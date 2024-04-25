#pragma once

#include <SFML/Graphics.hpp>

namespace sf
{
Vector2f operator/(const Vector2f& v, float scalar) { return {v.x / scalar, v.y / scalar}; }

Vector2f operator*(const Vector2f& v, float scalar) { return {v.x * scalar, v.y * scalar}; }

Vector2f operator*(float scalar, const Vector2f& v) { return {v.x * scalar, v.y * scalar}; }
}  // namespace sf

sf::Vector2f normalize(const sf::Vector2f& source)
{
	float length = sqrt((source.x * source.x) + (source.y * source.y));
	if (length != 0)
		return sf::Vector2f(source.x / length, source.y / length);
	else
		return source;
}

// Custom comparison function for sf::Vector2f
struct Vector2fCompare
{
	bool operator()(const sf::Vector2f& lhs, const sf::Vector2f& rhs) const
	{
		// Compare x values first
		if (lhs.x < rhs.x)
			return true;
		if (lhs.x > rhs.x)
			return false;

		// If x values are equal, compare y values
		return lhs.y < rhs.y;
	}
};

// Custom comparison function for sf::Vector2i
struct Vector2iCompare
{
	bool operator()(const sf::Vector2i& lhs, const sf::Vector2i& rhs) const
	{
		// Compare x values first
		if (lhs.x < rhs.x)
			return true;
		if (lhs.x > rhs.x)
			return false;

		// If x values are equal, compare y values
		return lhs.y < rhs.y;
	}
};