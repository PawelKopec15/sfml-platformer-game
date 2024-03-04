#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "GuiUtilities.hpp"

class LayoutManager
{
public:
	LayoutManager()  = default;
	~LayoutManager() = default;

	void setEdgeMargins(const GuiMargins& val) { edgeMargins = val; }
	void setElementMargins(const GuiMargins& val) { elementMargins = val; }
	void setAlignmentHor(const GuiAlignment& val) { alignmentHor = val; }
	void setAlignmentVer(const GuiAlignment& val) { alignmentVer = val; }

	const GuiMargins& getEdgeMargins() { return edgeMargins; }
	const GuiMargins& getElementMargins() { return elementMargins; }
	const GuiAlignment& getAlignmentHor() { return alignmentHor; }
	const GuiAlignment& getAlignmentVer() { return alignmentVer; }

	// Should return std::vector of sf::FloatRects, where n'th rect is the position
	// of the n'th element of the n'th innerElementSize.
	// If elements take up too much space, the outAvailableSpace will be updated.
	// It is then up to the caller entity to increase the space or ignore the info.
	std::vector<sf::FloatRect> doYourThing(sf::FloatRect& outAvailableSpace,
										   const std::vector<sf::Vector2f>& innerElementSizes)
	{
		auto minimumSpaceSize = calculateMinimumSpaceSize(outAvailableSpace, innerElementSizes);

		outAvailableSpace = {outAvailableSpace.left, outAvailableSpace.top,
							 std::max(outAvailableSpace.width, minimumSpaceSize.x),
							 std::max(outAvailableSpace.height, minimumSpaceSize.y)};

		return getEverythingLayedOut(outAvailableSpace, minimumSpaceSize, innerElementSizes);
	}

private:
	GuiMargins edgeMargins    = {0, 0, 0, 0};
	GuiMargins elementMargins = {1, 1, 1, 1};
	GuiAlignment alignmentHor = GuiAlignment::CENTER;
	GuiAlignment alignmentVer = GuiAlignment::CENTER;

protected:
	virtual sf::Vector2f calculateMinimumSpaceSize(const sf::FloatRect& availableSpace,
												   const std::vector<sf::Vector2f>& innerElementSizes)
	{
		return availableSpace.getSize();
	}

	virtual std::vector<sf::FloatRect> getEverythingLayedOut(const sf::FloatRect& availableSpace,
															 const sf::Vector2f& minimumSpaceSize,
															 const std::vector<sf::Vector2f>& innerElementSizes)
	{
		return std::vector<sf::FloatRect>();
	}
};

class VBoxLayoutManager : public LayoutManager
{
private:
	std::vector<sf::FloatRect> getEverythingLayedOut(const sf::FloatRect& availableSpace,
													 const sf::Vector2f& minimumSpaceSize,
													 const std::vector<sf::Vector2f>& innerElementSizes) override
	{
		std::vector<sf::FloatRect> toRet = {};

		const float extraSpacing = getAlignmentVer() == GuiAlignment::CENTER
									   ? (availableSpace.height - minimumSpaceSize.y) / (innerElementSizes.size() + 1)
									   : 0.f;

		const float centerX = availableSpace.left + (availableSpace.width / 2);

		float currentY = getAlignmentVer() == GuiAlignment::RIGHT
							 ? availableSpace.top + availableSpace.height - minimumSpaceSize.y
							 : currentY = availableSpace.top + getEdgeMargins().top + extraSpacing;

		for (auto size : innerElementSizes)
		{
			currentY += getElementMargins().top;

			float left;
			switch (getAlignmentHor())
			{
				default:
					left = centerX - (size.x / 2);
					break;

				case GuiAlignment::LEFT:
					left = availableSpace.left + getEdgeMargins().left + getElementMargins().left;
					break;

				case GuiAlignment::RIGHT:
					left = availableSpace.left + availableSpace.width - getEdgeMargins().right -
						   getElementMargins().right - size.x;
					break;
			}

			toRet.push_back({left, currentY, size.x, size.y});

			currentY += size.y + extraSpacing + getElementMargins().bottom;
		}

		return toRet;
	}

	sf::Vector2f calculateMinimumSpaceSize(const sf::FloatRect& availableSpace,
										   const std::vector<sf::Vector2f>& innerElementSizes) override
	{
		float totalHeight     = getEdgeMargins().top + getEdgeMargins().bottom;
		float totalWidth      = getEdgeMargins().left + getEdgeMargins().right;
		float maxElementWidth = 0.f;
		for (auto size : innerElementSizes)
		{
			totalHeight += size.y + getElementMargins().top + getElementMargins().bottom;
			maxElementWidth = std::max(maxElementWidth, size.x + getElementMargins().left + getElementMargins().right);
		}
		totalWidth += maxElementWidth;

		return {totalWidth, totalHeight};
	}
};