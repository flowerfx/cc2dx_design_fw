#ifndef  __MODEL_CATEOGRY_H__
#define __MODEL_CATEOGRY_H__

#include <string>
class Category 
{
public:
	 int categoryId;
	 std::string name;
	 int parentId;

	 Category();
	 Category(int id, std::string n, int pid);
	 virtual ~Category();
};

#endif // ! __MODEL_CATEOGRY_H__
