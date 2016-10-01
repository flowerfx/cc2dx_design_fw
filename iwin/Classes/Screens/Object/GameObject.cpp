#include "GameObject.h"

GameObject::GameObject()
{


}

GameObject::GameObject(RKString name_object)
{
	Init(name_object);
}

GameObject::~GameObject()
{

}

void GameObject::Init(RKString name_obj)
{
	if (p_IsInit)
	{
		CCLOG("object already init !");
		return;
	}
	bool res = OnInitWideObject(name_obj);

	CCASSERT(res, "init object failed !");
	p_IsInit = true;
}

void GameObject::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	if (!p_IsActive)
		return;

	wBasicObject::Draw(renderer, transform, flags);
}

void GameObject::Visit(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	if (!p_IsActive)
		return;

	wBasicObject::Visit(renderer, transform, flags);
}

void GameObject::Update(float dt)
{
	if (!p_IsActive)
		return;

	ObjectEntity::Update(dt);
}