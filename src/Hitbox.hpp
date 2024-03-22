#pragma once

#include "Area2D.hpp"

class Hitbox : public Area2D
{
public:
	template <typename... Args>
	explicit Hitbox(uint32_t selfMask, uint32_t otherMask, Args&&... args)
		: selfMask(selfMask), otherMask(otherMask), Area2D(std::forward<Args>(args)...)
	{}

	~Hitbox() = default;

	std::pair<bool, uint32_t> hitboxesConnect(Hitbox& other)
	{
		if (!(selfMask & other.getOtherMask()))
			return {false, 0};
		return {intersects(other), selfMask & other.getOtherMask()};
	}

	void setSelfMask(uint32_t val) { selfMask = val; }
	void setOtherMask(uint32_t val) { otherMask = val; }
	uint32_t getSelfMask() { return selfMask; }
	uint32_t getOtherMask() { return otherMask; }

protected:
	uint32_t selfMask;
	uint32_t otherMask;
};