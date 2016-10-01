#ifndef __BASIC_OBJECT_H__
#define __BASIC_OBJECT_H__

#include "../LoadDataXML.h"
#include "cocostudio/CocoStudio.h"

#include "cocos2d.h"

using namespace cocos2d;
using namespace cocostudio;
namespace Utility
{
	namespace Object
	{
		enum BASIC_ACTION
		{
			BA_ALL = -1,
			BA_NONE = 0,
			BA_MOVE_TO,
			BA_MOVE_BY,
			BA_ROTATE_TO,
			BA_ZOOM_TO,
			BA_FADE_TO,
			BA_TINT_TO,
			BA_MOVE_DELAY_TO,
			BA_MOVE_DELAY_BY,
			BA_ANIMATION,
			BA_PROGRESS,
			BA_COUNT
		};

		enum STATUS_ACTION
		{
			STATUS_IDLE = 0,
			STATUS_ACTIVE
		};

		struct STATE_ACTION
		{
			BASIC_ACTION	Action;
			STATUS_ACTION	Status;

			STATE_ACTION(BASIC_ACTION ac)
			{
				Action = ac;
				Status = STATUS_ACTION::STATUS_ACTIVE;
			}
			STATE_ACTION(BASIC_ACTION ac, STATUS_ACTION st)
			{
				Action = ac;
				Status = st;
			}
		};

		class BasicObject : public Ref
		{
		private:
			/*
			Support 2 style of basic object
			*/
			ProgressTimer * p_ObjectTimer;
			Sprite *	p_Object; //single sprite load from data // this can inclue action from many sprite
			Armature *  p_Armature; //animation of object create from cocos studio
			Label *		p_textLabel; //object is a text
			//
			bool	   p_IsUseAnimatEditor;
			bool	   p_IsText;
			//
			RKList<STATE_ACTION> p_listActionProcess;
			//
			RKList<BASIC_ACTION> p_listActionDone;
			//
			//
			bool	   p_IsVisible;
			bool	   p_IsOnUpdate;

			Color4B	   p_origin_color;

			RKString   p_name_cotain;

		protected:
			//
			float time_move_to;

			void FreeObject();

			virtual void LoadObjectFromSprite(Sprite * p, bool UseAntiAlias = false);
			virtual void LoadAnimateFromEditor(xml::AnimatureDec * animature_dec);
			virtual void LoadFontDecFromXML(xml::FontDec * font_dec);
			virtual void LoadAnimateFromXML(xml::AnimateDec * animation_dec);
			virtual void LoadValueBasicObject(xml::ObjectDec * object_dec);
			virtual void LoadValueProcessObject(xml::ObjectDec * object_dec, BASIC_ACTION LoadState = BA_ALL);

			virtual void UpdateActionProcess(float dt);


			virtual void DrawObject(Renderer *renderer, const Mat4& transform, uint32_t flags) ;
			virtual void VisitObject(Renderer *renderer, const Mat4& transform, uint32_t flags) ;
			virtual bool UpdateObject(float dt);

			bool ContainAction(BASIC_ACTION ba);
			void RemoveAction(BASIC_ACTION ba);
			void AddAction(BASIC_ACTION ba);

			virtual bool OnFinishAction(BASIC_ACTION ba); //return is remove its or not
			virtual void EventActionFinish();

			void OnSetActionCore(BASIC_ACTION type_ac, ActionInterval * ac1, ActionInterval * ac_reserve,int loop = 0, float time_delay = 0.0f, float delay_first = 0);

		public:
			//
			cocos2d::Node*  GetObject();

			void SetBlendFunc(BlendFunc blend);

			BasicObject();
			virtual ~BasicObject();

			virtual void Flip(bool UseY, bool b);

			virtual void Init() = 0;
			virtual void Draw(Renderer *renderer, const Mat4& transform, uint32_t flags) = 0;
			virtual void Visit(Renderer *renderer, const Mat4& transform, uint32_t flags) = 0;
			virtual void Update(float dt) = 0;

			virtual bool OnInitFirstValueBasicObject(RKString name, BASIC_ACTION action = BASIC_ACTION::BA_NONE, bool UseAntiAlias = false, bool IsUseProcessSprite = false);
			virtual bool OnInitFirstValueBasicObjectByObject(xml::ObjectDec * objec, BASIC_ACTION action = BASIC_ACTION::BA_NONE, bool UseAntiAlias = false,bool IsUseProcessSprite = false);
			//control move basic object
			virtual void SetToMoveToPoint(Vec2 point, float time_move);
			virtual void SetToMoveByVector(Vec2 point, float time_move);

