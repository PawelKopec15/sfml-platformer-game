#pragma once

#define DELTA_CORRECTION ((float)delta / 8192.f)
#define D_GRAV_CONSTANT 0.063f
#define D_TERMINAL_VEL 2.4f

// class GlobalVariables
// {
// public:
// 	static GlobalVariables &Get()
// 	{
// 		static GlobalVariables INSTANCE;
// 		return INSTANCE;
// 	}
// 	GlobalVariables(GlobalVariables &&)      = delete;
// 	GlobalVariables(const GlobalVariables &) = delete;
// 	GlobalVariables &operator=(GlobalVariables &&) = delete;
// 	GlobalVariables &operator=(const GlobalVariables &) = delete;

// 	float getGravityConstant() const { return gravityConstant; }
// 	void setGravityConstant(float val) { gravityConstant = val; }

//     float getTerminalVelocity() const { return gravityConstant; }
// 	void setTerminalVelocity(float val) { gravityConstant = val; }

// private:
// 	GlobalVariables() = default;

// 	float gravityConstant  = 0.000005;
// 	float terminalVelocity = 0.0003;
// };