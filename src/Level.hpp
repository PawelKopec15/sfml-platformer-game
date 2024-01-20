#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "CollisionBody.hpp"
#include "Vector2fFunctions.hpp"
#include "XMLParser.hpp"

class Level
{
public:
	std::map<sf::Vector2f, std::vector<std::shared_ptr<CollisionBody>>, Vector2fCompare> Collision;

	Level(const std::string& levelPath, bool print = false)
	{
		if (print)
			std::cout << "Building level from " << levelPath << " ..." << std::endl;

		if (parser.parseFile(levelPath))
			buildLevel();
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
	XMLParser parser;
	bool parseError = false;

	int width;
	int height;
	int tileWidth   = 16;
	int tileHeight  = 16;
	int chunkWidth  = 16;
	int chunkHeight = 16;

	std::vector<int> stringToVector(const std::string& input)
	{
		std::vector<int> result;
		std::stringstream ss(input);

		int number;
		char comma;

		while (ss >> number)
		{
			result.push_back(number);
			ss >> comma;  // Consume the comma
		}

		return result;
	}

	void buildLevel()
	{
		for (const auto& attr : parser.getRoot().attributes)
		{
			if (attr.name == "width")
				width = std::stoi(attr.value);
			else if (attr.name == "height")
				height = std::stoi(attr.value);
			else if (attr.name == "tilewidth")
				tileWidth = std::stoi(attr.value);
			else if (attr.name == "tileheight")
				tileHeight = std::stoi(attr.value);
		}
		for (const auto& child : parser.getRoot().children)
		{
			if (child.name == "editorsettings")
				handleEditorSettings(child);
			if (child.name == "layer")
				handleLayer(child);
		}
	}

	void handleEditorSettings(const XMLElement& editorSettings)
	{
		for (const auto& child : editorSettings.children)
		{
			if (child.name == "chunksize")
			{
				for (const auto& attr : child.attributes)
				{
					if (attr.name == "width")
						chunkWidth = std::stoi(attr.value);
					else if (attr.name == "height")
						chunkHeight = std::stoi(attr.value);
				}
			}
		}
	}

	void handleLayer(const XMLElement& layer)
	{
		for (const auto& chunk : layer.children[0].children)
		{
			int x = 0;
			int y = 0;

			std::vector<std::vector<int>> vals = {};
			for (const auto& attr : chunk.attributes)
			{
				if (attr.name == "x")
					x = std::stoi(attr.value);
				else if (attr.name == "y")
					y = std::stoi(attr.value);
				else if (attr.name == "")
					vals.push_back(stringToVector(attr.value));
			}

			// TODO TESTS ONLY
			for (size_t i = 0; i < vals.size(); ++i)
			{
				for (size_t j = 0; j < vals[i].size(); ++j)
				{
					switch (vals[i][j])
					{
						case 1:
							Collision[sf::Vector2f(x, y)].push_back(std::make_shared<CollisionBody>(
								CollisionBody(sf::Vector2f((x + j) * tileWidth, (y + i) * tileHeight))));
							break;

						default:
							break;
					}
				}
			}
		}
	}
};