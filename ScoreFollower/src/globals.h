#pragma once

#include "cf/Logger.h"

#ifdef NDEBUG
	#define LOG(...)
#else
	#define LOG(fmt, ...) Globals::Logger().Log((LogItem(fmt), __VA_ARGS__))
#endif

namespace cf {
namespace ScoreFollower {

class Globals
{
private:
	friend class GlobalsInitializer;

	static void Initialize();
	static void CleanUp();

public:
	static FileLogger & Logger();

private:
	static bool initialized_;
	static FileLogger * logger_;
};

class GlobalsInitializer
{
public:
	GlobalsInitializer() { Globals::Initialize(); }
	~GlobalsInitializer() { Globals::CleanUp(); }
};

} // namespace ScoreFollower
} // namespace cf
