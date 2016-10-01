#include "Province.h"
#include "LanguageMgr.h"
using namespace RKUtils;
using namespace Utility;

Province::Province()
{

}
Province::~Province()
{
    
}

std::vector<Province*> Province::list;

std::string Province::getProviceName(int id)
{
    int number_list = (int)list.size();
	for (auto p : list)
	{
		if (p->id == id)
		{
			return p->name;
		}
	}
	return LANG_STR("217").GetString();
}
