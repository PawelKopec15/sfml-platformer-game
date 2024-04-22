#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "ChunkMap.hpp"
#include "CollisionBody.hpp"
#include "StaticTile.hpp"
#include "TMXParser.hpp"

class Level
{
public:
	ChunkMap<StaticTile> Collision  = ChunkMap<StaticTile>();
	ChunkMap<StaticTile> Background = ChunkMap<StaticTile>();
	ChunkMap<StaticTile> Foreground = ChunkMap<StaticTile>();

	Level()  = default;
	~Level() = default;

	bool create(const std::string& levelPath, bool print = false)
	{
		bool parseError = parser.parse(levelPath, print);

		_handleTileLayers();

		camera.findCameraZones(parser.getMap());

		return parseError;
	}

	Camera& accessCamera() { return camera; }

	const sf::Color& getBackgroundColor() { return parser.getMap().bgColor; }

private:
	TMXParser parser;
	Camera camera;

	ChunkMap<StaticTile> _parseTileLayer(const TMXLayer& layer)
	{
		const auto tileWidth  = parser.getMap().tileWidth;
		const auto tileHeight = parser.getMap().tileHeight;

		const auto chunkWidth  = parser.getMap().editorSettings.chunkWidth;
		const auto chunkHeight = parser.getMap().editorSettings.chunkHeight;

		auto toRet = ChunkMap<StaticTile>(sf::Vector2f(chunkWidth, chunkHeight));

		for (const auto& chunk : layer.chunks)
		{
			for (size_t i = 0; i < chunk.second.data.size(); ++i)
			{
				for (size_t j = 0; j < chunk.second.data[i].size(); ++j)
				{
					const int data = chunk.second.data[i][j];

					if (data <= 0)
						continue;

					toRet.insertNewValue(sf::Vector2i(chunk.first.first, chunk.first.second),
										 StaticTile(sf::Vector2f((chunk.first.first + j) * tileWidth,
																 (chunk.first.second + i) * tileHeight),
													{(float)tileWidth, (float)tileHeight}, data - 1));
				}
			}
		}

		return toRet;
	}

	void _handleTileLayers()
	{
		for (const auto& layer : parser.getMap().layers)
		{
			if (layer.second.name == "Collision")
				Collision = _parseTileLayer(layer.second);
			else if (layer.second.name == "Background")
				Background = _parseTileLayer(layer.second);
			else if (layer.second.name == "Foreground")
				Foreground = _parseTileLayer(layer.second);
		}
	}
};