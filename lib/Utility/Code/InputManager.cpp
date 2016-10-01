#include "DisplayView.h"
#include "InputManager.h"
extern float scaleFactorTouch ;
namespace Utility
{
	extern float offset_x;
	namespace Control
	{
		InputManager * InputManager::m_Instace = nullptr;
		std::map<CONTROL_TYPE, bool> m_StatusList;

		CONTROL_TYPE InputManager::convertKeyToControl(EventKeyboard::KeyCode k)
		{
			if (k == EventKeyboard::KeyCode::KEY_ESCAPE)
				return CONTROL_TYPE::BACK_KEY;
			else if (k == EventKeyboard::KeyCode::KEY_SPACE)
				return CONTROL_TYPE::JUMP;
			else if (k == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
				return CONTROL_TYPE::MOVE_FORWARD;
			else if (k == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
				return CONTROL_TYPE::MOVE_BACKWARD;
			else if (k == EventKeyboard::KeyCode::KEY_DELETE)
				return CONTROL_TYPE::CANCEL_SELECT;
			else if (k == EventKeyboard::KeyCode::KEY_UP_ARROW)
				return CONTROL_TYPE::ROTATE_FORWARD;
			else if (k == EventKeyboard::KeyCode::KEY_DOWN_ARROW)
				return CONTROL_TYPE::ROTATE_BACKWARD;
			else if (k == EventKeyboard::KeyCode::KEY_R)
				return CONTROL_TYPE::TOGGLE_ROTATE;
			return CONTROL_TYPE::NONE;
		}

		void InputManager::update(float dt)
		{
			if (heldKeys.size() > 0)
			{
				if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_ESCAPE) != heldKeys.end()) {
					m_currentControl = CONTROL_TYPE::BACK_KEY;
				}
				else if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_SPACE) != heldKeys.end()) {
					m_currentControl = CONTROL_TYPE::JUMP;
				}
				else if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_RIGHT_ARROW) != heldKeys.end()) {
					m_currentControl = CONTROL_TYPE::MOVE_FORWARD;
				}
				else if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_LEFT_ARROW) != heldKeys.end()) {
					m_currentControl = CONTROL_TYPE::MOVE_BACKWARD;
				}
				else if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_DELETE) != heldKeys.end()) {
					m_currentControl = CONTROL_TYPE::CANCEL_SELECT;
				}
				else if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_UP_ARROW) != heldKeys.end()) {
					m_currentControl = CONTROL_TYPE::ROTATE_FORWARD;
				}
				else if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_DOWN_ARROW) != heldKeys.end()) {
					m_currentControl = CONTROL_TYPE::ROTATE_BACKWARD;
				}
				else if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_R) != heldKeys.end()) {
					m_currentControl = CONTROL_TYPE::TOGGLE_ROTATE;
				}
				else
				{
					m_currentControl = CONTROL_TYPE::NONE;
				}
			}

			if (p_isTouchOnScreen == STATE_TOUCH::STATE_RELEASE)
			{
				p_isTouchOnScreen = STATE_TOUCH::STATE_NONE;
				OnClearTouchReleaseList();
			}
		}
		bool InputManager::init()
		{
			//m_Instace = new InputManager();
			p_isTouchOnScreen = STATE_TOUCH::STATE_NONE;
			return true;
		}

		void InputManager::onKeyHold(float interval)
		{

		}
		void InputManager::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
		{
			if (std::find(heldKeys.begin(), heldKeys.end(), keyCode) == heldKeys.end()){
				heldKeys.push_back(keyCode);
			}
		}
		void InputManager::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
		{
			heldKeys.erase(std::remove(heldKeys.begin(), heldKeys.end(), keyCode), heldKeys.end());
		}

		void InputManager::onTouchesBegan(const std::vector<Touch*>& touches, cocos2d::Event  *event)
		{
			for (auto &item : touches)
			{
				auto touch = item;
				TouchPoint * touchPoint = new TouchPoint(touch->getLocation() * scaleFactorTouch , touch);
				touchPoint->pos.x -= offset_x;
				m_TouchList.insert(touch->getID(), touchPoint);
			}
			p_isTouchOnScreen = STATE_TOUCH::STATE_PRESS;
		}
		void InputManager::onTouchesMoved(const std::vector<Touch*>& touches, cocos2d::Event  *event)
		{
			for (auto &item : touches)
			{
				auto touch = item;
				if (m_TouchList.size() > 0)
				{
					auto pTP = m_TouchList.at(touch->getID());
					auto location = touch->getLocation()* scaleFactorTouch;
					location.x -= offset_x;
					pTP->pos = location;
					pTP->touch = touch;
				}
			}
			p_isTouchOnScreen = STATE_TOUCH::STATE_HOLD;
		}
		void InputManager::onTouchesEnded(const std::vector<Touch*>& touches, cocos2d::Event  *event)
		{
			for (auto &item : touches)
			{
				auto touch = item;
				if (m_TouchList.size() > 0)
				{
					auto pTP = m_TouchList.at(touch->getID());
                    m_TouchReleaseList.insert(touch->getID(), pTP);
					m_TouchList.erase(touch->getID());
				}
			}
			p_isTouchOnScreen = STATE_TOUCH::STATE_RELEASE;
		}
		void InputManager::onTouchesCancelled(const std::vector<Touch*>& touches, cocos2d::Event  *event)
		{
			onTouchesEnded(touches, event);
		}

		void InputManager::DispathTouchEvent(cocos2d::EventTouch * event_touch)
		{
			auto event_code = event_touch->getEventCode();
			switch (event_code)
			{
				case EventTouch::EventCode::BEGAN:
				{
					onTouchesBegan(event_touch->getTouches(), event_touch);
					break;
				}
				case EventTouch::EventCode::MOVED:
				{
					onTouchesMoved(event_touch->getTouches(), event_touch);
					break;
				}
				case EventTouch::EventCode::ENDED:
				{
					onTouchesEnded(event_touch->getTouches(), event_touch);
					break;
				}
				case EventTouch::EventCode::CANCELLED:
				{
					onTouchesCancelled(event_touch->getTouches(), event_touch);
					break;
				}
			}
		}

		InputManager::InputManager()
		{
			m_currentControl = CONTROL_TYPE::NONE;
			//this->retain();
		}

		InputManager::~InputManager()
		{
			this->autorelease();
		}

		CONTROL_TYPE InputManager::GetCurrentControlType()
		{
			return m_currentControl;
		}

		cocos2d::Touch* InputManager::GetTouch(int idx )
		{
			if (idx >= m_TouchList.size())
				return nullptr;
			else
				return m_TouchList.at(idx)->touch;
		}

		cocos2d::Touch* InputManager::GetTouchRelease(int idx)
		{
			if (idx >= m_TouchReleaseList.size())
				return nullptr;
			else
				return m_TouchReleaseList.at(idx)->touch;
		}

		cocos2d::Point InputManager::GetPosAtId(int idx)
		{
			if (idx >= m_TouchList.size())
				return cocos2d::Point(0, 0);
			else
				return m_TouchList.at(idx)->pos;
		}

		cocos2d::Point InputManager::GetPosAtIdRelease(int idx)
		{
			if (idx >= m_TouchReleaseList.size())
            {
				return cocos2d::Point(0, 0);
            }
			else
            {
                auto cur_pos = m_TouchReleaseList.at(idx);
                if(cur_pos)
                {
                    return cur_pos->pos;
                }
                return cocos2d::Point(0, 0);
            }
		}
		
		int InputManager::GetNumberTouch()
		{
			return (int)m_TouchList.size();
		}

		int InputManager::GetNumberTouchRelease()
		{
			return (int)m_TouchReleaseList.size();
		}

		bool InputManager::IsPressOnBound(cocos2d::Rect re)
		{
			if (re.containsPoint(m_TouchList.at(0)->pos))
				return true;
			return false;
		}

		bool InputManager::IsReleaseOnbutton(cocos2d::Rect re)
		{
			if (re.containsPoint(m_TouchReleaseList.at(0)->pos))
				return true;
			return false;
		}

		void InputManager::OnClearTouchReleaseList() 
		{ 
			
			m_TouchReleaseList.clear(); 
		}

		bool InputManager::IsTouchOnScreen() 
		{ 
			return p_isTouchOnScreen == STATE_TOUCH::STATE_PRESS; 
		}

		STATE_TOUCH InputManager::GetStateTouchOnScreen() 
		{ 
			return p_isTouchOnScreen; 
		}

		void InputManager::EndTouchOnScreen() 
		{
			p_isTouchOnScreen = STATE_TOUCH::STATE_NONE;
		}

		bool InputManager::IsThisButtonPress(CONTROL_TYPE c)
		{
			return false;
			//return InputManager::GetInstance()->m_StatusList.find(c)->second;
		}
	}
}


