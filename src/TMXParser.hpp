#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

struct Chunk
{
	std::vector<std::string> data;
	int x;
	int y;
	int width;
	int height;
};

struct Layer
{
	std::vector<Chunk> chunks;
	std::string name;
	int id;
	int width;
	int height;
};

struct Map
{
	std::vector<Layer> layers;
	int width;
	int height;
	int tileWidth;
	int tileHeight;
};

class TMXParser
{
public:
	TMXParser() = default;

	bool parseFile(const std::string& filename)
	{
		std::ifstream file(filename);
		if (!file.is_open())
		{
			std::cerr << "Error opening file: " << filename << std::endl;
			return false;
		}

		std::stringstream buffer;
		buffer << file.rdbuf();
		file.close();

		return parse(buffer.str());
	}

	void printParsedData() const
	{
		std::cout << "MAP\nwidth: " << map.width << "\theight: " << map.height << "\ttileWidth: " << map.tileWidth
				  << "\ttileHeight: " << map.tileHeight << std::endl;
		for (auto&& l : map.layers)
		{
			std::cout << "\nLAYER " << l.id << " \"" << l.name << "\"\nwidth: " << l.width << "\theight: " << l.height
					  << std::endl;

			for (auto&& c : l.chunks)
			{
				std::cout << "chunk x:" << c.x << "\ty:" << c.y << "\twidth:" << c.width << "\theight:" << c.height
						  << std::endl;
				for (auto&& line : c.data)
					std::cout << line << std::endl;
			}
		}
	}

	const Map& getMap() const { return map; }

private:
	enum class CurrentlyAt
	{
		NONE  = 0,
		MAP   = 1,
		LAYER = 2,
		CHUNK = 3,
	};

	Map map;
	CurrentlyAt currentlyAt = CurrentlyAt::NONE;

	bool parse(const std::string& xmlString)
	{
		std::istringstream xmlStream(xmlString);

		Layer currentLayer;
		Chunk currentChunk;

		std::string line;
		while (std::getline(xmlStream, line))
		{
			processLine(line, currentLayer, currentChunk);
		}

		return true;
	}

	void processLine(const std::string& line, Layer& currentLayer, Chunk& currentChunk)
	{
		// Use std::istringstream to split the line into words
		std::istringstream iss(line);
		std::vector<std::string> words;

		// Iterate through the words and add them to the vector
		{
			std::string word;
			while (iss >> word)
			{
				words.push_back(word);
			}
		}

		for (auto&& word : words)
		{
			const std::string charactersToRemove = "\">";

			// Use the erase-remove idiom to remove characters from the string
			word.erase(std::remove_if(
						   word.begin(), word.end(),
						   [&charactersToRemove](char c) { return charactersToRemove.find(c) != std::string::npos; }),
					   word.end());

			switch (currentlyAt)
			{
				case CurrentlyAt::NONE:
					if (word == "<map")
						currentlyAt = CurrentlyAt::MAP;
					break;

				case CurrentlyAt::MAP:
					if (word == "<layer")
					{
						Layer newLayer;
						currentLayer = newLayer;
						currentlyAt  = CurrentlyAt::LAYER;
					}
					else if (word.substr(0, 6) == "width=")
						map.width = stoi(word.substr(6, word.length() - 6));
					else if (word.substr(0, 7) == "height=")
						map.height = stoi(word.substr(7, word.length() - 7));
					else if (word.substr(0, 10) == "tilewidth=")
						map.tileWidth = stoi(word.substr(10, word.length() - 10));
					else if (word.substr(0, 11) == "tileheight=")
						map.tileHeight = stoi(word.substr(11, word.length() - 11));
					break;

				case CurrentlyAt::LAYER:
					if (word == "<chunk")
					{
						Chunk newChunk;
						currentChunk = newChunk;
						currentlyAt  = CurrentlyAt::CHUNK;
					}
					else if (word == "</layer")
					{
						map.layers.push_back(currentLayer);
						currentlyAt = CurrentlyAt::MAP;
					}
					else if (word.substr(0, 3) == "id=")
						currentLayer.id = stoi(word.substr(3, word.length() - 3));
					else if (word.substr(0, 5) == "name=")
						currentLayer.name = word.substr(5, word.length() - 5);
					else if (word.substr(0, 6) == "width=")
						currentLayer.width = stoi(word.substr(6, word.length() - 6));
					else if (word.substr(0, 7) == "height=")
						currentLayer.height = stoi(word.substr(7, word.length() - 7));

					break;

				case CurrentlyAt::CHUNK:
					if (word == "</chunk")
					{
						currentLayer.chunks.push_back(currentChunk);
						currentlyAt = CurrentlyAt::LAYER;
					}
					else if (word.substr(0, 2) == "x=")
						currentChunk.x = stoi(word.substr(2, word.length() - 2));
					else if (word.substr(0, 2) == "y=")
						currentChunk.y = stoi(word.substr(2, word.length() - 2));
					else if (word.substr(0, 6) == "width=")
						currentChunk.width = stoi(word.substr(6, word.length() - 6));
					else if (word.substr(0, 7) == "height=")
						currentChunk.height = stoi(word.substr(7, word.length() - 7));
					else
					{
						currentChunk.data.push_back(line);
					}
					break;

				default:
					break;
			}
		}
	}
};
