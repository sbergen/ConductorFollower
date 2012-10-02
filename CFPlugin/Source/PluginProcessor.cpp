/*
  ==============================================================================

    This file was auto-generated by the Jucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include <sstream>

#include <boost/make_shared.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "cf/serialization.h"
#include "cf/globals.h"
#include "cf/RTContext.h"

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "MidiReader.h"

using namespace cf;
using namespace cf::ScoreFollower;
//using namespace cf::FeatureExtractor;

//==============================================================================
CfpluginAudioProcessor::CfpluginAudioProcessor()
	: eventBuffer_(128)
	, resetting_(false)
{
	follower_ = ScoreFollower::Create(boost::make_shared<MidiReader>());
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

float CfpluginAudioProcessor::getParameter (int /*index*/)
{
    return 0.0f;
}

void CfpluginAudioProcessor::setParameter (int /*index*/, float /*newValue*/)
{
}

const String CfpluginAudioProcessor::getParameterName (int /*index*/)
{
    return String::empty;
}

const String CfpluginAudioProcessor::getParameterText (int /*index*/)
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

bool CfpluginAudioProcessor::isInputChannelStereoPair (int /*index*/) const
{
    return false;
}

bool CfpluginAudioProcessor::isOutputChannelStereoPair (int /*index*/) const
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

void CfpluginAudioProcessor::setCurrentProgram (int /*index*/)
{
}

const String CfpluginAudioProcessor::getProgramName (int /*index*/)
{
    return String::empty;
}

void CfpluginAudioProcessor::changeProgramName (int /*index*/, const String& /*newName*/)
{
}

//==============================================================================
void CfpluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	samplerate_ = sampleRate;
	samplesPerBlock_ = samplesPerBlock;

	follower_->SetBlockParameters(static_cast<unsigned>(sampleRate), samplesPerBlock);
}

void CfpluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void CfpluginAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	RTContext rt; // Debug RT stuff in debug mode

	auto timeAtStartOfBlock = time::now();
	GlobalsRef globals;

	{
		// This will allocate IFF there is an error.
		// Can't put the non-rt specifier within the while...
		NonRTSection nonRt;
		while (globals.ErrorBuffer()->dequeue(errorString_)) {
			juce::NativeMessageBox::showMessageBoxAsync(
				juce::AlertWindow::WarningIcon,
				juce::String("Error!"),
				juce::String(errorString_.c_str()));
		}
	}

	/************************************************************************************/

	// Update UI on every run, TODO make this happen every n times?
	changeBroadcaster.sendChangeMessage();

	if (resetting_.load()) { return; }

	/************************************************************************************/

	unsigned trackCount = follower_->StartNewBlock();

	if (trackCount == 0) {
		MidiPanic(midiMessages);
		return;
	}

	// Track 0 is supposed to be the tempo track
	for (unsigned i = 1; i < trackCount; ++i) {
		follower_->GetTrackEventsForBlock(i, eventBuffer_);
		auto events = eventBuffer_.AllEvents();
		events.ForEach([this, &midiMessages, i](samples_t sample, ScoreEventPtr message)
		{
			assert(sample.value() >= 0);
			assert(sample.value() < samplesPerBlock_);

			MidiMessage const & msg = midi_event_cast(message)->Message();

			// Prohibit all but note on and off for now 
			if (!msg.isNoteOnOrOff()) { 
				return;
			}

			// truncating is probably the right thing to do...
			midiMessages.addEvent(msg, static_cast<unsigned>(sample.value()));
		});
	}

	/*****************************************************************************************/

	// This is the place where you'd normally do the guts of your plugin's
    // audio processing...
	/*
    for (int channel = 0; channel < getNumInputChannels(); ++channel)
    {
        float* channelData = buffer.getSampleData (channel);
    }
	*/

    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
	/*
    for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
	*/

	// However, we reset all channels
	for (int i = 0; i < getNumOutputChannels(); ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }

	/******************************/

	time_quantity elapsedTime = time_cast<time_quantity>(time::now() - timeAtStartOfBlock);
	time_quantity blockSize((samplesPerBlock_ * score::samples) / (samplerate_ * score::samples_per_second));
	if (elapsedTime > (0.3 * blockSize)) {
		LOG("Possible xrun! Process callback took %1% (max: %2%)", elapsedTime, blockSize);
	}
}

void
CfpluginAudioProcessor::MidiPanic(MidiBuffer& midiMessages)
{
	for(int i = 1; i <= 16; ++i) {
		midiMessages.addEvent(MidiMessage::allNotesOff(i), 0);
		midiMessages.addEvent(MidiMessage::allSoundOff(i), 0);
	}
}

void CfpluginAudioProcessor::Reset()
{
	resetting_.store(true);

	// Ensure we have the only reference to follower
	boost::weak_ptr<ScoreFollower> weak_ptr(follower_);
	follower_.reset();
	assert(weak_ptr.expired());

	follower_ = ScoreFollower::Create(boost::make_shared<MidiReader>());
	follower_->SetBlockParameters(static_cast<unsigned>(samplerate_), samplesPerBlock_);

	resetting_.store(false);
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
	auto options = OptionsReader();

	std::ostringstream ss;
    boost::archive::text_oarchive oa(ss);
	oa << *options;

	std::string str = ss.str();
	destData.append(str.data(), str.length());
}

void CfpluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::string str(static_cast<const char *>(data), sizeInBytes);
	std::istringstream ss(str);
	boost::archive::text_iarchive ia(ss);

	auto writer = follower_->options().GetWriter();
	ia >> *writer;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CfpluginAudioProcessor();
}
