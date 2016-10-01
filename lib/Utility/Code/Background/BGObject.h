#ifndef __BG_OBJECT_H__
#define __BG_OBJECT_H__

#include "ObjectBGEntity.h"

#include "../../../../LIB/RKUtils/Code/RKThread_Code/RKThread.h"

#if USE_DYNAMIC_BG

using namespace RKUtils;
namespace Utility
{
	namespace Background
	{
		class BGLayer : public Ref
		{
		protected:
			Vector<BGEntity*> p_ListObject;

			std::vector<int> p_listIndexObjectFinish;

			RKString p_name_layer;
			RKString p_name_background;
			int p_index;

			BGEntity* generateBGEntityWithParam(BGEntity* previous, xml::LayerData * layer_contain);
			void InsertToListObjFinish(int idx);

			int p_number_object;
			int p_previous_size_remove;

			int p_LoopAction;

		public:
			BGLayer();
			virtual ~BGLayer();

			void Init(xml::LayerData * layer, RKString name_bg);
			void Update(float dt);
			void Draw(Renderer *renderer, const Mat4& transform, uint32_t flags);
			void Visit(Renderer *renderer, const Mat4& transform, uint32_t flags);

			void OnRemoveObjectFinish();

			RKString GetNameLayer() { return p_name_layer; }

			int GetCurrentLoopAction() { return p_LoopAction; }
		public:
			//some method that main game can handle action
			bool OnStopProcess(Object::BASIC_ACTION ba); //stop the process 
			bool OnPauseProcess(Object::BASIC_ACTION ba); //pause the process 
			bool OnResumeProcess(Object::BASIC_ACTION ba); //pause the process 
		};

		class BackgroundGame
		{
		protected:
			Vector<BGLayer *> p_ListObject;

			RKString p_name;
		public:
			BackgroundGame();
			virtual ~BackgroundGame();

			virtual void Init(RKString name);
			void Update(float dt);
			void Draw(Renderer *renderer, const Mat4& transform, uint32_t flags);
			void Visit(Renderer *renderer, const Mat4& transform, uint32_t flags);

			BGLayer * GetLayerByName(RKString name);

			void RemoveObjectFinish();

			int GetSizeObjectWithin();
		};
	}
}

#endif//USE_DYNAMIC_BG

#endif //__BG_OBJECT_H__

