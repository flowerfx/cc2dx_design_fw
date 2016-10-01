
#include "cocos2d.h"
#include "Screens/BaseScreen.h"
#include "Screens/ScreenManager.h"
#include "MainGame.h"
#include "Platform/Platform.h"
#include "Utils.h"

namespace Utils
{

	Vec2 s_bounding_offset = Vec2(15,15);

	std::vector<s64> s_popular_bet;

	#define NUM_EMOTION 60
	const char* k_emotions[] = { ":)", "o.O", ":'(",
		"3:-)", ":(", ":-O", "8-)", ":D", ">:(", "<3", "^_^", ":v", "(U)",
		"-_-", "8|", ":P", ":/", ";)", ":*", "(y)", ":~", ":B", ":|", ":+",
		":$", ":X", ":Z", ":((", ":-|", ":-H", "(w)", ":-F", ";T", ";P",
		";D", ";O", ";G", "|-)", ":!", ":L", ":>", ":;", ";F", ";-S", ":?",
		";-X", ";-!", "/--", "&-(", ":-L", ":-R", ">-|", "P-(", ":--|",
		"X-)", ";-A", "8*", "(shit)", "(n)", "(ok)" };

	const char* k_mapEmotions[] = { "Ā", "Ġ", "İ",
		"ŀ", "Ő", "Š", "ƀ", "Ȁ", "Ģ", "Ĳ", "ł", "Œ", "Ƣ", "ģ", "Ń", "œ",
		"ţ", "ų", "Ą", "Ĕ", "Ĥ", "Ĵ", "ń", "Ŕ", "ą", "ĕ", "ĥ", "ĵ", "Ņ",
		"ŕ", "Ć", "Ė", "Ħ", "Ķ", "ņ", "Ŗ", "ć", "ė", "ħ", "ķ", "Ň", "ŗ",
		"ȹ", "Ę", "Ɯ", "Ǿ", "ň", "Ř", "ċ", "ě", "ī", "Ļ", "ŋ", "ś", "Č",
		"Ĝ", "Ĭ", "ļ", "Ƀ", "Ŝ" };
#define NUMBER_BET 39
	s64 POPULAR_BET_NEW[NUMBER_BET] = { 0 , 1000, 2000, 3000, 5000, 7000, 10000, 15000, 20000, 30000, 50000,
		70000, 100000, 150000, 200000, 300000, 500000, 700000, 1000000, 1500000, 2000000, 3000000, 5000000, 7000000,
		10000000, 15000000, 20000000, 30000000, 50000000, 70000000, 100000000, 150000000, 200000000, 300000000,
		500000000, 700000000, 1000000000, 1500000000, 2000000000 };
	 
	std::string formatMoney_dot_ruby(s64 ruby)
	{
		if (ruby < 0)
		{
			return "";
		}
		else
		{
			return formatNUmber_dot(ruby) + " Ruby";
		}
	}

	std::string formatMoney_dot_win(s64 money)
	{
		if (money < 0)
		{
			return "";
		}
		else
		{
			return formatNUmber_dot(money) + " Win";
		}
	}

	std::string formatNUmber_dot(s64 money)
	{
		std::string numWithCommas = std::to_string(money);
		int insertPosition = numWithCommas.length() - 3;
		while (insertPosition > 0) {
			numWithCommas.insert(insertPosition, ",");
			insertPosition -= 3;
		}
		return numWithCommas;
	}

	char _toLowerCase(char c)
	{
		if (c >= (char)'A' && c <= (char)'Z')
		{
			c += (char)'a' - (char)'A';
		}
		return c;
	}

	std::string toLowerCase(std::string str)
	{
		for (size_t i = 0; i < str.size(); i++)
		{
			str[i] = Utils::_toLowerCase(str[i]);
		}
		return str;
	}

	std::string trim(std::string str)
	{
		std::string rs = "";
		int leftIdx;
		int rightIdx;
		for (size_t i = 0; i < str.size(); i++)
		{
			if (str[i] != ' ')
			{
				leftIdx = i;
				break;
			}
		}
		for (int i = str.size() - 1; i >= 0; i--)
		{
			if (str[i] != ' ')
			{
				rightIdx = i;
				break;
			}
		}
		if (leftIdx - rightIdx >= 0)
		{
			rs = "";
		}
		else
		{
			for (auto i = leftIdx; i <= rightIdx; i++)
			{
				rs += str[i];
			}
		}
		return rs;
	}

