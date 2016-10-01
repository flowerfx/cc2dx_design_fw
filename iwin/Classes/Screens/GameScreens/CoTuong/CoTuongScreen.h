#ifndef __GAME_COTUONG_H__
#define __GAME_COTUONG_H__

#include "Screens/GameScreens/GameScreens.h"
#include "UI/AdvanceUI/wLayout.h"

class CoTuongBoard;
class CoTuongPiece;

class CoTuongScreen : public GameScreen
{
private:
	WidgetEntity*		_board_widget;
	CoTuongBoard*		_board;

	CoTuongPiece*		_last_piece;
	
	TextBMFont*			_player_quotes[2];

	float				_totalTimeToPlayPlayer1;
	float				_totalTimeToPlayPlayer2;
	int					_totalTimes;
	int					_whoFirst;
	int					_myColor;
	int					_curSeatUpdateTime;

private:
	void				resetToNewBoard(int myColor, int turnColor);

protected:
	virtual void		  HandleAfterInitPlayer() override;
public:
	static CoTuongScreen* getInstance();


	CoTuongScreen();
	virtual ~CoTuongScreen();

	virtual bool InitGame(int game_id) override;
	virtual int Update(float dt)	   override;
	virtual void OnProcessWidgetChild(RKString name, UI_TYPE type_widget, Utility::UI_Widget::WidgetEntity * _widget) override;
	virtual void LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML)	override;
	void onTouchPiece(Ref* target, Widget::TouchEventType touch_type);
	void onTouchBoard(Ref* target, Widget::TouchEventType touch_type);
	
	void				onStartGame(int whoMoveFirst, int interval);
	void onMove(int whoMove, ubyte xS, ubyte yS, ubyte xD, ubyte yD, int nextMove, ubyte newPieceType);
	void onOpponentWantUndo();
	void onOpponentWantDraw();
	void onOpponentWantLose();
	
	// Send to Server
	void moveCoTuong(ubyte supCommand, ubyte roomId, ubyte boardId, ubyte xS, ubyte yS, ubyte xD, ubyte yD);
	void sendCommandUndoChess(ubyte subCommand, ubyte roomID, ubyte boardID, ubyte request);
	void sendCommandChess(ubyte subCommand, ubyte roomID, ubyte boardID);

	int					getID_CoUp(CoTuongPiece* p);
	std::vector<Vec2>	findMove(CoTuongPiece* p, int turnColor, bool isLogic);
	std::vector<Vec2> findAdvisor(CoTuongPiece* p, int turnColor, bool isLogic);
	std::vector<Vec2> findBishop(CoTuongPiece* p, int turnColor, bool isLogic);
	std::vector<Vec2> findKing(CoTuongPiece* p, int turnColor, bool isLogic);
	std::vector<Vec2> findPawn(CoTuongPiece* p, int turnColor, bool isLogic);
	std::vector<Vec2> findKnight(CoTuongPiece* p, int turnColor, bool isLogic);
	std::vector<Vec2> findRook(CoTuongPiece* p, int turnColor, int nutBiChan[], bool isLogic);
	std::vector<Vec2> findCannon(CoTuongPiece* p, int turnColor, bool isLogic);




	void				setTimeQuota(int userId, int time);
	int					getMyColor(int moveFirst);
	void				updateLabelTime(int playerid);

	std::string			formatTimer(int time);

};

#define getCoTuongScreen() CoTuongScreen::getInstance()

#endif //__GAME_XIZACH_H__

