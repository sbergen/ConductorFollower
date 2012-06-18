/*
  ==============================================================================

    This file was auto-generated by the Jucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "ScoreFollower/TimeUtils.h"

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "MidiReader.h"
#include "MidiManipulator.h"

using namespace cf;
using namespace cf::ScoreFollower;
using namespace cf::FeatureExtractor;

//==============================================================================
CfpluginAudioProcessor::CfpluginAudioProcessor()
	: shouldRun(false)
	, running_(false)
	, eventBuffer_(100)
	, trackCount_(0)
{
}

CfpluginAudioProcessor::~CfpluginAudioProcessor()
{
}

//==============================================================================
const String CfpluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int CfpluginAudioProcessor::getNumParameters()
{
    return 0;
}

float CfpluginAudioProcessor::getParameter (int index)
{
    return 0.0f;
}

void CfpluginAudioProcessor::setParameter (int index, float newValue)
{
}

const String CfpluginAudioProcessor::getParameterName (int index)
{
    return String::empty;
}

const String CfpluginAudioProcessor::getParameterText (int index)
{
    return String::empty;
}

const String CfpluginAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String CfpluginAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool CfpluginAudioProcessor::isInputChannelStereoPair (int index) const
{
    return false;
}

bool CfpluginAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return false;
}

bool CfpluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool CfpluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

int CfpluginAudioProcessor::getNumPrograms()
{
    return 0;
}

int CfpluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CfpluginAudioProcessor::setCurrentProgram (int index)
{
}

const String CfpluginAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void CfpluginAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void CfpluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	samplesPerBlock_ = samplesPerBlock;

	follower_.reset(new ScoreFollower(sampleRate, samplesPerBlock));

	MidiReader reader("C:\\sample.mid");
	follower_->CollectData(reader);
	trackCount_ = reader.TrackCount();

	// Lets see...
	shouldRun.store(true);
}

void CfpluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void CfpluginAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{

	/************************************************************************************/

	if (!shouldRun.load()) {
		running_ = false;
		return;
	}

	if (followerStatus().HasSomethingChanged())
	{
		changeBroadcaster.sendChangeMessage();
	}

	/************************************************************************************/

	// Track 0 is supposed to be the tempo track
	for (int i = 1; i < trackCount_; ++i) {
		follower_->StartNewBlock();
		follower_->GetTrackEventsForBlock(i, ::MidiManipulator(), eventBuffer_);
		auto events = eventBuffer_.AllEvents();

		while(!events.AtEnd()) {
			assert(events.timestamp() >= 0);
			assert(events.timestamp() < samplesPerBlock_);

			MidiMessage msg = events.data();
			midiMessages.addEvent(events.data(), events.timestamp());
			events.Next();
		}
	}

	/*****************************************************************************************/

	// This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    for (int channel = 0; channel < getNumInputChannels(); ++channel)
    {
        float* channelData = buffer.getSampleData (channel);
    }

    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
}

//==============================================================================
bool CfpluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* CfpluginAudioProcessor::createEditor()
{
    return new CfpluginAudioProcessorEditor (this);
}

//==============================================================================
void CfpluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CfpluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CfpluginAudioProcessor();
}
