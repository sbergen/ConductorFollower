#pragma once

#include <boost/scoped_ptr.hpp>

#include "cf/cf.h"
#include "cf/LockfreeThread.h"

#include "FeatureExtractor/Extractor.h"

#include "types.h"
#include "DimensionFeatureExtractor.h"
#include "SpeedFeatureExtractor.h"

namespace cf {
namespace FeatureExtractor {

class FeatureExtractor : public Extractor
{
public:
	FeatureExtractor();
	~FeatureExtractor();

public: // Extractor implementation
	void RegisterPosition(timestamp_t const & time, Point3D const & pos);
	void GetBeatsSince(timestamp_t const & since, GestureBuffer & beats);
	
	Point3D MagnitudeOfMovementSince(timestamp_t const & time);

private: // Actual feature extraction
	PositionBuffer positionBuffer_;
	DimensionFeatureExtractor dimExtractor_;
	SpeedFeatureExtractor speedExtractor_;
};

} // namespace FeatureExtractor
} // namespace cf
