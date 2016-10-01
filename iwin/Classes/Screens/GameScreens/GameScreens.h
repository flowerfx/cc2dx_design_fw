#ifndef __GAME_SCREENS_H__
#define __GAME_SCREENS_H__


//#include "cocos2d.h"

#include "Screens/HUDScreen.h"
#include "Network/Core/Message.h"
//using namespace cocos2d;
#include "InputManager.h"
class PlayerGroup;
class Player;
class TimeBarProcess;
enum TYPE_SETTING
{
	SETTING_MONEY = 0,
	SETTING_TIME ,
	SETTING_PASS,
	SETTING_PLAYER
};

class GameScreen : public HUDScreen
{
private:
	//thread
	std::map<RKString, xml::UILayerWidgetDec*> p_list_sample_dec;
	ThreadWrapper *		  p_thread_load_xml_player_dec;
	void				  start_thread_load_xml_player(RKString name);
	void				  parse_xml_player(int idx);
	int					  p_load_xml_state;
	RKString			  p_game_name_xml;
protected:
	//save player info for thread
	std::vector<Player*>  p_thread_player_infos;
	void				  p_set_player_after_init(std::vector<Player*> playerInfos);
	bool				  p_is_finish_load_player;
protected:
	//use a list to store param
	std::vector<iwincore::Message*>	  p_list_msg_return;
	virtual void		  HandleAfterInitPlayer();
public:
	virtual int			 PushReturnMsg(iwincore::Message* msg);
	virtual void		  OnHandleMsg(iwincore::Message* msg);
protected:
	GameType				p_current_game_type;
	Node	*				p_panel_game_screen_resource;
	TimeBarProcess*			p_timebar_process;
	
	WidgetEntity*			btn_bet_iwin;
	WidgetEntity*			p_panel_game_screen;

	WidgetEntity*			p_text_min_bet;
	WidgetEntity*			p_text_gamename;
	WidgetEntity*			p_text_other1;
	WidgetEntity*			p_text_money;
	WidgetEntity*			p_text_name;

	WidgetEntity*			p_bonus_effect;

	bool					p_enable_touch;

public:
	GameScreen();
	virtual ~GameScreen();

	virtual bool InitGame(int game_id) ;
	virtual  int InitComponent()															override;
	virtual  int Update(float dt)															override;
	virtual GameType CurrentGameType() ;
	virtual void LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML) ;
	virtual void ParseIntoCustom(xml::UILayerWidgetDec* xml_dec, WidgetEntity * p_panel_custom, RKString name_panel_xml) ;
	virtual void OnBeginFadeIn()															override;
	virtual void OnFadeInComplete()															override;
	virtual void OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)override;
	
	std::map<RKString, xml::UILayerWidgetDec*> LoadWidgetCustom(TiXmlDocument * p_objectXML);
	void addChildObject(Node * child, int z_order = 0);
	void removeChildObject(Node * child , bool isAll = false);


	void onCreateListPlayer(RKString name_game_xml);

	virtual bool SetMyInfoToPanel(RKString name, u64 number_money, int type_money);
	void LoadXMLListPosPlayer(TiXmlDocument * p_objectXML);
protected:

	virtual void InitPlayerChild(xml::UILayerWidgetDec* xml_dec, xml::BasicDec *  dec, int idx);
	void LoadXMLListPlayer(TiXmlDocument * p_objectXML);
public:

	int GetRoomID();
	int GetBoardID();
	bool IsMyInfoRoomOwner();

	virtual void OnCardTouchEnd() ;
	virtual void OnCardTouchDown() ;
	virtual void OnCardTouchMove()	;
	virtual void OnCardTouchCancel() ;
	virtual void OnHandleTouchUpdate(float dt);

	virtual void OnLoadDecPrivatePlayer(xml::BasicDec * dec, PlayerGroup * _player);
	virtual void OnChatInBoard(int fromID, RKString message);
	virtual void OnSystemChat(RKString message);
	virtual void OnMatchResult(std::vector<GameResult*> l);
	virtual void OnReceivedWaitTimeInfo(int sec);
