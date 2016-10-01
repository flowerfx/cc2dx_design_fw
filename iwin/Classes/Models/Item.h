#ifndef __ITEM_H__
#define __ITEM_H__
#include <string>
#include "cocos2d.h"
USING_NS_CC;
class Item
{
public:
	enum TYPE_ITEM
	{
		AVATAR = 0,
		ITEM ,
		MY_AVATARS,
		SHOP_AVATARS,
		EVENT_ITEM,
		FB_AVATAR
	};
    /**
     * Xac dinh loai cua item trong game.
     */
protected:
     ubyte itemType;

    /**
     * Khi khoi tao vat pham thi phai cho biet item thuoc loai nao.
     *
     * @param itemType
     */
public:
	Item(ubyte itemType);
	Item();
	virtual ~Item();

	ubyte getItemType();
    
public:
	/**
	* Id cua item. chi dung de lay hinh
	*/
     int id;
    /**
     * Bien nay de xac dinh avatar id cua user dang co, no khong giong nhu id.
     */
     int userAvatarID;
    /**
     * Name cua item.
     */
     std::string name;
    /**
     * Thong tin day du cua item.
     */
	 std::string description;
    /**
     * Thong tin ngan gon cua item.
     */
	 std::string shortDes;
    /**
     * Gia cua item.
     */
     int price;
    /**
     * Hạn sử dụng của item.
     */
     int expirationDate;
//    public int nCount = 0;
    /**
     * id nay dung de mo item
     */
     int userItemId ;
//    public int parentId = -1;
     bool isBuyUse ;
//    public boolean isSelect = false;
public:
	void setPrice(int p);

	Sprite* getIcon();

	std::string getName();
	std::string getDescription();
	std::string getSubDescription();
	std::string getContent();
	Sprite* getSubIcon();
private:
	Sprite* icon;
};

#endif // __ITEM_H__
