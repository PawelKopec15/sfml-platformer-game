#pragma once

#include <memory>

#include "GuiElement.hpp"

class GuiControlElement : public GuiElement
{
public:
	GuiControlElement(/* args */) {}
	~GuiControlElement() {}

	void setNeighbors(const std::shared_ptr<GuiControlElement>& up, const std::shared_ptr<GuiControlElement>& down,
					  const std::shared_ptr<GuiControlElement>& left, const std::shared_ptr<GuiControlElement>& right)
	{
		neighbourUp   = up;
		neighbourDown = down;
		neighbourLeft = left;
		neighbourUp   = up;
	}

protected:
	std::shared_ptr<GuiControlElement> neighbourUp    = nullptr;
	std::shared_ptr<GuiControlElement> neighbourDown  = nullptr;
	std::shared_ptr<GuiControlElement> neighbourLeft  = nullptr;
	std::shared_ptr<GuiControlElement> neighbourRight = nullptr;
};