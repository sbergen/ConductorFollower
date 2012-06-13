#include "cf/Logger.h"

#include <iostream>
#include <sstream>

#define LOG(fmt, ...) l.Log((LogItem(fmt), __VA_ARGS__));

using namespace cf;

BOOST_AUTO_TEST_SUITE(LoggerTests)

BOOST_AUTO_TEST_CASE(BasicTest)
{
	std::ostringstream log;
	std::ostringstream expected;
	Logger l(log);

	// Via logging interface
	LOG("plain")
	LOG("One arg: %1%", 42);
	LOG("Five args: %1%, %2%, %3%, %4%, %5%", 1, 2, 3, 4, 5);

	// Check that empty
	BOOST_CHECK_EQUAL(log.str(), "");

	// Now the same manually
	expected << "plain" << std::endl;
	expected << "One arg: 42" << std::endl;
	expected << "Five args: 1, 2, 3, 4, 5"<< std::endl;

	// Commit and check
	l.Commit();
	BOOST_CHECK_EQUAL(log.str(), expected.str());
}

BOOST_AUTO_TEST_SUITE_END()