#include "cf/Logger.h"

using namespace cf;

BOOST_AUTO_TEST_SUITE(LoggerTests)

BOOST_AUTO_TEST_CASE(BasicTest)
{
	Logger l;
	l.Log(LogItem("foo: %1%") % 1);
	l.Log(LogItem("bar: %1%") % 42);

	std::string foo("foo");
	l.Log(LogItem(foo));

	l.Commit(std::cout);
}

BOOST_AUTO_TEST_SUITE_END()