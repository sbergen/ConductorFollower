#pragma once

#include <fstream>

#include <boost/format.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

#include "Data/ParseException.h"

namespace cf {
namespace Data {

template<template<typename, typename> class TGrammar, typename TData>
void ParseFromFile(std::string const & filename, TData & data)
{
	typedef boost::spirit::basic_istream_iterator<char> iterator_type;
	typedef CommentSkipper<iterator_type> skipper_type;

	std::ifstream file(filename);

	if (!file.is_open())
	{
		auto error = boost::format("Unable to open file \"%1%\" for parsing") % filename;
		throw ParseException(error.str());
	}
	
	file.unsetf(std::ios::skipws);

	iterator_type iter(file);
    iterator_type end;
	skipper_type skipper;
	TGrammar<iterator_type, skipper_type> grammar;
	bool success = qi::phrase_parse(iter, end, grammar, skipper, data);

	if (!success && iter != end) {
		auto error = boost::format("Failed to parse file %1%") % filename;
		throw ParseException(error.str());
	}
}

} // namespace Data
} // namespace cf
