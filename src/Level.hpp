#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "CollisionBody.hpp"
#include "TMXParser.hpp"
#include "Vector2fFunctions.hpp"

class Level
{
public:
	std::map<sf::Vector2f, std::vector<std::shared_ptr<CollisionBody>>, Vector2fCompare> Collision;

	Level()  = default;
	~Level() = default;

	bool create(const std::string& levelPath, bool print = false)
	{
		bool parseError = parser.parse(levelPath, print);

		_handleDebugCollisionLayer();

		camera.findCameraZones(parser.getMap());

		return parseError;
	}

	Camera& accessCamera() { return camera; }

private:
	TMXParser parser;
	Camera camera;

	void _handleDebugCollisionLayer()
	{
		TMXLayer collisionLayer;
		for (const auto& layer : parser.getMap().layers)
		{
			if (layer.second.name == "Collision")
				collisionLayer = layer.second;
		}

		const auto tileWidth  = parser.getMap().tileWidth;
		const auto tileHeight = parser.getMap().tileHeight;

		for (const auto& chunk : collisionLayer.chunks)
		{
			for (size_t i = 0; i < chunk.second.data.size(); ++i)
			{
				for (size_t j = 0; j < chunk.second.data[i].size(); ++j)
				{
					switch (chunk.second.data[i][j])
					{
						case 1:
							Collision[sf::Vector2f(chunk.first.first, chunk.first.second)].push_back(
								std::make_shared<CollisionBody>(CollisionBody(sf::Vector2f(
									(chunk.first.first + j) * tileWidth, (chunk.first.second + i) * tileHeight))));
							break;

						default:
							break;
					}
				}
			}
		}
	}
};