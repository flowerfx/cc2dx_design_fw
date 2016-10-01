#include "FriendScreen.h"
#include "Network/Global/Globalservice.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wTextField.h"
#include "UI/AdvanceUI/wListViewTable.h"

#include "Models/ChatMessage.h"
#include "Models/iWinPlayer.h"

#include "Screens/GamePlayMgr.h"
#include "Screens/GameScreens/GameScreens.h"
FriendScreen::FriendScreen()
{
	p_menu_come_from = MENU_LAYER::MENU_NONE;

	p_search_player_layout = nullptr;

	p_ADDFRIEND_MESSAGE.clear();
	p_ADMIN_MESSAGE.clear();
	p_listRecentChat.clear();
	p_listFriendChat.clear();
	p_listPlayerFriend.clear();
	current_number_parse_avatar = 0;
	p_stage_get_list_friend = 0;

	p_current_friend_search = "";
	p_current_idx_search_parse = 0;

	p_search_myfriend_layout = nullptr;
	p_chat_friend_layout = nullptr;
}


FriendScreen::~FriendScreen()
{
	CLEAR_LIST_WHEN_LOGOUT();
}

int FriendScreen::Init()
{
	RKString _menu = "friend_screen";

	InitMenuWidgetEntity(_menu);

	CallInitComponent(true);

	return 1;
}

int	FriendScreen::InitComponent()
{
	p_search_player_layout = GetWidgetChildByName("friend_ui.search_player_layout.search_player");
	p_list_friend_main = GetWidgetChildByName("friend_ui.layout_main_friend_list.panel_list_friend.list_friend_");

	p_layout_pull_update = p_list_friend_main->GetParentWidget()->GetWidgetChildAtIdx(1);
	auto list_friend_main = static_cast<ListViewTableWidget*>(p_list_friend_main);
	list_friend_main->InitTable();
	list_friend_main->addEventListener(
		[this, list_friend_main](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		if (type == EventType_SetCell::ON_SET_CELL)
		{
			int current_idx_to = list_friend_main->GetCurrentIdxTo();
			OnParseListFriendMain(_widget, current_idx_to, p_current_friend_search);

		}
		else if (type == EventType_SetCell::ON_BEGIN_DRAG_CELL)
		{
			if (list_friend_main->current_stage_drag() == EventType_DragCell::DRAG_TOP_DOWN)
			{
				this->p_layout_pull_update->Visible(true);
				this->p_layout_pull_update->SetActionCommandWithName("ORIGIN_POS");
				this->p_layout_pull_update->GetWidgetChildAtIdx(0)->Visible(true);
				this->p_layout_pull_update->GetWidgetChildAtIdx(1)->Visible(false);
				this->p_layout_pull_update->GetWidgetChildAtIdx(2)->SetRotate(0.f);
			}
		}
		else if (type == EventType_SetCell::ON_DRAG_CELL)
		{
			if (list_friend_main->current_stage_drag() == EventType_DragCell::DRAG_TOP_DOWN)
			{
				Vec2 curpos = this->p_layout_pull_update->GetPosition();
				curpos -= list_friend_main->GetDeltaPos();
				this->p_layout_pull_update->SetPosition(curpos);

				float percentmove = list_friend_main->GetPercentMoveDistance();
				float ro = (100.f - percentmove) * 180.f / 100.f;
				this->p_layout_pull_update->GetWidgetChildAtIdx(2)->SetRotate(ro);
			}

		}
		else if (type == EventType_SetCell::ON_END_DRAG_CELL)
		{
			if (list_friend_main->current_stage_drag() == EventType_DragCell::DRAG_TOP_DOWN)
			{
				this->p_layout_pull_update->GetWidgetChildAtIdx(0)->Visible(false);
				this->p_layout_pull_update->GetWidgetChildAtIdx(1)->Visible(true);
				this->p_layout_pull_update->GetWidgetChildAtIdx(2)->SetRotate(180.f);
			}
		}
		else if (type == EventType_SetCell::ON_BEGIN_DROP_CELL)
		{
			if (list_friend_main->current_stage_drag() == EventType_DragCell::DRAG_TOP_DOWN)
			{
				if (list_friend_main->HaveEndDrag())
				{
					//reload list here
					GlobalService::requestFriendList();
					//
					GetSound->PlaySoundEffect("SFX_UPDATELIST");
				}
			}
		}
		else if (type == EventType_SetCell::ON_DROP_CELL)
		{
			if (list_friend_main->current_stage_drag() == EventType_DragCell::DRAG_TOP_DOWN)
			{
				Vec2 curpos = this->p_layout_pull_update->GetPosition();
				curpos -= list_friend_main->GetDeltaPos();
				this->p_layout_pull_update->SetPosition(curpos);

				float percentmove = list_friend_main->GetPercentMoveDistance();
				float ro = (100.f - percentmove) * 180.f / 100.f;
				this->p_layout_pull_update->GetWidgetChildAtIdx(2)->SetRotate(ro);
			}
		}
		else if (type == EventType_SetCell::ON_END_DROP_CELL)
		{
			if (list_friend_main->current_stage_drag() == EventType_DragCell::DRAG_TOP_DOWN)
			{
				this->p_layout_pull_update->Visible(false);
			}

		}
	});

	return 1;
}

int FriendScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}
	return 1;
}

void FriendScreen::VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onVisitMenuWidget(renderer, transform, flags);
}

void FriendScreen::DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onDrawMenuWidget(renderer, transform, flags);
}

void FriendScreen::OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_back")
		{
			ScrMgr->SwitchToMenu(p_menu_come_from, FRIEND_SCREEN);
		}
		else if (name == "btn_add_friend")
		{
			p_search_player_layout->GetParentWidget()->Visible(true);
			p_search_player_layout->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
				this->p_search_player_layout->ForceFinishAction();
				this->SetInteractiveInsteadOf(false, p_search_player_layout);
			}));
		}
		else if (name == "btn_clear_name_search")
		{
			static_cast<TextFieldWidget*>(_widget->GetParentWidget()->GetWidgetChildAtIdx(3))->ClearText();
			_widget->Visible(false);
		}
		else if (name == "btn_clear_player")
		{
			static_cast<TextFieldWidget*>(_widget->GetParentWidget()->GetWidgetChildAtIdx(0))->ClearText();
			_widget->Visible(false);
		}
		else if (name == "btn_close_panel_search")
		{
			p_search_player_layout->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
				this->p_search_player_layout->ForceFinishAction();
				this->p_search_player_layout->GetParentWidget()->Visible(false);
				this->SetInteractiveInsteadOf(true, p_search_player_layout);
			}));
		}
		else if (name == "btn_search_player")
		{
			auto w_text = _widget->GetParentWidget()->GetWidgetChildAtIdx(1)->GetWidgetChildAtIdx(0);
			auto current_text = static_cast<TextFieldWidget*>(w_text)->GetText();
			size_t leng_text = current_text.LengthUTF();
			if (leng_text <= 4)
			{
				ScrMgr->OnShowDialog("notice", L"tên quá ngắn", DIALOG_ONE_BUTTON);
			}
			else
			{
				GameController::searchNick = current_text.GetString();
				GlobalService::searchNick(current_text.GetString());
			}

		}
	}
	else if (type_widget == UI_TYPE::UI_TEXT_FIELD)
	{
		if (name == "name_search_input_DETACH_IME")
		{
			_widget->GetParentWidget()->GetWidgetChildAtIdx(4)->Visible(false);
		}
		else if (name == "name_search_input_ATTACH_IME")
		{
			RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();

			if (string_input.Length() > 0)
			{
				_widget->GetParentWidget()->GetWidgetChildAtIdx(4)->Visible(true);
			}
		}
		else if (name == "name_search_input_INSERT_TEXT")
		{
			_widget->GetParentWidget()->GetWidgetChildAtIdx(4)->Visible(true);
			p_current_friend_search = static_cast<TextFieldWidget*>(_widget)->GetText();
			p_current_idx_search_parse = 0;
			size_t number = GetNumberFriendContainName(p_current_friend_search);
			static_cast<ListViewTableWidget*>(p_list_friend_main)->SetNumberSizeOfCell(number , true);
		}
		else if (name == "name_search_input_DELETE_BACKWARD")
		{
			p_current_friend_search = static_cast<TextFieldWidget*>(_widget)->GetText();
			p_current_idx_search_parse = 0;
			size_t number = GetNumberFriendContainName(p_current_friend_search);
			static_cast<ListViewTableWidget*>(p_list_friend_main)->SetNumberSizeOfCell(number , true);
		}
	
		else if (name == "search_player_input_DETACH_IME")
		{
			_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(false);
		}
		else if (name == "search_player_input_ATTACH_IME")
		{
			RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();

			if (string_input.Length() > 0)
			{
				_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(true);
			}
		}
		else if (name == "search_player_input_INSERT_TEXT")
		{

			_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(true);
		}
	}

}