protected:
	/**
	* So nguoi choi lon nhat trong ban.
	*/
	int currentMaxplayer;
	/**
	* Phong dang choi.
	*/
	int _roomID;
	/**
	* Ban dang choi.
	*/
	int _boardID;
	/**
	* Money cuoc cua ban choi.
	*/
	s64 money;
	/**
	* ID chua chu ban.
	*/
	int _ownerID;
	/**
	* true - Dang choi game. false - Dang cho.
	*/
	bool _isStartGame;
	/**
	* Xac dinh xem game da ket thuc hay chua? true - Game da ket thuc.
	*/
	bool _isGameEnd ;
	/**
	* Thoi gian ket thuc luot.
	*/
	long dieTime;
	/**
	* Thoi gian hien thoi. milisecond
	*/
	long currentTime;
	/**
	* Time cua mot luot choi. second
	*/
	int totalTimes;
	/**
	*
	* mang chua vi tri cua nguoi choi
	*/
	std::vector<int> seats;

	//save the data of my info
	PlayerGroup* _myInfo;

	bool		_isViewer;
	bool		needReset;
	// virtual Label lblBoardInfo;

	/**
	* Share caption
	*/
	std::string shareCaption;
public:

	/**
	* Ham nay duoc goi khi server tra ve de kick minh ra khoi ban.
	*
	* @param playerID
	* @param reason
	*            may be null : ly do bi kick, neu rong hien cau thong bao cua
	*            client
	*/

	void kicked(int roomId, int boardId, int playerID, std::string reason);
	virtual bool onKeyBack();
	virtual void doBack();

	void clientQuit();
	void doQuit();

	void doConfirmQuit(std::string msg);
	/**
	* Ham nay dung de kick 1 user ra khoi game.
	*/
	virtual void doKick(int playerID);

	void getFriendListToInvite();

	void showProfileOf(int userID);

	/**
	* Set cho user vao trang thai ready hay k?
	*
	* @param isReady
	*/
	virtual void doReady();
	virtual void doReadyForReady();
	virtual void doQuickPlay();

	void resetReady();
	bool isPasswordSetted;

	/**
	* Ham nay dung de set lai tien dat cuoc khi server tra ve.
	*
	* @param money
	*/
	void setBetMoney(s64 money , s64 max_money = 0);
	/**
	* Remove the player when the player leaved the board. Khong nen override
	* ham nay.
	*
	* @param leaveID
	*/
	void removePlayer(int leaveID);
	/**
	* Thêm player vào danh sách
	*/
	void addPlayer(int seat, Player * p);

	/**
	* Khoi tao thong tin ban khi minh moi vao game. Khi overide ham nay phai
	* goi super truoc.
	*
	* @param roomID
	* @param boardID
	* @param ownerID
	* @param money
	* @param playerInfos
	*/
	//each screen have each style player inherrite from Player* so this function must be null or empty contain to avoid bug
	virtual void setPlayers(
		ubyte roomID,
		ubyte boardID,
		int ownerID,
		s64 money,
		std::vector<Player*> playerInfos) final;

	virtual void CreatePLayer(std::vector<Player*> playerInfos);
	virtual void UpdateInfoPlayer(ubyte roomID, ubyte boardID, int ownerID, s64 money , std::vector<Player*> playerInfos) final;
	virtual void SetMaxPlayer(int max_player) override;
	virtual void onBonusMoney(int playerID, int value, std::string description, s64 currMoney);
	virtual void onHandleBonusMoney(PlayerGroup * player, int value, s64 currMoney);

	virtual void onSomeOneReady(int playerID, bool isReady);
	virtual void onSomeOneQuickPlay(int playerID, bool isQuickPlay);
	/**
	* hàm này được gọi khi actionDone textfield ,hoặc nút OK trong dialog cài
	* đặt tiền cược.
	*
	* @param money
	*/
	virtual void doSetMoney(int money);
	/**
	* cập nhật thông tin bàn
	*/
	virtual void updateBoardInfo();
	virtual void resetSeat();
	/**
	* reset seat phuc vu cho viec join board khong nhay tien cua tat ca cac
	* thang
	*
	*/
	virtual void resetSeatNotUserAction();
	
	//reset all ready
	virtual void ResetReady();

	/**
	* Set chu phong cho game.
	*
	* @param newOwner
	*/
	void setOwner(int newOwner);
	
	void hideStartButton();
	
	void hideReadyButton();

	void showViewerLabel();

	void hideViewerLabel();

	bool hasOrtherPlayerInBoard();
	/**
	* Show button continue game.
	*/
	void setContinueButton();

	void hideContinueButton();
	
	void setStartButton();
	/**
	* Tra ve player co id cu the
	*
	* @param userID
	* @return null neu k tim thay.
	*/
	Player* getPlayer(int userID);
	
	Player* GetPlayerAtIdx(int idx);

	PlayerGroup* getSeatGroup(int seat);

	int			 getNumberPlayer();
	/**
	* Implement ham nay de goi khi reset toan bo game cho van moi.
	*/
	virtual void resetGame();
	/**
	* Goi ham nay de start game.
	*/
	virtual bool doStartGame();
	//
	/**
	* thuc hien ban phao bong
	*
	* @param userID
	*/
	void doFireWork(int userID);

	/**
	* lam mot so hieu ung, am thanh gi do khi minh thang , thuc hien cac hieu
	* ung chung cho tat ca game
	*/
	virtual void doSomethingWhenIWin();

	/**
	* lam mot so hieu ung, am thanh gi do khi minh thua, thuc hien cac hieu ung
	* chung cho tat ca game
	*/
	virtual void doSomethingWhenILose();

	/**
	* lam mot so hieu ung, am thanh gi do khi minh hoa, thuc hien cac hieu ung
	* chung cho tat ca game
	*/
	virtual void doSomethingWhenIRaw();

	//
	void showDialogSendGiftTo(int userID);
	
	//
	//happen when reconnect
	//
	virtual void onReconnect();


	PlayerGroup* getPlayerGroup(int playerID);
	void stopCountDownStart();
	void stopTimeBar();
	void setVisibleCountDownTime(bool isVisible);
	void startTimeBar(int interval,int remain);
	void stopAllTimePlayer();

	virtual		void setSeat(std::vector<Player*> l_players);
	virtual		void setPlayerSeat(std::vector<Player*> l_players);

	virtual		void onSomeOneJoinBoard(int seat, Player * p);
	virtual		void onSomeOneLeaveBoard(int leave, int newOwner);

	virtual		int GetSeatFromPlayerID(int playerID);
	virtual		void flyMoney(s64 money, int userID);

	virtual		void setInviteOrtherPeopleButton();

	size_t GetAvailableNumberPlayers();

	void clearSeat();

	PlayerGroup* getUserGroup(int playerID);
	PlayerGroup* GetSeatGroup(int seat);
	Player* getMyPlayer();
	Vec2 getPlayerPos(PlayerGroup* pg);
	
	bool IsShow();
	void updateTextWithMoneyType();
	void SetEnableTouch(bool value);

	void setMyMoney(s64 money);
	void setMyUsername(std::string username);
	void setRubyMoney(int iddb, s64 ruby);

	void onChat(int fromId, std::string message);
	void onBuzz(int fromId, std::string message);
	/**
	* nhận về bet range từ server
	*
	* @param min
	* @param max
	*/
	virtual void onBetRange(int min, s64 & max, s64 maxMoneyRoom);

private:	

	int findMyIndex(std::vector<Player*> l_players);

};

#endif //__GAME_SCREENS_H__

