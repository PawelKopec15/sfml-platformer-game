#pragma once

#include <list>
#include <memory>

#include "Collectable.hpp"
#include "MaskArea2D.hpp"

class Inventory
{
public:
	struct InventoryState
	{
		uint32_t coins = 0;
	};

	Inventory()  = default;
	~Inventory() = default;

	void addCoin() { inventoryState.coins++; }

	void addCollectBox(const std::shared_ptr<MaskArea2D>& boxPointer) { collectBoxes.push_back(boxPointer); }
	std::vector<std::shared_ptr<MaskArea2D>> getCollectBoxes() const { return collectBoxes; }

	InventoryState getInventoryState() const { return inventoryState; }
	void setInventoryState(const InventoryState& inventoryState_) { inventoryState = inventoryState_; }

	void checkIfCollectedAnything(std::list<Collectable>& outCollectables)
	{
		for (auto collectBox : collectBoxes)
		{
			auto it = outCollectables.begin();
			while (it != outCollectables.end())
			{
				if ((collectBox->getTargetMask() & it->accessCollectArea().getSelfMask()) &&
					collectBox->intersects(it->accessCollectArea()))
				{
					addCoin();
					it = outCollectables.erase(it);
				}
				else
					++it;
			}
		}
	}

private:
	InventoryState inventoryState;
	std::vector<std::shared_ptr<MaskArea2D>> collectBoxes;
};