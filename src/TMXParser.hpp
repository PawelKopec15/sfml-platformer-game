#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "XMLParser.hpp"

struct TMXObjectProperty
{
	std::string type  = "";
	std::string value = "";
};

struct TMXObject
{
	std::string name = "";
	std::string type = "";
	int x            = 0;
	int y            = 0;
	int width        = 0;
	int height       = 0;
	float rotation   = 0.f;

	std::map<std::string, TMXObjectProperty, std::less<>> properties = {};
};

struct TMXObjectGroup
{
	std::string name = "";

	std::map<int, TMXObject> objects = {};
};
struct TMXChunk
{
	int width  = 0;
	int height = 0;

	std::vector<std::vector<int>> data = {};
};

struct TMXLayer
{
	std::string name = "";
	int width        = 0;
	int height       = 0;

	std::map<std::pair<int, int>, TMXChunk> chunks = {};
};

struct TMXTileSet
{
	int firstGID       = 0;
	std::string source = {};
};

struct TMXEditorSettings
{
	int chunkWidth  = 0;
	int chunkHeight = 0;
};

struct TMXMap
{
	std::string orientation = "";
	std::string renderOrder = "";
	int width               = 0;
	int height              = 0;
	int tileWidth           = 0;
	int tileHeight          = 0;
	bool infinite           = false;

	TMXEditorSettings editorSettings;
	std::vector<TMXTileSet> tileSets           = {};
	std::map<int, TMXLayer> layers             = {};
	std::map<int, TMXObjectGroup> objectGroups = {};
};

class TMXParser
{
public:
	TMXParser()  = default;
	~TMXParser() = default;

	bool parse(const std::string& path, bool print = false)
	{
		if (print)
			std::cout << "Building level from " << path << " ..." << std::endl;

		XMLParser parser;

		if (parser.parseFile(path))
			parseFromXMLParser(parser.getRoot());
		else
			return false;

		if (print)
		{
			std::cout << "Building level done. Printing results:" << std::endl;
			printParsedData();
		}

		return true;
	}

	const TMXMap& getMap() { return map; }

	void printParsedData() const
	{
		std::cout << "Map Info:" << std::endl;
		std::cout << "  Orientation: " << map.orientation << std::endl;
		std::cout << "  Render Order: " << map.renderOrder << std::endl;
		std::cout << "  Width: " << map.width << std::endl;
		std::cout << "  Height: " << map.height << std::endl;
		std::cout << "  Tile Width: " << map.tileWidth << std::endl;
		std::cout << "  Tile Height: " << map.tileHeight << std::endl;
		std::cout << "  Infinite: " << (map.infinite ? "true" : "false") << std::endl;

		std::cout << "Editor Settings:" << std::endl;
		std::cout << "  Chunk Width: " << map.editorSettings.chunkWidth << std::endl;
		std::cout << "  Chunk Height: " << map.editorSettings.chunkHeight << std::endl;

		std::cout << "Tile Sets:" << std::endl;
		for (const auto& tileSet : map.tileSets)
		{
			std::cout << "  First GID: " << tileSet.firstGID << std::endl;
			std::cout << "  Source: " << tileSet.source << std::endl;
		}

		std::cout << "Layers:" << std::endl;
		for (const auto& layerPair : map.layers)
		{
			const TMXLayer& layer = layerPair.second;
			std::cout << "  Layer Name: " << layer.name << std::endl;
			std::cout << "  Layer Width: " << layer.width << std::endl;
			std::cout << "  Layer Height: " << layer.height << std::endl;

			std::cout << "  Chunks:" << std::endl;
			for (const auto& chunkPair : layer.chunks)
			{
				const std::pair<int, int>& chunkPosition = chunkPair.first;
				const TMXChunk& chunk                    = chunkPair.second;

				std::cout << "    Chunk Position: (" << chunkPosition.first << ", " << chunkPosition.second << ")"
						  << std::endl;
				std::cout << "    Chunk Width: " << chunk.width << std::endl;
				std::cout << "    Chunk Height: " << chunk.height << std::endl;

				std::cout << "    Data:" << std::endl;
				for (const auto& rowData : chunk.data)
				{
					for (const auto& tile : rowData)
					{
						std::cout << tile << " ";
					}
					std::cout << std::endl;
				}
			}
		}

		std::cout << "Object Groups:" << std::endl;
		for (const auto& objectGroupPair : map.objectGroups)
		{
			const TMXObjectGroup& objectGroup = objectGroupPair.second;
			std::cout << "  Object Group Name: " << objectGroup.name << std::endl;

			for (const auto& objectPair : objectGroup.objects)
			{
				const TMXObject& object = objectPair.second;
				std::cout << "    Object Name: " << object.name << std::endl;
				std::cout << "    Object Type: " << object.type << std::endl;
				std::cout << "    Object Position: (" << object.x << ", " << object.y << ")" << std::endl;
				std::cout << "    Object Size: " << object.width << " x " << object.height << std::endl;
				std::cout << "    Object Rotation: " << object.rotation << std::endl;

				std::cout << "    Object Properties:" << std::endl;
				for (const auto& propertyPair : object.properties)
				{
					const std::string& propertyName   = propertyPair.first;
					const TMXObjectProperty& property = propertyPair.second;
					std::cout << "      Property Name: " << propertyName << std::endl;
					std::cout << "      Property Type: " << property.type << std::endl;
					std::cout << "      Property Value: " << property.value << std::endl;
				}
			}
		}
	}

private:
	TMXMap map;

