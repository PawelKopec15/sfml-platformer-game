#pragma once

#include <SFML/Graphics.hpp>
#include <deque>
#include <memory>

#include "ColliderEntity.hpp"
#include "CollisionBody.hpp"
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

	CollisionResults StaticVectorDifferenceCollisionCheck(
		std::map<sf::Vector2f, std::vector<std::shared_ptr<CollisionBody>>, Vector2fCompare> &outCollision,
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

		outEntity.move(ejectionVector);

		CollisionResults toRet;
		checkCollisionResults(ejectionVector, toRet);
		return toRet;
	}

	CollisionResults StaticVectorDifferenceCollisionCheckForColliderEntity(
		std::map<sf::Vector2f, std::vector<std::shared_ptr<CollisionBody>>, Vector2fCompare> &outCollision,
		ColliderEntity &outEntity, const std::vector<sf::Vector2f> &chunks, bool debugPrint = false)
	{
		auto colRes = StaticVectorDifferenceCollisionCheck(outCollision, outEntity, chunks, debugPrint);

		handleCollisionResultsforColliderEntitySoft(outEntity, colRes);

		return colRes;
	}

	CollisionResults StaticTripleCollisionCheck(
		std::map<sf::Vector2f, std::vector<std::shared_ptr<CollisionBody>>, Vector2fCompare> &outCollision,
		ColliderEntity &outEntity, const std::vector<sf::Vector2f> &chunks, bool debugPrint = false)
	{
		auto chunksVector = getChunksVector(outCollision, chunks);

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

	CollisionResults StaticTripleCollisionForColliderEntity(
		std::map<sf::Vector2f, std::vector<std::shared_ptr<CollisionBody>>, Vector2fCompare> &outCollision,
		ColliderEntity &outEntity, const std::vector<sf::Vector2f> &chunks, bool debugPrint = false)
	{
		auto colRes =
			CollisionAlgorithms::Get().StaticTripleCollisionCheck(outCollision, outEntity, chunks, debugPrint);

		handleCollisionResultsforColliderEntitySoft(outEntity, colRes);

		return colRes;
	}

private:
	CollisionAlgorithms() = default;

	const float tccTolerance = 2.2f;

	std::vector<sf::Vector2f> getChunksVector(
		std::map<sf::Vector2f, std::vector<std::shared_ptr<CollisionBody>>, Vector2fCompare> &outCollision,
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

	void handleCollisionResultsforColliderEntity(ColliderEntity &outEntity, const CollisionResults &colRes)
	{
		if (colRes.shouldResetHor)
		{
			outEntity.setMoveVector(sf::Vector2f(0, outEntity.getMoveVector().y));
			outEntity.setOnWall(true);
		}
		else
			outEntity.setOnWall(false);

		if (colRes.shouldResetVer)
		{
			outEntity.setMoveVector(sf::Vector2f(outEntity.getMoveVector().x, 0));
			if (colRes.ejectedUp)
				outEntity.setOnFloor(true);
			if (colRes.ejectedDown)
				outEntity.setOnCeil(true);
		}
		else
		{
			outEntity.setOnFloor(false);
			outEntity.setOnCeil(false);
		}
	}

	void handleCollisionResultsforColliderEntitySoft(ColliderEntity &outEntity, const CollisionResults &colRes)
	{
		const float resetAmount = 0.05f;

		if (colRes.shouldResetHor && outEntity.getNextFrameResetHor())
		{
			outEntity.setMoveVector({outEntity.getMoveVector().x * resetAmount, outEntity.getMoveVector().y});
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
			outEntity.setMoveVector({outEntity.getMoveVector().x, outEntity.getMoveVector().y * resetAmount});
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
	}

	void checkCollisionResults(const sf::Vector2f &vec, CollisionResults &outCurrentResults)
	{
		const float threshold = 0.0001f;

		if (std::fabs(vec.x) > threshold)
		{
			outCurrentResults.shouldResetHor = true;
			if (vec.x > 0.f)
				outCurrentResults.ejectedRight = true;
			else
				outCurrentResults.ejectedLeft = true;
		}
		if (std::fabs(vec.y) > threshold)
		{
			outCurrentResults.shouldResetVer = true;
			if (vec.y > 0.f)
				outCurrentResults.ejectedDown = true;
			else
				outCurrentResults.ejectedUp = true;
		}
	}
};