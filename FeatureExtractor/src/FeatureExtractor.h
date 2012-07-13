#pragma once

#include <vector>

#include "cf/LockfreeThread.h"

#include "FeatureExtractor/Extractor.h"

#include "types.h"

namespace cf {
namespace FeatureExtractor {

class FeatureExtractor : public Extractor
{
public:
	FeatureExtractor();
	~FeatureExtractor();

public: // Extractor implementation
	void RegisterPosition(timestamp_t const & time, Point3D const & pos);
	Point3D MagnitudeOfMovementSince(timestamp_t const & time);
	Velocity3D AverageVelocitySince(timestamp_t const & time);

	void EnvelopesForTimespans(Box3D & total, std::vector<Box3D> & segments, std::vector<timestamp_t> const & times);
	void AverageVelocityForTimespans(Velocity3D & total, std::vector<Velocity3D> & segments, std::vector<timestamp_t> const & times);

private:
	void DoShortTimeAnalysis();
	void DetectStartGesture();

private: // Buffers
	PositionBuffer positionBuffer_;
	GestureBuffer beatBuffer_;
	GestureBuffer apexBuffer_;

private: // Other data
	Velocity3D prevAvgSeed_;
};

} // namespace FeatureExtractor
} // namespace cf
