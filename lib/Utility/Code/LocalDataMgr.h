#ifndef _LOCALDATAMGR_H_
#define _LOCALDATAMGR_H_
#pragma once
#include <map>
#include "Mutex.h"

// local config parameter
#define	GAME_LANGUAGE			"language"
#define GAME_FIRST_LAUNCH		"first_launch"
#define GAME_FIRST_LAUNCH_KEY	"first_time_launch"
#define GAME_LAST_INTROTUTO_FOR_TRACKING "last_introtuto_for_tracking"
#define GAME_INTRO_IS_FINISHED "intro_is_finished"

#define ACCOUNT_QUICK_LOGIN		"quickLogin"
#define ACCOUNT_QUICK_LOGIN_CHARACTER	"quickLoginCharacter"
#define ACCOUNT_QUICK_LOGIN_GC	"quickLoginGameCenter"


#define GAME_MUSIC				"music"
#define GAME_MUTE_MUSIC			"mute_music"
#define GAME_SFX				"sfx"
#define GAME_MUTE_SFX			"mute_sfx"
#define GAME_VO					"vo"
#define GAME_MUTE_VO			"mute_vo"

#define GAME_HIDE_HELMET		"hide_helmet"
#define GAME_CAMERA_TYPE		"camera_type"
#define GAME_CAMERA_SENSITIVITY "camera_sensitivity"
#define GAME_REVERSE_Y_AXIS		"reverse_y_axis"
#define GAME_TAP_TO_MOVE		"tap_to_move"
#define GAME_CAMERA_SHAKE		"camera_shake"
#define GAME_AUTO_ATTACK		"auto_attack"
#define GAME_USE_3GOR4G			"use_3Gor4G"
#define GAME_SHOW_FIGHTING_INFO "show_fight_info"
#define GAME_SHOW_HURT_INFO		"show_hurt_info"
#define GAME_SHOW_FRIEND_HP		"show_friend_hp"
#define GAME_SHOW_ENEMY_HP		"show_enemy_hp"
#define GAME_SHOW_PING_INFO		"show_ping_info"
#define GAME_PUSH_MSG			"push_msg"
#if defined(OS_WP8) || defined(OS_W8)
#define GAME_ALLOW_PUSH			"allow_push"
#endif

#define GAME_BREAK_AWAY_TIME	"break_away"

#define GAME_LEVEL_UP_FIVE		"level_five"
#define GAME_RATE_GAME			"rate_game"

#define GAME_LAST_LOGIN_SERVER_ID  "last_login_server_id"
#define GAME_LAST_LOGIN_CHAR_NAME  "last_login_char_name"

#define GAME_ACCOUNT_BANNED  "account_banned"
#define GAME_HERO_DEATH_REASON		"hero_death_reason"
#define GAME_ACCOUNT_LINKTO_SNS   "account_linkto_sns"
#define GAME_DATACENTER   "datacenter"
#define GAME_LAST_LOGIN_SNS_TYPE "last_login_sns_type"
#define GAME_LAST_SHOW_SNSCONNECT_SCREEN "last_show_sns_connect_screen"

#define TRACKING_NEW_CHARACTER   "tracking_new_character"

#if defined (OS_ANDROID) && USE_GOOGLE_API //tuyennt550 implement google achievement
#define CLEAR_ACHIEVEMENT_NOVICE "clear_achievement_novice"
#define CLEAR_ACHIEVEMENT_APPRENTICE "clear_achievement_apprentice"
#define CLEAR_ACHIEVEMENT_AMATEUR "clear_achievement_amateur"
#define CLEAR_ACHIEVEMENT_EXPERT "clear_achievement_expert"
#define CLEAR_ACHIEVEMENT_MASTER "clear_achievement_master"
#endif //(OS_ANDROID) && USE_GOOGLE_API
//////////////////////////////////////////////////////////////////////////

class CLocalDataMgr
{
private:
	static std::map<std::string , std::string> s_localCfgMap;
	static Mutex s_mutexLocalCfgMap;
public:
	CLocalDataMgr(void);
	~CLocalDataMgr(void);
	static void SetLocalConfig(const char *mask, const char *value);
	static void SetLocalConfig(const char *mask, const int value);
	static void SetLocalConfig(const char *mask, const float value);
	static void SetLocalConfig(const char *mask, const bool value);
	static bool GetLocalConfig(const char *mask, char *result);
	static bool GetLocalConfig(const char *mask, int &result);
	static bool GetLocalConfig(const char *mask, float &result);
	static bool GetLocalConfig(const char *mask, bool &result);

	static bool RemoveLocalConfig(const char *mask);	/// mask is "ALL_CLEAR" remove all
	static void LoadLocalConfig();
	static void SaveLocalConfig();
	static void ReleaseLocalConfig();
};
#endif