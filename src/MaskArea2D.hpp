#pragma once

#include "Area2D.hpp"

class MaskArea2D : public Area2D
{
public:
	MaskArea2D(const sf::Vector2f& position = sf::Vector2f(0, 0), const sf::Vector2f& size = sf::Vector2f(16, 16),
			   uint32_t selfMask = 0, uint32_t targetMask = 0, const sf::Color& color = sf::Color(0, 50, 255, 96))
		: Area2D(position, size, color), selfMask(selfMask), targetMask(targetMask)
	{}
	~MaskArea2D() = default;

	uint32_t getSelfMask() const { return selfMask; }
	void setSelfMask(const uint32_t& val) { selfMask = val; }

	uint32_t getTargetMask() const { return targetMask; }
	void setTargetMask(const uint32_t& val) { targetMask = val; }

private:
	uint32_t selfMask;
	uint32_t targetMask;
};