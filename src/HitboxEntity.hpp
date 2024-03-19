#pragma once
#include <memory>

#include "ColliderEntity.hpp"
#include "Hitbox.hpp"

class HitboxEntity : public ColliderEntity
{
public:
	template <typename... Args>
	HitboxEntity(const sf::Vector2f& position, Args&&... args) : ColliderEntity(position, std::forward<Args>(args)...)
	{}

	~HitboxEntity() override = default;

protected:
    std::shared_ptr<Hitbox> hitbox = nullptr;
};