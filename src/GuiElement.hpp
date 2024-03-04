#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "GuiUtilities.hpp"
#include "LayoutManagers.hpp"
#include "NineSlice.hpp"

class GuiElement
{
public:
	explicit GuiElement(const sf::FloatRect& rect) : rect(rect) {}
	GuiElement(const sf::FloatRect& rect, const std::shared_ptr<NineSlice>& nineSlice)
		: rect(rect), nineSlice(nineSlice)
	{}
	~GuiElement() = default;

	void setRect(const sf::FloatRect& val) { rect = val; }
	void setNineSlice(const std::shared_ptr<NineSlice>& val) { nineSlice = val; }

	void setInnerMargins(const GuiMargins& val) { innerMargins = val; }
	void setOuterMargins(const GuiMargins& val) { outerMargins = val; }

	void setLayoutManager(const std::shared_ptr<LayoutManager>& val) { layoutManager = val; }
	void setChildren(const std::vector<std::shared_ptr<GuiElement>>& val) { children = val; }
	void addChild(const std::shared_ptr<GuiElement>& val) { children.push_back(val); }

	const sf::FloatRect& getRect() { return rect; }

	sf::FloatRect getAvailableSpaceForChildren()
	{
		sf::FloatRect toRet;

		toRet.top    = rect.top + innerMargins.top + outerMargins.top;
		toRet.height = rect.height - innerMargins.top - innerMargins.bottom - outerMargins.top - outerMargins.bottom;
		toRet.left   = rect.left + innerMargins.left + outerMargins.left;
		toRet.width  = rect.width - innerMargins.left - innerMargins.right - outerMargins.left - outerMargins.right;

		return toRet;
	}

	bool layOutChildren()
	{
		if (!layoutManager)
			return false;

		std::vector<sf::Vector2f> sizes;
		for (const auto& child : children)
			sizes.push_back(child->rect.getSize());

		auto availableSpace = getAvailableSpaceForChildren();
		auto rects          = layoutManager->doYourThing(availableSpace, sizes);

		if (rects.size() != sizes.size())
			return false;

		for (size_t i = 0; i < children.size(); ++i)
			children[i]->setRect(rects[i]);

		return true;
	}

	void render(sf::RenderWindow& window)
	{
		if (nineSlice)
			window.draw(nineSlice->getDrawable(rect.getPosition(), {(int)rect.width, (int)rect.height}, false),
						&nineSlice->getTexture());

		layOutChildren();

		for (auto&& child : children)
			child->render(window);
	}

	void mouseEvent(sf::Event::EventType type, const sf::Vector2f& mousePosition)
	{
		if (mousePosition.x < rect.left || mousePosition.x > rect.left + rect.width || mousePosition.y < rect.top ||
			mousePosition.y > rect.top + rect.height)
			return;

		for (auto&& child : children)
			child->mouseEvent(type, mousePosition);

		_handleMouseEvent(type, mousePosition);
	}

protected:
	sf::FloatRect rect;
	std::shared_ptr<NineSlice> nineSlice = nullptr;
	GuiMargins innerMargins              = {6, 6, 6, 6};
	GuiMargins outerMargins              = {0, 0, 0, 0};

	std::shared_ptr<LayoutManager> layoutManager      = nullptr;
	std::vector<std::shared_ptr<GuiElement>> children = {};

	virtual void _handleMouseEvent(sf::Event::EventType type, const sf::Vector2f& mousePosition){};
};