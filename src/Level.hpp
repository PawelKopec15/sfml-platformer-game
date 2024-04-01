#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "CollisionBody.hpp"
#include "TMXParser.hpp"
#include "TerrainBody.hpp"
#include "Vector2fFunctions.hpp"

class Level
{
public:
	std::map<sf::Vector2f, std::vector<std::shared_ptr<CollisionBody>>, Vector2fCompare> Collision;
	std::map<sf::Vector2f, std::vector<std::shared_ptr<TerrainBody>>, Vector2fCompare> Terrain;

	Level()  = default;
	~Level() = default;

	bool create(const std::string& levelPath, bool print = false)
	{
		bool parseError = parser.parse(levelPath, print);

		_handleLayers();
		_handleObjectGroups();

		camera.findCameraZones(parser.getMap());

		return parseError;
	}

	Camera& accessCamera() { return camera; }

private:
	TMXParser parser;
	Camera camera;

	void _handleTerrainObjects(std::map<int, TMXObject> objects)
	{
		for (auto& object : objects)
		{
			if (object.second.type != "Terrain")
				continue;

			auto rect = sf::FloatRect(object.second.x, object.second.y, object.second.width, object.second.height);

			auto props = TerrainBody::TerrainProperties();

			props.noBottom = object.second.properties["NoBottom"].value == "true";
			props.noLeft   = object.second.properties["NoLeft"].value == "true";
			props.noRight  = object.second.properties["NoRight"].value == "true";
			props.noTop    = object.second.properties["NoTop"].value == "true";
		}
	}

	void _handleDebugCollisionLayer(const TMXLayer& collisionLayer)
	{
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

	void _handleLayers()
	{
		for (const auto& layer : parser.getMap().layers)
		{
			if (layer.second.name == "Collision")
				_handleDebugCollisionLayer(layer.second);
		}
	}

	void _handleObjectGroups()
	{
		for (const auto& objectGroup : parser.getMap().objectGroups)
		{
			if (objectGroup.second.name == "Terrain")
				_handleTerrainObjects(objectGroup.second.objects);
		}
	}
};