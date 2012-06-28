#pragma once

#include <string>

namespace cf {
namespace Data {

struct EnvelopeBase
{
	EnvelopeBase(double a, double d, double s)
		: attack(a), decay(d), sustain(s) {}

	double attack;
	double decay;
	double sustain;
};

struct EnvelopeTimes : public EnvelopeBase
{
	EnvelopeTimes() : EnvelopeBase(0.5, 0.5, 1.0) {}
};

struct EnvelopeLevels : public EnvelopeBase
{
	EnvelopeLevels() : EnvelopeBase(0.75, 0.5, 1.0) {}
};

struct InstrumentPatch
{
	std::string name;
	int keyswitch;

	EnvelopeTimes envelopeTimes;
	EnvelopeLevels envelopeLevels;
};

} // namespace Data
} // namespace cf
