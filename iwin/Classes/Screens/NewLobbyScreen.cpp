
#include "NewLobbyScreen.h"
#include "UI/AdvanceUI/wTextField.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wListView.h"
#include "UI/AdvanceUI/wScrollView.h"
#include "UI/AdvanceUI/wListViewTable.h"
#include "UI/AdvanceUI/wSlider.h"


#include "Network/Global/Globalservice.h"
#include "Common/GameController.h"
#include "Common/IwinScrollView.h"
#include "Common/IwinListView.h"
#include "Common/ChatBoard.h"
#include "Common/EmotionBoard.h"
#include "Social/SocialManager.h"
#include "Utils.h"
#include "Network/Core/ServerMgr.h"
#include "../Network/Game/GameService.h"

ClippingNode* createAvatarClippingNode(cocos2d::Size node_size)
{
	Sprite* clipping_spr = Sprite::createWithSpriteFrameName("mainscreen/newgui-mainscreen/khung_avatar_mainscreen.png");
	clipping_spr->setScale(node_size.width / clipping_spr->getContentSize().width);
	ClippingNode* node = ClippingNode::create(clipping_spr);
	node->setAlphaThreshold(.5f);
	return node;
}

NewLobbyScreen::NewLobbyScreen():
_scroll_view_with_bet(nullptr),
_cur_lobby_list_data(nullptr)
{
	p_current_game_id = 0;
	p_list_avatar_download.clear();
}

NewLobbyScreen::~NewLobbyScreen()
{
	CC_SAFE_DELETE(_scroll_view_with_bet);
	CC_SAFE_DELETE(_listview_board);
	CC_SAFE_DELETE(_chat_board);

	p_list_avatar_download.clear();
}

int NewLobbyScreen::Init()
{
	RKString _menu = "new_lobby_screen";
	float cur_ratio = GetGameSize().width / GetGameSize().height;
	if (GetGameSize().width <= 480) //ipad ratio
	{
		//_menu = "login_screen_small_size";
	}

	InitMenuWidgetEntity(_menu);

	p_scrollview_chat = GetWidgetChildByName("lobby_ui.panel_left.scrollview_chat");
	p_scrollitem_chat = GetWidgetChildByName("lobby_ui.chat_text");
	_chat_board = new ChatBoard();
	_chat_board->init((ScrollViewWidget*)(p_scrollview_chat), p_scrollitem_chat);

	cocos2d::Size emoboard_size = GetGameSize();
	_emotion_board = EmotionBoard::create(cocos2d::Size(emoboard_size.width, emoboard_size.height / 3));
	_emotion_board->setPosition(Vec2(.0f, -emoboard_size.height / 3));
	_emotion_board->setHandleChat(CC_CALLBACK_1(NewLobbyScreen::handleChatEmotion, this));
	addChild(_emotion_board);
	
	CallInitComponent(true);

	return 1;
}

int	NewLobbyScreen::InitComponent()
{
	p_scrollview_bet = GetWidgetChildByName("lobby_ui.panel_right_lobby.scrollview_bet");
	p_scrollitem_bet = GetWidgetChildByName("lobby_ui.panel_item_bet");

	p_panel_user_info = GetWidgetChildByName("lobby_ui.panel_user_info");
	p_panel_right_lobby = GetWidgetChildByName("lobby_ui.panel_right_lobby");
	p_panel_right_board = GetWidgetChildByName("lobby_ui.panel_right_board");

	p_chat_text_field = GetWidgetChildByName("lobby_ui.panel_left.Image_4.textfield_chat");

	p_money_value = p_panel_right_board->GetWidgetChildByName(".layout_play_with_money.btn_play_with_money.lbl_money_value");
	p_bet_slider = p_panel_right_board->GetWidgetChildByName(".slider_money");

	p_panel_right_lobby->SetVisible(true);
	p_panel_right_board->SetVisible(false);

	_scroll_view_with_bet = new IwinScrollView();
	_scroll_view_with_bet->init((ScrollViewWidget*)(p_scrollview_bet), p_scrollitem_bet, CC_CALLBACK_3(NewLobbyScreen::updateItemScrollViewBet, this), CC_CALLBACK_3(NewLobbyScreen::initItemScrollViewBet, this));
	_scroll_view_with_bet->setMaxItem(3);

	p_list_player_lobby = p_panel_right_board->GetWidgetChildByName(".tableview_board");
	static_cast<ListViewTableWidget*>(p_list_player_lobby)->addEventListener(
		[this](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		if (type == EventType_SetCell::ON_SET_CELL)
		{
			_widget->Visible(true);
			int current_idx_to = static_cast<ListViewTableWidget*>(this->p_list_player_lobby)->GetCurrentIdxTo();
			updateItemListViewBoard(_widget, current_idx_to);
		}
		else if (type == EventType_SetCell::ON_END_TOUCH_CELL)
		{
			auto ldb = static_cast<ListViewTableWidget*>(this->p_list_player_lobby);
			size_t current_idx = ldb->GetCurrentIdxSelected();
			if (current_idx >= 0 && current_idx < _cur_list_board.size())
			{
				auto idx_player = _cur_list_board.at(current_idx);

				int room_id = idx_player.getRoomId();
				int board_id = idx_player.getBoardId();
				bool password = idx_player.getPassword();
				if (!password)
				{
					GlobalService::joinBoard(room_id, board_id, "");
				}
			}
			else
			{
				PASSERT2(false, "have problem here!");
			}

		}
	});

	static_cast<ListViewTableWidget*>(p_list_player_lobby)->InitTable();
	return 1;
}

int NewLobbyScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}

	if (p_list_text_chat.size() > 0)
	{
		ChatLobbyItem* item = p_list_text_chat[0];
		_chat_board->addChat(item->getUserName(), Utils::chatConvert(item->getMessage()));
		p_list_text_chat.erase(p_list_text_chat.begin());
		delete item;
	}

	return 1;
}

void NewLobbyScreen::VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onVisitMenuWidget(renderer, transform, flags);
}

void NewLobbyScreen::DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onDrawMenuWidget(renderer, transform, flags);
}

void NewLobbyScreen::OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_BUTTON)
	{
		if (name == "btn_back")
		{
			ScrMgr->SwitchToMenu(MAIN_SCREEN, NEW_LOBBY_SCREEN)
			/*ReloadUI("xml/screens/LobbyScreen.xml");
			Init()*/;
		}
		else if (name == "btn_play_now")
		{
			LobbyItem* item = (LobbyItem*)(_widget->GetResource()->getUserData());
			GlobalService::doPlayNowLobby(item->getMoneyType(), item->getMoney());
		}
		else if (name == "btn_listboard")
		{
			/*LobbyItem* item = (LobbyItem*)(_widget->GetResource()->getUserData());
			GlobalService::getBoardFromLobby(item->getMoneyType(), item->getMoney())*/;
			showListBoard(true);
		}
		else if (name == "btn_listlobby")
		{
			showListBoard(false);
		}
		else if (name == "btn_ruby")
		{
			showLobby(RUBY_TYPE);
		}
		else if (name == "btn_win")
		{
			showLobby(WIN_TYPE);
		}
		else if (name == "btn_emotion")
		{
			if (!_emotion_board->isVisible())
			{
				_emotion_board->show();
				_widget->SetColor(Color3B::YELLOW);
			}
			else
			{
				_emotion_board->hide();
				_widget->SetColor(Color3B::WHITE);
			}
		}
		else if (name == "btn_send")
		{
			RKString cur_text = static_cast<TextFieldWidget*>(p_chat_text_field)->GetText();
			if (cur_text != "")
			{
				GlobalService::sendLobbyChat(GameController::getCurrentGameType(), GameController::myInfo->IDDB, cur_text.GetString());
			}
			static_cast<TextFieldWidget*>(p_chat_text_field)->ClearText();
			auto panel = p_chat_text_field->GetParentWidget();
			panel->SetActionCommandWithName("DEFOCUS", CallFunc::create([panel]() {
				panel->ForceFinishAction();
				panel->GetWidgetChildByName(".btn_send")->SetActionCommandWithName("DEFOCUS_POS");
			}));
		}
		else if (name == "btn_play_with_money")
		{
			GlobalService::doPlayNowLobby(GameController::currentMoneyType, _current_bet);
		}
		else if (name == "btn_rich")
		{
			if (ScrMgr->IsRubyState())
			{				
				ShowRichestRuby();
			}
			else
			{
				ShowRichestWin();
			}
		}
		else if (name == "btn_strong")
		{
			ShowStrongest();
		}
	}
	else if (type_widget == UI_TYPE::UI_SLIDER)
	{
		if (name == "slider_money_PERCENTAGE_CHANGED")
		{
			float value = static_cast<SliderWidget*>(_widget)->GetPercent();
			s64 money;
			int index = value / 100.0f * _max_money_slider;
			if (index >= 0 && index < Utils::s_popular_bet.size())
			{
				money = Utils::s_popular_bet[index];
				static_cast<TextWidget*>(p_money_value)->SetText(StringUtils::format("%s", Utils::formatNUmber_dot(money).c_str()));
				_current_bet = money;
			}
		}
		else if(name == "slider_money_SLIDEBALL_UP")
		{
			GlobalService::getBoardFromLobby(GameController::currentMoneyType, _current_bet);
		}
	}
	else if (type_widget == UI_TYPE::UI_TEXT_FIELD)
	{
		if (name == "textfield_chat_ATTACH_IME")
		{
			auto panel = _widget->GetParentWidget();
			panel->SetActionCommandWithName("FOCUS", CallFunc::create([panel , _widget]() {
				panel->ForceFinishAction();
				panel->GetWidgetChildByName(".btn_send")->SetActionCommandWithName("FOCUS_POS");
				Vec2 size_panel = panel->GetSize();
				static_cast<TextFieldWidget*>(_widget)->SetTouchZone(size_panel);
				static_cast<TextFieldWidget*>(_widget)->SetTextArea(Vec2(size_panel.x * 0.92, size_panel.y));
			}));
		}
		else if (name == "textfield_chat_DETACH_IME")
		{
			auto panel = _widget->GetParentWidget();
			panel->SetActionCommandWithName("DEFOCUS", CallFunc::create([panel , _widget]() {
				panel->ForceFinishAction();
				panel->GetWidgetChildByName(".btn_send")->SetActionCommandWithName("DEFOCUS_POS");
				Vec2 size_panel = panel->GetSize();
				static_cast<TextFieldWidget*>(_widget)->SetTouchZone(size_panel);
				static_cast<TextFieldWidget*>(_widget)->SetTextArea(Vec2(size_panel.x * 0.73, size_panel.y));
			}));
		}
	}

}

