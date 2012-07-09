#include "OpenNIHandTracker.h"

#include <iostream>

#include <boost/format.hpp>

#include "CallbackWrappers.h"
#include "OpenNIUtils.h"

namespace cf {
namespace MotionTracker {

OpenNIHandTracker::OpenNIHandTracker()
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
	OpenNIUtils::ResetErrors();

	XnStatus s = context_.Init();
	CheckXnStatus(s, "Context init");

	InitNodes();
	InitCallbacks();

	s = context_.StartGeneratingAll();
	CheckXnStatus(s, "Start generating");

	return !OpenNIUtils::ErrorsOccurred();
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

bool
OpenNIHandTracker::WaitForData()
{
	XnStatus s = context_.WaitAndUpdateAll();
	return CheckXnStatus(s, "WaitAndUpdateAll");
}

void
OpenNIHandTracker::InitNodes()
{
	XnStatus s = gestureGenerator_.Create(context_);
	CheckXnStatus(s, "Create gesture generator");

	s = handsGenerator_.Create(context_);
	CheckXnStatus(s, "Create hands generator");
}

void
OpenNIHandTracker::InitCallbacks()
{
	XnStatus s = gestureGenerator_.RegisterGestureCallbacks(
		&CallbackWrapper<xn::GestureGenerator&, const XnChar*, const XnPoint3D*, const XnPoint3D*, OpenNIHandTracker, &OpenNIHandTracker::GestureRecognizedCallback>,
		&CallbackWrapper<xn::GestureGenerator&, const XnChar*, const XnPoint3D*, XnFloat, OpenNIHandTracker, &OpenNIHandTracker::GestureProcessCallback>,
		this, gestureCallbackHandle_);
	CheckXnStatus(s, "Register gesture callbacks");

	s = handsGenerator_.RegisterHandCallbacks(
		&CallbackWrapper<xn::HandsGenerator&, XnUserID, const XnPoint3D*, XnFloat, OpenNIHandTracker, &OpenNIHandTracker::HandCreateCallback>,
		&CallbackWrapper<xn::HandsGenerator&, XnUserID, const XnPoint3D*, XnFloat, OpenNIHandTracker, &OpenNIHandTracker::HandUpdateCallback>,
		&CallbackWrapper<xn::HandsGenerator&, XnUserID, XnFloat, OpenNIHandTracker, &OpenNIHandTracker::HandDestroyCallback>,
		this, handsCallbackHandle_);
	CheckXnStatus(s, "Register hand callbacks");
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
	CheckXnStatus(s, "Remove gesture tracking");

	s = handsGenerator_.StartTracking(*endPosition);
	CheckXnStatus(s, "Start hand tracking");
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
	auto it = hands_.find(id);
	if (it != hands_.end())
	{
		it->second.observer.NewHandPosition(time, Point3D(position->X, position->Y, position->Z));
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
	return CheckXnStatus(s, "Start tracking gesture");
}

} // namespace MotionTracker
} // namespace cf
