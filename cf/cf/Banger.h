#pragma once

namespace cf {

// A "bangable" object, as in Max/MSP
class Banger
{
public:
	Banger()
		: version_()
		, checkedVersion_()
	{}

	// Default copy, move and assignemnt operators should be ok

public:
	void bang()
	{
		version_++;
	}

	bool peek() const
	{
		return version_ != checkedVersion_;
	}

	bool check() const
	{
		bool ret = peek();
		checkedVersion_ = version_;
		return ret;
	}

private:
	int version_;
	mutable int checkedVersion_;

private: // Serialization
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive &, const unsigned int /*version*/) {}
};

} // namespace cf
