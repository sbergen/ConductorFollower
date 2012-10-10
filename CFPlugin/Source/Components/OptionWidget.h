#pragma once

#include <sstream>

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include "../JuceLibraryCode/JuceHeader.h"

template<typename KeyType, typename ValueType, int Presentation>
class OptionWidget : public Component, public Slider::Listener
{
	typedef ValueType limited_type;

public:
	OptionWidget()
		: changed_(false)
		, description_(nullptr)
		, slider_(nullptr)
	{}

	~OptionWidget()
	{
		deleteAllChildren();
	}

	void Initialize(ValueType const & value)
	{
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

	void Update(ValueType & value)
	{
		if (!changed_) { return; }
		changed_ = false;

		value = slider_->getValue();
	}

public: // Slider::Listener implementation
	void sliderValueChanged (Slider *)
	{
		changed_ = true;
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
	bool changed_;
	StretchableLayoutManager layout_;
	Label * description_;
	Slider * slider_;
};

// Specialization for Files
template<typename KeyType, typename ValueType>
class OptionWidget<typename KeyType, typename ValueType, cf::Status::File>
	: public Component, public FilenameComponentListener
{
	typedef typename ValueType::value_type actual_type;
	BOOST_STATIC_ASSERT((boost::is_same<actual_type, std::string>::value));

public:
	OptionWidget()
		: changed_(false)
		, description_(nullptr)
		, fileChooser_(nullptr)
	{}

	~OptionWidget()
	{
		deleteAllChildren();
	}

	void Initialize(ValueType const & value)
	{
		addAndMakeVisible(description_ = new Label("desc", String(KeyType::description().c_str())));

		String val(static_cast<std::string>(value).c_str());
		addAndMakeVisible(fileChooser_ = new FilenameComponent(
			"file chooser", val,
			true, false, false, "*", "", "empty"));
		fileChooser_->addListener(this);

		layout_.setItemLayout(0, -0.2, -0.8, -0.5);
		layout_.setItemLayout(1, -0.2, -0.8, -0.5);
	}

	void Update(ValueType & value)
	{
		if (!changed_) { return; }
		changed_ = false;

		File file = fileChooser_->getCurrentFile();
		if (!file.exists()) {
			value = "";
		} else {
			std::ostringstream oss;
			oss << file.getFullPathName();
			value = oss.str();
		}
	}

public: // FilenameComponentListener implementation
	void filenameComponentChanged(FilenameComponent * /*fileComponent*/)
	{
		changed_ = true;
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
	bool changed_;
	StretchableLayoutManager layout_;
	Label * description_;
	FilenameComponent * fileChooser_;
};

// Specialization for bangs
template<typename KeyType, typename ValueType>
class OptionWidget<typename KeyType, typename ValueType, cf::Status::Bang>
	: public Component, public Button::Listener
{
public:
	OptionWidget()
		: clicked_(false)
		, button_(nullptr)
	{}

	~OptionWidget()
	{
		deleteAllChildren();
	}

	void Initialize(ValueType const & /*value*/)
	{
		addAndMakeVisible(button_ = new TextButton("button"));
		button_->setButtonText(String(KeyType::description().c_str()));
		button_->addListener(this);
	}

	void Update(ValueType & value)
	{
		if (!clicked_) { return; }
		clicked_ = false;
		value.bang();
	}

public: // Button::Listener implementation
	void buttonClicked (Button* /*button*/)
	{
		clicked_ = true;
	}

public: // GUI stuff
	void resized()
    {
		button_->setSize(getWidth(), getHeight());
    }

private:
	bool clicked_;
	TextButton * button_;
};
