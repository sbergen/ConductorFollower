/*
  ==============================================================================

    This file was auto-generated by the Jucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
CfpluginAudioProcessorEditor::CfpluginAudioProcessorEditor (CfpluginAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter)
	, playButton(0)
{
    // This is where our plugin's editor size is set.
    setSize (400, 300);

	addAndMakeVisible(playButton = new TextButton("Play"));
	playButton->setBounds(0, 0, 100, 100);

}

CfpluginAudioProcessorEditor::~CfpluginAudioProcessorEditor()
{
	deleteAllChildren();
}

//==============================================================================
void CfpluginAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::white);

	/*
    g.setColour (Colours::black);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!",
                      0, 0, getWidth(), getHeight(),
                      Justification::centred, 1);
	*/

	
}

void
CfpluginAudioProcessorEditor::buttonClicked(Button * button)
{

}

void
CfpluginAudioProcessorEditor::buttonStateChanged(Button * button)
{

}
