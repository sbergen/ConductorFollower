#pragma once

#include "cf/Logger.h"

#ifdef NDEBUG

#define LOG(...)

#else

#define LOG1(fmt) Globals::Logger().Log(LogItem(fmt));
#define LOG2(fmt, ...) Globals::Logger().Log((LogItem(fmt), __VA_ARGS__));

#define THIRD_ARG(arg1, arg2, arg3, ...) arg3
#define LOG_CHOOSER(...) THIRD_ARG(__VA_ARGS__, LOG2, LOG1)

#define LOG(...) LOG_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

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
