#pragma once

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
	Point3D MagnitudeOfMovementSince(timestamp_t const & time) { return dimExtractor_.MagnitudeSince(time); }
	Velocity3D AverageVelocityOfMovementSince(timestamp_t const & time) { return speedExtractor_.AverageVelocitySince(time); }

private:
	void UpdateLatestBeat(timestamp_t const & time);
	void DetectStartGesture();

private: // Buffers
	PositionBuffer positionBuffer_;
	GestureBuffer gestureBuffer_;

private: // Other data
	timestamp_t previousBeat_;

private: // slave extractors
	DimensionFeatureExtractor dimExtractor_;
	SpeedFeatureExtractor speedExtractor_;
};

} // namespace FeatureExtractor
} // namespace cf