void NewLobbyScreen::OnDeactiveCurrentMenu()
{
	if (p_menu_show_next == HUD_SCREEN)
	{
		vector<RKString> name_list;
		SocialMgr->RemoveAllCallBackExcept(name_list);
	}

	ScrMgr->CloseCurrentMenu(NEW_LOBBY_SCREEN);

	p_current_idx_add_ = -1;
	p_menu_show_next = MENU_NONE;

}


void NewLobbyScreen::OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)
{
	OnAllProcessFinishFadeIdx(idx, p_max_idx_process_first, status);
}

void NewLobbyScreen::OnBeginFadeOut()
{
	auto panel = p_panel_right_board->GetWidgetChildByName(".layout_play_with_money");
	auto wline = panel->GetWidgetChildByName(".line_glow_1");
	wline->GetResource()->stopAllActions();
	wline = panel->GetWidgetChildByName(".line_glow_2");
	wline->GetResource()->stopAllActions();
	wline = panel->GetWidgetChildByName(".star_glow");
	wline->GetResource()->stopAllActions();
}

void NewLobbyScreen::OnBeginFadeIn()
{
	_chat_board->reset();

	showListBoard(true);
}

void NewLobbyScreen::ParseUserUI()
{
	User * user = GetUser;
	if (user == nullptr)
	{
		PASSERT2(false, "user is null");
		return;
	}
	RKString nick_name = user->nickName;

	p_panel_user_info->GetWidgetChildByName(".Image_12.Image_13")->SetVisible(GameController::currentMoneyType != BASE_MONEY);
	p_panel_user_info->GetWidgetChildByName(".Image_12.Image_13_0")->SetVisible(GameController::currentMoneyType == BASE_MONEY);

	static_cast<TextWidget*>(p_panel_user_info->GetWidgetChildByName(".lbl_username"))->SetText(nick_name , true);
	if (GameController::currentMoneyType == BASE_MONEY)
	{
		static_cast<TextWidget*>(p_panel_user_info->GetWidgetChildByName(".Image_12.lbl_money"))->SetText(Utils::formatNumber_win(GameController::myInfo->money), true);
	}
	else
	{
		static_cast<TextWidget*>(p_panel_user_info->GetWidgetChildByName(".Image_12.lbl_money"))->SetText(Utils::formatNumber_win(GameController::myInfo->ruby), true);
	}


	auto panel_avatar = p_panel_user_info->GetWidgetChildByName(".btn_avatar.panel_avatar");

	GameMgr->HandleAvatarDownload(user->avatarID, user->nickName,
		[this, panel_avatar](void * data, void * str, int tag)
	{
		this->ParseImgToPanel(panel_avatar, data, str, tag);
	});
}

