#pragma once
#include <codecvt>
#include <locale>
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

		textDrawable = font->getTextDrawable(text, rect.getPosition() + textPosCorrection, color, monospaced);

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

	const sf::Vector2f textPosCorrection = sf::Vector2f(0, -4);

	void _renderSelf(sf::RenderWindow& window) override
	{
		this->GuiElement::_renderSelf(window);

		if (textDrawable.first.getVertexCount())
		{
			if (rect.getPosition() + textPosCorrection != textDrawable.second.getPosition())
				generateTextDrawable();

			window.draw(textDrawable.first, &font->getFontTexture());
		}
	}

	void _printSelfName(const std::string& prefix) const override { std::cout << prefix << "GuiLabel" << std::endl; }
	void _printSelfInfo(const std::string& prefix) const override
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> wstringToStringConverter;

		std::cout << prefix << "TEXT: \'" << wstringToStringConverter.to_bytes(text) << "\'" << std::endl;
		std::cout << prefix << "COLOR: " << (int)color.r << "-" << (int)color.b << "-" << (int)color.g << "-"
				  << (int)color.a << std::endl;
		std::cout << prefix << "MONOSPACED: " << (monospaced == 0 ? "NO(0)" : std::to_string(monospaced)) << std::endl;

		std::cout << prefix << "FONT PTR? " << (font == nullptr ? "NO" : "YES") << std::endl;
		std::cout << prefix << "TEXT_DRAWABLE READY? " << (textDrawable.first.getVertexCount() == 0 ? "NO" : "YES")
				  << std::endl;
		if (textDrawable.first.getVertexCount())
			std::cout << prefix << "TEXT_DRAWABLE RECT: left:" << textDrawable.second.left
					  << " top:" << textDrawable.second.top << " width:" << textDrawable.second.width
					  << " height:" << textDrawable.second.height << std::endl;

		this->GuiElement::_printSelfInfo(prefix);
	}
};