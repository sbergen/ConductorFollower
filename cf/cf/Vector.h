#pragma once

#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/core/coordinate_system.hpp>

#include <boost/units/quantity.hpp>
#include <boost/units/systems/si.hpp>

namespace cf {

template<typename Unit, typename Rep = double>
class Vector3D
{
public:
	typedef Rep raw_type;
	typedef boost::units::quantity<Unit, Rep> quantity;

public: // ctors
	Vector3D() : x_(), y_(), z_() {}
	Vector3D(raw_type x, raw_type y, raw_type z) : x_(x), y_(y), z_(z) {}
	Vector3D(quantity x, quantity y, quantity z) : x_(x.value()), y_(y.value()), z_(z.value()) {}

public:
	// To quantity
	quantity get_x() const { return quantity::from_value(x_); }
	quantity get_y() const { return quantity::from_value(y_); }
	quantity get_z() const { return quantity::from_value(z_); }

	// quantity setters
	void set_x(quantity const & val) { x_ = val.value(); }
	void set_y(quantity const & val) { y_ = val.value(); }
	void set_z(quantity const & val) { z_ = val.value(); }

	// raw getters
	raw_type raw_x() const { return x_; }
	raw_type raw_y() const { return y_; }
	raw_type raw_z() const { return z_; }

	// raw setters
	void set_raw_x(raw_type const & val) { x_ = val; }
	void set_raw_y(raw_type const & val) { y_ = val; }
	void set_raw_z(raw_type const & val) { z_ = val; }

protected:
	raw_type x_;
	raw_type y_;
	raw_type z_;
};

struct Point3D : public Vector3D<boost::units::si::length>
{
	Point3D() {}
	Point3D(raw_type x, raw_type y, raw_type z) : Vector3D(x, y, z) {}
	Point3D(quantity x, quantity y, quantity z) : Vector3D(x, y, z) {}
};


struct Velocity3D : public Vector3D<boost::units::si::velocity>
{
	Velocity3D() {}
	Velocity3D(raw_type x, raw_type y, raw_type z) : Vector3D(x, y, z) {}
	Velocity3D(quantity x, quantity y, quantity z) : Vector3D(x, y, z) {}
};

} // namespace cf
