#include "ObjectEntity.h"

ObjectEntity::ObjectEntity()
{
	p_IsActive = true;
	p_IsOnControl = false;
	p_IsInit = false;
	p_type_unit = "";
}

ObjectEntity::ObjectEntity(RKString text)
{
	p_IsActive = true;
	p_IsOnControl = false;
	p_IsInit = false;
	p_type_unit = text;
}

void ObjectEntity::Init(RKString name_obj)
{

}

ObjectEntity::~ObjectEntity()
{
	p_IsActive = true;
	p_IsOnControl = false;
}

void ObjectEntity::Update(float dt)
{
	wBasicObject::Update(dt);
}

bool ObjectEntity::OnFinishAction()
{
	return true;
}