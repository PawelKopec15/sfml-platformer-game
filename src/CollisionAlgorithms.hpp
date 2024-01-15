#pragma once

#include <SFML/Graphics.hpp>

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
		std::map<sf::Vector2f, std::vector<CollisionBody>, Vector2fCompare> &outCollision, HitboxEntity &outEntity,
		const std::vector<sf::Vector2f> chunks, bool debugPrint = false)
	{
		auto chunksVector = chunks;

		// If no chunk coordinates are specified, everything goes
		if (chunks.empty())
		{
			for (const auto &pair : outCollision)
				chunksVector.push_back(pair.first);
		}

		bool secondRound = false;
		bool thirdRound  = false;

		CollisionResults toRet;

		for (const auto &coord : chunksVector)
		{
			auto &&vector = outCollision[coord];

			if (_tccRound1(outEntity, vector, toRet))
				secondRound = true;
		}
		if (secondRound)
			for (const auto &coord : chunksVector)
			{
				auto &&vector = outCollision[coord];
				if (_tccRound2(outEntity, vector, toRet, debugPrint))
					thirdRound = true;
			}
		if (thirdRound)
			for (const auto &coord : chunksVector)
			{
				auto &&vector = outCollision[coord];
				_tccRound3(outEntity, vector, debugPrint);
			}

		return toRet;
	}

	CollisionResults StaticTripleCollisionForHitboxEntity(
		std::map<sf::Vector2f, std::vector<CollisionBody>, Vector2fCompare> &outCollision, HitboxEntity &outEntity,
		const std::vector<sf::Vector2f> chunks, bool debugPrint = false)
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

	const float tccTolerance = 2.f;

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

	bool _tccRound1(HitboxEntity &outEntity, std::vector<CollisionBody> &outVector, CollisionResults &outCurrentResults)
	{
		bool toRet = false;
		for (auto &&cb : outVector)
		{
			cb.setCacheVector(sf::Vector2f(0, 0));

			if (cb.collidesWith(outEntity.accessCollider()))
			{
				cb.setColor(sf::Color(0, 200, 0, 128));

				auto overlapVec = outEntity.accessCollider().getOverlapVector(cb);

				auto check = std::fabs(overlapVec.x) / std::fabs(overlapVec.y);
				if (check > tccTolerance || check < 1.f / tccTolerance)
				{
					auto curVec = outEntity.accessCollider().getEjectionVector(cb, overlapVec);
					outEntity.move(curVec);

					checkCollisionResults(curVec, outCurrentResults);
				}
				else
				{
					cb.setCacheVector(overlapVec);
					toRet = true;
				}
			}
			else
				cb.setColor(sf::Color(180, 180, 180, 128));
		}
		return toRet;
	}

	bool _tccRound2(HitboxEntity &outEntity, std::vector<CollisionBody> &outVector, CollisionResults &outCurrentResults,
					bool debugPrint)
	{
		bool toRet = false;
		for (auto &&cb : outVector)
		{
			if (cb.getCacheVector() != sf::Vector2f(0, 0) && cb.collidesWith(outEntity.accessCollider()))
			{
				cb.setColor(sf::Color(0, 200, 200, 128));

				auto overlapVec = outEntity.accessCollider().getOverlapVector(cb);
				if (overlapVec == cb.getCacheVector())
				{
					auto curVec = outEntity.accessCollider().getEjectionVector(cb, overlapVec);

					if (debugPrint)
						std::cout << "-2\tcache: " << std::fixed << std::setprecision(4) << cb.getCacheVector().x
								  << " \t" << cb.getCacheVector().y << " \tejection: " << -curVec.x << " \t"
								  << -curVec.y << std::endl;

					outEntity.move(curVec);

					checkCollisionResults(curVec, outCurrentResults);

					cb.setCacheVector(sf::Vector2f(0, 0));
				}
				else
				{
					cb.setCacheVector(overlapVec);
					toRet = true;
				}
			}
		}
		return toRet;
	}

	void _tccRound3(HitboxEntity &outEntity, std::vector<CollisionBody> &outVector, bool debugPrint)
	{
		for (auto &&cb : outVector)
		{
			if (cb.getCacheVector() != sf::Vector2f(0, 0) && cb.collidesWith(outEntity.accessCollider()))
			{
				cb.setColor(sf::Color(0, 255, 255, 128));

				auto overlapVec = outEntity.accessCollider().getOverlapVector(cb);

				auto curVec = outEntity.accessCollider().getEjectionVector(cb, overlapVec);

				if (debugPrint)
					std::cout << "---3!!\tcache: " << std::fixed << std::setprecision(4) << cb.getCacheVector().x
							  << " \t" << cb.getCacheVector().y << " \tejection:\t" << -curVec.x << " \t" << -curVec.y
							  << std::endl;

				outEntity.move(curVec);
			}
		}
	}
};