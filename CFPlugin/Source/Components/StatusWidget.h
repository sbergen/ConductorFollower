#pragma once

// JUCE headers last, as usual...
#include "../JuceLibraryCode/JuceHeader.h"

template<typename KeyType, typename ValueType>
class StatusWidget : public Component
{
public:
	~StatusWidget()
	{
		deleteAllChildren();
	}

	void Initialize(ValueType const & value)
	{
		addAndMakeVisible(description_ = new Label("desc", String(KeyType::description().c_str())));

		ValueType::value_type val = value.value();
		addAndMakeVisible(value_ = new Label("val", String(val)));

		layout_.setItemLayout(0, -0.2, -0.8, -0.6);
		layout_.setItemLayout(1, -0.2, -0.8, -0.4);
	}

	void Update(ValueType value)
	{
		ValueType::value_type val;
		if (value.LoadIfChanged(val)) {
			value_->setText(String(val), true);
		}
	}

public: // GUI stuff
	void resized()
    {
        Component* comps[] = { description_, value_ };
        layout_.layOutComponents (comps, 2,
                                    0, 0, getWidth(), getHeight(),
                                    false, true);
    }

private:
	StretchableLayoutManager layout_;
	Label * description_;
	Label * value_;
};
