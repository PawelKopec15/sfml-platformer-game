#pragma once

#include <SFML/Graphics.hpp>
#include <deque>
#include <memory>

#include "ColliderEntity.hpp"
#include "CollisionBody.hpp"
#include "StaticTile.hpp"
#include "Vector2fFunctions.hpp"

class CollisionAlgorithms
{
public:
	static CollisionAlgorithms &Get()
	{
		static CollisionAlgorithms INSTANCE;
		return INSTANCE;
	}
	CollisionAlgorithms(CollisionAlgorithms &&)      = delete;
	CollisionAlgorithms(const CollisionAlgorithms &) = delete;
	CollisionAlgorithms &operator=(CollisionAlgorithms &&) = delete;
	CollisionAlgorithms &operator=(const CollisionAlgorithms &) = delete;

	sf::Vector2f AABBWithStaticBodiesCollisionCheck(
		std::map<sf::Vector2f, std::vector<std::shared_ptr<StaticTile>>, Vector2fCompare> &outCollision,
		ColliderEntity &outEntity, const std::vector<sf::Vector2f> &chunks, bool debugPrint = false)
	{
		auto chunksVector = getChunksVector(outCollision, chunks);

		std::vector<sf::Vector2f> orientedOverlapVectors;

		for (const auto &coord : chunksVector)
		{
			for (auto &&cb : outCollision[coord])
			{
				if (cb->intersects(outEntity.accessCollider()))
				{
					cb->setColor(sf::Color(120, 180, 120, 96));
					orientedOverlapVectors.push_back(outEntity.accessCollider().getOverlapVectorOriented(*cb));
				}
				else
					cb->setColor(sf::Color(180, 180, 180, 96));
			}
		}

		for (auto &first : orientedOverlapVectors)
		{
			for (auto &second : orientedOverlapVectors)
			{
				if (first == second)
					continue;

				if (first.x * second.x < 0.f)
				{
					first.x  = 0.f;
					second.x = 0.f;
				}
				if (first.y * second.y < 0.f)
				{
					first.y  = 0.f;
					second.y = 0.f;
				}
			}
		}

		sf::Vector2f maxVec(0.f, 0.f);

		for (const auto &vec : orientedOverlapVectors)
		{
			maxVec.x = std::fabs(vec.x) > std::fabs(maxVec.x) ? vec.x : maxVec.x;
			maxVec.y = std::fabs(vec.y) > std::fabs(maxVec.y) ? vec.y : maxVec.y;
		}

		sf::Vector2f ejectionVector(0.f, 0.f);

		if (orientedOverlapVectors.size() == 1)
		{
			if (std::fabs(maxVec.y) <= std::fabs(maxVec.x))
				ejectionVector.y = maxVec.y;
			else
				ejectionVector.x = maxVec.x;
		}
		else
			ejectionVector = maxVec;

		return ejectionVector;
	}

private:
	CollisionAlgorithms() = default;

	const float tccTolerance = 2.2f;

	std::vector<sf::Vector2f> getChunksVector(
		std::map<sf::Vector2f, std::vector<std::shared_ptr<StaticTile>>, Vector2fCompare> &outCollision,
		const std::vector<sf::Vector2f> &chunks)
	{
		auto chunksVector = chunks;

		// If no chunk coordinates are specified, everything goes
		if (chunks.empty())
		{
			for (const auto &pair : outCollision)
				chunksVector.push_back(pair.first);
		}

		return chunksVector;
	}
};