#pragma once
#include "CollisionBody.hpp"

class StaticTile : public CollisionBody
{
public:
	StaticTile(const sf::Vector2f& position = sf::Vector2f(0, 0), const sf::Vector2f& size = sf::Vector2f(16, 16),
			   uint32_t tileId = 0)
		: CollisionBody(position, size, sf::Color(200, 200, 200, 96)), tileId(tileId){};
	~StaticTile() = default;

	uint32_t getTileId() const { return tileId; }

private:
	uint32_t tileId;
};