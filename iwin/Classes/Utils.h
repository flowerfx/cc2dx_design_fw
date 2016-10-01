#ifndef __UTILS_H__
#define __UTILS_H__

#include "cocos2d.h"
#include <string>
#include <vector>
#include "Common/Common.h"
USING_NS_CC;

namespace Utils
{
	extern std::vector<s64> s_popular_bet;

	char _toLowerCase(char c);

	extern Vec2 s_bounding_offset;

	std::string formatMoney_dot_ruby(s64 ruby);
	std::string formatMoney_dot_win(s64 money);

	std::string formatNUmber_dot(s64 money);

	std::string toLowerCase(std::string str);
	std::string trim(std::string str);

	s64	getMaxValidBet(s64 value);
	int			getMaxValidBetIdx(s64 value);

	std::string formatNumber(s64 n);
	std::string formatNumber_win(s64 n);
	std::string formatNumber_ruby(s64 n);

	s64 GetNumberFromFormat(std::string format);

	const wchar_t * GetWChar(const char *c);
	std::string getAvataUrlPlayer(int idUser, std::string userName);

	int countStringLine(std::string str);

	std::string chatConvert(std::string chat);
	std::string getEmotionStr(int idx);
	void		startWaitDialog(std::string content);
	void		hideWaitDialog();
	void		showDialogNotice(std::string content);

	//
	std::vector<s64> GetBetPopularNew();
	s64 GetBetValueNew(int idx);
	int GetRangeBetWithMoney(s64 &min_bet, s64 &max_bet, s64 curent_money , s64 min);
	s64 getMaxValidBetNew(s64 value);
	int GetIndexOfValue(s64 value);
};

#endif // __UTILS_H__