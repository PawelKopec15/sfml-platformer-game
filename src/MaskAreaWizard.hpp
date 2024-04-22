#pragma once

#include <set>
#include <vector>

#include "ChunkMap.hpp"
#include "MaskArea2D.hpp"

class MaskAreaWizard
{
public:
	explicit MaskAreaWizard(const sf::Vector2f& chunkSize) : chunkedAreas(ChunkMap<MaskArea2D>(chunkSize)) {}
	~MaskAreaWizard() = default;

	void addArea(const std::shared_ptr<MaskArea2D>& areaPointer)
	{
		chunkedAreas.insertAuto(areaPointer->getRect(), areaPointer);
	}

	std::set<std::shared_ptr<MaskArea2D>> getValidIntersections(const std::shared_ptr<MaskArea2D>& attackingAreaPointer)
	{
		auto set = chunkedAreas.gatherFromChunks(chunkedAreas.findUnderlyingChunks(attackingAreaPointer->getRect()));

		std::set<std::shared_ptr<MaskArea2D>> toRet;

		for (auto&& ma : set)
		{
			if ((attackingAreaPointer->getTargetMask() & ma->getSelfMask()) && attackingAreaPointer->intersects(*ma))
				toRet.insert(ma);
		}

		return toRet;
	}

private:
	ChunkMap<MaskArea2D> chunkedAreas;
};