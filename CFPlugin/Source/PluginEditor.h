/*
  ==============================================================================

    This file was auto-generated by the Jucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#ifndef __PLUGINEDITOR_H_D516F0C3__
#define __PLUGINEDITOR_H_D516F0C3__

#include <boost/fusion/include/for_each.hpp>

#include "ScoreFollower/FollowerStatus.h"
#include "ScoreFollower/FollowerOptions.h"
#include "ScoreFollower/StatusEvents.h"

#include "MotionTracker/EventQueue.h"

#include "Visualizer/Data.h"
#include "Visualizer/Visualizer.h"

#include "../JuceLibraryCode/JuceHeader.h"
#include "../JuceLibraryCode/JucePluginCharacteristics.h"
#include "PluginProcessor.h"

#include "Components/StatusWidget.h"
#include "Components/OptionWidget.h"

using namespace cf::ScoreFollower;

//==============================================================================
/**
*/
class CfpluginAudioProcessorEditor
	: public AudioProcessorEditor
	, public ChangeListener
{
public:
    CfpluginAudioProcessorEditor (CfpluginAudioProcessor* ownerFilter);
    ~CfpluginAudioProcessorEditor();

    //==============================================================================
    // This is just a standard Juce paint method...
    void paint (Graphics& g);

public:
	void changeListenerCallback(ChangeBroadcaster *source);

	CfpluginAudioProcessor* ownerFilter;

private: // UI functions
	void BuildUI();
	void BuildWidgets();

private: // UI constants

	// Making dynamic UIs with Juce is retarded,
	// so we just do this...
	static int const optionsWidth = 300;
	static int const optionWidgetHeight = 40;
	static int const statusWidgetHeight = 30;

	static int const visualizationWidth = 700;
	static int const visualizationHeight = 700;

	static int const totalWidth = optionsWidth + visualizationWidth;
	static int const totalHeight = visualizationHeight;

private: // UI Data

	typedef Status::FollowerStatus::transformed<StatusWidget>::type FollowerStatusWidgets;
	FollowerStatusWidgets statusWidgets;

	typedef Options::FollowerOptions::transformed<StatusWidget>::type FollowerOptionWidgets;
	FollowerOptionWidgets optionWidgets;

	cf::Visualizer::Visualizer::Ptr visualizer_;

private: // Event stuff
	void ConsumeEvent(cf::MotionTracker::Event const & e);
	void ConsumeEvent(cf::ScoreFollower::StatusEvent const & e);

	cf::Visualizer::DataBufferPtr visualizationData_;
	boost::shared_ptr<cf::MotionTracker::EventQueue> eventQueue_;


private: // Template functions (just for keeping the stuff above cleaner)
	template<typename TWidgets, typename TSource>
	void BuildOneWidgetSet(int & yPos, int height, TWidgets & widgets, TSource & source)
	{
		WidgetInitializer<TWidgets> initializer(widgets);
		boost::fusion::for_each(source->map(), initializer);

		std::vector<Component *> components;
		WidgetCollector<std::vector<Component *> > collector(components);
		boost::fusion::for_each(widgets, collector);

		for(auto it = components.begin(); it != components.end(); ++it) {
			addAndMakeVisible(*it);
			(*it)->setBounds(0, yPos, optionsWidth, height);
			yPos += height;
		}
	}
};


#endif  // __PLUGINEDITOR_H_D516F0C3__
