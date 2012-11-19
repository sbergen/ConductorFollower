/*
  ==============================================================================

    This file was auto-generated by the Jucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "MotionTracker/EventProvider.h"
#include "MotionTracker/Event.h"

#include "ScoreFollower/StatusEvents.h"

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "Components/WidgetHelpers.h"

//==============================================================================
CfpluginAudioProcessorEditor::CfpluginAudioProcessorEditor (CfpluginAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter)
	, ownerFilter(ownerFilter)
	, visualizer_(cf::Visualizer::Visualizer::Create())
	, eventQueue_(ownerFilter->EventProvider().GetEventQueue())
{
    // This is where our plugin's editor size is set.
    setSize (totalWidth, totalHeight);
	BuildUI();
	ownerFilter->changeBroadcaster.addChangeListener(this);
}

CfpluginAudioProcessorEditor::~CfpluginAudioProcessorEditor()
{
	ownerFilter->changeBroadcaster.removeChangeListener(this);
	// The automatic child components are in the fusion::maps,
	// and deleteAllChildren() should NOT be called!
}

void
CfpluginAudioProcessorEditor::BuildUI()
{
	BuildWidgets();

	addAndMakeVisible(visualizer_.get());
	visualizer_->setBounds(optionsWidth, 0, totalWidth, visualizationHeight);
	visualizer_->SetSize(visualizationWidth, visualizationHeight);
}

void
CfpluginAudioProcessorEditor::BuildWidgets()
{
	int yPos = 0;
	BuildOneWidgetSet(yPos, statusWidgetHeight, statusWidgets, ownerFilter->StatusReader());
	BuildOneWidgetSet(yPos, optionWidgetHeight, optionWidgets, ownerFilter->OptionsReader());
}

//==============================================================================
void CfpluginAudioProcessorEditor::paint (Graphics& g)
{
	g.fillAll (Colours::white);

	visualizationData_.reset();
	cf::MotionTracker::Event e;
	while (eventQueue_->DequeueEvent(e)) {
		ConsumeEvent(e);
	}

	cf::ScoreFollower::StatusEvent se;
	while (ownerFilter->StatusEventProvider().DequeueEvent(se)) {
		ConsumeEvent(se);
	}

	if (visualizationData_) {
		auto reader = visualizationData_->GetReader();
		visualizer_->UpdateData(*reader);
	}

	// TODO optimize
	repaint();
}

void
CfpluginAudioProcessorEditor::changeListenerCallback(ChangeBroadcaster * /*source*/)
{
	{
		auto status = ownerFilter->StatusReader();
		WidgetUpdater<FollowerStatusWidgets> updater(statusWidgets);
		boost::fusion::for_each(status->map(), updater);
	}

	{
		auto options = ownerFilter->OptionsWriter();
		WidgetUpdater<FollowerOptionWidgets> updater(optionWidgets);
		boost::fusion::for_each(options->map(), updater);
	}
}

void
CfpluginAudioProcessorEditor::ConsumeEvent(cf::MotionTracker::Event const & e)
{
	using cf::MotionTracker::Event;
	using cf::Visualizer::DataBufferPtr;
	using cf::Visualizer::Position;

	switch (e.type()) {
	case Event::VisualizationData:
#ifdef NDEBUG
		visualizationData_ = e.data<DataBufferPtr>();
#endif
		break;
	case Event::VisualizationHandPosition:
		visualizer_->NewHandPosition(e.timestamp(), e.data<Position>());
		break;
	case Event::Beat:
		visualizer_->NewBeat(e.timestamp());
		break;
	}
}

void
CfpluginAudioProcessorEditor::ConsumeEvent(cf::ScoreFollower::StatusEvent const & e)
{
	using cf::ScoreFollower::StatusEvent;
	using cf::ScoreFollower::BarPhaseEvent;
	using cf::ScoreFollower::BeatEvent;

	switch (e.type()) {
	case StatusEvent::BarPhase:
		visualizer_->NewBarPhase(e.timestamp(), e.data<BarPhaseEvent>().phase);
		break;
	case StatusEvent::Beat:
		{
		auto data = e.data<BeatEvent>();
		visualizer_->NewBeatPhaseInfo(e.timestamp(), data.phase, data.offset);
		}
		break;
	}
}
