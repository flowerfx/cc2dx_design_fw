
#include "MainGame.h"
#include "Item.h"
Item::Item(ubyte itemType)
{
	this->itemType = itemType;
}

Item::Item()
{
	itemType = TYPE_ITEM::ITEM;
}

Item::~Item()
{
	icon->release();
}

ubyte Item::getItemType()
{
	return itemType;
}

void Item::setPrice(int p) 
{
        price = p;
        shortDes = "" + std::to_string(p) + "$";
  }
   
 Sprite* Item::getIcon() 
{
    if (icon == nullptr)
    {
		GameMgr->HandleAvatarDownload(this->id, name, [this](void * data, void *str, int tag) {
			this->icon = GameMgr->CreateAvatarFromImg(data, str);
		});
    }  	
	return icon;
}

std::string Item::getName() 
{
     return name;
}

std::string Item::getDescription() {
        return description;
    }

std::string Item::getSubDescription() {
        return shortDes;
    }

std::string Item::getContent() {
        return "";
    }

Sprite* Item::getSubIcon() {
		return nullptr;
	}

