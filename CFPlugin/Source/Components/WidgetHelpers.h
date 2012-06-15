#pragma once

template<typename MapType>
class WidgetInitializer
{
public:
	WidgetInitializer(MapType & map) : map_(map) {}

	template<typename PairType>
	void operator()(PairType & pair) const
	{
		boost::fusion::at_key<typename PairType::first_type>(map_).Initialize(pair.second);
	}

private:
	mutable MapType & map_;
};

template<typename MapType>
class WidgetUpdater
{
public:
	WidgetUpdater(MapType & map) : map_(map) {}

	template<typename PairType>
	void operator()(PairType & pair) const
	{
		boost::fusion::at_key<typename PairType::first_type>(map_).Update(pair.second);
	}

private:
	mutable MapType & map_;
};

template<typename TContainer>
class WidgetCollector
{
public:
	WidgetCollector(TContainer & container) : container_(container) {}

	template<typename PairType>
	void operator()(PairType & pair) const
	{
		container_.push_back(static_cast<Component *>(&pair.second));
	}

private:
	mutable TContainer & container_;
};
