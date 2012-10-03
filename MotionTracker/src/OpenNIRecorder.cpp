#include "OpenNIRecorder.h"

namespace cf {
namespace MotionTracker {

OpenNIRecorder::OpenNIRecorder(Mode mode, const XnChar * filename)
	: mode_(mode)
	, filename_(filename)
	, utils_()
{
}

OpenNIRecorder::~OpenNIRecorder()
{
	recorder_.Release();
	player_.Release();
}

void
OpenNIRecorder::PreNodeInit(xn::Context & context)
{
	XnStatus s;

	switch (mode_)
	{
	case Playback:
		s = context.OpenFileRecording(filename_, player_);
		CheckXnStatus(utils_, s, "Init playback");
		break;
	case Record:
	case Disabled:
		// Do nothing
		break;
	}
}

void
OpenNIRecorder::PostNodeInit(xn::Context & context)
{
	XnStatus s;
	xn::ProductionNode node;

	switch (mode_)
	{
	case Record:
		s = recorder_.Create(context);
		CheckXnStatus(utils_, s, "Create recorder");

		s = recorder_.SetDestination(XN_RECORD_MEDIUM_FILE, filename_);
		CheckXnStatus(utils_, s, "Init recorder");

		s = context.FindExistingNode(XN_NODE_TYPE_DEPTH, node);
		CheckXnStatus(utils_, s, "Get Depth Node");

		s = recorder_.AddNodeToRecording(node);
		CheckXnStatus(utils_, s, "Add node to recorder");
		break;
	case Playback:
	case Disabled:
		// Do nothing
		break;
	}
}

} // namespace MotionTacker
} // namespace cf
