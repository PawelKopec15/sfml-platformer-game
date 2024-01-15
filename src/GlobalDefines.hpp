#pragma once

#define DELTA_CORRECTION delta / 8192
#define D_GRAV_CONSTANT 0.07f
#define D_TERMINAL_VEL 2.2f

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