#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "CollisionBody.hpp"
#include "StaticTile.hpp"
#include "TMXParser.hpp"
#include "Vector2fFunctions.hpp"

class Level
{
public:
	std::map<sf::Vector2f, std::vector<std::shared_ptr<StaticTile>>, Vector2fCompare> Collision;

	Level()  = default;
	~Level() = default;

	bool create(const std::string& levelPath, bool print = false)
	{
		bool parseError = parser.parse(levelPath, print);

		_handleLayers();

		camera.findCameraZones(parser.getMap());

		return parseError;
	}

	Camera& accessCamera() { return camera; }

	const sf::Color& getBackgroundColor() { return parser.getMap().bgColor; }

private:
	TMXParser parser;
	Camera camera;

	void _handleCollisionLayer(const TMXLayer& collisionLayer)
	{
		const auto tileWidth  = parser.getMap().tileWidth;
		const auto tileHeight = parser.getMap().tileHeight;

		for (const auto& chunk : collisionLayer.chunks)
		{
			for (size_t i = 0; i < chunk.second.data.size(); ++i)
			{
				for (size_t j = 0; j < chunk.second.data[i].size(); ++j)
				{
					const int data = chunk.second.data[i][j];

					if (data <= 0)
						continue;

					Collision[sf::Vector2f(chunk.first.first, chunk.first.second)].push_back(
						std::make_shared<StaticTile>(StaticTile(
							sf::Vector2f((chunk.first.first + j) * tileWidth, (chunk.first.second + i) * tileHeight),
							{(float)tileWidth, (float)tileHeight}, data - 1)));
				}
			}
		}
	}

	void _handleLayers()
	{
		for (const auto& layer : parser.getMap().layers)
		{
			if (layer.second.name == "Collision")
				_handleCollisionLayer(layer.second);
		}
	}
};