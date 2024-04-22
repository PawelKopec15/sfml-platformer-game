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
	explicit ChunkMap(const sf::Vector2f& chunkSize = sf::Vector2f(16, 16)) : chunkSize(chunkSize){};
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
		auto chunks = findUnderlyingChunks(valBounds);

		// Insert p into relevant chunks
		for (const auto& chunk : chunks)
		{
			chunkMap[chunk].push_back(p);
		}
	}

	std::set<sf::Vector2i, Vector2iCompare> findUnderlyingChunks(const sf::FloatRect& rect)
	{
		return findUnderlyingChunks(rect, chunkSize);
	}

	static std::set<sf::Vector2i, Vector2iCompare> findUnderlyingChunks(const sf::FloatRect& _rect,
																		const sf::Vector2f& _chunkSize) noexcept
	{
		// Find the corners of the bounding box
		sf::Vector2f topLeft(_rect.left, _rect.top);
		sf::Vector2f topRight(_rect.left + _rect.width, _rect.top);
		sf::Vector2f bottomLeft(_rect.left, _rect.top + _rect.height);
		sf::Vector2f bottomRight(_rect.left + _rect.width, _rect.top + _rect.height);

		// Find the chunks each corner resides in
		sf::Vector2i topLeftChunk =
			sf::Vector2i(static_cast<int>(topLeft.x / _chunkSize.x), static_cast<int>(topLeft.y / _chunkSize.y));
		sf::Vector2i topRightChunk =
			sf::Vector2i(static_cast<int>(topRight.x / _chunkSize.x), static_cast<int>(topRight.y / _chunkSize.y));
		sf::Vector2i bottomLeftChunk =
			sf::Vector2i(static_cast<int>(bottomLeft.x / _chunkSize.x), static_cast<int>(bottomLeft.y / _chunkSize.y));

		//? sf::Vector2i bottomRightChunk = sf::Vector2i(static_cast<int>(bottomRight.x / _chunkSize.x),
		//? 											 static_cast<int>(bottomRight.y / _chunkSize.y));

		// Find all chunks that the bounding box spans
		std::set<sf::Vector2i, Vector2iCompare> chunks;
		for (int x = topLeftChunk.x; x <= topRightChunk.x; ++x)
		{
			for (int y = topLeftChunk.y; y <= bottomLeftChunk.y; ++y)
			{
				chunks.insert(sf::Vector2i(x, y));
			}
		}

		return chunks;
	}

	std::set<std::shared_ptr<T>> gatherFromChunks()
	{
		auto temp = std::vector<sf::Vector2i>();
		return _gatherFromChunks(temp);
	}
	std::set<std::shared_ptr<T>> gatherFromChunks(const std::vector<sf::Vector2i>& targets)
	{
		return _gatherFromChunks(targets);
	}
	std::set<std::shared_ptr<T>> gatherFromChunks(const std::set<sf::Vector2i>& targets)
	{
		return _gatherFromChunks(targets);
	}
	std::set<std::shared_ptr<T>> gatherFromChunks(const std::set<sf::Vector2i, Vector2iCompare>& targets)
	{
		return _gatherFromChunks(targets);
	}

	sf::Vector2f getChunkSize() const { return chunkSize; }
	void setChunkSize(const sf::Vector2f& val) { chunkSize = val; }

private:
	std::map<sf::Vector2i, std::vector<std::shared_ptr<T>>, Vector2iCompare> chunkMap;
	sf::Vector2f chunkSize;

	template <typename Container>
	std::set<std::shared_ptr<T>> _gatherFromChunks(const Container& targets)
	{
		auto toRet = std::set<std::shared_ptr<T>>();

		// If no chunks are specified, everything goes
		if (targets.empty())
		{
			for (const auto& pair : chunkMap)
			{
				for (const auto& t : chunkMap[pair.first])
					toRet.insert(t);
			}
		}
		else
		{
			for (const auto& chunk : targets)
			{
				for (const auto& t : chunkMap[chunk])
					toRet.insert(t);
			}
		}

		return toRet;
	}
};
