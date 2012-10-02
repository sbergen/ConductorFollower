#pragma once

#include <fstream>
#include <boost/spirit/include/support_istream_iterator.hpp>

namespace cf {
namespace Data {

template<template<typename, typename> class TGrammar, typename TData>
bool ParseFromFile(std::string const & filename, TData & data)
{
	if (filename == "") { return false; }

	typedef boost::spirit::basic_istream_iterator<char> iterator_type;
	typedef CommentSkipper<iterator_type> skipper_type;

	std::ifstream file(filename);
	assert(file.is_open());
	file.unsetf(std::ios::skipws);

	iterator_type iter(file);
    iterator_type end;
	skipper_type skipper;
	TGrammar<iterator_type, skipper_type> grammar;
	bool success = qi::phrase_parse(iter, end, grammar, skipper, data);

	return (success && iter == end);
}

} // namespace Data
} // namespace cf
