#pragma once

struct GuiMargins
{
	float top    = 0.f;
	float bottom = 0.f;
	float left   = 0.f;
	float right  = 0.f;
	GuiMargins(float top, float bottom, float left, float right) : top(top), bottom(bottom), left(left), right(right) {}
};

enum class GuiAlignment
{
	LEFT   = -1,
	CENTER = 0,
	RIGHT  = 1,
};