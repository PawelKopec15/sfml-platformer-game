#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <iostream>

class NineSlice
{
public:
	NineSlice()  = default;
	~NineSlice() = default;

	bool setTexture(const std::string& path)
	{
		if (!texture.loadFromFile(path))
			return false;
		return true;
	}

	void setSlicing(const sf::IntRect& fullRect, const sf::IntRect& centerSlice)
	{
		textureRect       = fullRect;
		this->centerSlice = centerSlice;
	}
	void setSlicing(const sf::IntRect& fullRect)
	{
		const int hor = fullRect.width / 3;
		const int ver = fullRect.height / 3;

		setSlicing(fullRect, sf::IntRect(fullRect.left + hor, fullRect.top + ver, hor, ver));
	}

	void setTextureRectOnly(const sf::IntRect& fullRect) { textureRect = fullRect; }

	sf::VertexArray getDrawable(float x, float y, int width, int height, bool tiled = true,
								const sf::Color& color = sf::Color::White)
	{
		sf::VertexArray drawable(sf::Quads);

		if (!textureRect.width || !textureRect.height)
			return drawable;
		if (textureRect.width <= 0 || textureRect.height <= 0)
			setSlicing(sf::IntRect(0, 0, textureRect.width, textureRect.height));

		// Calculate dimensions of the slices
		const int texLeftWidth    = std::max(centerSlice.left - textureRect.left, 0);
		const int texRightWidth   = std::max(textureRect.width - centerSlice.left - centerSlice.width, 0);
		const int texTopHeight    = std::max(centerSlice.top - textureRect.top, 0);
		const int texBottomHeight = std::max(textureRect.height - centerSlice.top - centerSlice.height, 0);

		const int centerSpaceWidth  = std::max(width - texLeftWidth - texRightWidth, 0);
		const int centerSpaceHeight = std::max(height - texTopHeight - texBottomHeight, 0);

		const std::array<float, 3> xPositions    = {x, x + texLeftWidth, x + texLeftWidth + centerSpaceWidth};
		const std::array<float, 3> xTexPositions = {0, (float)centerSlice.left,
													(float)(centerSlice.left + centerSlice.width)};

		// Top
		if (texTopHeight > 0)
		{
			if (texLeftWidth > 0)
				// Top left corner
				addQuad(drawable, sf::Vector2f(xPositions[0], y), sf::Vector2f(texLeftWidth, texTopHeight),
						sf::Vector2f(xTexPositions[0], 0), sf::Vector2f(texLeftWidth, texTopHeight), color);

			if (centerSpaceWidth > 0)
				// Top side
				addSomethingBigger(drawable, sf::Vector2f(xPositions[1], y),
								   sf::Vector2f(centerSpaceWidth, texTopHeight), sf::Vector2f(xTexPositions[1], 0),
								   sf::Vector2f(centerSlice.width, texTopHeight), color, tiled);

			if (texRightWidth > 0)
				// Top right corner
				addQuad(drawable, sf::Vector2f(xPositions[2], y), sf::Vector2f(texRightWidth, texTopHeight),
						sf::Vector2f(xTexPositions[2], 0), sf::Vector2f(texRightWidth, texTopHeight), color);
		}

		// Center
		if (centerSpaceHeight > 0)
		{
			const float hereY    = y + texTopHeight;
			const float hereTexY = centerSlice.top;
			if (texLeftWidth > 0)
				// Left side
				addSomethingBigger(drawable, sf::Vector2f(xPositions[0], hereY),
								   sf::Vector2f(texLeftWidth, centerSpaceHeight),
								   sf::Vector2f(xTexPositions[0], hereTexY),
								   sf::Vector2f(centerSlice.left, centerSlice.height), color, tiled);
			if (centerSpaceWidth > 0)
				// Center
				addSomethingBigger(drawable, sf::Vector2f(xPositions[1], hereY),
								   sf::Vector2f(centerSpaceWidth, centerSpaceHeight),
								   sf::Vector2f(xTexPositions[1], hereTexY),
								   sf::Vector2f(centerSlice.width, centerSlice.height), color, tiled);
			if (texRightWidth > 0)
				// Right side
				addSomethingBigger(drawable, sf::Vector2f(xPositions[2], hereY),
								   sf::Vector2f(texRightWidth, centerSpaceHeight),
								   sf::Vector2f(xTexPositions[2], hereTexY),
								   sf::Vector2f(texRightWidth, centerSlice.height), color, tiled);
		}

		// Bottom
		if (texBottomHeight > 0)
		{
			const float hereY    = y + texTopHeight + centerSpaceHeight;
			const float hereTexY = centerSlice.top + centerSlice.height;

			if (texLeftWidth > 0)
				// Bottom left corner
				addQuad(drawable, sf::Vector2f(xPositions[0], hereY), sf::Vector2f(texLeftWidth, texBottomHeight),
						sf::Vector2f(xTexPositions[0], hereTexY), sf::Vector2f(texLeftWidth, texBottomHeight), color);

			if (centerSpaceWidth > 0)
				// Bottom side
				addSomethingBigger(drawable, sf::Vector2f(xPositions[1], hereY),
								   sf::Vector2f(centerSpaceWidth, texBottomHeight),
								   sf::Vector2f(xTexPositions[1], hereTexY),
								   sf::Vector2f(centerSlice.width, texBottomHeight), color, tiled);

			if (texRightWidth > 0)
				// Bottom right corner
				addQuad(drawable, sf::Vector2f(xPositions[2], hereY), sf::Vector2f(texRightWidth, texBottomHeight),
						sf::Vector2f(xTexPositions[2], hereTexY), sf::Vector2f(texRightWidth, texBottomHeight), color);
		}

		return drawable;
	}