void NewLobbyScreen::OnFadeInComplete()
{
	BaseScreen::OnFadeInComplete();
	auto panel = p_panel_right_board->GetWidgetChildByName(".layout_play_with_money");
	auto wline = panel->GetWidgetChildByName(".line_glow_1");
	wline->SetActionCommandWithName("MOVE");
	wline = panel->GetWidgetChildByName(".line_glow_2");
	wline->SetActionCommandWithName("MOVE");
	wline = panel->GetWidgetChildByName(".star_glow");
	wline->SetActionCommandWithName("ROTATE");
}

void NewLobbyScreen::onSetGameTypeSuccessed()
{
	GlobalService::getLobbyList(GameController::getCurrentGameType(), GameController::currentMoneyType);
	GlobalService::getGlobalChatList();
	std::string server_name = GetServerMgr()->getCurServer()._name;
	std::string title = server_name + "-" + GameController::getCurrentGameName();
	setTitle(title);

	ScrMgr->setBtnTaixiuDirty();
}

void NewLobbyScreen::showListBoard(bool visible)
{
	p_panel_right_lobby->SetVisible(!visible);
	p_panel_right_board->SetVisible(visible);
	GetWidgetChildByName("lobby_ui.top_panel.btn_listlobby")->SetVisible(visible);
	GetWidgetChildByName("lobby_ui.top_panel.btn_listboard")->SetVisible(!visible);

	ScrMgr->SetScreenIDDirty();
}

void NewLobbyScreen::showLobby(int type)
{
	GlobalService::getLobbyList(GameController::getCurrentGameType(), type);
	p_panel_right_lobby->GetWidgetChildByName(".panel_button.tab_ruby_select")->SetVisible(type == RUBY_TYPE);
	p_panel_right_lobby->GetWidgetChildByName(".panel_button.tab_win_select")->SetVisible(type == WIN_TYPE);
}

