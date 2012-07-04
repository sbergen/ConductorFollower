#pragma once

#include "StringExtractor.h"

#include "../JuceLibraryCode/JuceHeader.h"

// Basic type, shows text. Specializations and template selctor below
template<typename KeyType, typename ValueType, int Presentation>
class InformationWidget : public Component
{
	typedef typename ValueType::value_type value_type;

public:
	~InformationWidget()
	{
		deleteAllChildren();
	}

	void Initialize(ValueType const & value)
	{
		addAndMakeVisible(description_ = new Label("desc", String(KeyType::description().c_str())));

		auto val = static_cast<value_type>(value);
		addAndMakeVisible(value_ = new Label("val", ExtractString(val)));

		layout_.setItemLayout(0, -0.2, -0.8, -0.3);
		layout_.setItemLayout(1, -0.2, -0.8, -0.7);
	}

	void Update(ValueType const & value)
	{
		value_type val;
		if (value.LoadIfChanged(val)) {
			value_->setText(ExtractString(val), true);
		}
	}

private:
	String ExtractString(value_type const & val)
	{
		return StringExtractor<value_type>()(val);
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

// Bar graph specialization
template<typename KeyType, typename ValueType>
class InformationWidget<KeyType, ValueType, cf::Status::Bar> : public Component
{
	typedef typename ValueType::value_type value_type;

public:
	InformationWidget() : value_(nullptr) {}

	~InformationWidget()
	{
		deleteAllChildren();
	}

	void Initialize(ValueType const & value)
	{
		value_ = &value;

		addAndMakeVisible(description_ = new Label("desc", String(KeyType::description().c_str())));

		addAndMakeVisible(valueRect_ = new DrawableRectangle());

		layout_.setItemLayout(0, -0.2, -0.8, -0.5);
		layout_.setItemLayout(1, -0.2, -0.8, -0.5);

		Update(value, true);
	}

	void Update(ValueType const & value, bool forceUpdate = false)
	{
		value_ = &value;

		value_type val;
		if (forceUpdate) {
			val = static_cast<value_type>(value);
		} else {
			if (!value.LoadIfChanged(val)) { return; }
		}
		
		int range = value_type::max_value - value_type::min_value;
		float fraction = (val - value_type::min_value) / range;

		if (fraction < 0.01) {
			valueRect_->setFill(FillType(Colours::red));
			fraction = 0.01;
		} else if(fraction > 0.99) {
			valueRect_->setFill(FillType(Colours::red));
			fraction = 1.0;
		} else {
			valueRect_->setFill(FillType(Colours::green));
		}

		Rectangle<float> rect(
			valueRect_->getX(), valueRect_->getY(),
			fraction * getWidth(), valueRect_->getHeight());
		valueRect_->setRectangle(rect);
	}

public: // GUI stuff
	void resized()
    {
        Component* comps[] = { description_, valueRect_ };
        layout_.layOutComponents (comps, 2,
                                    0, 0, getWidth(), getHeight(),
                                    true, true);

		if (value_) { Update(*value_, true); }
    }

private:
	ValueType const * value_;
	StretchableLayoutManager layout_;
	Label * description_;
	DrawableRectangle * valueRect_;
};
