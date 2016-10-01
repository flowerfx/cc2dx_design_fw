#include "BGObject.h"
#if USE_DYNAMIC_BG

namespace Utility
{
	namespace Background
	{
		BGLayer::BGLayer()
		{
			p_name_layer = "";
			p_name_background = "";
			p_LoopAction = -1; //last forever;
		}

		BGLayer::~BGLayer()
		{
			for (int i = 0; i < p_ListObject.size(); i++)
			{
				p_ListObject.at(i)->autorelease();
			}
			p_ListObject.clear();
			p_listIndexObjectFinish.clear();
		}

		void BGLayer::Init(xml::LayerData * layer , RKString name_bg)
		{
			if (!layer)
			{
				CCASSERT(false, "ERROR ! layer invalid");
				return;
			}

			p_name_background = name_bg;
			p_name_layer = layer->p_NameLayer;
			p_index = layer->p_index_layer;
			p_number_object = layer->GetNumberObject();
			p_LoopAction = layer->p_loopAction;

			for (int i = 0; i < p_number_object; i++)
			{
				BGEntity* _bg_entity = generateBGEntityWithParam(i > 0 ? p_ListObject.at(i - 1) : NULL, layer);
				p_ListObject.insert(p_ListObject.size(), _bg_entity);
			}
		}

		void BGLayer::InsertToListObjFinish(int idx)
		{
			for (size_t i = 0; i < p_listIndexObjectFinish.size(); i++)
			{
				if (idx == p_listIndexObjectFinish[i])
				{
					return;
				}
			}
			p_listIndexObjectFinish.push_back(idx);

			//sort max
			//int max_number = idx;
			//for (int i = 0; i < p_listIndexObjectFinish.size(); i++)
			//{
			//	if (p_listIndexObjectFinish[i] > max_number)
			//	{
			//		p_listIndexObjectFinish.erase(p_listIndexObjectFinish.begin() + i);
			//		p_listIndexObjectFinish.insert(p_listIndexObjectFinish.begin() + i, max_number);
			//	}
			//}
		}

		void BGLayer::OnRemoveObjectFinish()
		{
			int size_object_remove = p_listIndexObjectFinish.size();

			if (p_LoopAction == 0)
			{

				p_listIndexObjectFinish.clear();
				size_object_remove = 0;
			}
			else if (p_LoopAction < 0)
			{
				p_LoopAction = -1;
			}
			//
			if (size_object_remove <= 0)
				return;

			p_LoopAction--;

			for (int i = size_object_remove - 1; i >= 0; i--)
			{
				int idx = p_listIndexObjectFinish[i];
				//
				p_ListObject.at(idx)->release();
				p_ListObject.erase(idx);
			}
			auto _data_background = XMLMgr->GetBackground(p_name_background);
			auto _data_layer = _data_background->GetLayerData(RKString(std::to_string(p_index)) + "#" + p_name_layer);
			//add object base on number remove
			for (int i = 0; i < size_object_remove; i++)
			{
				BGEntity* _bg_entity = generateBGEntityWithParam(p_ListObject.size() > 0 ? p_ListObject.at(p_ListObject.size() - 1) : NULL, _data_layer);
				p_ListObject.insert(p_ListObject.size(), _bg_entity);
			}
			//
			p_listIndexObjectFinish.clear();
			p_previous_size_remove = size_object_remove;

		}

		void BGLayer::Update(float dt)
		{
			for (int i = 0; i < p_ListObject.size(); i++)
			{
				p_ListObject.at(i)->Update(dt);
				if (p_ListObject.at(i)->IsFinishAction())
				{
					InsertToListObjFinish(i);
				}
			}
		}
		
		void BGLayer::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			for (int i = 0; i < p_ListObject.size(); i++)
			{
				p_ListObject.at(i)->Draw(renderer, transform, flags);
			}
		}
		
		void BGLayer::Visit(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			for (int i = 0; i < p_ListObject.size(); i++)
			{
				p_ListObject.at(i)->Visit(renderer, transform, flags);
			}
		}

