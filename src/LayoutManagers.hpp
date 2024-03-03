#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class LayoutManager
{
public:
	LayoutManager()  = default;
	~LayoutManager() = default;

	// Should return std::vector of sf::FloatRects, where n'th rect is the position
	// of the n'th element of the n'th innerElementSize.
	// TODO If elements take up too much space, the availableSpace will be updated.
	// TODO It is then up to the caller entity to increase the space or ignore the info.
	virtual std::vector<sf::FloatRect> doYourThing(sf::FloatRect& availableSpace,
												   const std::vector<sf::Vector2f>& innerElementSizes)
	{
		return std::vector<sf::FloatRect>();
	}
};

class VBoxLayoutManager : public LayoutManager
{
public:
	std::vector<sf::FloatRect> doYourThing(sf::FloatRect& availableSpace,
										   const std::vector<sf::Vector2f>& innerElementSizes) override
	{
		std::vector<sf::FloatRect> toRet = {};

		float totalElementsHeight = 0.f;
		for (auto size : innerElementSizes)
			totalElementsHeight += size.y;

		const float extraSpacing = (availableSpace.height - totalElementsHeight) / (innerElementSizes.size() + 1);
		const float centerX      = availableSpace.left + (availableSpace.width / 2);

		float currentY = availableSpace.top + extraSpacing;

		for (auto size : innerElementSizes)
		{
			toRet.push_back({centerX - (size.x / 2), currentY, size.x, size.y});
			currentY += size.y + extraSpacing;
		}

		return toRet;
	}
};

class HBoxLayoutManager : public LayoutManager
{
public:
	std::vector<sf::FloatRect> doYourThing(sf::FloatRect& availableSpace,
										   const std::vector<sf::Vector2f>& innerElementSizes) override
	{
		std::vector<sf::FloatRect> toRet = {};

		float totalElementsWidth = 0.f;
		for (auto size : innerElementSizes)
			totalElementsWidth += size.x;

		const float extraSpacing = (availableSpace.width - totalElementsWidth) / (innerElementSizes.size() + 1);
		const float centerY      = availableSpace.top + (availableSpace.height / 2);

		float currentX = availableSpace.left + extraSpacing;

		for (auto size : innerElementSizes)
		{
			toRet.push_back({currentX, centerY - (size.y / 2), size.x, size.y});
			currentX += size.x + extraSpacing;
		}

		return toRet;
	}
};