void NewLobbyScreen::updateItemScrollViewBet(IwinScrollView* scrollview, WidgetEntity* entity, int idx)
{
	LobbyItem* item = _cur_lobby_list_data->getLobbyItem(idx);
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_count_board"))->SetText(StringUtils::format("%d", item->getBoardsWaitingCount()));
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_count_user"))->SetText(StringUtils::format("%d", item->getUsersWaitingCount()));
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_money"))->SetText(StringUtils::format("%s", Utils::formatNUmber_dot(item->getMoney()).c_str()));
	cocos2d::Vec2 icon_size = entity->GetWidgetChildByName(".icon_coin")->GetSize();
	cocos2d::Size str_size = static_cast<TextBMFont*>(static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_money"))->GetResource())->getContentSize();
	float scale = static_cast<TextBMFont*>(static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_money"))->GetResource())->getScale();
	Vec2 item_size = entity->GetSize();
	int offsetx = (item_size.x - str_size.width * scale - icon_size.x) / 2;
	entity->GetWidgetChildByName(".icon_ruby")->SetVisible(item->getMoneyType() == RUBY_TYPE);
	entity->GetWidgetChildByName(".icon_coin")->SetVisible(item->getMoneyType() != RUBY_TYPE);
	Vec2 pos = entity->GetWidgetChildByName(".icon_coin")->GetPosition();
	entity->GetWidgetChildByName(".icon_coin")->SetPosition(Vec2(offsetx + icon_size.x / 2, pos.y));
	entity->GetWidgetChildByName(".icon_ruby")->SetPosition(Vec2(offsetx + icon_size.x / 2, pos.y));
	entity->GetWidgetChildByName(".lbl_money")->SetPosition(Vec2(offsetx + icon_size.x, pos.y));
	entity->GetWidgetChildByName(".btn_play_now")->GetResource()->setUserData(item);
}

void NewLobbyScreen::initItemScrollViewBet(IwinScrollView* scrollview, WidgetEntity* entity, int idx)
{
	p_scrollview_bet->GetParentLayer()->AddEventType(xml::TYPE_UI_WIDGET::WIDGET_BUTTON, entity->GetWidgetChildByName(".btn_play_now"));
}


void NewLobbyScreen::updateItemListViewBoard(WidgetEntity* entity, int idx)
{
	BoardOwner owner = _cur_list_board.at(idx);
	entity->Visible(true);
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_name"))->SetText(owner.getUsername(), true);
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_money"))->SetText(StringUtils::format("%d", owner.getMoney()), true);
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_win"))->SetText(std::to_string(owner.getWin()), true);
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_lose"))->SetText(std::to_string(owner.getLose()), true);
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_draw"))->SetText(std::to_string(owner.getDraw()), true);
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_quit"))->SetText(std::to_string(owner.getLeave()) , true);
	for (int i = 0; i < 6; i++)
	{
		entity->GetWidgetChildByName(StringUtils::format(".icon_konguoi_%d", i))->SetVisible(i < owner.getCountMaxPlayer());
	}
	for (int i = 0; i < 6; i++)
	{
		entity->GetWidgetChildByName(StringUtils::format(".icon_conguoi_%d", i))->SetVisible(i < owner.getCountPlayer());
	}

	auto panel_avatar = entity->GetWidgetChildByName(".panel_avatar");

	if (p_list_avatar_download.size() > 0 && p_list_avatar_download.find(owner.getUsername()) != p_list_avatar_download.end())
	{
		this->ParseSpriteToPanel(panel_avatar, p_list_avatar_download.at(owner.getUsername()), 100);
	}
}


void NewLobbyScreen::onReceiveLobbyList(LobbyList* lobby_list)
{

	CC_SAFE_DELETE(_cur_lobby_list_data);
	_cur_lobby_list_data = lobby_list;

	_scroll_view_with_bet->reload(_cur_lobby_list_data->getList().size());

	Utils::s_popular_bet.clear();
	std::vector<LobbyItem*> list_lobby = lobby_list->getList();
	for (auto l : list_lobby)
	{
		Utils::s_popular_bet.push_back(l->getMoney());
	}



	s64 findBoardMoney = 0;
	int index = 0;
	if ( GameController::isBaseMoney()) {
		findBoardMoney = (s64)(GameController::myInfo->money / 10);
		_max_money_slider = Utils::getMaxValidBetIdx(GameController::myInfo->money);
	}
	else
	{
		findBoardMoney = (s64)(GameController::myInfo->ruby / 10);
		_max_money_slider = Utils::getMaxValidBetIdx(GameController::myInfo->ruby);
	}

	if (findBoardMoney < Utils::s_popular_bet[0]) {
		findBoardMoney = Utils::s_popular_bet[0];
		index = 0;
	}
	else {
		for (size_t i = 0; i < Utils::s_popular_bet.size(); i++) 
		{
			if (Utils::s_popular_bet[i] > findBoardMoney) 
			{
				findBoardMoney = Utils::s_popular_bet[i - 1];
				index = i - 1;
				break;
			}
		}
	}



	if (findBoardMoney > -1) {
		_current_bet = findBoardMoney;
		// lần đâu tiên vô từ tự động đăng nhập, set mức cược này
		static_cast<SliderWidget*>(p_bet_slider)->SetPercent(index * 1.0f / _max_money_slider * 100);
		static_cast<TextWidget*>(p_money_value)->SetText(StringUtils::format("%s", Utils::formatNUmber_dot(Utils::s_popular_bet[index]).c_str()));
	}

	GlobalService::getBoardFromLobby(GameController::currentMoneyType, _current_bet);
}

void NewLobbyScreen::onReceiveOwnerBoardList(std::vector<BoardOwner> boards, int win, int lose, int draw, int leave)
{
	_cur_list_board = boards;

	static_cast<ListViewTableWidget*>(p_list_player_lobby)->ResetListView();
	static_cast<ListViewTableWidget*>(p_list_player_lobby)->SetNumberSizeOfCell(_cur_list_board.size());
	p_panel_right_board->GetWidgetChildByName(".lbl_no_board")->SetVisible(_cur_list_board.size() == 0);
	p_panel_right_board->GetWidgetChildByName(".lbl_tutorial_create_board")->SetVisible(_cur_list_board.size() == 0);
	p_panel_right_board->GetWidgetChildByName(".icon_arrow")->SetVisible(_cur_list_board.size() == 0);
	p_panel_right_board->GetWidgetChildByName(".menu_bar")->SetVisible(_cur_list_board.size() != 0);
	if (_cur_list_board.size() == 0) {
		updateNoBoardLabel(_current_bet);
	}


	//download avatar in save into the list
	for (size_t i = 0; i < _cur_list_board.size(); i++)
	{
		auto idx_player = _cur_list_board[i];
		{
			GameMgr->HandleAvatarDownload(idx_player.getAvatar(), idx_player.getUsername(),
				[this, idx_player, i](void * data, void * str, int tag)
			{
				Image * img = (Image *)data;
				if (this->_cur_list_board.size() > 0)
				{
					Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, str);
					this->p_list_avatar_download.insert(idx_player.getUsername(), sprite_img);

					auto widget_lb = static_cast<ListViewTableWidget*>(p_list_player_lobby);
					if ((size_t)i < widget_lb->GetListWidgetSample().size())
					{
						auto widget = widget_lb->GetListWidgetSample()[i];
						if (widget)
						{
							auto panel_avatar = widget->GetWidgetChildByName(".panel_avatar");
							this->ParseSpriteToPanel(panel_avatar, sprite_img, 100);
						}
					}
				}
			});
		}
	}

}

void NewLobbyScreen::onReceiveChatList(ChatLobbyList* chatLobbyList)
{
	std::vector<ChatLobbyItem*> lobby_items = chatLobbyList->getChatList();
	for (auto it : lobby_items)
	{
		p_list_text_chat.push_back(it);		
	}
}

void NewLobbyScreen::handleChatEmotion(int idx)
{
	RKString cur_text = static_cast<TextFieldWidget*>(p_chat_text_field)->GetText();
	cur_text += Utils::getEmotionStr(idx);
	static_cast<TextFieldWidget*>(p_chat_text_field)->SetText(cur_text);
}

void NewLobbyScreen::setTitle(std::string text)
{
	static_cast<TextWidget*>(GetWidgetChildByName("lobby_ui.top_panel.lbl_title"))->SetText(text , true);
}


void NewLobbyScreen::updateNoBoardLabel(s64 money)
{
	TextWidget* no_board_label = static_cast<TextWidget*>(p_panel_right_board->GetWidgetChildByName(".lbl_no_board"));
	std::string text = StringUtils::format("There is currently no available tables at %s", Utils::formatMoney_dot_win(money).c_str());
	static_cast<TextWidget*>(p_panel_right_board->GetWidgetChildByName(".lbl_tutorial_create_board"))->SetText("Please press play button to create a new table");
	no_board_label->SetText(text);
	float scale = static_cast<TextBMFont*>(no_board_label->GetResource())->getScale();
	cocos2d::Size text_size = static_cast<TextBMFont*>(no_board_label->GetResource())->getContentSize();
	text_size.width *= scale;
	text_size.height *= scale;

	int pos_left = no_board_label->GetPosition().x - text_size.width / 2;
	WidgetEntity* arrow = p_panel_right_board->GetWidgetChildByName(".icon_arrow");
	Vec2 cur_pos = arrow->GetPosition();
	arrow->SetPosition(Vec2(pos_left, cur_pos.y));
}

void NewLobbyScreen::SetCurrentGameID(int id)
{
	p_current_game_id = id;

	bool p_is_ruby_lobby = ScrMgr->IsRubyState();
	if (p_is_ruby_lobby)
	{
		GetWidgetChildByName("lobby_ui.background_1")->Visible(false);
		GetWidgetChildByName("lobby_ui.background_2")->Visible(true);
	}
	else
	{
		GetWidgetChildByName("lobby_ui.background_1")->Visible(true);
		GetWidgetChildByName("lobby_ui.background_2")->Visible(false);
	}


	if (p_previous_game_id != id)
	{
		ResetLeaderboard();

		static_cast<ListViewTableWidget*>(p_list_player_lobby)->ResetListView();
		p_list_avatar_download.clear();
		_cur_list_board.clear();
	}
	p_previous_game_id = id;
}

void NewLobbyScreen::ResetLeaderboard(bool cleanup)
{
	GetCommonScr->LB_ResetLeaderboard(cleanup);
}

void NewLobbyScreen::ShowRichestWin()
{
	auto scr_common = GetCommonScr;
	scr_common->SetScreenComeFrom(this);
	GetCommonScr->LB_ShowRichestWin();
}

void NewLobbyScreen::ShowRichestRuby()
{
	auto scr_common = GetCommonScr;
	scr_common->SetScreenComeFrom(this);
	GetCommonScr->LB_ShowRichestRuby();
}

void NewLobbyScreen::ShowStrongest()
{
	auto scr_common = GetCommonScr;
	scr_common->SetScreenComeFrom(this);
	GetCommonScr->LB_ShowStrongest(p_current_game_id);
}

int NewLobbyScreen::GetStateNewLobbyMenu()
{
	if (p_panel_right_board->Visible())
	{
		return 0;
	}
	else
	{
		if (p_panel_right_lobby->Visible())
		{
			return 1;
		}
	}
	return 2;
}