void FriendScreen::OnDeactiveCurrentMenu()
{
	ScrMgr->CloseCurrentMenu(FRIEND_SCREEN);
	p_menu_show_next = MENU_NONE;

	if (ScrMgr->GetGamePlayMgr() && ScrMgr->GetGamePlayMgr()->GetCurrentGameScreen() != nullptr)
	{
		ScrMgr->GetGamePlayMgr()->GetCurrentGameScreen()->setVisible(true);
	}
}

void FriendScreen::OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)
{
	OnAllProcessFinishFadeIdx(idx, p_max_idx_process_first, status);
}

std::vector<IwinMessage*> FriendScreen::GET_MESSAGE(int state)
{
	if (state == 0)
	{
		return p_ADDFRIEND_MESSAGE;
	}
	else
	{
		return p_ADMIN_MESSAGE;
	}
}

int FriendScreen::GetNumberFriend()
{
	return (p_listPlayerFriend.size() + p_ADDFRIEND_MESSAGE.size());
}

void FriendScreen::PUSH_MESSAGE(int state, std::vector<IwinMessage*> vec)
{
	if (state == 0)
	{
        for(auto msg : p_ADDFRIEND_MESSAGE)
        {
            delete msg;
        }
        p_ADDFRIEND_MESSAGE.clear();
        
        
		for (auto msg : vec)
		{
			p_ADDFRIEND_MESSAGE.push_back(msg);

			int id_avatar = msg->avatar;
			int IDDB = msg->fromID;
			GameMgr->HandleAvatarDownload(id_avatar, msg->fromName,
				[this, IDDB, id_avatar](void * data, void * str, int tag)
			{
				Image * img = (Image *)data;
				if (img && img->getWidth() <= 0)
				{
					CC_SAFE_DELETE(img);
					return;
				}
				Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data , str);
				this->p_list_avatar_download.insert(IDDB, sprite_img);

				auto widget_lb = static_cast<ListViewTableWidget*>(p_list_friend_main);
				if (this->current_number_parse_avatar < (int)widget_lb->GetListWidgetSample().size())
				{
					auto widget = widget_lb->GetListWidgetSample()[this->current_number_parse_avatar];
					if (widget)
					{
						auto panel_avatar = widget->GetWidgetChildByName(".panel_avatar_clip");
						this->ParseSpriteToPanel(panel_avatar, sprite_img, 100);
						this->current_number_parse_avatar++;
					}
				}
			});
		}
		p_stage_get_list_friend++;
		if (p_stage_get_list_friend >= 2)
		{
			static_cast<ListViewTableWidget*>(p_list_friend_main)->SetNumberSizeOfCell(GetNumberFriend());
			p_stage_get_list_friend = 0;
		}
	}
	else
	{
        for(auto msg : p_ADMIN_MESSAGE)
        {
            delete msg;
        }
        p_ADMIN_MESSAGE.clear();
        
		for (auto msg : vec)
		{
			p_ADMIN_MESSAGE.push_back(msg);
		}
	}
}