		BGEntity* BGLayer::generateBGEntityWithParam(BGEntity* previous, xml::LayerData * layer_contain)
		{
			BGEntity * _bg_entity = new BGEntity();

			int number_object_contain = layer_contain->p_listObject.size();
			static int idx_contain = 0;
			if (layer_contain->p_isObjectRandom)
			{
				idx_contain = GetRandom(0, number_object_contain - 1);
			}
			else
			{
				idx_contain++;
				if (idx_contain >= layer_contain->p_listObject.size())
				{
					idx_contain = 0;
				}
			}
			xml::ObjectDec * _obj_dec = layer_contain->GetObjectBGDecByIdx(idx_contain);

			Vec2 pos = layer_contain->p_list_layer_element_dec.at("Pos")->GetDataVector2("origin");
			pos.x = pos.x * GetGameSize().width;
			pos.y = pos.y * GetGameSize().height;

			if (layer_contain->p_IsObjectPosRandom )
			{
				cocos2d::Rect RectAppear__ = layer_contain->GetBoundRandomPos();

				int random_x = GetRandom(RectAppear__.origin.x, RectAppear__.origin.x + RectAppear__.size.width);
				int random_y = GetRandom(RectAppear__.origin.y, RectAppear__.origin.y + RectAppear__.size.height);
				pos.x = random_x;
				pos.y = random_y;
			}
			//
			_bg_entity->OnInitFirstValueBasicObjectByObject(_obj_dec);
			if (!previous || layer_contain->p_IsObjectPosRandom) //is the first bg 
			{
				_bg_entity->SetPositionBG(pos);
			}
			else
			{
				int distance_each_obj = layer_contain->GetDistanceObject();
				Vec2 _pos = previous->GetPositionBG();
				//
				Vec2 _direct = Vec2(layer_contain->direct_x_Obj(), layer_contain->direct_y_Obj());
				//check the specific
				if (_direct.x == 0 && _direct.y > 0)
				{
					_pos.y = _pos.y - (previous->GetSizeBG().y / 2.f);
				}
				else if (_direct.x == 0 && _direct.y < 0)
				{
					_pos.y = _pos.y + (previous->GetSizeBG().y / 2.f);
				}
				else if (_direct.x > 0 && _direct.y == 0)
				{
					_pos.x = _pos.x - (previous->GetSizeBG().x / 2.f);
				}
				else if (_direct.x < 0 && _direct.y == 0)
				{
					_pos.x = _pos.x + (previous->GetSizeBG().x / 2.f);
				}
				//
				Vec2 _pos_ = Vec2(_pos.x + _direct.x * distance_each_obj, _pos.y + _direct.y * distance_each_obj);
				//check the specific
				if (_direct.x == 0 && _direct.y > 0)
				{
					_pos_.y = _pos_.y - (_bg_entity->GetSizeBG().y / 2.f);
				}
				else if (_direct.x == 0 && _direct.y < 0)
				{
					_pos_.y = _pos_.y + (_bg_entity->GetSizeBG().y / 2.f);
				}
				else if (_direct.x > 0 && _direct.y == 0)
				{
					_pos_.x = _pos_.x - (_bg_entity->GetSizeBG().x / 2.f);
				}
				else if (_direct.x < 0 && _direct.y == 0)
				{
					_pos_.x = _pos_.x + (_bg_entity->GetSizeBG().x / 2.f);
				}
				//
				_bg_entity->SetPositionBG(_pos_);
			}

			if (layer_contain->p_IsScaleRandom)
			{
				Vec2 _size_entity = _bg_entity->GetSizeBG();
				float value_random = layer_contain->GetScaleRandom();
				_size_entity.x = _size_entity.x * value_random;
				_size_entity.y = _size_entity.y * value_random;
				_bg_entity->SetSizeBG(_size_entity);
			}
			//
			if (layer_contain->p_IsRotateRandom)
			{
				float value_random = layer_contain->GetRotateRandom();
				_bg_entity->SetAngle(value_random);
			}
			//
			if (layer_contain->p_IsOpacityRandom)
			{
				RKUtils::BYTE   value_random = layer_contain->GetOpacityRandom();
				_bg_entity->SetOpacity(value_random);
			}
			//
			if (layer_contain->p_IsColorRandom)
			{
				Color4B value_random = layer_contain->GetColorRandom();
				_bg_entity->SetColor(value_random);
			}

			_bg_entity->SetActionDec(layer_contain->p_list_layer_element_dec);

			xml::BasicDec*__data_layer = nullptr;		
			//ppos
			float _time_move = 0;
			float _delay_time_move = 0;
			__data_layer = layer_contain->p_list_layer_element_dec.at("Pos");
			Vec2 vector_move__ = Vec2(0, 0);
			if (__data_layer && __data_layer->GetDataVector2("vector") != Vec2(0, 0))
			{
				Vec2 vector_move__ = __data_layer->GetDataVector2("vector");
				vector_move__.x = vector_move__.x * GetGameSize().width;
				vector_move__.y = vector_move__.y * GetGameSize().height;

				float delay_time_move = __data_layer->GetDatafloat("delay");
				Vec2 vector_move_nor = Vec2(0, 0);
				if (__data_layer->GetDataInt("type") == 0)
				{
					Vec2 vector_move = vector_move__;
				

					vector_move_nor = vector_move;
					//
					vector_move.x = vector_move.x + _bg_entity->GetPositionBG().x;
					vector_move.y = vector_move.y + _bg_entity->GetPositionBG().y;
					//get time
					float time_move = __data_layer->GetDatafloat("time");
					//delay time
					float delay_time_move = __data_layer->GetDatafloat("delay");
					//
					if (layer_contain->p_IsObjectPosRandom)
					{
						time_move = layer_contain->GetTimeMoveRandom();
						Vec2 direct_move_ran = layer_contain->GetVectorMoveRandom();
						if (direct_move_ran != Vec2(0, 0))
						{
							vector_move_nor = direct_move_ran;
							vector_move.x = direct_move_ran.x + _bg_entity->GetPositionBG().x;
							vector_move.y = direct_move_ran.y + _bg_entity->GetPositionBG().y;
						}
						float delay_time_move__ = layer_contain->GetDelayTimeRandom();
						if (delay_time_move__ != 0)
						{
							delay_time_move = delay_time_move__;
						}
					}
					//
					if (delay_time_move <= 0)
					{
						_bg_entity->SetToMoveByVector(vector_move, time_move);
					}
					else
					{
						_bg_entity->OnSetProcessMoveDelayByActionByDec(
							__data_layer, vector_move, 0, time_move, false, true, delay_time_move
							);
					}
					vector_move_nor.normalize();
					_delay_time_move = delay_time_move;
					_time_move = time_move;
				}
				else
				{
					Vec2 point_move_to = vector_move__;
					//
					//get time
					float time_move = __data_layer->GetDatafloat("time");
					//delay time
					float delay_time_move = __data_layer->GetDatafloat("delay");
					//
					if (layer_contain->p_IsObjectPosRandom)
					{
						time_move = layer_contain->GetTimeMoveRandom();
						Vec2 direct_move_ran = layer_contain->GetVectorMoveRandom();
						if (direct_move_ran != Vec2(0, 0))
						{
							point_move_to.x = direct_move_ran.x;
							point_move_to.y = direct_move_ran.y;
						}
						float delay_time_move__ = layer_contain->GetDelayTimeRandom();
						if (delay_time_move__ != 0)
						{
							delay_time_move = delay_time_move__;
						}
					}

	

					if (delay_time_move <= 0)
					{
						_bg_entity->SetToMoveToPoint(point_move_to, time_move);
					}
					else
					{
						_bg_entity->OnSetProcessMoveDelayToActionByDec(
							__data_layer, point_move_to, 0, time_move, false, true, delay_time_move
							);
					}
					vector_move_nor.x = point_move_to.x - _bg_entity->GetPositionBG().x;
					vector_move_nor.y = point_move_to.y - _bg_entity->GetPositionBG().y;
					vector_move_nor.normalize();
					_delay_time_move = delay_time_move;
					_time_move = time_move;
				}
				//
				float rotate_invoke = __data_layer->GetDataInt("rotate_invoke");
				if (rotate_invoke >= 0)
				{
					Vec2 vector_object_bg = Vec2(-1, 0);

					float dot_product = vector_object_bg.dot(vector_move_nor);
					float len1 = vector_object_bg.length();
					float len2 = vector_move_nor.length();

					float cos_a = dot_product / (len1 * len2);
					float sin_a = sqrt(1 - cos_a* cos_a);

					if ((vector_move_nor.x > 0 && vector_move_nor.y < 0)
						//||(vector_move_nor.x > 0 && vector_move_nor.y > 0) 
						|| (vector_move_nor.x < 0 && vector_move_nor.y < 0)
						//||(vector_move_nor.x < 0 && vector_move_nor.y > 0) 
						)
					{
						sin_a *= -1;
					}

					float angle__ = 90;
					if (sin_a < 0)
					{
						angle__ = 270;
					}
					if (cos_a != 0)
					{
						angle__ = atanf(sin_a / cos_a);
					}

					_bg_entity->SetAngle((angle__ * 180 / 3.14f) + rotate_invoke);
				}
				//
				Vec2 ThresHoldRemove__ = __data_layer->GetDataVector2("threadHoldRemove");
				ThresHoldRemove__.x = ThresHoldRemove__.x * GetGameSize().width;
				ThresHoldRemove__.y = ThresHoldRemove__.x * GetGameSize().height;
				//
				_bg_entity->SetTheThreadHold(ThresHoldRemove__, vector_move__);
			}
			//

			//action zoom
			__data_layer = layer_contain->p_list_layer_element_dec.find("Size") != layer_contain->p_list_layer_element_dec.end() ?
				layer_contain->p_list_layer_element_dec.at("Size") : nullptr;
			if (__data_layer)
			{
				Vec2 _size_entity = _bg_entity->GetSizeBG();
				float delta_scale__ = __data_layer->GetDatafloat("scale") * GetGameSize().width;
				if (!layer_contain->p_IsScaleRandom)
				{
					//_size_entity.x = _size_entity.x - delta_scale__;
					//_size_entity.y = _size_entity.y - delta_scale__;
					_bg_entity->SetSizeBG(_size_entity);
				}
				float time_delay__ = 0;
				bool reserve = false;
				if (__data_layer->GetDataInt("IsFade") == 1)
				{
					if (_delay_time_move <= 0)
						time_delay__ = _time_move - (2 * __data_layer->GetDatafloat("time"));
					else
						time_delay__ = (_time_move * 2) + _delay_time_move - (2 * __data_layer->GetDatafloat("time"));

					reserve = true;
				}
				else
				{
					time_delay__ = __data_layer->GetDatafloat("delay");
					if (time_delay__ > 0)
						reserve = true;
				}

				_bg_entity->OnSetProcessScaleActionByDec(__data_layer, delta_scale__, 0, 0, false, reserve, time_delay__);
			}

			//action opacity
			__data_layer = layer_contain->p_list_layer_element_dec.find("Opacity") != layer_contain->p_list_layer_element_dec.end() ?
				layer_contain->p_list_layer_element_dec.at("Opacity") : nullptr;

			if (__data_layer)
			{
				if (!layer_contain->p_IsColorRandom)
				{
					_bg_entity->SetOpacity(__data_layer->GetDataInt("first"));
				}
				float time_delay__ = 0;
				bool reserve = false;
				if (__data_layer->GetDataInt("IsFade") == 1)
				{
					if (_delay_time_move <= 0)
						time_delay__ = _time_move - (2 * __data_layer->GetDatafloat("time"));
					else
						time_delay__ = (_time_move * 2) + _delay_time_move - (2 * __data_layer->GetDatafloat("time"));

					reserve = true;
				}
				else
				{
					time_delay__ = __data_layer->GetDatafloat("delay");
					if (time_delay__ > 0)
						reserve = true;
				}

				_bg_entity->OnSetProcessFadeActionByDec(__data_layer, _bg_entity->GetOpacity(), __data_layer->GetDataInt("origin"), 0, 0, false, reserve, time_delay__);
			}

			//action rotate
			__data_layer = layer_contain->p_list_layer_element_dec.find("Rotate") != layer_contain->p_list_layer_element_dec.end() ?
				layer_contain->p_list_layer_element_dec.at("Rotate") : nullptr;

			if (__data_layer)
			{
				if (!layer_contain->p_IsRotateRandom)
				{
					_bg_entity->SetAngle(__data_layer->GetDatafloat("origin"));
				}
				float time_delay__ = 0;
				bool reserve = false;
				
				time_delay__ = __data_layer->GetDatafloat("delay");
				if (time_delay__ > 0)
					reserve = true;

				_bg_entity->OnSetProcessRotateActionByDec(__data_layer, __data_layer->GetDataInt("angle"), 0, 0, false, reserve, time_delay__);
			}

			//action tint
			__data_layer = layer_contain->p_list_layer_element_dec.find("Color") != layer_contain->p_list_layer_element_dec.end() ?
				layer_contain->p_list_layer_element_dec.at("Color") : nullptr;

			if (__data_layer)
			{
				if (!layer_contain->p_IsColorRandom)
				{
					_bg_entity->SetColor(__data_layer->GetDataColor("origin"));
				}
				float time_delay__ = 0;
				bool reserve = false;

				time_delay__ = __data_layer->GetDatafloat("delay");
				if (time_delay__ > 0)
					reserve = true;

				_bg_entity->OnSetProcessColorActionByDec(__data_layer, __data_layer->GetDataColor("target"), 0, 0, false, reserve, time_delay__);
			}

			//action process
			__data_layer = layer_contain->p_list_layer_element_dec.find("Process") != layer_contain->p_list_layer_element_dec.end() ?
				layer_contain->p_list_layer_element_dec.at("Process") : nullptr;

			if (__data_layer)
			{
				float percent__ = __data_layer->GetDatafloat("origin");
				float time__ = __data_layer->GetDatafloat("time");

				if (layer_contain->p_IsProcessRandom)
				{
					percent__ = layer_contain->GetPercentRandom();
					time__ = layer_contain->GetTimeProcessRandom();
				}
				float time_delay__ = 0;
				bool reserve = false;

				time_delay__ = __data_layer->GetDatafloat("delay");
				if (time_delay__ > 0)
					reserve = true;

				_bg_entity->OnSetProcessProcessByActionByDec(__data_layer, percent__, __data_layer->GetDataInt("loop"), time__, false, reserve, time_delay__);
			}
			_bg_entity->SetBlendFunc(BlendFunc::ADDITIVE);
			return _bg_entity;
		}
		