	std::map<std::string, TMXObject, std::less<>> objectTemplates;

	std::vector<int> stringToIntVector(const std::string& input) const
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

	int getIntAttributeValue(const XMLElement& el, const std::string& attrName) const
	{
		for (const auto& attr : el.attributes)
			if (attr.name == attrName)
				return std::stoi(attr.value);

		return -1;
	}
	std::string getAttributeValue(const XMLElement& el, const std::string& attrName) const
	{
		for (const auto& attr : el.attributes)
			if (attr.name == attrName)
				return attr.value;

		return "";
	}

	TMXObjectProperty parseObjectProperty(const XMLElement& property) const
	{
		TMXObjectProperty toRet;
		toRet.type  = getAttributeValue(property, "type");
		toRet.value = getAttributeValue(property, "value");
		return toRet;
	}

	TMXObject parseObject(const XMLElement& object)
	{
		TMXObject toRet;

		for (const auto& attr : object.attributes)
		{
			if (attr.name == "template")
			{
				auto it = objectTemplates.find(attr.value);

				if (it != objectTemplates.end())
					toRet = it->second;
				else
				{
					XMLParser tempParser;
					TMXObject templateObject;
					if (tempParser.parseFile(attr.value))
						templateObject = parseObject(tempParser.getRoot().children[0]);

					objectTemplates[attr.value] = templateObject;

					toRet = templateObject;
				}
			}
			else if (attr.name == "name")
				toRet.name = attr.value;
			else if (attr.name == "type")
				toRet.type = attr.value;
			else if (attr.name == "x")
				toRet.x = std::stoi(attr.value);
			else if (attr.name == "y")
				toRet.y = std::stoi(attr.value);
			else if (attr.name == "width")
				toRet.width = std::stoi(attr.value);
			else if (attr.name == "height")
				toRet.height = std::stoi(attr.value);
			else if (attr.name == "rotation")
				toRet.rotation = std::stof(attr.value);
		}
		for (const auto& property : object.children[0].children)
		{
			toRet.properties[getAttributeValue(property, "name")] = parseObjectProperty(property);
		}

		return toRet;
	}

	TMXObjectGroup parseObjectGroup(const XMLElement& objectGroup)
	{
		TMXObjectGroup toRet;

		toRet.name = getAttributeValue(objectGroup, "name");

		for (const auto& object : objectGroup.children)
		{
			toRet.objects[getIntAttributeValue(object, "id")] = parseObject(object);
		}

		return toRet;
	}

	TMXChunk parseChunk(const XMLElement& chunk) const
	{
		TMXChunk toRet;

		for (const auto& attr : chunk.attributes)
		{
			if (attr.name == "width")
				toRet.width = std::stoi(attr.value);
			else if (attr.name == "height")
				toRet.height = std::stoi(attr.value);
			else if (attr.name == "")
				toRet.data.push_back(stringToIntVector(attr.value));
		}

		return toRet;
	}

	TMXLayer parseLayer(const XMLElement& layer) const
	{
		TMXLayer toRet;

		for (const auto& attr : layer.attributes)
		{
			if (attr.name == "name")
				toRet.name = attr.value;
			else if (attr.name == "width")
				toRet.width = std::stoi(attr.value);
			else if (attr.name == "height")
				toRet.height = std::stoi(attr.value);
		}

		for (const auto& chunk : layer.children[0].children)
		{
			toRet.chunks[{getIntAttributeValue(chunk, "x"), getIntAttributeValue(chunk, "y")}] = (parseChunk(chunk));
		}

		return toRet;
	}

	TMXEditorSettings parseEditorSettings(const XMLElement& el) const
	{
		TMXEditorSettings toRet;

		for (const auto& child : el.children)
		{
			if (child.name == "chunksize")
			{
				for (const auto& attr : child.attributes)
				{
					if (attr.name == "width")
						toRet.chunkWidth = std::stoi(attr.value);
					else if (attr.name == "height")
						toRet.chunkHeight = std::stoi(attr.value);
				}
			}
		}

		return toRet;
	}

	void parseFromXMLParser(const XMLElement& root)
	{
		for (const auto& attr : root.attributes)
		{
			if (attr.name == "orientation")
				map.orientation = attr.value;
			else if (attr.name == "renderorder")
				map.renderOrder = attr.value;
			else if (attr.name == "width")
				map.width = std::stoi(attr.value);
			else if (attr.name == "height")
				map.height = std::stoi(attr.value);
			else if (attr.name == "tilewidth")
				map.tileWidth = std::stoi(attr.value);
			else if (attr.name == "tileheight")
				map.tileHeight = std::stoi(attr.value);
			else if (attr.name == "infinite")
				map.infinite = attr.value != "0";
		}

		for (const auto& child : root.children)
		{
			if (child.name == "editorsettings")
				map.editorSettings = parseEditorSettings(child);

			// TODO TILESET

			else if (child.name == "layer")
				map.layers[getIntAttributeValue(child, "id")] = parseLayer(child);
			else if (child.name == "objectgroup")
				map.objectGroups[getIntAttributeValue(child, "id")] = parseObjectGroup(child);
		}
	}
};