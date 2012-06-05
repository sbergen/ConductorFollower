/*
  ==============================================================================

    This file was auto-generated by the Jucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#ifndef __PLUGINPROCESSOR_H_C2A1EFF0__
#define __PLUGINPROCESSOR_H_C2A1EFF0__

#include <boost/atomic.hpp>
#include <boost/scoped_ptr.hpp>

#include "cf/cf.h"
#include "ScoreFollower/Follower.h"

// JUCE headers last, as usual...
#include "../JuceLibraryCode/JuceHeader.h"
#include "../JuceLibraryCode/JucePluginCharacteristics.h"


//==============================================================================
/**
*/
class CfpluginAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    CfpluginAudioProcessor();
    ~CfpluginAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();

    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);

    //==============================================================================
    AudioProcessorEditor* createEditor();
    bool hasEditor() const;

    //==============================================================================
    const String getName() const;

    int getNumParameters();

    float getParameter (int index);
    void setParameter (int index, float newValue);

    const String getParameterName (int index);
    const String getParameterText (int index);

    const String getInputChannelName (int channelIndex) const;
    const String getOutputChannelName (int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;

    bool acceptsMidi() const;
    bool producesMidi() const;

    //==============================================================================
    int getNumPrograms();
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);

    //==============================================================================
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);

public: // Own stuff...
	boost::atomic<bool> shouldRun;

private:

	bool running_;
	unsigned samplesPerBlock_;

	typedef cf::ScoreFollower::Follower<MidiMessage> ScoreFollower;
	typedef ScoreFollower::BlockBuffer MidiEventBuffer;
	
	boost::scoped_ptr<ScoreFollower> follower_;
	MidiEventBuffer eventBuffer_;


private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CfpluginAudioProcessor);
};

#endif  // __PLUGINPROCESSOR_H_C2A1EFF0__