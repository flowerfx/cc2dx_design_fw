#include "LanguageMgr.h"
#include "FileManager.h"

namespace Utility
{
	namespace Language
	{
		Language * Language::p_Instance = nullptr;

		Language::Language()
		{
			p_list_string.clear();
			p_lang_dec.clear();
			p_current_language = LANGUAGE_TYPE::LANG_NONE;
		}

		Language::~Language()
		{
			p_list_string.clear();
			p_lang_dec.clear();
			p_current_language = LANGUAGE_TYPE::LANG_NONE;
		}

		void Language::InsertString(RKString id, RKString str)
		{
			if (str.Contains("\\n"))
			{
				str.ReplaceAll("\\n", "\n");
			}
			if (str.Contains("\\"))
			{
				str.ReplaceAll("\\", "");
			}
			char first_char_id = std::string(id.GetString())[0];
			char second_char_id = std::string(id.GetString())[1];

			if (p_list_string.contain(first_char_id))
			{
				auto group_str = p_list_string.at(first_char_id);
				if (group_str->list_block.contain(second_char_id))
				{
					auto block_str = group_str->list_block.at(second_char_id);
					block_str->list_str.insert(id, str);
				}
				else
				{
					auto _block_str = new block_str();
					_block_str->list_str.insert(id, str);
					group_str->list_block.insert(second_char_id, _block_str);
				}
			}
			else
			{
				auto _group_str = new group_str();
				auto _block_str = new block_str();
				_block_str->list_str.insert(id, str);
				_group_str->list_block.insert(second_char_id, _block_str);
				p_list_string.insert(first_char_id, _group_str);
			}

			//p_list_string.insert(id, str);
		}

		bool Language::SetNewLanguage(LANGUAGE_TYPE lang)
		{
			if (p_current_language == lang)
				return false;
			p_current_language = lang;
			p_lang_dec.clear();
			for (int i = LANGUAGE_TYPE::LANG_VI; i < LANGUAGE_TYPE::LANG_COUNT; i++)
			{
				auto _lang = XMLMgr->GetLangDecByType((LANGUAGE_TYPE)(i));
				p_lang_dec.insert(std::pair<LANGUAGE_TYPE, RKString>((LANGUAGE_TYPE)(i), _lang->dec));
			}

			auto cur_lang = XMLMgr->GetLangDecByType(p_current_language);
			if (!cur_lang)
			{
				return false;
			}
			auto path_language = cur_lang->path;
			p_list_string.clear();

			FILE * pFile = File::GetFile(path_language.GetString(), "rb");
			if (pFile)
			{
				long lSize;
				char * buffer;
				size_t result;

				fseek(pFile, 0, SEEK_END);
				lSize = ftell(pFile);
				rewind(pFile);

				// allocate memory to contain the whole file:
				buffer = (char*)malloc(sizeof(char)*lSize);
				if (buffer == NULL)
				{
					PASSERT2(buffer != nullptr, "Memory error !");
				}

				// copy the file into the buffer:
				result = fread(buffer, 1, lSize, pFile);
				if (result != lSize)
				{
					PASSERT2(result != lSize, "Reading error !");
				}
				RKString str_lang = buffer;

				auto list_str_lang = str_lang.Split("\n");

				for (size_t i = 0; i < list_str_lang.Size(); i++)
				{
					RKString detail_str_lang = list_str_lang.GetAt(i);
					auto sep_str_lang = detail_str_lang.SplitFirst("=");
                    if(sep_str_lang.Size() <= 0 )
                        continue;
                    RKString str_1 = sep_str_lang.GetAt(0);
                    RKString str_2 = "";
                    if(sep_str_lang.Size() > 1)
                    {
                        str_2 = sep_str_lang.GetAt(1);
                    }
					InsertString(str_1,str_2);
				}

				delete buffer;
				buffer = nullptr;
				fclose(pFile);

				return true;
			}

			return false;
		}

		bool Language::Find_Str_With_Id(RKString id, RKString & str)
		{
			char first_char_id = std::string(id.GetString())[0];

			char second_char_id = ' ';
			if (id.Length() > 1)
			{
				second_char_id = std::string(id.GetString())[1];
			}

			if (p_list_string.contain(first_char_id))
			{
				auto group_str = p_list_string.at(first_char_id);
				if (group_str->list_block.contain(second_char_id))
				{
					auto block_str = group_str->list_block.at(second_char_id);
					if (block_str->list_str.contain(id))
					{
						str = block_str->list_str.at(id);
						return true;
					}
				}
			}
			str = "";
			return false;
		}

		RKString Language::GetString(RKString id)
		{
			RKString str = "";
			if (Find_Str_With_Id(id, str))
			{
				return str;
			}
			return id;

			/*if (p_list_string.size() <= 0 || p_list_string.find(id) == p_list_string.end())
				return id;
			if (p_list_string.at(id) == "")
				return id;
			return p_list_string.at(id);*/
		}

		LANGUAGE_TYPE Language::GetCurLanguage()
		{
			return p_current_language;
		}

		RKString Language::GetLangDec(LANGUAGE_TYPE type)
		{
			if (p_lang_dec.size() <= 0 || p_lang_dec.find(type) == p_lang_dec.end())
				return "";
			return p_lang_dec.at(type);
		}

	}
}