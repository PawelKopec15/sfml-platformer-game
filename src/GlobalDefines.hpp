#pragma once

#define DELTA_CORRECTION ((float)delta / 8192.f)
#define D_GRAV_CONSTANT 0.055f
#define D_TERMINAL_VEL 2.4f

// class Singleton
// {
// public:
// 	static Singleton &Get()
// 	{
// 		static Singleton INSTANCE;
// 		return INSTANCE;
// 	}
// 	Singleton(Singleton &&)      = delete;
// 	Singleton(const Singleton &) = delete;
// 	Singleton &operator=(Singleton &&) = delete;
// 	Singleton &operator=(const Singleton &) = delete;

// private:
// 	Singleton() = default;
// };