void FriendScreen::OnReceiveFriendList()
{
 	PASSERT2(false, "not yet handle this, throw assert to remind!");
	p_listPlayerFriend = GameController::FriendList;

	for (Player * p : p_listPlayerFriend)
	{
		int id_avatar = p->avatar;
		int IDDB = p->IDDB;

		GameMgr->HandleAvatarDownload(id_avatar, p->name,
			[this, IDDB, id_avatar](void * data, void * str, int tag)
		{
			Image * img = (Image *)data;
			if (img && img->getWidth() <= 0)
			{
				CC_SAFE_DELETE(img);
				return;
			}
			Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, str);
			this->p_list_avatar_download.insert(IDDB, sprite_img);
			auto widget_lb = static_cast<ListViewTableWidget*>(p_list_friend_main);
			if (this->current_number_parse_avatar < (int)widget_lb->GetListWidgetSample().size())
			{
				auto widget = widget_lb->GetListWidgetSample()[this->current_number_parse_avatar];
				if (widget)
				{
					auto panel_avatar = widget->GetWidgetChildByName(".panel_avatar_clip");
					this->ParseSpriteToPanel(panel_avatar, sprite_img, 100);
					this->current_number_parse_avatar++;
				}
			}
		});
	}

	p_stage_get_list_friend++;
	if (p_stage_get_list_friend >= 2)
	{
		static_cast<ListViewTableWidget*>(p_list_friend_main)->SetNumberSizeOfCell(GetNumberFriend());
		p_stage_get_list_friend = 0;
	}
}

void FriendScreen::ParseListFriend(WidgetEntity* _widget, Player * p)
{
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title_id"))->SetText(p->name);
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".msg_id"))->SetText(p->userProfile->status);
	_widget->GetWidgetChildAtIdx(8)->Visible(false);
	if (p->isReady)
	{
		if (p->onlineInfo != "")
		{
			_widget->GetWidgetChildByName(".ico_online")->Visible(false);
			_widget->GetWidgetChildByName(".ico_playing")->Visible(true);
		}
		else
		{
			_widget->GetWidgetChildByName(".ico_online")->Visible(true);
			_widget->GetWidgetChildByName(".ico_playing")->Visible(false);

		}

		_widget->GetWidgetChildByName(".ico_offline")->Visible(false);
	}
	else
	{
		_widget->GetWidgetChildByName(".ico_online")->Visible(false);
		_widget->GetWidgetChildByName(".ico_offline")->Visible(true);
		_widget->GetWidgetChildByName(".ico_playing")->Visible(false);
	}
}

void FriendScreen::ParseListWaitingFriend(WidgetEntity* _widget, ChatMessage* player)
{
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title_id"))->SetText(player->fromName);
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".msg_id"))->SetText(player->message);
	auto list_btn = _widget->GetWidgetChildAtIdx(8);
	list_btn->Visible(true);
	_widget->GetWidgetChildByName(".msg_id");
	_widget->GetWidgetChildByName(".ico_online")->Visible(true);
	_widget->GetWidgetChildByName(".ico_offline")->Visible(false);
	_widget->GetWidgetChildByName(".ico_playing")->Visible(false);
}

