#pragma once

#include <SFML/Graphics.hpp>
#include <codecvt>
#include <iostream>
#include <locale>
#include <map>
#include <sstream>
#include <string>

#include "XMLParser.hpp"

class BitmapFont
{
public:
	BitmapFont()  = default;
	~BitmapFont() = default;

	bool create(const std::string& texturePath, const std::string& FNTPath)
	{
		XMLParser parser;

		if (!fontTexture.loadFromFile(texturePath))
			return false;

		if (!parser.parseFile(FNTPath))
			return false;

		parseFontData(parser.getRoot());

		return true;
	}

	sf::VertexArray getTextDrawable(const std::wstring& str, float x, float y,
									const sf::Color& color = sf::Color::White, int monospaced = 0)
	{
		sf::VertexArray textDrawable(sf::Quads);

		int line                 = 0;
		float accumulatedXOffset = x;

		for (auto ch : str)
		{
			if (ch == L'\n' || ch == L'\r')
			{
				++line;
				accumulatedXOffset = x;
				continue;
			}
			if (!fontCharacters.count(ch))
				continue;

			const auto data = fontCharacters[ch];
			const auto rect = data.rect;

			const float displayPointLeft = accumulatedXOffset + data.offset.x;
			const float displayPointTop  = y + data.offset.y + line * lineHeight + additionalSpacing.y;

			textDrawable.append(
				sf::Vertex(sf::Vector2f(displayPointLeft, displayPointTop), color, sf::Vector2f(rect.left, rect.top)));

			textDrawable.append(sf::Vertex(sf::Vector2f(displayPointLeft + rect.width, displayPointTop), color,
										   sf::Vector2f(rect.left + rect.width, rect.top)));

			textDrawable.append(sf::Vertex(sf::Vector2f(displayPointLeft + rect.width, displayPointTop + rect.height),
										   color, sf::Vector2f(rect.left + rect.width, rect.top + rect.height)));

			textDrawable.append(sf::Vertex(sf::Vector2f(displayPointLeft, displayPointTop + rect.height), color,
										   sf::Vector2f(rect.left, rect.top + rect.height)));

			accumulatedXOffset +=
				monospaced == 0 ? data.xAdvance + additionalSpacing.x : monospaced * (monospaced / data.xAdvance);
		}

		return textDrawable;
	}
	sf::VertexArray getTextDrawable(const std::wstring& str, const sf::Vector2f& pos,
									const sf::Color& color = sf::Color::White, int monospaced = 0)
	{
		return getTextDrawable(str, pos.x, pos.y, color, monospaced);
	}

	const sf::Texture& getFontTexture() { return fontTexture; }

	void setAdditionalSpacing(const sf::Vector2i& val) { additionalSpacing = val; }
	const sf::Vector2i& getAdditionalSpacing() { return additionalSpacing; }

private:
	std::string name = "NULL";
	int lineHeight   = 0;
	int size         = 0;

	sf::Texture fontTexture;

	struct BitmapCharacterData
	{
		std::string letter  = "NULL";
		sf::IntRect rect    = sf::IntRect({0, 0}, {0, 0});
		sf::Vector2i offset = sf::Vector2i(0, 0);
		int xAdvance        = 0;
	};

	sf::Vector2i additionalSpacing = {0, 0};

	std::map<wchar_t, BitmapCharacterData> fontCharacters;

	void parseCharacterData(const XMLElement& ch)
	{
		wchar_t code = 0;
		BitmapCharacterData data;

		for (const auto& at : ch.attributes)
		{
			if (at.name == "id")
				code = (wchar_t)std::stoi(at.value);

			else if (at.name == "x")
				data.rect.left = std::stoi(at.value);
			else if (at.name == "y")
				data.rect.top = std::stoi(at.value);
			else if (at.name == "width")
				data.rect.width = std::stoi(at.value);
			else if (at.name == "height")
				data.rect.height = std::stoi(at.value);

			else if (at.name == "xoffset")
				data.offset.x = std::stoi(at.value);
			else if (at.name == "yoffset")
				data.offset.y = std::stoi(at.value);

			else if (at.name == "xadvance")
				data.xAdvance = std::stoi(at.value);

			else if (at.name == "letter")
				data.letter = at.value;
		}

		if (code != 0)
			fontCharacters[code] = data;
	}

	void parseCommonData(const XMLElement& common)
	{
		for (const auto& at : common.attributes)
		{
			if (at.name == "lineHeight")
				lineHeight = std::stoi(at.value);
		}
	}

	void parseInfoData(const XMLElement& info)
	{
		for (const auto& at : info.attributes)
		{
			if (at.name == "size")
				size = std::stoi(at.value);
			else if (at.name == "face")
				name = at.value;
		}
	}

	void parseFontData(const XMLElement& root)
	{
		for (const auto& child : root.children)
		{
			if (child.name == "info")
				parseInfoData(child);
			else if (child.name == "common")
				parseCommonData(child);
			else if (child.name == "chars")
			{
				for (const auto& ch : child.children)
					parseCharacterData(ch);
			}
		}
	}
};