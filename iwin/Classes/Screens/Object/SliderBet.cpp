#include "SliderBet.h"
#include "UI/AdvanceUI/WidgetEntity.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wSlider.h"
#include "Utils.h"


SliderBet::SliderBet(Utility::UI_Widget::WidgetEntity* xmlLayer)
{
	p_layer_bet = xmlLayer;

	p_lb_max = p_layer_bet->GetWidgetChildByName(".lb_max_win");
	p_lb_value = p_layer_bet->GetWidgetChildByName(".lb_win_value");
	p_slider = p_layer_bet->GetWidgetChildByName(".slider_bet_money");
	_pos_value_money_y = p_lb_value->GetPosition().y;
	SetRangeValue();
	_currentBet = -1;
	setMinMax(Utils::GetBetValueNew(0), Utils::GetBetValueNew(10));
}
SliderBet::~SliderBet()
{

}

void SliderBet::setMinMax(s64  min, s64 max)
{
	s64 minBet = min;
	s64 maxBet = max;
	
	int idx_min = -1;
	for (size_t i = 0 ; i < _range.size() - 1 ; i++)
	{
		if (min >= _range[i] && min < _range[i+1])
		{
			idx_min = i;
			minBet = _range[i];
			break;
		}
	}
	if (idx_min < 0)
	{
		if (min >= _range[_range.size() - 1])
		{
			minBet = _range[_range.size() - 1];
		}
		else
		{
			minBet = _range[0];
		}
	}
	
	int idx_max = -1;
	for (size_t i = 0; i < _range.size() - 1; i++)
	{
		if (max >= _range[i] && max < _range[i + 1])
		{
			idx_max = i ;
			maxBet = _range[i];
			break;
		}
	}
	if (idx_max < 0)
	{
		if (min >= _range[_range.size() - 1])
		{
			maxBet = _range[_range.size() - 1];
		}
		else
		{
			maxBet = _range[0];
		}
	}

	int delta = abs(idx_max - idx_min);
	static_cast<SliderWidget*>(p_slider)->SetPart(delta);
	static_cast<TextWidget*>(p_lb_max)->SetText(Utils::formatNumber(maxBet));
	if (_currentBet < 0)
	{
		static_cast<TextWidget*>(p_lb_value)->SetText(Utils::formatNumber(minBet));
	}

	_maxBet = maxBet;
	_minBet = minBet;
}

void SliderBet::setValue(int val)
{
	float percent = (_maxBet / val) * 100.f;
	static_cast<SliderWidget*>(p_slider)->SetPercent(percent);
	_value = val;
}

void SliderBet::onChangeValue()
{
	float value = static_cast<SliderWidget*>(p_slider)->GetPercent();
	auto range = rangeValue();
	int index = value * range.size() /100;

	if (range.size() > 0)
	{
		if (index >= range.size())
		{
			index = range.size() - 1;
		}
		_value = range[index];
		static_cast<TextWidget*>(p_lb_value)->SetText(Utils::formatNumber(_value));
		_currentBet = _value;
		p_lb_value->SetPosition(Vec2(p_lb_value->GetPosition().x, _pos_value_money_y + (value/100) *p_slider->getWidth()));
	}
	//int money = Utils::getMaxValidBet(value / 100 * GameController::myInfo->money);
}

float SliderBet::getValue()
{
	return _value;
}

std::vector<s64> SliderBet::rangeValue()
{
	std::vector<s64> range;
	for (auto r : _range)
	{
		if (r >= _minBet && r <= _maxBet)
		{
			range.push_back(r);
		}
	}
	return range;
}

void SliderBet::SetRangeValue()
{
	_range = Utils::GetBetPopularNew();
}