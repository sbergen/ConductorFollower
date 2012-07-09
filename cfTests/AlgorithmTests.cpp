#include <boost/test/unit_test.hpp>

#include <cmath>
#include <vector>

#include "cf/algorithm.h"

using namespace cf;

BOOST_AUTO_TEST_SUITE(AlgorithmTests)

template<typename T>
struct distance
{
	T operator() (T a, T b)
	{
		return std::abs(a - b);
	}
};

BOOST_AUTO_TEST_CASE(TestNearestNeighbourEmpty)
{
	std::vector<int> v;
	BOOST_CHECK_THROW(nearest_neighbour_linear(v.begin(), v.end(), 0, distance<int>()), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(TestNearestNeighbourSingular)
{
	std::vector<int> v;
	v.push_back(1);
	int best = nearest_neighbour_linear(v.begin(), v.end(), 0, distance<int>());
	BOOST_CHECK_EQUAL(best, 1);
}

BOOST_AUTO_TEST_CASE(TestNearestNeighbourMatching)
{
	std::vector<int> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(30);
	v.push_back(40);
	int best = nearest_neighbour_linear(v.begin(), v.end(), 5, distance<int>());
	BOOST_CHECK_EQUAL(best, 2);
}

BOOST_AUTO_TEST_CASE(TestNearestNeighbourNonMatching)
{
	std::vector<int> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	v.push_back(4);
	int best = nearest_neighbour_linear(v.begin(), v.end(), 2.8, distance<double>());
	BOOST_CHECK_EQUAL(best, 3);
}

BOOST_AUTO_TEST_SUITE_END()