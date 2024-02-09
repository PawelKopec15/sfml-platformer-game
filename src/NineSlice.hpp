#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <string>

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

	void setSlicesAuto(const sf::IntRect& fullRect)
	{
		const int hor = fullRect.width / 3;
		const int ver = fullRect.height / 3;

		textureRect = fullRect;
		centerSlice = sf::IntRect(fullRect.left + hor, fullRect.top + ver, fullRect.width - hor, fullRect.height - ver);
	}
	void setSlicesByCenter(const sf::IntRect& fullRect, const sf::IntRect& centerSlice)
	{
		textureRect       = fullRect;
		this->centerSlice = centerSlice;
	}

	void setTextureRectOnly(const sf::IntRect& fullRect) { textureRect = fullRect; }

	sf::VertexArray getDrawable(float x, float y, int width, int height, bool tiled = true,
								const sf::Color& color = sf::Color::White)
	{
		sf::VertexArray drawable(sf::Quads);

		// Ensure that the texture is loaded before proceeding
		if (!textureRect.width || !textureRect.height)
			return drawable;

		// Calculate dimensions of the slices
		int leftWidth    = textureRect.left - centerSlice.left;
		int rightWidth   = textureRect.width - centerSlice.left - centerSlice.width;
		int topHeight    = textureRect.top - centerSlice.top;
		int bottomHeight = textureRect.height - centerSlice.top - centerSlice.height;

		// If tiled is true, calculate the number of tiles required
		int tileX = tiled ? (width - leftWidth - rightWidth) / centerSlice.width : 1;
		int tileY = tiled ? (height - topHeight - bottomHeight) / centerSlice.height : 1;

		// Draw the top
		drawTop(drawable, x + leftWidth, y, width - leftWidth - rightWidth, topHeight, color, tiled);

		// Draw the bottom
		drawBottom(drawable, x + leftWidth, y + height - bottomHeight, width - leftWidth - rightWidth, bottomHeight,
				   color, tiled);

		// Draw the left side
		drawLeftSide(drawable, x, y + topHeight, leftWidth, height - topHeight - bottomHeight, color, tiled);

		// Draw the right side
		drawRightSide(drawable, x + width - rightWidth, y + topHeight, rightWidth, height - topHeight - bottomHeight,
					  color, tiled);

		// Draw the center
		drawCenter(drawable, x + leftWidth, y + topHeight, width - leftWidth - rightWidth,
				   height - topHeight - bottomHeight, color, tiled);

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

	void drawTop(sf::VertexArray& outDrawable, float x, float y, int width, int height, const sf::Color& color,
				 bool tiled)
	{
		if (tiled)
			drawHorizontalTiled(outDrawable, x, y, width, centerSlice.top - textureRect.top, color);
		else
			drawQuad(outDrawable, x, y, width, centerSlice.top - textureRect.top, color);
	}

	void drawBottom(sf::VertexArray& outDrawable, float x, float y, int width, int height, const sf::Color& color,
					bool tiled)
	{
		if (tiled)
			drawHorizontalTiled(outDrawable, x, y, width, textureRect.height - centerSlice.top - centerSlice.height,
								color);
		else
			drawQuad(outDrawable, x, y, width, textureRect.height - centerSlice.top - centerSlice.height, color);
	}

	void drawLeftSide(sf::VertexArray& outDrawable, float x, float y, int width, int height, const sf::Color& color,
					  bool tiled)
	{
		if (tiled)
			drawVerticalTiled(outDrawable, x, y, centerSlice.left - textureRect.left, height, color);
		else
			drawQuad(outDrawable, x, y, centerSlice.left - textureRect.left, height, color);
	}

	void drawRightSide(sf::VertexArray& outDrawable, float x, float y, int width, int height, const sf::Color& color,
					   bool tiled)
	{
		if (tiled)
			drawVerticalTiled(outDrawable, x, y, textureRect.width - centerSlice.left - centerSlice.width, height,
							  color);
		else
			drawQuad(outDrawable, x, y, textureRect.width - centerSlice.left - centerSlice.width, height, color);
	}

	void drawCenter(sf::VertexArray& outDrawable, float x, float y, int width, int height, const sf::Color& color,
					bool tiled)
	{
		if (tiled)
			drawTiled(outDrawable, x, y, width, height, color);
		else
			drawQuad(outDrawable, x, y, width, height, color);
	}

	void drawQuad(sf::VertexArray& outDrawable, float x, float y, int width, int height, const sf::Color& color)
	{
		outDrawable.append(sf::Vertex(sf::Vector2f(x, y), color, sf::Vector2f(textureRect.left, textureRect.top)));
		outDrawable.append(
			sf::Vertex(sf::Vector2f(x + width, y), color, sf::Vector2f(textureRect.left + width, textureRect.top)));
		outDrawable.append(sf::Vertex(sf::Vector2f(x + width, y + height), color,
									  sf::Vector2f(textureRect.left + width, textureRect.top + height)));
		outDrawable.append(
			sf::Vertex(sf::Vector2f(x, y + height), color, sf::Vector2f(textureRect.left, textureRect.top + height)));
	}

	void drawHorizontalTiled(sf::VertexArray& outDrawable, float x, float y, int width, int height,
							 const sf::Color& color)
	{
		int posX = 0;
		while (posX < width)
		{
			int tileWidth = std::min(width - posX, centerSlice.width);
			drawQuad(outDrawable, x + posX, y, tileWidth, height, color);
			posX += tileWidth;
		}
	}

	void drawVerticalTiled(sf::VertexArray& outDrawable, float x, float y, int width, int height,
						   const sf::Color& color)
	{
		int posY = 0;
		while (posY < height)
		{
			int tileHeight = std::min(height - posY, centerSlice.height);
			drawQuad(outDrawable, x, y + posY, width, tileHeight, color);
			posY += tileHeight;
		}
	}

	void drawTiled(sf::VertexArray& outDrawable, float x, float y, int width, int height, const sf::Color& color)
	{
		int posX = 0;
		while (posX < width)
		{
			int posY      = 0;
			int tileWidth = std::min(width - posX, centerSlice.width);
			while (posY < height)
			{
				int tileHeight = std::min(height - posY, centerSlice.height);
				drawQuad(outDrawable, x + posX, y + posY, tileWidth, tileHeight, color);
				posY += tileHeight;
			}
			posX += tileWidth;
		}
	}
};