	s64 getMaxValidBet(s64 value)
	{
		for (int i = 1; i < s_popular_bet.size(); i++)
		{
			if (value < s_popular_bet[i])
			{
				if (i == 1)
				{
					return 0;
				}
				else
				{
					return s_popular_bet[i - 1];
				}
			}
		}
		return s_popular_bet.back();
	}
	
	int getMaxValidBetIdx(s64 value)
	{
		for (int i = 1; i < s_popular_bet.size(); i++)
		{
			if (value < s_popular_bet[i])
			{
				if (i == 1)
				{
					return 0;
				}
				else
				{
					return i - 1;
				}
			}
		}
		return s_popular_bet.size() - 1;
	}
	std::string formatNumber_win(s64 n)
	{
		if (n < 0)
		{
			return "";
		}
		else
		{
			//@PDT: TODO
			return formatNumber(n) + " Win";
		}
	}

	std::string formatNumber_ruby(s64 n)
	{
		if (n < 0)
		{
			return "";
		}
		else
		{
			//@PDT: TODO
			return formatNumber(n) + " Ruby";
		}
	}

	std::string formatNumber(s64 n)
	{
		if (n >= 1000 || n <= -1000)
		{
			long temp = n / 1000;
			std::string numberstring = "";
			int count = 0;
			long temp_k = 0;
			long temp_m = 0;
			while (temp > 999)
			{
				count++;
				if (count == 1)
				{
					temp_k = temp % 1000;
				}
				else if (count > 1)
				{
					temp_m = temp % 1000;
				}

				temp = temp / 1000;
			}
			if (count == 0)
			{
				numberstring = std::to_string(temp) + "K";
			}
			else if (count == 1)
			{
				std::string str = "";
				if (temp_k >= 10)
				{
					if (temp_k < 100)
					{
						str = ".0" + std::to_string(temp_k);
					}
					else if (temp_k >= 100)
					{
						str = "." + std::to_string(temp_k);
					}
					str.erase(str.size() - 1, 1);
				}

				numberstring = std::to_string(temp) + str + "M";
			}
			else
			{
				std::string str = "";
				if (temp_m >= 10)
				{
					if (temp_m < 100)
					{
						str = ".0" + std::to_string(temp_k);
					}
					else if (temp_m >= 100)
					{
						str = "." + std::to_string(temp_m);
					}
					str.erase(str.size() - 1, 1);
				}

				numberstring = std::to_string(temp) + str + "B";
			}
			return numberstring;
		}
		else
			return "" + std::to_string(n);
	}

	const wchar_t * GetWChar(const char *c)
	{
		const size_t cSize = strlen(c) + 1;
		std::wstring wc(cSize, L'#');
		mbstowcs(&wc[0], c, cSize);
		return wc.data();
	}

	std::string getImageType() 
	{
		cocos2d::Size game_size = GetGameSize();

		if (game_size.width < 400){
			return "small";
		}
		else if (game_size.width >= 400 && game_size.width < 680) {
			return "small";
		}
		else if (game_size.width >= 680 && game_size.width < 880) {
			return "normal";
		}
		else{
			return "large";
		}
		return "normal";
	}

	std::string getAvataUrlPlayer(int idAvata, std::string userName)
	{
		string template_url = "";
		if (idAvata >= 0)
		{
			template_url = "http://av.iwin.vn/a/items/" + std::to_string(idAvata);
		}
		else
		{
			template_url = "http://av.iwin.vn/a/i/" + userName;
		}

		template_url += "/avatar?width=";
		template_url += std::to_string(100);
		template_url += "&height=";
		template_url += std::to_string(100);
		template_url += "&type=";
		template_url += getImageType();
		template_url += "&redirect=true";

		return template_url;
	}

	int countStringLine(std::string str)
	{
		if (str == "")
			return 0;
		int number_char = str.size();
		int number_line = 1;
		int i = 0;
		do
		{
			if (str[i] == 10)
			{
				number_line++;
			}
			i++;
		} while (i < number_char);
		return number_line;
	}
	
	
	std::string chatConvert(std::string chat)
	{
		std::string rs;
		for (auto i = 0; i < NUM_EMOTION; i++)
		{
			do {
				int offset = chat.find(k_emotions[i]);
				if (offset != std::string::npos)
				{
					chat.replace(offset, strlen(k_emotions[i]), k_mapEmotions[i]);
				}
				else 
				{
					break;
				}
			} while (true);

		}
		return chat;
	}

