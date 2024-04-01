#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "CollisionBody.hpp"
#include "NineSlice.hpp"

class TerrainBody
{
public:
	struct TerrainProperties
	{
		bool noBottom = false;
		bool noLeft   = false;
		bool noRight  = false;
		bool noTop    = false;
	};

	TerrainBody(const CollisionBody& collisionBody, const std::shared_ptr<NineSlice>& graphical,
				const TerrainProperties& properties)
		: collisionBody(collisionBody), graphical(graphical)
	{
		renderProperties = NineSlice::RenderProperties(
			{properties.noLeft || properties.noTop, properties.noTop, properties.noTop || properties.noRight,
			 properties.noLeft, false, properties.noRight, properties.noLeft || properties.noBottom,
			 properties.noBottom, properties.noRight || properties.noBottom});

		renderRect = sf::FloatRect(this->collisionBody.getPosition(), this->collisionBody.getSize());

		if (properties.noTop)
			renderRect.top -= graphical->getTexTopHeight();
		if (properties.noLeft)
			renderRect.left -= graphical->getTexLeftWidth();
		if (properties.noBottom)
			renderRect.height += graphical->getTexBottomHeight();
		if (properties.noRight)
			renderRect.width += graphical->getTexRightWidth();
	}
	~TerrainBody() = default;

	CollisionBody& accessCollisionBody() { return collisionBody; }

	sf::VertexArray getDrawable()
	{
		return graphical->getDrawable(renderRect.getPosition(), renderRect.getSize(), renderProperties);
	}

	const sf::Texture& getTexture() { return graphical->getTexture(); }

private:
	CollisionBody collisionBody;
	std::shared_ptr<NineSlice> graphical;

	sf::FloatRect renderRect;
	NineSlice::RenderProperties renderProperties;
};