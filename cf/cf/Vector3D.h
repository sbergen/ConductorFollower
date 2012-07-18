#pragma once

#include <boost/array.hpp>
#include <boost/units/quantity.hpp>

namespace cf {

template<typename Unit, typename Rep = double>
class Vector3D
{
public:
	typedef Rep raw_type;
	typedef boost::units::quantity<Unit, Rep> quantity;
	typedef boost::array<raw_type, 3> data_type;

public: // ctors
	Vector3D() { data_.assign(raw_type()); }

	Vector3D(quantity x, quantity y, quantity z)
	{
		set_x(x);
		set_y(y);
		set_z(z);
	}

	Vector3D(data_type const & data)
		: data_(data) {}

public:
	// To quantity
	quantity get_x() const { return quantity::from_value(raw_x()); }
	quantity get_y() const { return quantity::from_value(raw_y()); }
	quantity get_z() const { return quantity::from_value(raw_z()); }

	// quantity setters
	void set_x(quantity const & val) { set_raw_x(val.value()); }
	void set_y(quantity const & val) { set_raw_y(val.value()); }
	void set_z(quantity const & val) { set_raw_z(val.value()); }

	// raw getters
	raw_type raw_x() const { return data_[0]; }
	raw_type raw_y() const { return data_[1]; }
	raw_type raw_z() const { return data_[2]; }

	// raw setters
	void set_raw_x(raw_type const & val) { data_[0] = val; }
	void set_raw_y(raw_type const & val) { data_[1] = val; }
	void set_raw_z(raw_type const & val) { data_[2] = val; }

public: // Access to iteratable data
	data_type const & data() const { return data_; }
	data_type & data() { return data_; }

public: // Transform

	template<typename T, typename F>
	T transform(F f) const
	{
		return T(f(get_x()), f(get_y()), f(get_z()));
	}

private:
	data_type data_;
};

} // namespace cf
