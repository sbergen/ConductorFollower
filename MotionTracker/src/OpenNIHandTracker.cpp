#include "OpenNIHandTracker.h"

#include <iostream>

#include <boost/make_shared.hpp>
#include <boost/units/systems/si/prefixes.hpp>

#include "cf/RTContext.h"

#include "CallbackWrappers.h"
#include "VisualizationObserver.h"

namespace cf {
namespace MotionTracker {

namespace si = boost::units::si;

OpenNIHandTracker::OpenNIHandTracker()
	: utils_(std::cerr)
	, recorder_(OpenNIRecorder::Disabled, "recording.oni")
{
}

OpenNIHandTracker::~OpenNIHandTracker()
{
	gestureGenerator_.Release();
	handsGenerator_.Release();
	context_.Release();
}

bool
OpenNIHandTracker::Init()
{
	utils_.ResetErrors();

	XnStatus s = context_.Init();
	CheckXnStatus(utils_, s, "Context init");

	recorder_.PreNodeInit(context_);
	InitNodes();
	recorder_.PostNodeInit(context_);
	InitCallbacks();

	s = context_.StartGeneratingAll();
	CheckXnStatus(utils_, s, "Start generating");

	return !utils_.ErrorsOccurred();
}

bool
OpenNIHandTracker::StartTrackingHand(Gesture gesture, HandObserver & observer)
{
	handRequestQueue_.push(OpenNIHandRequest(gesture, observer));
	return ProcessNextHandRequest();
}

bool
OpenNIHandTracker::StopTrackingHand(HandObserver & observer)
{
	// TODO implement
	return true;
}

void
OpenNIHandTracker::AddVisualizationObserver(VisualizationObserver & observer)
{
	// Init buffers to prevent allocations in the future
	xn::DepthMetaData dmd;
	depthGenerator_.GetMetaData(dmd);
	observer.InitVisualizationData(dmd.XRes(), dmd.YRes());

	visualizationObservers_.push_back(boost::ref(observer));
}

bool
OpenNIHandTracker::WaitForData()
{
	XnStatus s = context_.WaitAndUpdateAll();
	if (!CheckXnStatus(utils_, s, "WaitAndUpdateAll")) { return false; }

	NotifyVisualizationObservers();

	return true;
}

void
OpenNIHandTracker::InitNodes()
{
	XnStatus s = gestureGenerator_.Create(context_);
	CheckXnStatus(utils_, s, "Create gesture generator");

	s = handsGenerator_.Create(context_);
	handsGenerator_.SetSmoothing(static_cast<XnFloat>(0.3));
	CheckXnStatus(utils_, s, "Create hands generator");

	s = context_.FindExistingNode(XN_NODE_TYPE_DEPTH, depthGenerator_);
	CheckXnStatus(utils_, s, "Get depth generator");
}

void
OpenNIHandTracker::InitCallbacks()
{
	XnStatus s = gestureGenerator_.RegisterGestureCallbacks(
		&CallbackWrapper<xn::GestureGenerator&, const XnChar*, const XnPoint3D*, const XnPoint3D*, OpenNIHandTracker, &OpenNIHandTracker::GestureRecognizedCallback>,
		&CallbackWrapper<xn::GestureGenerator&, const XnChar*, const XnPoint3D*, XnFloat, OpenNIHandTracker, &OpenNIHandTracker::GestureProcessCallback>,
		this, gestureCallbackHandle_);
	CheckXnStatus(utils_, s, "Register gesture callbacks");

	s = handsGenerator_.RegisterHandCallbacks(
		&CallbackWrapper<xn::HandsGenerator&, XnUserID, const XnPoint3D*, XnFloat, OpenNIHandTracker, &OpenNIHandTracker::HandCreateCallback>,
		&CallbackWrapper<xn::HandsGenerator&, XnUserID, const XnPoint3D*, XnFloat, OpenNIHandTracker, &OpenNIHandTracker::HandUpdateCallback>,
		&CallbackWrapper<xn::HandsGenerator&, XnUserID, XnFloat, OpenNIHandTracker, &OpenNIHandTracker::HandDestroyCallback>,
		this, handsCallbackHandle_);
	CheckXnStatus(utils_, s, "Register hand callbacks");
}

void
OpenNIHandTracker::GestureRecognizedCallback(
	xn::GestureGenerator& generator,
	const XnChar* gesture,
	const XnPoint3D* idPosition,
	const XnPoint3D* endPosition)
{
	OpenNIHandRequest & request = handRequestQueue_.front();

	XnStatus s = gestureGenerator_.RemoveGesture(request.gestureName);
	CheckXnStatus(utils_, s, "Remove gesture tracking");

	s = handsGenerator_.StartTracking(*endPosition);
	CheckXnStatus(utils_, s, "Start hand tracking");
}

void
OpenNIHandTracker::GestureProcessCallback(
	xn::GestureGenerator& generator,
	const XnChar* gesture,
	const XnPoint3D* position,
	XnFloat progress)
{
	// Nothing to do
}

void
OpenNIHandTracker::HandCreateCallback(
	xn::HandsGenerator& generator,
    XnUserID id,
	const XnPoint3D* position,
    XnFloat time)
{
	NonRTSection nonRt; // Not critical

	{
		OpenNIHandRequest & request = handRequestQueue_.front();
		request.isPending = false;
		request.observer.HandFound();
		hands_.insert(HandMap::value_type(id, request));
	}
		
	handRequestQueue_.pop();
	ProcessNextHandRequest();
}

void
OpenNIHandTracker::HandUpdateCallback(
	xn::HandsGenerator& generator,
    XnUserID id,
	const XnPoint3D* position,
    XnFloat time)
{
	// This one is RT critical

	auto it = hands_.find(id);
	if (it != hands_.end())
	{
		coord_t x(position->X * si::milli * si::meter);
		coord_t y(position->Y * si::milli * si::meter);
		coord_t z(position->Z * si::milli * si::meter);

		it->second.observer.NewHandPosition(time, Point3D(x, y, z));

		// Update visualization stat
		XnPoint3D projected;
		depthGenerator_.ConvertRealWorldToProjective(1, position, &projected);
		visualizationHandPos_.x = projected.X;
		visualizationHandPos_.y = projected.Y;
	}
	else
	{
		// TODO handle error
	}
}
	
void
OpenNIHandTracker::HandDestroyCallback(
	xn::HandsGenerator& generator,
    XnUserID id,
	XnFloat time)
{
	NonRTSection nonRt; // Not critical

	auto it = hands_.find(id);
	if (it != hands_.end())
	{
		it->second.observer.HandLost();
	}
	else
	{
		// TODO handle error
	}

	hands_.erase(it);
}


bool
OpenNIHandTracker::ProcessNextHandRequest()
{
	if (handRequestQueue_.empty()) { return true; }

	OpenNIHandRequest & request = handRequestQueue_.front();
	if (request.isPending) { return true; }
		
	request.isPending = true;
	XnStatus s = gestureGenerator_.AddGesture(request.gestureName, 0);
	return CheckXnStatus(utils_, s, "Start tracking gesture");
}

void
OpenNIHandTracker::NotifyVisualizationObservers()
{
	xn::DepthMetaData dmd;
	depthGenerator_.GetMetaData(dmd);

	for (auto it = std::begin(visualizationObservers_); it != std::end(visualizationObservers_); ++it) {
		auto vd = it->get().GetVisualizationData();
		vd->Update(dmd.XRes(), dmd.YRes(), dmd.ZRes(), dmd.FrameID(), dmd.Data());
		vd->SetHandPosition(visualizationHandPos_);
		it->get().NewVisualizationData();
	}
}

} // namespace MotionTracker
} // namespace cf
