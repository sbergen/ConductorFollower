#include "cf/Logger.h"

using namespace cf;

#define LOG(fmt, ...) l.Log((LogItem(fmt), __VA_ARGS__));

#include <iostream>

BOOST_AUTO_TEST_SUITE(LoggerTests)

BOOST_AUTO_TEST_CASE(BasicTest)
{
	Logger l(std::cout);

	LOG("plain")
	LOG("One arg: %1%", 42);
	LOG("Three args: %1%", 1, 2, 3);

	l.Commit();
}

BOOST_AUTO_TEST_SUITE_END()