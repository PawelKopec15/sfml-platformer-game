#pragma once

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "CollisionBody.hpp"
#include "TMXParser.hpp"
#include "Vector2fFunctions.hpp"

class Level
{
public:
	std::map<sf::Vector2f, std::vector<CollisionBody>, Vector2fCompare> Collision;

	Level(const std::string& levelPath, bool print = false)
	{
		if (print)
			std::cout << "Building level from " << levelPath << " ..." << std::endl;

		TMXParser parser;
		if (parser.parseFile(levelPath))
			buildLevel(parser);
		else
			parseError = true;

		if (print)
		{
			std::cout << "Building done. Printing parser data:\n" << std::endl;
			parser.printParsedData();
		}
	}

	~Level() = default;

private:
	bool parseError = false;

	int width;
	int height;
	int tileWidth  = 16;
	int tileHeight = 16;

	void buildLevel(TMXParser& parser)
	{
		width      = parser.getMap().width;
		height     = parser.getMap().height;
		tileWidth  = parser.getMap().tileWidth;
		tileHeight = parser.getMap().tileHeight;

		for (const auto& layer : parser.getMap().layers)
			handleLayer(layer);
	}

	void handleLayer(const Layer& layer)
	{
		if (layer.name == "Collision")
		{
			for (auto&& c : layer.chunks)
			{
				for (size_t i = 0; i < c.height; ++i)
				{
					std::istringstream ss(c.data[i]);

					// Vector to store the integers
					std::vector<int> intVector;

					// Temporary variable to store each integer while parsing
					std::string token;

					// Parse the string using getline and ',' as the delimiter
					while (std::getline(ss, token, ','))
					{
						// Convert the string to an integer and push it into the vector
						int intValue = std::stoi(token);
						intVector.push_back(intValue);
					}

					for (size_t j = 0; j < c.width; ++j)
					{
						switch (intVector[j])
						{
							case 1:
								Collision[sf::Vector2f(c.x, c.y)].push_back(
									CollisionBody(sf::Vector2f((c.x + j) * tileWidth, (c.y + i) * tileHeight)));
								break;

							default:
								break;
						}
					}
				}
			}
		}
	}
};