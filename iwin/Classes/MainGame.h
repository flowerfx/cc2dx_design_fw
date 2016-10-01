#ifndef __MAIN_GAME_H__
#define __MAIN_GAME_H__

#include "cocos2d.h"
#include "DisplayView.h"
#include "SoundManager.h"
#include "Common/GameController.h"
#include "Network/IwinProtocol.h"
#include "Constants.h"

#define LIST_AGR(type_data)	void*data1, void*data2,void*data3,void*data4,void*data5,void*data6
#define LIST_AGR_H(type_data)	void*data1=type_data, void*data2=type_data,void*data3=type_data,void*data4=type_data,void*data5=type_data,void*data6=type_data

#define STR_SAVE_NAME_LOGIN		"NameLogin"
#define STR_SAVE_PASS_LOGIN		"PassLogin"
#define STR_SAVE_TYPE_LOGIN		"TypeLogin"
#define STR_SAVE_SERVER_CHOOSE  "ServerLogin"
#define STR_SAVE_SOUND_VOLUME   "SoundVolume"
#define STR_SAVE_MUSIC_VOLUME   "MusicVolume"
#define STR_SAVE_LANGUAGE		"Language"
#define STR_SAVE_INVITE_STATE	"InviteState"
#define STR_SAVE_BRIVATE_STATE	"BrivateState"
#define STR_SAVE_FB_STATE		"FBState"
namespace iwinmesage
{
	class HeartBeatConfiguration;
}


class MainGame : public cocos2d::Layer
{
private:
	static MainGame * p_Instance;

	bool p_IsInitialized;

	int p_loadingStep;
    
    bool PauseTheGame;

	int	 p_stage_error_message;

	HeartBeatConfiguration*			p_heartBeat = nullptr;
	bool							_allowSendHeartBeat;
	bool							_allowDisconnectWhenTimeOut;
	int								_timeOutIntervalCounter;
	int								_heartbeatIntervalCounter;
	//cocos2d::ui::TextBMFont* _profile_font;
public:
	// implement the "static node()" method manually
	CREATE_FUNC(MainGame);
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* scene();
	//
	static MainGame * GetInstance()
	{
		if (!p_Instance)
		{
			p_Instance = new MainGame();
		}
		return p_Instance;
	}
	MainGame();
	~MainGame();
	//
	void EndScence();
	//update and main method
	virtual void update(float dt);
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags) ;
	//
	bool InitTheGame(float dt);

	bool InitLoadingScreen();
	//
	//method with cocos2d engine

	void InitForFirstScence(cocos2d::Director * d)
	{
		d->runWithScene(MainGame::scene());
	}
    
    void SetPauseTheGame(bool pause) { PauseTheGame = pause;}



public:
	void ResetMessageStage() { p_stage_error_message = 0; }
	//Network class
	void Network_CheckUserNameExist(RKUtils::RKString name);
	void Network_LoginToGame(RKUtils::RKString name, RKUtils::RKString pass);
	void Network_RegisterNewAccount(RKUtils::RKString name, RKUtils::RKString pass);
	void Network_getSMSInfoForgotPassword(RKUtils::RKString name);
	void Network_loginFacebook(std::string token);
	void Network_MessageHandler(int id , void * data , LIST_AGR_H(nullptr));
	void Network_GetCaptchaInfo(RKString id);
	void Network_CheckCaptchaResult(RKString id, RKString captcha);
	void Network_HandleGetForgottenPassword(ubyte type, RKString user_name);

	RKUtils::RKString GetUrlAvatarFrom(int id, bool is_ID = true);
	void HandleAvatarDownload(int id , RKUtils::RKString name,  const std::function<void(void * data,void * str,int tag)> & func , bool is_ID = true);
	void HandleURLImgDownload(RKString url, RKUtils::RKString name, const std::function<void(void * data, void * str, int tag)> & func);
	void HandleAvatarDownload(std::string url, const std::function<void(void * data, void * str, int tag)> & func);

	Sprite * CreateAvatarFromImg(void * img ,void * str);
private:
	std::map<RKString, Texture2D*> p_list_texture_avatar_download;
public:
	void updateUnreadMsg();

	void startHeartBeat(HeartBeatConfiguration* config);
	void stopHeartBeat();
	void onHeartBeatResponse();

	virtual void setUserData(void *userData) override;
};

#define GameMgr MainGame::GetInstance()
#define HANDLE_MSG GameMgr->Network_MessageHandler

#endif //__MAIN_GAME_H__
