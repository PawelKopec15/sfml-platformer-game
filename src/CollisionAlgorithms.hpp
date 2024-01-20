#pragma once

#include <SFML/Graphics.hpp>
#include <deque>
#include <memory>

#include "CollisionBody.hpp"
#include "HitboxEntity.hpp"
#include "Vector2fFunctions.hpp"

struct CollisionResults
{
	bool shouldResetHor = false;
	bool shouldResetVer = false;
	bool ejectedUp      = false;
	bool ejectedDown    = false;
	bool ejectedLeft    = false;
	bool ejectedRight   = false;
};

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

	CollisionResults StaticTripleCollisionCheck(
		std::map<sf::Vector2f, std::vector<std::shared_ptr<CollisionBody>>, Vector2fCompare> &outCollision,
		HitboxEntity &outEntity, const std::vector<sf::Vector2f> chunks, bool debugPrint = false)
	{
		auto chunksVector = chunks;

		// If no chunk coordinates are specified, everything goes
		if (chunks.empty())
		{
			for (const auto &pair : outCollision)
				chunksVector.push_back(pair.first);
		}

		std::deque<std::shared_ptr<CollisionBody>> collisionBodies;
		for (const auto &coord : chunksVector)
		{
			for (auto &cb : outCollision[coord])
			{
				if (cb->intersects(outEntity.accessCollider()))
				{
					cb->setColor(sf::Color(120, 180, 120, 96));
					cb->setCacheVector(sf::Vector2f(0, 0));
					collisionBodies.push_back(cb);
				}
				else
					cb->setColor(sf::Color(180, 180, 180, 96));
			}
		}

		CollisionResults toRet;

		for (int i = 0; i < 3; ++i)
		{
			for (auto it = collisionBodies.begin(); it != collisionBodies.end(); ++it)
			{
				if ((*it)->intersects(outEntity.accessCollider()))
				{
					auto overlapVec = outEntity.accessCollider().getOverlapVector(*(*it));

					auto check = std::fabs(overlapVec.x) / std::fabs(overlapVec.y);

					if (check > tccTolerance || check < 1.f / tccTolerance || overlapVec == (*it)->getCacheVector() ||
						i > 1)
					{
						(*it)->setColor(sf::Color(0, 200, i * 120, 96));

						auto curVec = outEntity.accessCollider().getEjectionVector(*(*it), overlapVec);
						outEntity.move(curVec);

						checkCollisionResults(curVec, toRet);

						(*it)->setCacheVector(sf::Vector2f(0, 0));

						if (debugPrint && i > 0)
							std::cout << "-" << i + 1 << " YES\tcache: " << std::fixed << std::setprecision(4)
									  << (*it)->getCacheVector().x << " \t" << (*it)->getCacheVector().y
									  << " \tejection: " << -curVec.x << " \t" << -curVec.y << std::endl;
					}
					else
					{
						(*it)->setCacheVector(overlapVec);
						if (debugPrint && i > 0)
							std::cout << "-" << i + 1 << " NO\tcache: " << std::fixed << std::setprecision(4)
									  << (*it)->getCacheVector().x << " \t" << (*it)->getCacheVector().y << std::endl;
					}
				}
			}
		}

		return toRet;
	}

	CollisionResults StaticTripleCollisionForHitboxEntity(
		std::map<sf::Vector2f, std::vector<std::shared_ptr<CollisionBody>>, Vector2fCompare> &outCollision,
		HitboxEntity &outEntity, const std::vector<sf::Vector2f> chunks, bool debugPrint = false)
	{
		auto colRes =
			CollisionAlgorithms::Get().StaticTripleCollisionCheck(outCollision, outEntity, chunks, debugPrint);

		if (colRes.shouldResetHor && outEntity.getNextFrameResetHor())
		{
			outEntity.setMoveVector(sf::Vector2f(0, outEntity.getMoveVector().y));
			outEntity.setOnWall(true);
		}
		else if (colRes.shouldResetHor)
			outEntity.setNextFrameResetHor(true);
		else if (outEntity.getNextFrameResetHor())
		{
			outEntity.setNextFrameResetHor(false);
			outEntity.setOnWall(false);
		}

		if (colRes.shouldResetVer && outEntity.getNextFrameResetVer())
		{
			outEntity.setMoveVector(sf::Vector2f(outEntity.getMoveVector().x, 0));
			if (colRes.ejectedUp)
				outEntity.setOnFloor(true);
			if (colRes.ejectedDown)
				outEntity.setOnCeil(true);
		}
		else if (colRes.shouldResetVer)
			outEntity.setNextFrameResetVer(true);
		else if (outEntity.getNextFrameResetVer())
		{
			outEntity.setNextFrameResetVer(false);
			outEntity.setOnFloor(false);
			outEntity.setOnCeil(false);
		}

		return colRes;
	}

private:
	CollisionAlgorithms() = default;

	const float tccTolerance = 1.8f;

	void checkCollisionResults(const sf::Vector2f &vec, CollisionResults &outCurrentResults)
	{
		if (std::fabs(vec.x) > 0.1)
		{
			outCurrentResults.shouldResetHor = true;
			if (vec.x > 0)
				outCurrentResults.ejectedRight = true;
			else
				outCurrentResults.ejectedLeft = true;
		}
		if (std::fabs(vec.y) > 0.1)
		{
			outCurrentResults.shouldResetVer = true;
			if (vec.y > 0)
				outCurrentResults.ejectedDown = true;
			else
				outCurrentResults.ejectedUp = true;
		}
	}
};