	std::string getEmotionStr(int idx)
	{
		return k_emotions[idx];
	}

	void startWaitDialog(std::string content)
	{
		ScrMgr->OnShowDialog(Platform::getAppName(), content, TYPE_DIALOG::DIALOG_ZERO_BUTTON_WAIT);
	}

	void hideWaitDialog()
	{
		ScrMgr->OnHideDialog();
	}

	void showDialogNotice(std::string content)
	{
		ScrMgr->OnShowDialog(LangMgr->GetString("notice"), content, TYPE_DIALOG::DIALOG_ONE_BUTTON);
	}

	std::vector<s64> GetBetPopularNew()
	{
		std::vector<s64> range;
		for (int i = 0; i < 38; i++)
		{
			range.push_back(POPULAR_BET_NEW[i]);
		}
		return range;
	}

	s64 GetBetValueNew(int idx)
	{
		if (idx < 0)
			idx = 0;
		if (idx > 37)
			idx = 37;
		return POPULAR_BET_NEW[idx];
	}

	int GetRangeBetWithMoney(s64 &min_bet, s64 &max_bet, s64 curent_money, s64 min)
	{
		int idx_min = -1;
		for (size_t i = 0; i < NUMBER_BET - 1; i++)
		{
			if (min >= POPULAR_BET_NEW[i] && min < POPULAR_BET_NEW[i + 1])
			{
				idx_min = i;
				min_bet = POPULAR_BET_NEW[i];
				break;
			}
		}
		if (idx_min < 0)
		{
			if (min >= POPULAR_BET_NEW[NUMBER_BET - 1])
			{
				min_bet = POPULAR_BET_NEW[NUMBER_BET - 1];
			}
			else
			{
				min_bet = POPULAR_BET_NEW[0];
			}
		}

		int idx_max = -1;
		for (size_t i = 0; i < NUMBER_BET - 1; i++)
		{
			if (curent_money >= POPULAR_BET_NEW[i] && curent_money < POPULAR_BET_NEW[i + 1])
			{
				idx_max = i;
				max_bet = POPULAR_BET_NEW[i];
				break;
			}
		}
		if (idx_max < 0)
		{
			if (curent_money >= POPULAR_BET_NEW[NUMBER_BET - 1])
			{
				max_bet = POPULAR_BET_NEW[NUMBER_BET - 1];
			}
			else
			{
				max_bet = POPULAR_BET_NEW[0];
			}
		}

		int delta = abs(idx_max - idx_min);
		return delta;
	}

	s64 getMaxValidBetNew(s64 value)
	{
		for (size_t i = 0; i < NUMBER_BET - 1; i++)
		{
			if (value >= POPULAR_BET_NEW[i] && value < POPULAR_BET_NEW[i + 1])
			{
				return POPULAR_BET_NEW[i];
			}
		}
		if (value < POPULAR_BET_NEW[0])
		{
			return 0;
		}
		else if (value >= POPULAR_BET_NEW[NUMBER_BET - 1])
		{
			return POPULAR_BET_NEW[NUMBER_BET - 1];
		}
		return 0;
	}

	s64 GetNumberFromFormat(std::string format)
	{
		RKString str = format;
		if (format == "")
			return 0;
		str.ReplaceAll(".", "");
		str.ReplaceAll(",", "");
		str.ReplaceAll("WIN", "");
		str.ReplaceAll("RUBY", "");
		u64 number = atoll(str.GetString());
		return number;
	}

	int GetIndexOfValue(s64 value)
	{
		for (size_t i = 0; i < NUMBER_BET - 1; i++)
		{
			if (value >= POPULAR_BET_NEW[i] && value < POPULAR_BET_NEW[i + 1])
			{
				return i;
			}
		}
		if (value < POPULAR_BET_NEW[0])
		{
			return 0;
		}
		else if (value >= POPULAR_BET_NEW[NUMBER_BET - 1])
		{
			return NUMBER_BET - 1;
		}
		return 0;
	}

}
