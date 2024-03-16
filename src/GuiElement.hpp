#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
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

	void setParent(const std::shared_ptr<GuiElement>& val) { parent = val; }

	void setRect(const sf::FloatRect& val) { rect = val; }
	void setNineSlice(const std::shared_ptr<NineSlice>& val) { nineSlice = val; }

	void setInnerMargins(const GuiMargins& val) { innerMargins = val; }
	void setOuterMargins(const GuiMargins& val) { outerPadding = val; }

	void setLayoutManager(const std::shared_ptr<LayoutManager>& val) { layoutManager = val; }
	void addChildren(const std::vector<std::shared_ptr<GuiElement>>& val)
	{
		for (auto&& child : val)
		{
			child->setParent(std::shared_ptr<GuiElement>(this));
			children.push_back(child);
		}
	}
	void addChild(const std::shared_ptr<GuiElement>& val)
	{
		val->setParent(std::shared_ptr<GuiElement>(this));
		children.push_back(val);
	}

	const sf::FloatRect& getRect() { return rect; }

	void render(sf::RenderWindow& window)
	{
		if (!_layOutChildren() && parent)
		{
			parent->childHasBeenResized(window);
			return;
		}

		_renderSelf(window);

		for (auto&& child : children)
			child->render(window);
	}

	void childHasBeenResized(sf::RenderWindow& window) { render(window); }

	void mouseEvent(sf::Event::EventType type, const sf::Vector2f& mousePosition)
	{
		if (mousePosition.x < rect.left || mousePosition.x > rect.left + rect.width || mousePosition.y < rect.top ||
			mousePosition.y > rect.top + rect.height)
			return;

		for (auto&& child : children)
			child->mouseEvent(type, mousePosition);

		_handleMouseEvent(type, mousePosition);
	}

	// void setInFocus(bool val) { inFocus = val; }
	// bool isInFocus() { return inFocus; }

	void print(const std::string& prefix = "")
	{
		_printSelfName(prefix);
		std::cout << prefix << "---------------------------" << std::endl;
		_printSelfInfo(prefix);

		if (children.empty())
			std::cout << prefix << "{ }" << std::endl;
		else
		{
			std::cout << prefix << "{" << std::endl;
			for (const auto& child : children)
			{
				child->print(prefix + "  ");
			}
			std::cout << prefix << "}" << std::endl;
		}
	}

protected:
	sf::FloatRect rect;
	std::shared_ptr<NineSlice> nineSlice = nullptr;
	GuiMargins innerMargins              = {0, 0, 0, 0};
	GuiMargins outerPadding              = {0, 0, 0, 0};

	std::shared_ptr<GuiElement> parent                = nullptr;
	std::vector<std::shared_ptr<GuiElement>> children = {};
	std::shared_ptr<LayoutManager> layoutManager      = nullptr;

	// bool inFocus = false;

	sf::FloatRect _getAvailableSpaceForChildren()
	{
		sf::FloatRect toRet;

		toRet.top    = rect.top + innerMargins.top + outerPadding.top;
		toRet.height = rect.height - innerMargins.top - innerMargins.bottom - outerPadding.top - outerPadding.bottom;
		toRet.left   = rect.left + innerMargins.left + outerPadding.left;
		toRet.width  = rect.width - innerMargins.left - innerMargins.right - outerPadding.left - outerPadding.right;

		return toRet;
	}

	// Returns false if this needs to be resized.
	bool _layOutChildren()
	{
		if (!layoutManager)
			return true;

		std::vector<sf::Vector2f> sizes;
		for (const auto& child : children)
			sizes.push_back(child->rect.getSize());

		auto availableSpace               = _getAvailableSpaceForChildren();
		const auto previousAvailableSpace = availableSpace;
		auto rects                        = layoutManager->doYourThing(availableSpace, sizes);

		if (availableSpace != previousAvailableSpace)
		{
			rect.top = availableSpace.top - innerMargins.top - outerPadding.top;
			rect.height =
				availableSpace.height + innerMargins.top + innerMargins.bottom + outerPadding.top + outerPadding.bottom;
			rect.left = availableSpace.left - innerMargins.left - outerPadding.left;
			rect.width - availableSpace.width + innerMargins.left + innerMargins.right + innerMargins.left +
				innerMargins.right;
			return false;
		}

		if (rects.size() != sizes.size())
			return true;

		for (size_t i = 0; i < children.size(); ++i)
			children[i]->setRect(rects[i]);

		return true;
	}

	virtual void _renderSelf(sf::RenderWindow& window)
	{
		if (nineSlice)
		{
			window.draw(nineSlice->getDrawable(rect.getPosition(), {(int)rect.width, (int)rect.height}, false),
						&nineSlice->getTexture());
		}
	}

	virtual void _handleMouseEvent(sf::Event::EventType type,
								   const sf::Vector2f& mousePosition){/* Do nothing by default */};

	virtual void _printSelfName(const std::string& prefix) const { std::cout << prefix << "GuiElement" << std::endl; }
	virtual void _printSelfInfo(const std::string& prefix) const
	{
		std::cout << prefix << "RECT: left:" << rect.left << " top:" << rect.top << " width:" << rect.width
				  << " height:" << rect.height << std::endl;
		std::cout << prefix << "NINESLICE PTR? " << (nineSlice == nullptr ? "NO" : "YES") << std::endl;
		std::cout << prefix << "INNER MARGINS: top:" << innerMargins.top << " bottom:" << innerMargins.bottom
				  << " left:" << innerMargins.left << " right:" << innerMargins.right << std::endl;
		std::cout << prefix << "OUTER PADDING: top:" << outerPadding.top << " bottom:" << outerPadding.bottom
				  << " left:" << outerPadding.left << " right:" << outerPadding.right << std::endl;
		std::cout << prefix << "LAYOUT MANAGER PTR? " << (layoutManager == nullptr ? "NO" : "YES") << std::endl;
		std::cout << prefix << "PARENT PTR? " << (parent == nullptr ? "NO" : "YES") << std::endl;
		// std::cout << prefix << "IS IN FOCUS? " << (inFocus ? "YES" : "NO") << std::endl;
	}
};