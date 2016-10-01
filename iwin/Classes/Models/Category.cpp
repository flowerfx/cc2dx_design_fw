#include "Category.h"

Category::Category() 
{
	categoryId = 0;
	name = "";
	parentId = 0;
}

Category::Category(int id, std::string n, int pid) 
{
		categoryId = id;
		name = n;
		parentId = pid;
}

Category::~Category()
{

}