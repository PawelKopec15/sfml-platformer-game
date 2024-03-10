#pragma once
#include <memory>

#include "BitmapFont.hpp"
#include "GuiElement.hpp"

class GuiLabel : public GuiElement
{
public:
	explicit GuiLabel(const sf::FloatRect& rect, const std::shared_ptr<BitmapFont>& font, const std::wstring& text,
					  const sf::Color& color = sf::Color::White, int monospaced = 0)
		: GuiElement(rect), font(font), text(text), color(color), monospaced(monospaced)
	{
		generateTextDrawable();
	}
	GuiLabel(const sf::FloatRect& rect, const std::shared_ptr<NineSlice>& nineSlice,
			 const std::shared_ptr<BitmapFont>& font, const std::wstring& text,
			 const sf::Color& color = sf::Color::White, int monospaced = 0)
		: GuiElement(rect, nineSlice), font(font), color(color), monospaced(monospaced)
	{
		generateTextDrawable();
	}

	bool generateTextDrawable()
	{
		if (!font)
			return false;

		textDrawable = font->getTextDrawable(text, rect.getPosition(), color, monospaced);

		rect.height = std::max(rect.height, textDrawable.second.height);
		rect.width  = std::max(rect.width, textDrawable.second.width);

		return true;
	}

	void setText(const std::wstring& val) { text = val; }
	void setColor(const sf::Color& val) { color = val; }
	void setMonospaced(int val) { monospaced = val; }

protected:
	std::shared_ptr<BitmapFont> font = nullptr;
	std::pair<sf::VertexArray, sf::FloatRect> textDrawable;

	std::wstring text;
	sf::Color color;
	int monospaced;

	void _renderSelf(sf::RenderWindow& window) override
	{
		this->GuiElement::_renderSelf(window);

		if (textDrawable.first.getVertexCount())
		{
			if (rect != textDrawable.second)
				generateTextDrawable();

			window.draw(textDrawable.first, &font->getFontTexture());
		}
	}
};