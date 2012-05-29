#pragma once

#include <string>
#include <map>
#include <queue>

#include <XnCppWrapper.h>

#include "MotionTracker/Gestures.h"
#include "MotionTracker/HandTracker.h"
#include "MotionTracker/HandObserver.h"

namespace cf {
namespace MotionTracker {

class OpenNIHandTracker : public HandTracker
{
public:
	OpenNIHandTracker();
	~OpenNIHandTracker();

public:
	// HandTracker implementation
	bool Init();
	bool StartTrackingHand(Gesture gesture, HandObserver & observer);
	bool StopTrackingHand(HandObserver & observer);
	bool WaitForData();

protected:
	virtual void InitNodes();
	void InitCallbacks();

private: // Callbacks

	void GestureRecognizedCallback(
		xn::GestureGenerator& generator,
		const XnChar* gesture,
		const XnPoint3D* idPosition,
		const XnPoint3D* endPosition);

	void GestureProcessCallback(
		xn::GestureGenerator& generator,
		const XnChar* gesture,
		const XnPoint3D* position,
		XnFloat progress);

	void HandCreateCallback(
		xn::HandsGenerator& generator,
        XnUserID id,
		const XnPoint3D* position,
        XnFloat time);

	void HandUpdateCallback(
		xn::HandsGenerator& generator,
        XnUserID id,
		const XnPoint3D* position,
        XnFloat time);
	
	void HandDestroyCallback(
		xn::HandsGenerator& generator,
        XnUserID id,
		XnFloat time);

private: // Hand request stuff

	class OpenNIHandRequest
	{
	public:
		OpenNIHandRequest(Gesture gesture, HandObserver & observer)
			: observer(observer)
			, gestureName(0)
			, isPending(false)
				
		{
			switch(gesture)
			{
			case GestureWave:
				gestureName = "Wave";
				break;
			}
		}

		HandObserver & observer;
		XnChar const * gestureName;
		bool isPending;
	};

	std::queue<OpenNIHandRequest> handRequestQueue_;

	typedef std::map<XnUserID, OpenNIHandRequest> HandMap;
	HandMap hands_;

	bool ProcessNextHandRequest();

private: // OpenNI stuff

	xn::Context context_;

	xn::GestureGenerator gestureGenerator_;
	XnCallbackHandle gestureCallbackHandle_;

	xn::HandsGenerator handsGenerator_;
	XnCallbackHandle handsCallbackHandle_;
};

} // namespace MotionTacker
} // namespace cf