			//set action 
			void OnSetProcessScaleAction(float delta_scale = 0.f, float loop = 0.f, float time = 0.0f, bool forceSet = false, bool reserve = true, float delayTime = 0.f, float delay_first = 0.f);
			void OnSetProcessRotateAction(float angle_rotate = 0.f, float loop = 0.f, float time = 0.0f, bool forceSet = false, bool reserve = true, float delayTime = 0.f, float delay_first = 0.f);
			void OnSetProcessFadeAction(int fade_from = -1, int fade_to = -1, float loop = 0.f, float time = 0.0f, bool forceSet = false, bool reserve = true, float delayTime = 0.f, float delay_first = 0.f);
			void OnSetProcessColorAction(Color4B color = Color4B::WHITE, float loop = 0.f, float time = 0.0f, bool forceSet = false, bool reserve = true, float delayTime = 0.f, float delay_first = 0.f);
			void OnSetProcessMoveDelayToAction(Vec2 position, float loop = 0.f, float time = 0.0f, bool forceSet = false, bool reserve = true, float delayTime = 0.f, float delay_first = 0.f);
			void OnSetProcessMoveDelayByAction(Vec2 vector, float loop = 0.f, float time = 0.0f, bool forceSet = false, bool reserve = true, float delayTime = 0.f, float delay_first = 0.f);
			void OnSetProcessProcessByAction(float percent, float loop = 0.f, float time = 0.0f, bool forceSet = false, bool reserve = true, float delayTime = 0.f, float delay_first = 0.f);



			void OnSetProcessScaleActionByDec(xml::BasicDec* dec, float delta_scale = 0.f, float loop = 0.f, float time = 0.0f, bool forceSet = false, bool reserve = true, float delayTime = 0.f, float delay_first=0.f);
			void OnSetProcessRotateActionByDec(xml::BasicDec* dec, float angle_rotate = 0.f, float loop = 0.f, float time = 0.0f, bool forceSet = false, bool reserve = true, float delayTime = 0.f, float delay_first = 0.f);
			void OnSetProcessFadeActionByDec(xml::BasicDec* dec, int fade_from = -1, int fade_to = -1, float loop = 0.f, float time = 0.0f, bool forceSet = false, bool reserve = true, float delayTime = 0.f, float delay_first = 0.f);
			void OnSetProcessColorActionByDec(xml::BasicDec* dec, Color4B color = Color4B::WHITE, float loop = 0.f, float time = 0.0f, bool forceSet = false, bool reserve = true, float delayTime = 0.f, float delay_first = 0.f);
			void OnSetProcessMoveDelayToActionByDec(xml::BasicDec* dec, Vec2 position, float loop = 0.f, float time = 0.0f, bool forceSet = false, bool reserve = true, float delayTime = 0.f, float delay_first = 0.f);
			void OnSetProcessMoveDelayByActionByDec(xml::BasicDec* dec, Vec2 vector, float loop = 0.f, float time = 0.0f, bool forceSet = false, bool reserve = true, float delayTime = 0.f, float delay_first = 0.f);
			void OnSetProcessProcessByActionByDec(xml::BasicDec* dec, float percent, float loop = 0.f, float time = 0.0f, bool forceSet = false, bool reserve = true, float delayTime = 0.f, float delay_first = 0.f);

			//stop action now 

			void OnStopActionIm(BASIC_ACTION ba);
			void OnPauseActionIm(BASIC_ACTION ba);
			void OnResumeActionIm(BASIC_ACTION ba);
			/*

			GET / SET method of basic object

			*/

			void SetTimeMoveTo(float time = 0.0f);

			virtual void SetSize(Vec2 Size);
			virtual Vec2 GetSize();

			virtual void SetScale(Vec2 scale);
			virtual Vec2 GetScale();

			virtual void SetOpacity(RKUtils::BYTE opacity);
			virtual RKUtils::BYTE GetOpacity();

			virtual void SetAngle(float angle);
			virtual float GetAngle();

			virtual void SetOriginColor();
			virtual void SetColor(Color4B color);
			virtual Color4B GetColor();

			virtual void SetPosition(Vec2 pos);
			virtual Vec2 GetPosition();

			cocos2d::Rect GetBound();

			bool IsVisible() { return p_IsVisible; }
			void SetVisible(bool val) { 
				p_IsVisible = val; 
			}

			bool IsOnpUpdate() { return p_IsOnUpdate; }
			void SetOnUpdate(bool val) {
				p_IsOnUpdate = val;
			}

			void SetText(RKString str){
				if (p_IsText)
				{
					p_textLabel->setString(str.GetString());
				}
			}

			int GetActionProcessRemain() {
				return p_listActionProcess.Size();
			}
		};
	}
}
#endif //__BASIC_OBJECT_H__

