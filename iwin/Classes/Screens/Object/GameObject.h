#ifndef __OBJECT_H__
#define __OBJECT_H__


#include "cocos2d.h"
#include "ObjectEntity.h"
using namespace cocos2d;

class GameObject : public ObjectEntity
{
private:

public:
	GameObject();
	GameObject(RKString name_object);
	virtual ~GameObject();

	virtual void Init(RKString name_obj);
	virtual void Draw(Renderer *renderer, const Mat4& transform, uint32_t flags);
	virtual void Visit(Renderer *renderer, const Mat4& transform, uint32_t flags);
	virtual void Update(float dt);
};

#endif //__OBJECT_H__

