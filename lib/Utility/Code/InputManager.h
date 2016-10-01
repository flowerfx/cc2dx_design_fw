#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_
#include "cocos2d.h"
USING_NS_CC;
using namespace std;
namespace Utility
{
	namespace Control
	{
		enum CONTROL_TYPE
		{
			NONE = 0,
			MOVE_FORWARD,
			MOVE_BACKWARD,
			JUMP,
			BACK_KEY,
			CANCEL_SELECT,
			ROTATE_FORWARD,
			ROTATE_BACKWARD,
			TOGGLE_ROTATE,
			TOUCH_BEGAN,
			TOUCH_MOVED,
			TOUCH_ENDED,
			TOUCH_CANCELLED,
			COUNT
		};

		enum STATE_TOUCH
		{
			STATE_NONE = 0,
			STATE_PRESS,
			STATE_HOLD,
			STATE_RELEASE,
			STATE_COUNT
		};

		class TouchPoint : public Ref
		{
		public:
			TouchPoint(cocos2d::Point p , cocos2d::Touch* t) : pos(p){
				touch = new Touch(*t);
                touch->autorelease();
			}

			TouchPoint() : pos(0, 0) { touch = nullptr; }
			~TouchPoint(){ 
				if (touch)
                {
                    touch = nullptr;
                }
			}
			cocos2d::Point pos;
			cocos2d::Touch* touch;
		};
		class InputManager : public Ref
		{
		private:
			static InputManager * m_Instace;
			CONTROL_TYPE m_currentControl;

			vector<EventKeyboard::KeyCode> heldKeys;

			bool HaveEventInput;

			Map<int, TouchPoint*> m_TouchList;
			Map<int, TouchPoint*> m_TouchReleaseList;
			std::map<CONTROL_TYPE, bool> m_StatusList;

			CONTROL_TYPE convertKeyToControl(EventKeyboard::KeyCode k);
			
			STATE_TOUCH p_isTouchOnScreen;
		public:

			static InputManager * GetInstance()
			{
				if (!m_Instace)
				{
					m_Instace = new InputManager();
				}
				return m_Instace;
			}

			CONTROL_TYPE GetCurrentControlType();
			cocos2d::Point GetPosAtId(int idx);
			cocos2d::Point GetPosAtIdRelease(int idx);
			cocos2d::Touch* GetTouch(int idx = 0);
			cocos2d::Touch* GetTouchRelease(int idx = 0);
			
			int GetNumberTouch();
			int GetNumberTouchRelease();
		
			bool IsPressOnBound(cocos2d::Rect re);	
			bool IsReleaseOnbutton(cocos2d::Rect re);
			
			void update(float dt);
			bool init();

			void onKeyHold(float interval);
			void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
			void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
			void onTouchesBegan(const std::vector<Touch*>& touches, cocos2d::Event  *event);
			void onTouchesMoved(const std::vector<Touch*>& touches, cocos2d::Event  *event);
			void onTouchesEnded(const std::vector<Touch*>& touches, cocos2d::Event  *event);
			void onTouchesCancelled(const std::vector<Touch*>& touches, cocos2d::Event  *event);

			void DispathTouchEvent(cocos2d::EventTouch * event_touch);

			void OnClearTouchReleaseList();

			bool IsTouchOnScreen();
			STATE_TOUCH GetStateTouchOnScreen();

			void EndTouchOnScreen();

			bool IsThisButtonPress(CONTROL_TYPE c);

			CREATE_FUNC(InputManager);

			InputManager();
			virtual ~InputManager();
		};
	}
	#define InputMgr Control::InputManager::GetInstance()
}

#endif//_INPUT_MANAGER_H_