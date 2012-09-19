#pragma once

#include <string>
#include <map>
#include <queue>
#include <vector>

#include <boost/ref.hpp>

#include <XnCppWrapper.h>

#include "Visualizer/Data.h"

#include "Gestures.h"
#include "HandTracker.h"
#include "HandObserver.h"
#include "OpenNIUtils.h"
#include "OpenNIRecorder.h"

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
	void AddVisualizationObserver(VisualizationObserver & observer);
	bool WaitForData();

protected:
	void InitNodes();
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

	OpenNIUtils utils_;

	xn::Context context_;

	xn::GestureGenerator gestureGenerator_;
	XnCallbackHandle gestureCallbackHandle_;

	xn::HandsGenerator handsGenerator_;
	XnCallbackHandle handsCallbackHandle_;

	xn::DepthGenerator depthGenerator_;

private: // Recording

	OpenNIRecorder recorder_;

private: // Visualization observers

	void NotifyVisualizationObservers();

	std::vector<boost::reference_wrapper<VisualizationObserver> > visualizationObservers_;
	Visualizer::DataPtr visualizationData_;
	Visualizer::Data::Position visualizationHandPos_;
};

} // namespace MotionTacker
} // namespace cf
