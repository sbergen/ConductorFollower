#pragma once

#include <sstream>

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include "../JuceLibraryCode/JuceHeader.h"

template<typename KeyType, typename ValueType, int Presentation>
class OptionWidget : public Component, public Slider::Listener
{
	typedef typename ValueType::value_type limited_type;

public:
	~OptionWidget()
	{
		deleteAllChildren();
	}

	void Initialize(ValueType & value)
	{
		value_ = &value;
		addAndMakeVisible(description_ = new Label("desc", String(KeyType::description().c_str())));

		auto val = static_cast<ValueType::value_type>(value);
		addAndMakeVisible(slider_ = new Slider("slider"));
		slider_->setSliderStyle(Slider::LinearHorizontal);
		double stepSize = limited_type::is_integral::value ? 1.0 : 0.0;
		slider_->setRange(limited_type::min_value, limited_type::max_value, stepSize);
		slider_->setValue(val);
		slider_->addListener(this);

		layout_.setItemLayout(0, -0.2, -0.8, -0.5);
		layout_.setItemLayout(1, -0.2, -0.8, -0.5);
	}

public: // Slider::Listener implementation
	void sliderValueChanged (Slider *slider)
	{
		*value_ = slider->getValue();
	}

public: // GUI stuff
	void resized()
    {
        Component* comps[] = { description_, slider_ };
        layout_.layOutComponents (comps, 2,
                                    0, 0, getWidth(), getHeight(),
                                    true, true);
    }

private:
	StretchableLayoutManager layout_;
	Label * description_;
	Slider * slider_;
	ValueType * value_;
};

// Specialization for Files
template<typename KeyType, typename ValueType>
class OptionWidget<typename KeyType, typename ValueType, cf::StatusPresentation::File>
	: public Component, public FilenameComponentListener
{
	typedef typename ValueType::value_type actual_type;
	BOOST_STATIC_ASSERT((boost::is_same<actual_type, std::string>::value));

public:
	~OptionWidget()
	{
		deleteAllChildren();
	}

	void Initialize(ValueType & value)
	{
		value_ = &value;
		addAndMakeVisible(description_ = new Label("desc", String(KeyType::description().c_str())));

		String val(static_cast<std::string>(value).c_str());
		addAndMakeVisible(fileChooser_ = new FilenameComponent(
			"file chooser", val,
			true, false, false, "*", "", "empty"));
		fileChooser_->addListener(this);

		layout_.setItemLayout(0, -0.2, -0.8, -0.5);
		layout_.setItemLayout(1, -0.2, -0.8, -0.5);
	}

public: // FilenameComponentListener implementation
	void filenameComponentChanged(FilenameComponent *fileComponent)
	{
		File file = fileComponent->getCurrentFile();
		if (!file.exists()) {
			*value_ = "";
		} else {
			std::ostringstream oss;
			oss << file.getFullPathName();
			*value_ = oss.str();
		}
	}

public: // GUI stuff
	void resized()
    {
        Component* comps[] = { description_, fileChooser_ };
        layout_.layOutComponents (comps, 2,
                                    0, 0, getWidth(), getHeight(),
                                    true, true);
    }

private:
	StretchableLayoutManager layout_;
	Label * description_;
	FilenameComponent * fileChooser_;
	ValueType * value_;
};
