#ifndef _LOCALDATAMGR_H_
#define _LOCALDATAMGR_H_
#include <map>
#include "RKString_Code/RKString.h"
#include "LoadDataXML.h"
using namespace RKUtils;
namespace Utility
{
	namespace Language
	{
		struct block_str
		{
			wMap<RKString, RKString> list_str;
			char id_block;

			block_str()
			{
				list_str.clear();
			}
			virtual ~block_str()
			{
				list_str.clear();
			}
		};

		struct group_str
		{
			wMap<char, block_str*> list_block;
			char id_group;
			group_str()
			{
				list_block.clear();
			}
			virtual ~group_str()
			{
				list_block.clear();
			}
		};

		class Language
		{
		private:
			wMap<char, group_str*> p_list_string;
			std::map<LANGUAGE_TYPE, RKString> p_lang_dec;
			LANGUAGE_TYPE p_current_language;

			static Language * p_Instance;
		public:
			static Language * GetInstance()
			{
				if (!p_Instance)
				{
					p_Instance = new Language();
				}
				return p_Instance;
			}

			Language();
			virtual ~Language();

			void InsertString(RKString id, RKString str);
			bool SetNewLanguage(LANGUAGE_TYPE lang);
			int GetNumberLang() { return LANGUAGE_TYPE::LANG_COUNT - 1; }
			RKString GetLangDec(LANGUAGE_TYPE type);
			RKString GetString(RKString id);
			bool Find_Str_With_Id(RKString id , RKString & str);

			LANGUAGE_TYPE GetCurLanguage();
		};
	}
#define LangMgr Language::Language::GetInstance()
#define LANG_STR Language::Language::GetInstance()->GetString	
};
#endif