	sf::VertexArray getDrawable(const sf::Vector2f& pos, const sf::Vector2i& size, bool tiled = true,
								const sf::Color& color = sf::Color::White)
	{
		return getDrawable(pos.x, pos.y, size.x, size.y, tiled, color);
	}

	const sf::Texture& getTexture() { return texture; }

private:
	sf::Texture texture;
	sf::IntRect textureRect;
	sf::IntRect centerSlice;

	void addQuad(sf::VertexArray& outDrawable, const sf::Vector2f& pos, const sf::Vector2f& size,
				 const sf::Vector2f& relativeTexPos, const sf::Vector2f& texSize, const sf::Color& color)
	{
		auto texPos = relativeTexPos + sf::Vector2f(textureRect.left, textureRect.top);

		outDrawable.append(sf::Vertex(sf::Vector2f(pos.x, pos.y), color, sf::Vector2f(texPos.x, texPos.y)));

		outDrawable.append(
			sf::Vertex(sf::Vector2f(pos.x + size.x, pos.y), color, sf::Vector2f(texPos.x + texSize.x, texPos.y)));

		outDrawable.append(sf::Vertex(sf::Vector2f(pos.x + size.x, pos.y + size.y), color,
									  sf::Vector2f(texPos.x + texSize.x, texPos.y + texSize.y)));

		outDrawable.append(
			sf::Vertex(sf::Vector2f(pos.x, pos.y + size.y), color, sf::Vector2f(texPos.x, texPos.y + texSize.y)));
	}

	void addTiled(sf::VertexArray& outDrawable, const sf::Vector2f& pos, const sf::Vector2f& size,
				  const sf::Vector2f& relativeTexPos, const sf::Vector2f& texSize, const sf::Color& color)
	{
		const int tilesHor         = (int)size.x / (int)texSize.x;
		const int tilesVer         = (int)size.y / (int)texSize.y;
		const float extraPixelsHor = size.x - tilesHor * texSize.x;
		const float extraPixelsVer = size.y - tilesVer * texSize.y;

		for (size_t i = 0; i < tilesVer; ++i)
		{
			for (size_t j = 0; j < tilesHor; ++j)
				addQuad(outDrawable, sf::Vector2f(pos.x + j * texSize.x, pos.y + i * texSize.y), texSize,
						relativeTexPos, texSize, color);
			if (extraPixelsHor > 0)
				addQuad(outDrawable, sf::Vector2f(pos.x + tilesHor * texSize.x, pos.y + i * texSize.y),
						sf::Vector2f(extraPixelsHor, texSize.y), relativeTexPos,
						sf::Vector2f(extraPixelsHor, texSize.y), color);
		}
		if (extraPixelsVer > 0)
		{
			for (size_t j = 0; j < tilesHor; ++j)
				addQuad(outDrawable, sf::Vector2f(pos.x + j * texSize.x, pos.y + tilesHor * texSize.y),
						sf::Vector2f(texSize.x, extraPixelsVer), relativeTexPos,
						sf::Vector2f(texSize.x, extraPixelsVer), color);

			if (extraPixelsHor > 0)
				addQuad(outDrawable, sf::Vector2f(pos.x + tilesHor * texSize.x, pos.y + tilesHor * texSize.y),
						sf::Vector2f(extraPixelsHor, extraPixelsVer), relativeTexPos,
						sf::Vector2f(extraPixelsHor, extraPixelsVer), color);
		}
	}

	void addSomethingBigger(sf::VertexArray& outDrawable, const sf::Vector2f& pos, const sf::Vector2f& size,
							const sf::Vector2f& relativeTexPos, const sf::Vector2f& texSize, const sf::Color& color,
							bool tiled)
	{
		if (tiled)
			addTiled(outDrawable, pos, size, relativeTexPos, texSize, color);
		else
			addQuad(outDrawable, pos, size, relativeTexPos, texSize, color);
	}
};