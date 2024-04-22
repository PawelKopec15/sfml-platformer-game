#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <memory>
#include <vector>

#include "Vector2fFunctions.hpp"

template <typename T>
class ChunkMap
{
public:
	explicit ChunkMap(const sf::Vector2f& chunkSize) : chunkSize(chunkSize){};
	~ChunkMap() = default;

	// Overload [] operator
	std::vector<std::shared_ptr<T>>& operator[](const sf::Vector2i& index) { return chunkMap[index]; }

	void insertNewValue(const sf::Vector2i& chunk, const T& val)
	{
		chunkMap[chunk].push_back(std::make_shared<T>(val));
	}
	void insertNewValuePointer(const sf::Vector2i& chunk, const std::shared_ptr<T>& p) { chunkMap[chunk].push_back(p); }
	void insertAuto(const sf::FloatRect& valBounds, const std::shared_ptr<T>& p)
	{
		// Empty for now
	}

	std::set<std::shared_ptr<T>> gatherFromChunks(const std::vector<sf::Vector2i>& targets = {})
	{
		auto toRet = std::set<std::shared_ptr<T>>();

		auto targetList = targets;
		if (targetList.empty())
		{
			for (const auto pair : chunkMap)
				targetList.push_back(pair.first);
		}

		for (const auto chunk : targetList)
		{
			for (const auto t : chunkMap[chunk])
				toRet.insert(t);
		}

		return toRet;
	}

	sf::Vector2f getChunkSize() const { return chunkSize; }
	void setChunkSize(const sf::Vector2f& chunkSize_) { chunkSize = chunkSize_; }

private:
	std::map<sf::Vector2i, std::vector<std::shared_ptr<T>>, Vector2iCompare> chunkMap;
	sf::Vector2f chunkSize;
};