void FriendScreen::OnParseListFriendMain(WidgetEntity* _widget , int idx, RKString name_fill)
{
	int iddb = -1;

	if (name_fill != "")
	{
		if (p_current_idx_search_parse < (int)p_ADDFRIEND_MESSAGE.size())
		{
			for (int i = p_current_idx_search_parse; i < (int)p_ADDFRIEND_MESSAGE.size() ; i++)
			{
				auto p = p_ADDFRIEND_MESSAGE[i];
				p_current_idx_search_parse++;
				if (RKString(p->fromName).Contains(name_fill))
				{
					iddb = p->fromID;
					ParseListWaitingFriend(_widget, p);
					break;
				}
			}
		}

		if(p_current_idx_search_parse >= (int)p_ADDFRIEND_MESSAGE.size())
		{
			for (int i = (p_current_idx_search_parse - (int)p_ADDFRIEND_MESSAGE.size()); i < (int)p_listPlayerFriend.size(); i++ )
			{
				auto p = p_listPlayerFriend[i];
				p_current_idx_search_parse++;
				if (p_current_idx_search_parse >= (int)(p_ADDFRIEND_MESSAGE.size() + p_listPlayerFriend.size()))
				{
					p_current_idx_search_parse = 0;
				}
				if (RKString(p->name).Contains(name_fill))
				{
					iddb = p->IDDB;
					ParseListFriend(_widget, p);
					break;
				}
			}
		}
	}
	else
	{
		if (idx < (int)p_ADDFRIEND_MESSAGE.size())
		{
			ChatMessage* player = p_ADDFRIEND_MESSAGE[idx];
			iddb = player->fromID;
			ParseListWaitingFriend(_widget, player);
		}
		else
		{
			Player * p = p_listPlayerFriend[idx - p_ADDFRIEND_MESSAGE.size()];
			iddb = p->IDDB;
			ParseListFriend(_widget, p);
		}
	}


	auto panel_avatar = _widget->GetWidgetChildByName(".panel_avatar_clip");
	if (p_list_avatar_download.size() > 0 && p_list_avatar_download.find(iddb) != p_list_avatar_download.end())
	{
		this->ParseSpriteToPanel(panel_avatar, p_list_avatar_download.at(iddb), 100);
	}
	
	_widget->GetWidgetChildByName(".bot_line")->Visible(idx % 2 == 0);
	
}

size_t FriendScreen::GetNumberFriendContainName(RKString name)
{
	size_t number = 0; 
	for (auto p : p_ADDFRIEND_MESSAGE)
	{
		if (RKString(p->fromName).Contains(name))
		{
			number++;
		}
	}
	for (auto p : p_listPlayerFriend)
	{
		if (RKString(p->name).Contains(name))
		{
			number++;
		}
	}
	return number;
}

void FriendScreen::CLEAR_LIST_WHEN_LOGOUT()
{
	static_cast<ListViewTableWidget*>(p_list_friend_main)->ResetListView();

	for (auto msg : p_ADDFRIEND_MESSAGE)
	{
		delete msg;
	}
	p_ADDFRIEND_MESSAGE.clear();

	for (auto msg : p_ADMIN_MESSAGE)
	{
		delete msg;
	}
	p_ADMIN_MESSAGE.clear();

	p_listPlayerFriend.clear();

	for (auto it : p_list_avatar_download)
	{
		it.second->removeFromParentAndCleanup(true);
		it.second->release();
	}

	p_list_avatar_download.clear();

	current_number_parse_avatar = 0;
	p_stage_get_list_friend = 0;

	p_current_friend_search = "";
	p_current_idx_search_parse = 0;
}

int FriendScreen::GetStateFriendMenu()
{
	if (p_search_myfriend_layout && p_search_myfriend_layout->Visible())
	{
		return 0;
	}
	else
	{
		if (p_chat_friend_layout && p_chat_friend_layout->Visible())
		{
			return 1;
		}
	}
	return 2;
}

void FriendScreen::onReceiveServerWarning(RKString msg)
{
	ScrMgr->OnShowDialog("error", msg, DIALOG_ONE_BUTTON);
	GetSound->PlaySoundEffect("SFX_BUZZ");
}