		bool BGLayer::OnStopProcess(Object::BASIC_ACTION ba)
		{
			//remove the action now
			for (int i = 0; i < p_ListObject.size(); i++)
			{
				p_ListObject.at(i)->OnStopActionIm(ba);
			}
			return true;
		}

		bool BGLayer::OnPauseProcess(Object::BASIC_ACTION ba)
		{
			//pause the action now
			for (int i = 0; i < p_ListObject.size(); i++)
			{
				p_ListObject.at(i)->OnPauseActionIm(ba);
			}
			return true;
		}

		bool BGLayer::OnResumeProcess(Object::BASIC_ACTION ba)
		{
			for (int i = 0; i < p_ListObject.size(); i++)
			{
				p_ListObject.at(i)->OnResumeActionIm(ba);
			}
			return true;
		}


		/*
		
		
		*/

		BackgroundGame::BackgroundGame()
		{
			p_name = "";
		}
		BackgroundGame::~BackgroundGame()
		{
			p_ListObject.clear();
		}

		void BackgroundGame::Init(RKString name)
		{
			p_name = name;

			auto _data_background = XMLMgr->GetBackground(name);
			if (_data_background)
			{
				int _number_layer = _data_background->GetNumberLayer();
				for (int i = 0; i < _number_layer; i++)
				{
					BGLayer * _layer_data = new BGLayer();
					_layer_data->Init(_data_background->GetLayerDataByIdx(i), p_name);
					p_ListObject.insert(p_ListObject.size(), _layer_data);
				}
			}
		}
		void BackgroundGame::Update(float dt)
		{

			for (int i = 0; i < p_ListObject.size(); i++)
			{
				p_ListObject.at(i)->Update(dt);
#ifndef USE_THREAD_BG
				p_ListObject.at(i)->OnRemoveObjectFinish();
#endif
			}
		}
		void BackgroundGame::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			for (int i = 0; i < p_ListObject.size(); i++)
			{
				p_ListObject.at(i)->Draw(renderer, transform, flags);
			}
		}
		void BackgroundGame::Visit(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			for (int i = 0; i < p_ListObject.size(); i++)
			{
				p_ListObject.at(i)->Visit(renderer, transform, flags);
			}
		}

		BGLayer * BackgroundGame::GetLayerByName(RKString name)
		{
			for (int i = 0; i < p_ListObject.size(); i++)
			{
				if (p_ListObject.at(i)->GetNameLayer() == name)
				{
					return p_ListObject.at(i);
				}
			}
			return nullptr;
		}

		void BackgroundGame::RemoveObjectFinish()
		{
			std::vector<int> ActionEndLoop;
			for (int i = 0; i < p_ListObject.size(); i++)
			{
				p_ListObject.at(i)->OnRemoveObjectFinish();
				if (p_ListObject.at(i)->GetCurrentLoopAction() == 0)
				{
					ActionEndLoop.push_back(i);
				}
			}

			for (int i = ActionEndLoop.size() - 1; i >= 0; i--)
			{
				p_ListObject.erase(p_ListObject.begin() + ActionEndLoop.at(i));
			}
		}

		int BackgroundGame::GetSizeObjectWithin() { 
			return p_ListObject.size();
		}
	}
}

#endif//USE_DYNAMIC_BG