#include "LoadDataXML.h"
#include "DisplayView.h"
#include <string>
#include <regex>
std::vector<std::string> split(const std::string& s, char seperator)
{
	std::vector<std::string> output;

	std::string::size_type prev_pos = 0, pos = 0;

	while ((pos = s.find(seperator, pos)) != std::string::npos)
	{
		std::string substring(s.substr(prev_pos, pos - prev_pos));

		output.push_back(substring);

		prev_pos = ++pos;
	}

	output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word

	return output;
}

namespace Utility
{
	namespace xml
	{
#pragma region load_texture_dec_xml
		AtlasChild::AtlasChild() :
			Size(0, 0),
			Position(-1, -1)
		{
		}
		AtlasChild::~AtlasChild()
		{
			Size = Vec2(0, 0);
			Position = Vec2(0, 0);
		}
		bool AtlasChild::IsValidPosition()
		{
			if (Position.x < 0 || Position.y < 0)
				return false;
			return true;
		}

		AtlasTexture::AtlasTexture() :
			NameImage(""),
			NamePath("")
		{
		}
		AtlasTexture::~AtlasTexture()
		{
			dDataSize.clear();
		}
		AtlasChild AtlasTexture::GetAtlasChild(RKString str)
		{
			if (dDataSize.size() > 0 && dDataSize.find(str) != dDataSize.end())
			{
				return dDataSize.at(str);
			}
			AtlasChild a;
			return a;
		}
		void AtlasTexture::InsertAtlasChild(RKString str, AtlasChild d)
		{
			dDataSize.insert(std::pair<RKString, AtlasChild >(str, d));
		}
		int AtlasTexture::GetNumberAtlas()
		{
			return (int)dDataSize.size();
		}

		TextPackerDetail::TextPackerDetail()
		{
			texture_path = "";
			texture_plist = "";
		}
		TextPackerDetail::TextPackerDetail(RKString path, RKString plist)
		{
			texture_path = path;
			texture_plist = plist;
		}
		bool TextPackerDetail::IsValid() {
			return texture_path != "";
		}

		TextPacker::TextPacker()
		{
		}
		TextPacker::~TextPacker()
		{
			p_list_texture_packer.clear();
		}
		TextPackerDetail TextPacker::GetTexPackByName(RKString str, int idx)
		{
			if (p_list_texture_packer.size() > 0 && p_list_texture_packer.find(str) != p_list_texture_packer.end())
			{
				if (idx < 0 || idx >= (int)p_list_texture_packer.at(str).size())
				{
					TextPackerDetail a;
					return a;
				}
				return p_list_texture_packer.at(str).at(idx);
			}
			TextPackerDetail a;
			return a;
		}
		void TextPacker::InsertTexPack(RKString str, std::vector<TextPackerDetail> d)
		{
			p_list_texture_packer.insert(std::pair<RKString, std::vector<TextPackerDetail> >(str, d));
		}
		int TextPacker::GetNumberTextureByName(RKString name)
		{
			if (p_list_texture_packer.size() > 0 && p_list_texture_packer.find(name) != p_list_texture_packer.end())
			{
				return p_list_texture_packer.at(name).size();
			}
			return 0;
		}
		int TextPacker::GetNumberTexPack()
		{
			return (int)p_list_texture_packer.size();
		}

		TextAtlasDetail::TextAtlasDetail()
		{
			name_global_path = "";
			name_sub_fix = "";
			texture_atlas_path = "";
			list_texture_path.clear();
		}
		TextAtlasDetail::TextAtlasDetail(
			RKString _sub_fix,
			RKString _name_path, 
			RKString path_atlas, 
			std::vector<RKString> _list_texture_path)
		{
			name_global_path = _name_path;
			name_sub_fix = _sub_fix;
			texture_atlas_path = path_atlas;
			list_texture_path = _list_texture_path;
		}
		TextAtlasDetail::~TextAtlasDetail()
		{
			name_global_path = "";
			texture_atlas_path = "";
			list_texture_path.clear();
		}
		bool TextAtlasDetail::IsValid()
		{
			return texture_atlas_path != "";
		}

		zone_textAtlas_detail::zone_textAtlas_detail()
		{
			subfix = "";
			endfix = "";
			list_texture.clear();
		}
		zone_textAtlas_detail::zone_textAtlas_detail(RKString _subfix, RKString _refix, std::vector<TextAtlasDetail> _list_texture)
		{
			subfix = _subfix;
			endfix = _refix;
			list_texture = _list_texture;
		}
		zone_textAtlas_detail::~zone_textAtlas_detail()
		{
			subfix = "";
			endfix = "";
			list_texture.clear();
		}


		TextAtlas::TextAtlas()
		{
			p_list_texture.clear();
			SizeFit = Vec2(0, 0);
		}
		TextAtlas::~TextAtlas()
		{
			p_list_texture.clear();
			SizeFit = Vec2(0, 0);
		}
		zone_textAtlas_detail TextAtlas::GetTexAtlasByName(RKString str)
		{
			if (p_list_texture.size() > 0 && p_list_texture.find(str) != p_list_texture.end())
			{
				return p_list_texture.at(str);
			}
			zone_textAtlas_detail a;
			return a;
		}
		void TextAtlas::InsertTexAtlas(RKString str, zone_textAtlas_detail d)
		{
			p_list_texture.insert(std::pair<RKString, zone_textAtlas_detail>(str, d));
		}
		int TextAtlas::GetNumberTexAtlasByName(RKString name)
		{
			return (int)p_list_texture.at(name).list_texture.size();
		}
		int TextAtlas::GetNumberTexAtlas()
		{
			return (int)p_list_texture.size();
		}


		TexFrameDetail::TexFrameDetail()
		{
			texture_path = "";
		}

		TexFrameDetail::TexFrameDetail(RKString path, xml::BasicDec _value)
		{
			texture_path = path;
			value = _value;
		}

		TexFrameDetail::~TexFrameDetail()
		{
			texture_path = "";
			value.clearAllData();
		}

		TexFrame::TexFrame()
		{
			list_tex_frame_sheet.clear();
		}

		TexFrame::~TexFrame()
		{
			list_tex_frame_sheet.clear();
		}

		TexFrameDetail TexFrame::GetTexFrameDetailByName(RKString str)
		{
			if (list_tex_frame_sheet.size() > 0 && list_tex_frame_sheet.find(str) != list_tex_frame_sheet.end())
			{
				return list_tex_frame_sheet.at(str);
			}
			return TexFrameDetail();
		}

		void TexFrame::InsertTexFrameDetail(RKString str, TexFrameDetail d)
		{
			list_tex_frame_sheet.insert({str , d});
		}

		int TexFrame::GetNumberTexFrameDetail()
		{
			return (int)list_tex_frame_sheet.size();
		}

		ListTexFrame::ListTexFrame()
		{
			list_tex_frame.clear();
		}
		ListTexFrame::~ListTexFrame()
		{
			list_tex_frame.clear();
		}
		TexFrame ListTexFrame::GetTexFrameByName(RKString str)
		{
			if (list_tex_frame.size() > 0 && list_tex_frame.find(str) != list_tex_frame.end())
			{
				return list_tex_frame.at(str);
			}
			return TexFrame();
		}
		void ListTexFrame::InsertTexFrame(RKString str, TexFrame d)
		{
			list_tex_frame.insert({ str , d });
		}

		int ListTexFrame::GetNumberTexFrame()
		{
			return (int)list_tex_frame.size();
		}
		
#pragma endregion load_texture_dec_xml

#pragma region load_object_basic_xml
		FontDec::FontDec()
		{
				idx = 0;
				path = "";
				name = "";
				d_size = 0;
				d_width_border = 0;
				d_color[0] = 255; d_color[1] = 255; d_color[2] = 255; d_color[3] = 255;
				d_color_border[0] = 255; d_color_border[1] = 255; d_color_border[2] = 255; d_color_border[3] = 255;
				IsUnicode = false;
				IsBitmap = false;
		}
		FontDec:: ~FontDec() {}



		SpriteDetail::SpriteDetail(RKString _str_img, cocos2d::Rect _rect_img)
		{
			str_img = _str_img;
			rect_img = _rect_img;
		}
		SpriteDetail::SpriteDetail(RKString _str_img)
		{
			str_img = _str_img;
			rect_img = cocos2d::Rect(-1, -1, 0, 0);
		}
		SpriteDetail::SpriteDetail()
		{
			str_img = "";
			rect_img = cocos2d::Rect(-1, -1, 0, 0);
		}
		SpriteDetail::~SpriteDetail()
		{
			str_img = "";
		}


		AnimatureDec::AnimatureDec()
		{
			name = "";
			pathTexture = "";
			pathPlist = "";
			PathJson = "";
			SpeedScale = 1.0f;
			idxPlay = 0;
		}
		AnimatureDec::~AnimatureDec()
		{
			name = "";
			pathTexture = "";
			pathPlist = "";
			PathJson = "";
			SpeedScale = 1.0f;
			idxPlay = 0;
		}

		AnimateDec::AnimateDec()
		{
			time_delay = 0.1f;
			loop = -1; //loop forever

			tag_action = 0;
		}
		AnimateDec::~AnimateDec()
		{
			p_SpriteFrame.clear();
		}
		void AnimateDec::InsertSpriteFrameName(RKString name)
		{
			p_SpriteFrame.push_back(SpriteDetail(name));
		}

		void AnimateDec::InsertSpriteFrameNameRect(RKString name, cocos2d::Rect rec)
		{
			p_SpriteFrame.push_back(SpriteDetail(name, rec));
		}

		SpriteDetail AnimateDec::GetSpriteFrameNameAtIdx(int idx)
		{
			if (idx >= 0 && (unsigned int)idx < p_SpriteFrame.size())
			{
				return p_SpriteFrame.at((unsigned int)idx);
			}

			return SpriteDetail();
		}
		int AnimateDec::GetNumberSprite()
		{
				return p_SpriteFrame.size();
		}

		ObjectDec::ObjectDec()
			{
				idx = 0;
				name = "";
				name_source = NULL;
				animature_dec = NULL;
				animation_dec = NULL;
				Move_delay_dec = NULL;
				Position_dec = NULL;
				Size_dec = NULL;
				Opacity_dec = NULL;
				Rotate_dec = NULL;
				Rotate_around_dec = NULL;
				Color_dec = NULL;
				Process_dec = NULL;
				text_dec = NULL;
				StickScreen = 0;
			}
		ObjectDec::~ObjectDec()
			{
				if (name_source) {
					delete name_source;
					name_source = NULL;
				}
				if (animature_dec) {
					delete animature_dec;
					animature_dec = NULL;
				}
				if (Position_dec) {
					delete Position_dec;
					Position_dec = NULL;
				}
				if (Size_dec) {
					delete Size_dec;
					Size_dec = NULL;
				}
				if (Opacity_dec) {
					delete Opacity_dec;
					Opacity_dec = NULL;
				}
				if (Rotate_dec) {
					delete Rotate_dec;
					Rotate_dec = NULL;
				}
				if (Rotate_around_dec) {
					delete Rotate_around_dec;
					Rotate_around_dec = NULL;
				}
				if (Color_dec) {
					delete Color_dec;
					Color_dec = NULL;
				}
				if (Move_delay_dec)
				{
					delete Move_delay_dec;
					Move_delay_dec = NULL;
				}
				if (Process_dec)
				{
					delete Process_dec;
					Process_dec = NULL;
				}

				if (text_dec)
				{
					delete text_dec;
					text_dec = NULL;
				}
			}
#pragma endregion load_object_basic_xml

#pragma region wide_obj_dec_xml
        Wide_ObjectDec::Wide_ObjectDec()
        {
            name_object = "";
            type_object = -1;
            //
            name_source = "";
            animature_dec = nullptr;
            animation_dec = nullptr;
            text_dec = nullptr;
            //
            OriginValue = new BasicDec();
            CommonValue = new BasicDec();
            FirstValue = new BasicDec();
            //
            OriginValue->InsertDataVector2("pos_ratio", Vec2(0, 0));
            OriginValue->InsertDataVector2("scale_ratio", Vec2(1, 1));
            OriginValue->InsertDatafloat("rotate", 0.f);
            OriginValue->InsertDataInt("opacity", 255);
            OriginValue->InsertDataColor("tint", Color4B::WHITE);
            OriginValue->InsertDataInt("Process", 100);
            //
           // FirstValue->InsertDataVector2("pos_ratio", Vec2(0, 0));
          //  FirstValue->InsertDataVector2("scale_ratio", Vec2(1, 1));
          //  FirstValue->InsertDatafloat("rotate", 0.f);
          //  FirstValue->InsertDataInt("opacity", 255);
          //  FirstValue->InsertDataColor("tint", Color4B::WHITE);
          //  FirstValue->InsertDataInt("Process", 100);
            //
            design_size = Vec2(0, 0);
            //
            CommonValue->InsertDataInt("visible", 1);
            parent_object = nullptr;//not delete the parent;
            list_child_object.clear();
            p_list_command.clear();
        }
        
        Wide_ObjectDec::~Wide_ObjectDec()
        {
            //CC_SAFE_DELETE(OriginValue);
            //CC_SAFE_DELETE(CommonValue);
            //CC_SAFE_DELETE(FirstValue);
            OriginValue->clearAllData();
            OriginValue = nullptr;
            CommonValue->clearAllData();
            CommonValue = nullptr;
            FirstValue->clearAllData();
            FirstValue = nullptr;
            
            type_object = -1;
            name_source.Clear();
            CC_SAFE_DELETE(animature_dec);
            CC_SAFE_DELETE(animation_dec);
            CC_SAFE_DELETE(text_dec);
            
            parent_object = nullptr;
            p_list_command.clear();
            for (size_t i = 0; i < list_child_object.size(); i++)
            {
                delete list_child_object.at(i);
            }
            
            list_child_object.clear();
        }
        
        void Wide_ObjectDec::InsertWideObjectChild(Wide_ObjectDec * child_object)
        {
            list_child_object.push_back(child_object);
        }
        
        const char* Wide_ObjectDec::GetCommandByName(RKString name) const
        {
            if (p_list_command.size() > 0 && p_list_command.find(name) != p_list_command.end())
            {
                return p_list_command.at(name).GetString();
            }
            return nullptr;
        }
        
        void Wide_ObjectDec::InsertCommand(RKString name, RKString command)
        {
            p_list_command.insert(std::pair<RKString, RKString>(name, command));
        }
        
        unsigned int Wide_ObjectDec::GetCommandSize()
        {
            return p_list_command.size();
        }
        
        std::pair<RKString, RKString> Wide_ObjectDec::GetIterCommandAtIdx(int idx)
        {
            int i = 0;
            for (auto it = p_list_command.begin(); it != p_list_command.end(); it++)
            {
                if (i == idx)
                {
                    return std::pair<RKString, RKString>(it->first, it->second);
                }
                i++;
            }
            return std::pair<RKString, RKString>("", "");
        }
#pragma endregion wide_obj_dec_xml

#pragma region basic_dec_xml
		BasicDec::BasicDec()
		{
			/*__name_vector_.clear();
			__name_float_.clear();
			__name_int_.clear();
			__name_string_.clear();
			__name_vector3_.clear();

			__name_color_.clear();*/
		}
		BasicDec::~BasicDec()
		{
			clearAllData();
		}

		void BasicDec::operator=(const BasicDec & Ba)
		{
			__name_color_ = Ba.__name_color_;
			__name_vector_ = Ba.__name_vector_;
			__name_float_ = Ba.__name_float_;
			__name_int_ = Ba.__name_int_;
			__name_string_ = Ba.__name_string_;
			__name_vector3_ = Ba.__name_vector3_;
		}

		void BasicDec::clearAllData()
		{
			__name_vector_.clear();
			__name_float_.clear();
			__name_int_.clear();
			__name_string_.clear();
			__name_vector3_.clear();

			__name_color_.clear();
		}
		//insert
		void BasicDec::InsertDataInt(std::string name, const int data)
		{
			__name_int_.insert(name, data);
		}
		void BasicDec::InsertDatafloat(std::string name, const float data)
		{
			__name_float_.insert(name, data);
		}
		void BasicDec::InsertDataChar(std::string name, const std::string data)
		{
			__name_string_.insert(name, data);
		}
		void BasicDec::InsertDataVector2(std::string name, const Vec2 data)
		{
			__name_vector_.insert(name, data);
		}
		void BasicDec::InsertDataVector3(std::string name, const Vec3 data)
		{
			__name_vector3_.insert(name, data);
		}
		void BasicDec::InsertDataColor(std::string name, const Color4B data)
		{
			__name_color_.insert(name, data);
		}
		//check find data
		bool BasicDec::HaveDataInt(std::string name)
		{
			if (__name_int_.size() > 0 && __name_int_.contain(name))
				return true;
			return false;
		}
		bool BasicDec::HaveDatafloat(std::string name)
		{
			if (__name_float_.size() > 0 && __name_float_.contain(name))
				return true;
			return false;
		}
		bool BasicDec::HaveDataChar(std::string name)
		{
			if (__name_string_.size() > 0 && __name_string_.contain(name))
				return true;
			return false;
		}
		bool BasicDec::HaveDataVector2(std::string name)
		{
			if (__name_vector_.size() > 0 && __name_vector_.contain(name))
				return true;
			return false;
		}
		bool BasicDec::HaveDataVector3(std::string name)
		{
			if (__name_vector3_.size() > 0 && __name_vector3_.contain(name))
				return true;
			return false;
		}
		bool BasicDec::HaveDataColor(std::string name)
		{
			if (__name_color_.size() > 0 && __name_color_.contain(name))
				return true;
			return false;
		}
		//get data
		const int BasicDec::GetDataInt(std::string name)
		{
			if (__name_int_.size() > 0 && __name_int_.contain(name))
			{
				return __name_int_.at(name);
			}
            if ("use_atlas" == name) {
                return 1;
            }
			return 0;
		}
		const float BasicDec::GetDatafloat(std::string name)
		{
			if (__name_float_.size() > 0 && __name_float_.contain(name))
				return __name_float_.at(name);
			return 0;
		}
        const std::string BasicDec::GetDataChar(std::string name)
		{
			if (__name_string_.size() > 0 && __name_string_.contain(name))
				return __name_string_.at(name);
			return "";
		}
		const Vec2 BasicDec::GetDataVector2(std::string name)
		{
			if (__name_vector_.size() > 0 && __name_vector_.contain(name))
				return __name_vector_.at(name).ToVec2();
			return Vec2(0, 0);
		}
		const Vec3 BasicDec::GetDataVector3(std::string name)
		{
			if (__name_vector3_.size() > 0 && __name_vector3_.contain(name))
				return __name_vector3_.at(name).ToVec3();
			return Vec3(0, 0, 0);
		}
		const Color4B BasicDec::GetDataColor(std::string name)
		{
			if (__name_color_.size() > 0 && __name_color_.contain(name))
				return __name_color_.at(name).ToColor();
			return Color4B::WHITE;
		}
		//replace 
		void BasicDec::ReplaceDataInt(std::string name, const int data)
		{
			__name_int_.erase(name);
            InsertDataInt(name, data);
		}
		void BasicDec::ReplaceDatafloat(std::string name, const float data)
		{
			__name_float_.erase(name);
            InsertDatafloat(name, data);
		}
		void BasicDec::ReplaceDataChar(std::string name, const std::string data)
		{
			__name_string_.erase(name);
            InsertDataChar(name, data);
		}
		void BasicDec::ReplaceDataVector2(std::string name, const Vec2 data)
		{
			__name_vector_.erase(name);
            InsertDataVector2(name, data);
		}
		void BasicDec::ReplaceDataVector3(std::string name, const Vec3 data)
		{
			__name_vector3_.erase(name);
			InsertDataVector3(name, data);
		}
		void BasicDec::ReplaceDataColor(std::string name, const Color4B data)
		{
			__name_color_.erase(name);
            InsertDataColor(name, data);
		}

        BasicDec * BasicDec::Clone()
        {
            BasicDec * dec = new BasicDec();
            dec->__name_color_ = this->__name_color_;
			dec->__name_vector_ = this->__name_vector_;
			dec->__name_float_ = this->__name_float_;
			dec->__name_int_ = this->__name_int_;
			dec->__name_string_ = this->__name_string_;
			dec->__name_vector3_ = this->__name_vector3_;
			return dec;
		}

		std::map<std::string, std::string> BasicDec::GetListChar() {		
			return __name_string_.toMap();
		}

		std::map<std::string, float>	 BasicDec::GetListFloat(){
			return __name_float_.toMap();
		}

		std::map<std::string, int>		 BasicDec::GetListInt() {
			return __name_int_.toMap();
		}

#pragma endregion basic_dec_xml

#pragma region basic_w_ui_dec_xml
		sound_click_dec::sound_click_dec()
		{
			id_click = "";
			delayTime = 0;
			loop = 0;
		}
		sound_click_dec::sound_click_dec(sound_click_dec * value)
		{
			id_click = value->id_click;
			delayTime = value->delayTime;
			loop = value->loop;
		}
		sound_click_dec::~sound_click_dec()
		{
			id_click = "";
			delayTime = 0;
			loop = 0;
		}

		time_process_ui::time_process_ui()
		{
			time_process = 0;
			ID_SOUND_FADE_IN = "";
			ID_SOUND_FADE_OUT = "";
			delayTime = 0;
			loop = 0;
		}
		time_process_ui::~time_process_ui()
		{
			time_process = 0;
			ID_SOUND_FADE_IN = "";
			ID_SOUND_FADE_OUT = "";
			delayTime = 0;
			loop = 0;
		}

		UIWidgetDec::UIWidgetDec()
		{
			typeValue = WIDGET_NONE;

			OriginValue = new BasicDec();
			FirstValue = new BasicDec();
			GeneralValue = new BasicDec();
			//
			sound_click = nullptr;
			p_menu_widget = nullptr;

			TimeProcessFade = 1.0f;
			index_appear = 0;
			visible = 1;
			index = 0;
            NameWidget = "";    
			p_parent_layer = nullptr;//not delete the parent;
		}
		UIWidgetDec::~UIWidgetDec()
		{
			//CC_SAFE_DELETE(sound_click);
			CC_SAFE_DELETE(OriginValue);
			CC_SAFE_DELETE(FirstValue);
			CC_SAFE_DELETE(GeneralValue);

			actionFade.clear();
			p_list_command.clear();
			p_parent_layer = nullptr;
			p_menu_widget = nullptr;
			sound_click = nullptr;
		}
        void UIWidgetDec::InitValue()
        {
            OriginValue->InsertDataVector2("pos_ratio", Vec2(0, 0));
            OriginValue->InsertDataVector2("scale_ratio", Vec2(1, 1));
            OriginValue->InsertDatafloat("rotate", 0.f);
            OriginValue->InsertDataInt("opacity", 255);
            
            //FirstValue->InsertDataVector2("pos_ratio", Vec2(0, 0));
            //FirstValue->InsertDataVector2("scale_ratio", Vec2(1, 1));
            //FirstValue->InsertDatafloat("rotate", 0.f);
            //FirstValue->InsertDataInt("opacity", 255);
            
            //
            GeneralValue->InsertDataInt("not_fade_pos", 0);
            GeneralValue->InsertDataInt("not_fade_scale", 0);
            GeneralValue->InsertDataInt("not_fade_rotate", 0);
            GeneralValue->InsertDataInt("not_fade_opacity", 0);
            GeneralValue->InsertDataColor("tint", Color4B::WHITE);
        }
        
		void UIWidgetDec::SetActionFade(UI_STATE_FADE_ACTION af)
		{
			actionFade.push_back(af);
		}
        const char* UIWidgetDec::GetCommandByName(std::string name)
		{
			if (p_list_command.size() > 0 && p_list_command.find(name) != p_list_command.end())
			{
                return p_list_command.at(name).c_str();
			}
			return nullptr;
		}
		void UIWidgetDec::InsertCommand(std::string name, std::string command)
		{
            p_list_command.insert(std::pair<std::string, std::string>(name, command));
		}
		unsigned int UIWidgetDec::GetCommandSize() {
			return p_list_command.size();
		}
		std::pair<std::string, std::string> UIWidgetDec::GetIterCommandAtIdx(int idx)
		{
			int i = 0;
			for (auto it = p_list_command.begin(); it != p_list_command.end(); it++)
			{
				if (i == idx)
				{
					return std::pair<std::string, std::string>(it->first, it->second);
				}
				i++;
			}
			return std::pair<std::string, std::string>("", "");
		}

		UILayerWidgetDec::UILayerWidgetDec()
		{
			p_list_child_widget.clear();
		}
		UILayerWidgetDec::~UILayerWidgetDec()
		{
			for (unsigned int i = 0; i < p_list_child_widget.size(); i++)
			{
				CC_SAFE_DELETE(p_list_child_widget.at((unsigned int)i));
			}
			p_list_child_widget.clear();
		}
		void UILayerWidgetDec::InsertChildWidget(UILayerWidgetDec * widget)
		{
			p_list_child_widget.push_back(widget);
		}
		UILayerWidgetDec * UILayerWidgetDec::GetChildWidgetByName(std::string name)
		{
			for (unsigned int i = 0; i < p_list_child_widget.size(); i++)
			{
                if (p_list_child_widget.at(i)->NameWidget == name)
				{
					return p_list_child_widget.at(i);
				}
			}
			return 0;
		}
		bool UILayerWidgetDec::IsTopLayerMenu()
		{
			return (p_parent_layer == nullptr);
		}
		UILayerWidgetDec * UILayerWidgetDec::Clone()
		{
			UILayerWidgetDec * clone = new UILayerWidgetDec();
			memcpy(clone, this, sizeof(this));
			clone->FirstValue = this->FirstValue->Clone();
			clone->OriginValue = this->OriginValue->Clone();
			clone->GeneralValue = this->GeneralValue->Clone();
			clone->NameWidget = this->NameWidget;
			clone->index = this->index;
			clone->actionFade = this->actionFade;
			clone->index_appear = this->index_appear;
			clone->p_list_command = this->p_list_command;

			clone->TimeProcessFade = this->TimeProcessFade;
			clone->typeValue = this->typeValue;
			clone->visible = this->visible;
			for (size_t i = 0; i < p_list_child_widget.size(); i++)
			{
				clone->p_list_child_widget.push_back(this->p_list_child_widget.at(i)->Clone());
			}

			return clone;

		}

		void UILayerWidgetDec::RecorrectLayerParent(UILayerWidgetDec * parent_layer)
		{
			for (size_t i = 0; i < p_list_child_widget.size(); i++)
			{
				p_list_child_widget[i]->p_parent_layer = parent_layer;
				p_list_child_widget[i]->RecorrectLayerParent(p_list_child_widget[i]);
			}
		}
		
		void UILayerWidgetDec::RecorrectMenuParent(UIMenuWidgetDec * menu_widget)
		{
			for (size_t i = 0; i < p_list_child_widget.size(); i++)
			{
				p_list_child_widget[i]->p_menu_widget = menu_widget;
				p_list_child_widget[i]->RecorrectMenuParent(menu_widget);
			}
		}


		UIMenuWidgetDec::UIMenuWidgetDec()
		{
			NameMenuWidget = "";
			DesignSize = Vec2(0, 0);
			max_idx_first = 0;
		}
		UIMenuWidgetDec::UIMenuWidgetDec(std::string Name, Vec2 size)
		{
			NameMenuWidget = Name;
			DesignSize = size;
			max_idx_first = 0;
		}
		UIMenuWidgetDec::~UIMenuWidgetDec()
		{
			for (auto it = p_listElementChild.begin(); it != p_listElementChild.end(); it++)
			{
				if (it->second)
				{
					delete it->second;
					it->second = nullptr;
				}
			}
			p_listElementChild.clear();
			p_listChildElementByName.clear();
			time_process.clear();
		}
		UILayerWidgetDec * UIMenuWidgetDec::GetElementChildByName(std::string str) {
			//
			if (p_listElementChild.size() <= 0)
			{
				return 0;
			}
			for (auto it = p_listElementChild.begin(); it != p_listElementChild.end(); it++)
			{
				int idx = it->second->index;
                RKString te =it->first;
				auto strName = te.Split("#");
                if (idx == atoi(strName[0].GetString()) && str == std::string(strName[1].GetString()))
				{
					return it->second;
				}
			}
			return 0;
		}
		void UIMenuWidgetDec::InsertChildElement(std::string str, int idx, UILayerWidgetDec* e)
		{
			str = std::string(std::to_string(idx)) + "#" + str;
			p_listElementChild.insert(std::pair<std::string, UILayerWidgetDec*>(str, e));
		}
        std::vector<std::string> UIMenuWidgetDec::GetListChildName()
		{
			return p_listChildElementByName;
		}
		void UIMenuWidgetDec::InsertNameToList(std::string name)
		{
			p_listChildElementByName.push_back(name);
		}
		time_process_ui UIMenuWidgetDec::GetTimeProcess(int idx)
		{
			if (idx >= 0 && (unsigned int)idx < time_process.size())
				return time_process.at(idx);
			time_process_ui temp;
			return temp;
		}
        
		int UIMenuWidgetDec::GetNumberTimeProcess()
		{
			return (int)time_process.size();
		}

#pragma endregion basic_w_ui_dec_xml


		LayerData::LayerData()
		{
			p_DistanceEachObject = Vec2(0, 0);
			p_delta_distance_x = Vec2(0, 0);
			p_delta_distance_y = Vec2(0, 0);
			p_number_object = Vec2(1, 2); //range object init at 1 to 2
			p_TimeDelay = 0.f;
			p_processTime = 0.25f;

			p_TimeMoveRandom = Vec2(p_processTime, p_processTime);
			p_timeDelayRandom = Vec2(0, 0);

			p_idxAppearRandomOnEdgeBound = 0;
			p_IsObjectPosRandom = false;
			p_IsScaleRandom = false;
			p_IsRotateRandom = false;
			p_IsOpacityRandom = false;
			p_IsColorRandom = false;
			p_IsProcessRandom = false;

			p_isObjectRandom = true;

			p_randomScale = Vec2(100, 100); // scale 1
			p_randomRotate = Vec2(0, 0);/// angle 0
			p_randomOpacity = Vec2(255, 255); //opacity 255
			p_randomColor_r = p_randomColor_g = p_randomColor_b = Vec2(255, 255); //white color

			p_timeProcessRandom = Vec2(0, 0);
			p_percentProcessRandom = Vec2(100, 100);

			p_BoundRandomPos = cocos2d::Rect(0, 0, GetGameSize().width, GetGameSize().height);

			p_direct_move_random_x = Vec2(0, 0);
			p_direct_move_random_y = Vec2(0, 0);

			p_loopAction = -1;
		}
		LayerData::~LayerData()
		{
			p_listObject.clear();
			p_list_layer_element_dec.clear();
		}
		ObjectDec * LayerData::GetObjectBGDecByName(RKString name)
		{
			if (p_listObject.size() > 0 && p_listObject.find(name) != p_listObject.end())
			{
				return p_listObject.at(name);
			}
			return NULL;
		}

		ObjectDec * LayerData::GetObjectBGDecByIdx(int idx)
		{
			if ((int)p_listObject.size() < 0 || (unsigned int)idx >= p_listObject.size())
				return NULL;
			for (auto it = p_listObject.begin(); it != p_listObject.end(); ++it)
			{
				if (idx <= 0)
					return it->second;
				else
					idx--;
			}
			return nullptr;
		}

		void LayerData::InsertObjectBGDec(RKString name, ObjectDec * val)
		{
			p_listObject.insert(std::pair<RKString, ObjectDec*>(name, val));
		}

		int LayerData::GetSizeObjDec() {
			return p_listObject.size();
		}

		float LayerData::GetRandomObject(Vec2 range, int div)
		{
			float val = 0;
			int random = GetRandom((int)range.x, (int)range.y);
			if (range.x == range.y || range.x == 0)
			{
				if (div != 0)
				{
					return (float)random / div;
				}
				else
				{
					return (float)random;
				}
			}
			if (div != 0)
			{
				val = (float)random / div;
			}
			else
			{
				val = (float)random;
			}
			return val;
		}

		int LayerData::GetNumberObject()
		{
			int val = (int)GetRandomObject(p_number_object);
			return val;
		}

		int LayerData::GetDistanceObject()
		{
			int val = (int)GetRandomObject(p_DistanceEachObject);
			return val;
		}

		float LayerData::direct_x_Obj()
		{
			float val = GetRandomObject(p_delta_distance_x, MAX((int)p_delta_distance_x.x, (int)p_delta_distance_x.y));
			return val;
		}

		float LayerData::direct_y_Obj()
		{
			float val = GetRandomObject(p_delta_distance_y, MAX((int)p_delta_distance_y.x, (int)p_delta_distance_y.y));
			return val;
		}

		float LayerData::GetScaleRandom()
		{
			float val = GetRandomObject(p_randomScale, 100);
			return val;
		}

		float LayerData::GetRotateRandom()
		{
			float val = GetRandomObject(p_randomScale);
			return val;
		}

		RKUtils::BYTE LayerData::GetOpacityRandom()
		{
			RKUtils::BYTE val = (RKUtils::BYTE)GetRandomObject(p_randomOpacity);
			return val;
		}

		Color4B LayerData::GetColorRandom()
		{
			RKUtils::BYTE val_r = (RKUtils::BYTE)GetRandomObject(p_randomColor_r);
			RKUtils::BYTE val_g = (RKUtils::BYTE)GetRandomObject(p_randomColor_g);
			RKUtils::BYTE val_b = (RKUtils::BYTE)GetRandomObject(p_randomColor_b);
			Color4B val = Color4B(val_r, val_g, val_b, 255);
			return val;
		}

		cocos2d::Rect LayerData::GetBoundRandomPos()
		{
			return
				cocos2d::Rect(
					Vec2(p_BoundRandomPos.origin.x * GetGameSize().width, p_BoundRandomPos.origin.y * GetGameSize().height),
					cocos2d::Size(p_BoundRandomPos.size.width * GetGameSize().width, p_BoundRandomPos.size.height * GetGameSize().height));
		}

		float LayerData::GetTimeMoveRandom()
		{
			float val = GetRandomObject(p_TimeMoveRandom * 10);
			return val / 10.f;
		}

		float LayerData::GetDelayTimeRandom()
		{
			float val = GetRandomObject(p_timeDelayRandom * 10);
			return val / 10.f;
		}

		Vec2 LayerData::GetVectorMoveRandom()
		{
			Vec2 val;
			val.x = GetRandomObject(p_direct_move_random_x) * GetGameSize().width;
			val.y = GetRandomObject(p_direct_move_random_y) * GetGameSize().height;

			return val;
		}

		float LayerData::GetPercentRandom()
		{
			float val = GetRandomObject(p_percentProcessRandom);
			return val;
		}

		float LayerData::GetTimeProcessRandom()
		{
			float val = GetRandomObject(p_timeProcessRandom * 10);
			return val / 10.f;
		}

		Background::Background()
		{
			p_Name = "";
		}
		Background::~Background()
		{
			p_ListObject.clear();
		}

		LayerData * Background::GetLayerData(RKString str)
		{
			if (p_ListObject.size() > 0 && p_ListObject.find(str) != p_ListObject.end())
				return p_ListObject.at(str);
			return 0;
		}

		LayerData * Background::GetLayerDataByIdx(int idx)
		{
			if (p_ListObject.size() > 0)
			{
				int i = 0;
				for (auto it = p_ListObject.begin(); it != p_ListObject.end(); it++)
				{
					if (i == idx)
					{
						return it->second;
					}
					i++;
				}
			}
			return NULL;
		}

		void Background::InsertLayerData(RKString str, LayerData * c)
		{
			p_ListObject.insert(std::pair<RKString, LayerData*>(str, c));
		}

		RKString Background::GetNameBGLayer() { return p_Name; }

		int Background::GetNumberLayer() { return  p_ListObject.size(); }

		/*
		use for load sound dec
		*/

		SoundDec::SoundDec()
		{
			name_id = "";
			path = "";
			delay_first = 0;
			loop = 1;
			type_sound = "sound";
		}
		SoundDec::~SoundDec()
		{
			name_id = "";
			path = "";
			delay_first = 0;
			loop = 1;
			type_sound = "sound";
		}


		LangDec::LangDec()
		{
			 name_id = "";
			 path = "";
			 dec = "";
		}

		LangDec::~LangDec()
		{
			name_id = "";
			path = "";
			dec = "";
		}

		LoadDataXML *		 LoadDataXML::p_Instance = NULL;
		LoadDataXML::LoadDataXML()
		{
			p_HProfile = HARDWARD_PROFILE::HARDWARE_HIGH;

			p_texture_packer_xml = nullptr;
			p_texture_atlas_xml = nullptr;

			p_listLangDec.clear();

			p_xml_loader_config.clear();
			p_table_screen_xml_load.clear();
		}

		LoadDataXML::~LoadDataXML()
		{
			p_list_font.clear();

			p_listWideObject.clear();

			if (p_texture_packer_xml)
			{
				delete p_texture_packer_xml;
				p_texture_packer_xml = nullptr;
			}

			if (p_texture_atlas_xml)
			{
				delete p_texture_atlas_xml;
				p_texture_atlas_xml = nullptr;
			}

			p_listLangDec.clear();
			p_xml_loader_config.clear();
			p_table_screen_xml_load.clear();
		}


		void LoadDataXML::OnLoaderXML()
		{
			std::string TargetXML = std::string(LOADER_XML);

			OnLoadXML<LoadDataXML>(std::mem_fn(&LoadDataXML::OnLoadXMLLoader), this, TargetXML);
		}

		HARDWARD_PROFILE LoadDataXML::GetHarwardProfile()
		{
			return p_HProfile;
		}

		void LoadDataXML::OnLoadXMLLoader(TiXmlDocument * objectXMl)
		{
			auto Root = objectXMl->FirstChildElement("GAME");
			if (!Root)
			{
				PASSERT2(false, "XML fail to load!");
				return;
			}

			auto root_loader_xml = Root->FirstChildElement("list_loader");
			if (!root_loader_xml)
			{
				PASSERT2(false, "XML loader fail to load!");
				return;
			}

			auto loader_xml = root_loader_xml->FirstChildElement("loader");
			do
			{
				if (loader_xml)
				{
					RKString name_loader = loader_xml->Attribute("name");

					auto path_loader = loader_xml->FirstChildElement("path");
					RKString path = path_loader->Attribute("value");

					p_xml_loader_config.insert(std::pair<RKString, RKString>(name_loader, path));

					loader_xml = loader_xml->NextSiblingElement();
				}
			} while (loader_xml);
		}

		void LoadDataXML::OnLoadXML1(std::function<void(TiXmlDocument*)> func, RKString path_xml)
		{
			bool IsLoadXMLSucceed = false;
			bool p_IsUseArkXML = false;
#ifdef USE_ARK_ARCHIVE
			p_IsUseArkXML = true;
#endif

			TiXmlDocument * p_ObjectXML = NULL;
			p_ObjectXML = new TiXmlDocument();

			std::string TargetXML = path_xml.GetString();


#ifdef USE_ARK_ARCHIVE
			if (p_IsUseArkXML)
			{
				tinyxml2::XMLError error = p_ObjectXML->LoadFileArk(TargetXML);
				if (error != tinyxml2::XMLError::XML_NO_ERROR)
				{
					p_IsUseArkXML = false;
					IsLoadXMLSucceed = false;
				}
				else
				{
					IsLoadXMLSucceed = true;
				}

			}
#endif
			if (!p_IsUseArkXML)
			{
				FILE * f = File::GetFile((TargetXML).c_str(), "rb");
				if (f)
				{
					p_ObjectXML->LoadFile(f);
					IsLoadXMLSucceed = true;
					fclose(f);
				}
				else
				{
					IsLoadXMLSucceed = false;
				}
			}

			if (IsLoadXMLSucceed)
			{
					func(p_ObjectXML);
			}
			else
			{
				PASSERT2(IsLoadXMLSucceed == true, "couldnot load xml loader correctly !");
			}
			delete p_ObjectXML;
			p_ObjectXML = NULL;
		}
		
		void LoadDataXML::OnLoadXMLData1(RKString name_loader, std::function<void(TiXmlDocument*)> func)
		{
			std::string TargetXML = "";

			if (p_xml_loader_config.size() > 0 && p_xml_loader_config.find(name_loader) != p_xml_loader_config.end())
			{
				TargetXML = p_xml_loader_config.at(name_loader).GetString();
			}
			else
			{
				PASSERT2(TargetXML == "", "nameloader not found !");
			}
			OnLoadXML1(func, TargetXML);
		}
		
		/*
		-----------------------------------------------------
		*/

		void LoadDataXML::InsertTextureXML(RKString str, AtlasTexture * te)
		{
			p_ListTextureXML.insert(std::pair<RKString, AtlasTexture*>(str, te));
		}
        
		void LoadDataXML::OnLoadTextureAtlasDecXML(TiXmlDocument * objectXMl)
		{
			auto Root = objectXMl->FirstChildElement("GAME");
			if (!Root)
			{
				PASSERT2(Root == nullptr, "root must not be null! ");
				return;
			}

			Vec2 screen_size = GetGameSize();

			auto rootTexture = Root->FirstChildElement("texture");
			if (rootTexture)
			{
				auto textPack = rootTexture->FirstChildElement("texture_atlas");
				do
				{
					if (textPack)
					{
						auto size_fit_design = textPack->Attribute("fit_size");
						auto range_size_x = textPack->Attribute("range_size_x");

						Vec2 _rg_size_x = ConvertToVector2(range_size_x);

						bool IsFitForThisDesign = false;

						//check the screen in the range of the design size
						if (screen_size.x >= _rg_size_x.x && (screen_size.x <= _rg_size_x.y || _rg_size_x.y <= -1)
							)
						{
							IsFitForThisDesign = true;
						}

						if (IsFitForThisDesign)
						{
							p_texture_atlas_xml = new TextAtlas();
							p_texture_packer_xml = new TextPacker();
							p_texture_frame_xml = new ListTexFrame();

							auto tex = textPack->FirstChildElement("tex");
							do
							{
								if (tex)
								{
									RKString name_tex = tex->Attribute("name");

									std::vector<TextPackerDetail> list_text_path;
									zone_textAtlas_detail		  zone_texatlas;
									TexFrame					  tex_frame;

									if (tex->Attribute("subfix"))
									{
										zone_texatlas.subfix = tex->Attribute("subfix");
									}

									if (tex->Attribute("endfix"))
									{
										zone_texatlas.endfix = tex->Attribute("endfix");
									}

									auto tex_path = tex->FirstChildElement("path");
									do
									{
										if (tex_path)
										{
											if (tex_path->Attribute("texture") != nullptr)
											{
												RKString path_tex = tex_path->Attribute("texture");
												RKString plist_tex = tex_path->Attribute("plist");
												list_text_path.push_back(TextPackerDetail(path_tex, plist_tex));
											}
											else if (tex_path->Attribute("atlas_path") != nullptr)
											{
												RKString name_fix = tex_path->Attribute("name");
												RKString path_atlas = tex_path->Attribute("atlas_path");
												RKString global_path = tex_path->Attribute("global_path");
												std::vector<RKString> list_path_texture;
												auto tex_path_detail = tex_path->FirstChildElement("texture");
												do
												{
													if (tex_path_detail)
													{
														RKString _path_texture = tex_path_detail->Attribute("path");
														list_path_texture.push_back(_path_texture);
														tex_path_detail = tex_path_detail->NextSiblingElement();
													}
												} while (tex_path_detail);
												zone_texatlas.list_texture.push_back(TextAtlasDetail(name_fix, global_path,path_atlas, list_path_texture));

											}
											else if (tex_path->Attribute("tex_path") != nullptr)
											{
												RKString name = tex_path->Attribute("name");
												RKString path_tex = tex_path->Attribute("tex_path");

												
												auto tex_frame_detail_xml = tex_path->FirstChildElement("texture");
												do
												{
													if (tex_frame_detail_xml)
													{
														RKString name_t = tex_frame_detail_xml->Attribute("name");
														BasicDec * value = new BasicDec();

														auto common_value_xml = tex_frame_detail_xml->FirstChildElement("common");
														do
														{
															if (common_value_xml)
															{
																RKString name_common_value = common_value_xml->Attribute("name");
																RKString common_value = common_value_xml->Attribute("value");
																bool res = OnParseValueIntoXMLDec(name_common_value, common_value, value, nullptr);

																common_value_xml = common_value_xml->NextSiblingElement();
															}
														} while (common_value_xml);
																				
														tex_frame.InsertTexFrameDetail(name_t, TexFrameDetail(path_tex, *value));
														
														//delete value;
														tex_frame_detail_xml = tex_frame_detail_xml->NextSiblingElement();
													}
												} while (tex_frame_detail_xml);
											}
											tex_path = tex_path->NextSiblingElement();
										}
									} while (tex_path);


									p_texture_packer_xml->InsertTexPack(name_tex, list_text_path);
									p_texture_packer_xml->SizeFit = ConvertToVector2(size_fit_design);

									p_texture_atlas_xml->InsertTexAtlas(name_tex, zone_texatlas);
									p_texture_atlas_xml->SizeFit = ConvertToVector2(size_fit_design);

									p_texture_frame_xml->InsertTexFrame(name_tex, tex_frame);
									p_texture_frame_xml->SizeFit = ConvertToVector2(size_fit_design);

									tex = tex->NextSiblingElement("tex");
								}

							} while (tex);

							auto root_font = textPack->FirstChildElement("font");
							if (root_font)
							{

								auto font_detail = root_font->FirstChildElement("fontdetail");
								do
								{
									if (font_detail)
									{
										FontDec * font_dec = new FontDec();
										font_dec->idx = atoi(font_detail->Attribute("id"));
										font_dec->path = font_detail->Attribute("namepath");
										RKString name_font = font_detail->Attribute("name");
										//
										font_dec->name = name_font;
										//
										auto default_value = font_detail->FirstChildElement("default");
										if (default_value)
										{
											font_dec->d_size = atof(default_value->Attribute("size"));
											if (default_value->Attribute("borderwdx"))
											{
												font_dec->d_width_border = atof(default_value->Attribute("borderwdx"));
											}

											RKString color = default_value->Attribute("color");

											auto val_slipt = color.Split("_");
											font_dec->d_color[0] = atoi(val_slipt[0].GetString());
											font_dec->d_color[1] = atoi(val_slipt[1].GetString());
											font_dec->d_color[2] = atoi(val_slipt[2].GetString());
											font_dec->d_color[3] = atoi(val_slipt[3].GetString());

											if (default_value->Attribute("colorborder"))
											{
												color = default_value->Attribute("colorborder");

												val_slipt = color.Split("_");
												font_dec->d_color_border[0] = atoi(val_slipt[0].GetString());
												font_dec->d_color_border[1] = atoi(val_slipt[1].GetString());
												font_dec->d_color_border[2] = atoi(val_slipt[2].GetString());
												font_dec->d_color_border[3] = atoi(val_slipt[3].GetString());
											}
											else
											{
												font_dec->d_color_border[0] = 255;
												font_dec->d_color_border[1] = 255;
												font_dec->d_color_border[2] = 255;
												font_dec->d_color_border[3] = 255;
											}
											if (default_value->Attribute("unicode"))
											{
												font_dec->IsUnicode = (atoi(default_value->Attribute("unicode")) != 0);
											}
											if (default_value->Attribute("bitmap"))
											{
												font_dec->IsBitmap = (atoi(default_value->Attribute("bitmap")) != 0);
											}
										}

										InsertFontToList(name_font, font_dec);

										font_detail = font_detail->NextSiblingElement();
									}

								} while (font_detail);

							}


							break;
						}

						textPack = textPack->NextSiblingElement();
					}
				} while (textPack);

			}
			else
			{
				PASSERT2(rootTexture == nullptr, "rootTexture must not be null! ");
			}

		}

        void LoadDataXML::OnLoadTexurePackerDecXML(TiXmlDocument * objectXMl)
        {
            auto Root = objectXMl->FirstChildElement("GAME");
            if (!Root)
            {
                PASSERT2(Root == nullptr, "root must not be null! ");
                return;
            }
            
            Vec2 screen_size = GetGameSize();
            
            auto rootTexture = Root->FirstChildElement("texture");
            if (rootTexture)
            {
                auto textPack = rootTexture->FirstChildElement("texture_pack");
                do
                {
                    if (textPack)
                    {
                        auto size_fit_design = textPack->Attribute("fit_size");
                        auto range_size_x = textPack->Attribute("range_size_x");
                        
                        Vec2 _rg_size_x = ConvertToVector2(range_size_x);
                        
                        bool IsFitForThisDesign = false;
                        
                        //check the screen in the range of the design size
                        if(screen_size.x >=_rg_size_x.x && (screen_size.x <= _rg_size_x.y || _rg_size_x.y <= -1)
                            )
                        {
                            IsFitForThisDesign = true;
                        }
                        
                        if(IsFitForThisDesign)
                        {
                            
                            p_texture_packer_xml = new TextPacker();
                            
                            auto tex = textPack->FirstChildElement("tex");
                            do
                            {
                                if(tex)
                                {
									RKString name_tex = tex->Attribute("name");
									std::vector<TextPackerDetail> list_text_path;
									auto tex_path = tex->FirstChildElement("path");
									do
									{
										if (tex_path)
										{
											RKString path_tex = tex_path->Attribute("texture");
											RKString plist_tex = tex_path->Attribute("plist");
											list_text_path.push_back(TextPackerDetail(path_tex, plist_tex));
											tex_path = tex_path->NextSiblingElement();
										}
									} while (tex_path);
                                    
                                    p_texture_packer_xml->InsertTexPack(name_tex, list_text_path);
                                    p_texture_packer_xml->SizeFit = ConvertToVector2(size_fit_design);
                            
                                    tex = tex->NextSiblingElement("tex");
                                }
                                
                            }while(tex);
                            
                            auto root_font = textPack->FirstChildElement("font");
                            if (root_font)
                            {
                                
                                auto font_detail = root_font->FirstChildElement("fontdetail");
                                do
                                {
                                    if (font_detail)
                                    {
                                        FontDec * font_dec = new FontDec();
                                        font_dec->idx = atoi(font_detail->Attribute("id"));
                                        font_dec->path = font_detail->Attribute("namepath");
                                        RKString name_font = font_detail->Attribute("name");
                                        //
                                        font_dec->name = name_font;
                                        //
                                        auto default_value = font_detail->FirstChildElement("default");
                                        if (default_value)
                                        {
                                            font_dec->d_size = atof(default_value->Attribute("size"));
											if (default_value->Attribute("borderwdx"))
											{
												font_dec->d_width_border = atof(default_value->Attribute("borderwdx"));
											}

                                            RKString color = default_value->Attribute("color");
                                            
                                            auto val_slipt = color.Split("_");
                                            font_dec->d_color[0] = atoi(val_slipt[0].GetString());
                                            font_dec->d_color[1] = atoi(val_slipt[1].GetString());
                                            font_dec->d_color[2] = atoi(val_slipt[2].GetString());
                                            font_dec->d_color[3] = atoi(val_slipt[3].GetString());
                                            
											if (default_value->Attribute("colorborder"))
											{
												color = default_value->Attribute("colorborder");

												val_slipt = color.Split("_");
												font_dec->d_color_border[0] = atoi(val_slipt[0].GetString());
												font_dec->d_color_border[1] = atoi(val_slipt[1].GetString());
												font_dec->d_color_border[2] = atoi(val_slipt[2].GetString());
												font_dec->d_color_border[3] = atoi(val_slipt[3].GetString());
											}
											else
											{
												font_dec->d_color_border[0] = 255;
												font_dec->d_color_border[1] = 255;
												font_dec->d_color_border[2] = 255;
												font_dec->d_color_border[3] = 255;
											}
											if (default_value->Attribute("unicode"))
											{
												font_dec->IsUnicode = (atoi(default_value->Attribute("unicode")) != 0);
											}
											if (default_value->Attribute("bitmap"))
											{
												font_dec->IsBitmap = (atoi(default_value->Attribute("bitmap")) != 0);
											}
                                        }
                                        
                                        InsertFontToList(name_font, font_dec);

										font_detail = font_detail->NextSiblingElement();
                                    }
                                   
                                } while (font_detail);
                                
                            }

                            
                            break;
                        }

                        textPack = textPack->NextSiblingElement();
                    }
                }while(textPack);
                
            }
            else
            {
                PASSERT2(rootTexture == nullptr, "rootTexture must not be null! ");
            }
        }
        
		void LoadDataXML::OnLoadTextureDecXML(TiXmlDocument * p_ObjectXML)
		{
			auto Root = p_ObjectXML->FirstChildElement("GAME");
			if (!Root)
				return;

			auto rootTexture = Root->FirstChildElement("texture");
			if (rootTexture)
			{
				auto texDetail = rootTexture->FirstChildElement("textureDetail");
				do
				{
					if (texDetail)
					{

						RKString extension = "";
#if defined OS_IOS
						extension = texDetail->Attribute("ios");
#elif defined OS_ANDROID_
						extension = texDetail->Attribute("android");
#else
						extension = texDetail->Attribute("win32");
#endif
						auto path = texDetail->Attribute("namepath") + extension;

						RKString nameTexture = texDetail->Attribute("name");

						AtlasTexture * new_texture = new AtlasTexture();
						new_texture->NameImage = nameTexture;
						new_texture->NamePath = path;

						auto t_sprite = texDetail->FirstChildElement("sprite");
						do
						{
							if (t_sprite)
							{
								AtlasChild  child;
								//
								child.nameSprite = t_sprite->Attribute("name");
								RKString rect = t_sprite->Attribute("rect");
								auto rect_detail = rect.Split("-");
								child.Position = Vec2(atof(rect_detail.GetAt(0).GetString()), atof(rect_detail.GetAt(1).GetString()));
								child.Size = Vec2(atof(rect_detail.GetAt(2).GetString()), atof(rect_detail.GetAt(3).GetString()));
								//
								new_texture->InsertAtlasChild(child.nameSprite, child);
								//
								t_sprite = t_sprite->NextSiblingElement();
							}

						} while (t_sprite);

						InsertTextureXML(nameTexture, new_texture);

						texDetail = texDetail->NextSiblingElement();
					}
				} while (texDetail);

			}

			rootTexture = Root->FirstChildElement("font");
			if (rootTexture)
			{

				auto font_detail = rootTexture->FirstChildElement("fontdetail");
				do
				{
					if (font_detail)
					{
						FontDec * font_dec = new FontDec();
						font_dec->idx = atoi(font_detail->Attribute("id"));
						font_dec->path = font_detail->Attribute("namepath");
						RKString name_font = font_detail->Attribute("name");
						//
						font_dec->name = name_font;
						//
						auto default_value = font_detail->FirstChildElement("default");
						if (default_value)
						{
							font_dec->d_size = atof(default_value->Attribute("size"));
							font_dec->d_width_border = atof(default_value->Attribute("borderwdx"));
							RKString color = default_value->Attribute("color");

							auto val_slipt = color.Split("_");
							font_dec->d_color[0] = atoi(val_slipt[0].GetString());
							font_dec->d_color[1] = atoi(val_slipt[1].GetString());
							font_dec->d_color[2] = atoi(val_slipt[2].GetString());
							font_dec->d_color[3] = atoi(val_slipt[3].GetString());

							color = default_value->Attribute("colorborder");

							val_slipt = color.Split("_");
							font_dec->d_color_border[0] = atoi(val_slipt[0].GetString());
							font_dec->d_color_border[1] = atoi(val_slipt[1].GetString());
							font_dec->d_color_border[2] = atoi(val_slipt[2].GetString());
							font_dec->d_color_border[3] = atoi(val_slipt[3].GetString());

							font_dec->IsUnicode = (atoi(default_value->Attribute("unicode")) != 0);
						}

						InsertFontToList(name_font, font_dec);
					}

					font_detail = font_detail->NextSiblingElement();
				} while (font_detail);

			}
			Root = NULL;

		}

		TextPacker * LoadDataXML::GetTexturePackerXML()
		{
			return p_texture_packer_xml;
		}

		AtlasTexture * LoadDataXML::GetAtlastTextureByName(RKString name)
		{
			if (p_ListTextureXML.size() > 0)
			{
				if (p_ListTextureXML.find(name) != p_ListTextureXML.end())
				{
					return p_ListTextureXML.at(name);
				}
			}
			return NULL;
		}

		AtlasTexture * LoadDataXML::GetAtlasTextureByIdx(int idx)
		{
			if (p_ListTextureXML.size() > 0 && idx < (int)p_ListTextureXML.size())
			{
				auto it = p_ListTextureXML.begin();
				while (idx > 0)
				{
					it++;
					idx--;
				}
				return it->second;
			}
			return NULL;
		}

		TextAtlas * LoadDataXML::GetTextureAtlasXML()
		{
			return p_texture_atlas_xml;
		}

		ListTexFrame  * LoadDataXML::GetTextureFrameXML()
		{
			return p_texture_frame_xml;
		}
		/*
		-----------------------------------------------------
		*/

		void  LoadDataXML::InsertFontToList(RKString name, FontDec* ddc)
		{
			p_list_font.insert(std::pair<RKString, FontDec*>(name, ddc));
		}

		FontDec*  LoadDataXML::GetFontDecByName(RKString name)
		{
			if (p_list_font.size() > 0 && p_list_font.find(name) != p_list_font.end())
			{
				return p_list_font.at(name);
			}
			return NULL;
		}

		FontDec* LoadDataXML::GetFontDecByIdx(int idx)
		{
			if (p_list_font.size() > 0)
			{
				for (auto it = p_list_font.begin(); it != p_list_font.end(); it++)
				{
					if (it->second->idx == idx)
					{
						return it->second;
					}
				}
			}
			return NULL;
		}

		/*
		-----------------------------------------------------
		*/

		Background * LoadDataXML::GetBackground(RKString str)
		{
			if (p_ListBackground.size() > 0 && p_ListBackground.find(str) != p_ListBackground.end())
				return p_ListBackground.at(str);
			return 0;
		}

		void LoadDataXML::InsertBackground(RKString str, Background * c)
		{
			p_ListBackground.insert(std::pair<RKString, Background*>(str, c));
		}

		void LoadDataXML::OnLoadBackgroundDecXML(TiXmlDocument * objectXMl)
		{
			auto Root = objectXMl->FirstChildElement("GAME");
			if (!Root)
				return;

			auto BackgroundRoot = Root->FirstChildElement("BackGround");

			if (BackgroundRoot)
			{
				Vec2 designSize = ConvertToVector2(BackgroundRoot->Attribute("designSize"));

				auto BGStyle = BackgroundRoot->FirstChildElement("BGStyle");
				do
				{
					if (BGStyle)
					{

						RKString name = BGStyle->Attribute("name");
						Background * _background = new Background();
						
						auto _Layer = BGStyle->FirstChildElement("layer");
						do
						{
							if (_Layer)
							{
								LayerData * _layerData = new LayerData();
								RKString _name_layer = _Layer->Attribute("name");
								RKString _id_layer = _Layer->Attribute("id");
								_layerData->p_index_layer = atoi(_id_layer.GetString());
								_layerData->p_NameLayer = _name_layer;
								auto __Loop = _Layer->Attribute("loop");
								if (__Loop)
								{
									_layerData->p_loopAction = atoi(__Loop);
								}

								//get the config 

								auto _config_layer = _Layer->FirstChildElement("config");
								if (_config_layer)
								{
									auto _data_config = _config_layer->Attribute("delay_time");
									if (_data_config)
									{
										_layerData->p_TimeDelay = atoi(_data_config);
									}

									_data_config = _config_layer->Attribute("process_time");
									if (_data_config)
									{
										_layerData->p_processTime = atof(_data_config);
									}

									_data_config = _config_layer->Attribute("distance");
									if (_data_config)
									{
										_layerData->p_DistanceEachObject = ConvertToVector2(_data_config);
									}

									_data_config = _config_layer->Attribute("delta_distance");
									if (/*_layerData->p_DistanceEachObject != Vec2(0, 0) && */_data_config)
									{
										auto _split_x_y = RKString(_data_config).Split("_");
										_layerData->p_delta_distance_x = Vec2(atoi(_split_x_y[0].GetString()),
																			  atoi(_split_x_y[1].GetString()));
										_layerData->p_delta_distance_y = Vec2(atoi(_split_x_y[2].GetString()),
																			  atoi(_split_x_y[3].GetString()));
									}
									//get number object
									auto _child_config_layer = _config_layer->FirstChildElement("number");
									if (_child_config_layer)
									{
										_layerData->p_number_object = ConvertToVector2(_child_config_layer->Attribute("value"));
									}
									//is the random
									_child_config_layer = _config_layer->FirstChildElement("Random");
									if (_child_config_layer)
									{

										auto random_child__ = _child_config_layer->FirstChildElement("Pos");
										if (random_child__)
										{
											_layerData->p_IsObjectPosRandom = (atoi(random_child__->Attribute("random")) != 0);
											//pos
											auto __str_data = random_child__->Attribute("value");
											if (_layerData->p_IsObjectPosRandom && __str_data)
											{
												auto list__ = RKString(__str_data).Split("_");
												_layerData->p_BoundRandomPos.setRect(
													atof(list__[0].GetString()) / designSize.x,
													atof(list__[1].GetString()) / designSize.y,
													atof(list__[2].GetString())	/ designSize.x,
													atof(list__[3].GetString())	/ designSize.y
													);

											}
											__str_data = random_child__->Attribute("time");
											if (_layerData->p_IsObjectPosRandom && __str_data)
											{
												_layerData->p_TimeMoveRandom = ConvertToVector2(__str_data);
											}

											__str_data = random_child__->Attribute("delay");
											if (_layerData->p_IsObjectPosRandom && __str_data)
											{
												_layerData->p_timeDelayRandom = ConvertToVector2(__str_data);
											}

											__str_data = random_child__->Attribute("value_move_random");
											if (_layerData->p_IsObjectPosRandom && __str_data)
											{
												auto list__ = RKString(__str_data).Split("_");
												
												_layerData->p_direct_move_random_x.x = atof(list__[0].GetString())  / designSize.x;
												_layerData->p_direct_move_random_x.y = atof(list__[1].GetString())  / designSize.y;
												_layerData->p_direct_move_random_y.x = atof(list__[2].GetString())  / designSize.x;
												_layerData->p_direct_move_random_y.y = atof(list__[3].GetString())  / designSize.y;
												
											}

											__str_data = random_child__->Attribute("edge_bound");
											if (_layerData->p_IsObjectPosRandom && __str_data)
											{
												_layerData->p_idxAppearRandomOnEdgeBound = (atoi(__str_data));
											}
										}
										//scale
										random_child__ = _child_config_layer->FirstChildElement("Size");
										if (random_child__)
										{
											_layerData->p_IsScaleRandom = (atoi(random_child__->Attribute("random")) != 0);
											//random scale
											auto __str_data = random_child__->Attribute("value");
											if (__str_data && _layerData->p_IsScaleRandom)
											{
												_layerData->p_randomScale = ConvertToVector2(__str_data);
											}
										}
										//rotate
										random_child__ = _child_config_layer->FirstChildElement("Rotate");
										if (random_child__)
										{
											_layerData->p_IsRotateRandom = (atoi(random_child__->Attribute("random"))!=0);
											//random rotate
											auto __str_data = random_child__->Attribute("value");
											if (__str_data && _layerData->p_IsRotateRandom)
											{
												_layerData->p_randomRotate = ConvertToVector2(__str_data);
											}
										}

										//Opcity

										random_child__ = _child_config_layer->FirstChildElement("Opacity");
										if (random_child__)
										{
											_layerData->p_IsOpacityRandom = (atoi(random_child__->Attribute("random")) != 0);

											//random Opacity
											auto 	__str_data = random_child__->Attribute("value");
											if (__str_data && _layerData->p_IsOpacityRandom)
											{
												_layerData->p_randomOpacity = ConvertToVector2(__str_data);
											}
										}

										//color
										random_child__ = _child_config_layer->FirstChildElement("Color");
										if (random_child__)
										{
											_layerData->p_IsColorRandom = (atoi(random_child__->Attribute("random")) != 0);
											auto __str_data = random_child__->Attribute("value");
											if (__str_data && _layerData->p_IsColorRandom)
											{
												auto _color_random = RKString(__str_data).Split("_");

												_layerData->p_randomColor_r = ConvertToVector2(_color_random[0] + "_" + _color_random[1]);
												_layerData->p_randomColor_g = ConvertToVector2(_color_random[2] + "_" + _color_random[3]);
												_layerData->p_randomColor_b = ConvertToVector2(_color_random[4] + "_" + _color_random[5]);
											}
										}

										//process
										random_child__ = _child_config_layer->FirstChildElement("Process");
										if (random_child__)
										{
											_layerData->p_IsProcessRandom = (atoi(random_child__->Attribute("random")) != 0);
											auto __str_data = random_child__->Attribute("value");
											if (__str_data && _layerData->p_IsProcessRandom)
											{
												_layerData->p_percentProcessRandom = ConvertToVector2(__str_data);
												_layerData->p_timeProcessRandom = ConvertToVector2(random_child__->Attribute("time"));
											}
										}

									}

									//get position
									_child_config_layer = _config_layer->FirstChildElement("Pos");							
									if (_child_config_layer)
									{
										BasicDec * __dec = GenerateBasicDec(_child_config_layer, "origin", "vector", "loop", "time", "delay", "delay_first" ,"type", "threadHoldRemove", "rotate_invoke" ,"reserve_move", NULL);


										Vec2 data_vector = __dec->GetDataVector2("vector");
										if (data_vector != Vec2(0,0))
										{
											data_vector.x = data_vector.x  / designSize.x;
											data_vector.y = data_vector.y  / designSize.y;

											__dec->ReplaceDataVector2("vector", data_vector);
										}

										data_vector = __dec->GetDataVector2("threadHoldRemove");
										if (data_vector != Vec2(0, 0))
										{
											data_vector.x = data_vector.x  / designSize.x;
											data_vector.y = data_vector.y  / designSize.y;

											__dec->ReplaceDataVector2("threadHoldRemove", data_vector);
										}

										data_vector = __dec->GetDataVector2("origin");
										if (data_vector != Vec2(0, 0))
										{
											data_vector.x = data_vector.x  / designSize.x;
											data_vector.y = data_vector.y  / designSize.y;

											__dec->ReplaceDataVector2("origin", data_vector);
										}

										_layerData->p_list_layer_element_dec.insert(std::pair<RKString, BasicDec *>("Pos", __dec));
									}
									//get Size
									_child_config_layer = _config_layer->FirstChildElement("Size");
									if (_child_config_layer)
									{
										BasicDec * __dec = GenerateBasicDec(_child_config_layer, "origin", "scale", "time", "loop", "delay", "delay_first", "type", "IsFade", NULL);
										
										if (__dec->GetDatafloat("time") == 0)
										{
											__dec->ReplaceDatafloat("time", _layerData->p_processTime);
										}
										if (__dec->GetDatafloat("delay") <= 0)
										{
											__dec->ReplaceDatafloat("delay", _layerData->p_TimeDelay);
										}

										Vec2 origin_size__ = Vec2(__dec->GetDataVector2("origin").x / designSize.x, __dec->GetDataVector2("origin").y / designSize.y);
										__dec->ReplaceDataVector2("origin", origin_size__);
										float origin_scale__ = __dec->GetDatafloat("scale") / designSize.x;
										__dec->ReplaceDatafloat("scale", origin_scale__);
										_layerData->p_list_layer_element_dec.insert(std::pair<RKString, BasicDec *>("Size", __dec));
									}
									//get Rotate
									_child_config_layer = _config_layer->FirstChildElement("Rotate");
									if (_child_config_layer)
									{
										BasicDec * __dec = GenerateBasicDec(_child_config_layer, "origin", "angle", "time", "loop", "delay", "delay_first", "type", NULL);
										if (__dec->GetDatafloat("time") == 0)
										{
											__dec->ReplaceDatafloat("time", _layerData->p_processTime);
										}
										if (__dec->GetDatafloat("delay") <= 0)
										{
											__dec->ReplaceDatafloat("delay", _layerData->p_TimeDelay);
										}

										_layerData->p_list_layer_element_dec.insert(std::pair<RKString, BasicDec *>("Rotate", __dec));
									}
									//get Opacity
									_child_config_layer = _config_layer->FirstChildElement("Opacity");
									if (_child_config_layer)
									{
										BasicDec * __dec = GenerateBasicDec(_child_config_layer, "origin", "first", "time", "loop", "delay", "delay_first", "type", "IsFade", NULL);
										if (__dec->GetDatafloat("time") == 0)
										{
											__dec->ReplaceDatafloat("time", _layerData->p_processTime);
										}
										if (__dec->GetDatafloat("delay") <= 0)
										{
											__dec->ReplaceDatafloat("delay", _layerData->p_TimeDelay);
										}

										_layerData->p_list_layer_element_dec.insert(std::pair<RKString, BasicDec *>("Opacity", __dec));
									}
									//get Color
									_child_config_layer = _config_layer->FirstChildElement("Color");
									if (_child_config_layer)
									{
										BasicDec * __dec = GenerateBasicDec(_child_config_layer, "origin", "target", "time", "loop", "delay", "delay_first", "type", NULL);
										if (__dec->GetDatafloat("time") == 0)
										{
											__dec->ReplaceDatafloat("time", _layerData->p_processTime);
										}
										if (__dec->GetDatafloat("delay") <= 0)
										{
											__dec->ReplaceDatafloat("delay", _layerData->p_TimeDelay);
										}

										_layerData->p_list_layer_element_dec.insert(std::pair<RKString, BasicDec *>("Color", __dec));
									}

									//get process
									_child_config_layer = _config_layer->FirstChildElement("Process");
									if (_child_config_layer)
									{
										BasicDec * __dec = GenerateBasicDec(_child_config_layer, "origin", "time", "loop", "delay", "delay_first", "type", NULL);
										if (__dec->GetDatafloat("time") == 0)
										{
											__dec->ReplaceDatafloat("time", _layerData->p_processTime);
										}
										if (__dec->GetDatafloat("delay") <= 0)
										{
											__dec->ReplaceDatafloat("delay", _layerData->p_TimeDelay);
										}

										_layerData->p_list_layer_element_dec.insert(std::pair<RKString, BasicDec *>("Process", __dec));
									}
								}

								//get the resource include
								auto _object_contain = _Layer->FirstChildElement("object_contain");
								if (_object_contain)
								{

									if (_object_contain->Attribute("IsRandom"))
									{
										_layerData->p_isObjectRandom = (atoi(_object_contain->Attribute("IsRandom")) != 0);
									}
									//load each object
									auto _object = _object_contain->FirstChildElement("object");
									do
									{
										if (_object)
										{
											ObjectDec * __object_dec = new ObjectDec();

											RKString _name_object = _object->Attribute("name");
											__object_dec->name = _name_object;
											//
											auto _child_element_object = _object->FirstChildElement("source");
											if (_child_element_object)
											{
												auto __data_str = _child_element_object->Attribute("name");
												int size_name = RKString(__data_str).Length();
												__object_dec->name_source = new char[size_name + 1];
												memcpy(__object_dec->name_source, __data_str, size_name + 1);
												
												//load animation if defined
												_child_element_object = _object->FirstChildElement("Animate");
												if (_child_element_object)
												{
													__object_dec->animation_dec = new AnimateDec();
													auto __data_str = _child_element_object->Attribute("time_delay");
													if (__data_str)
													{
														__object_dec->animation_dec->time_delay = atof(__data_str);
													}
													__data_str = _child_element_object->Attribute("loop");
													if (__data_str)
													{
														__object_dec->animation_dec->loop = atoi(__data_str);
													}
													auto __sprite_animation = _child_element_object->FirstChildElement("sprite");
													do
													{
														if (__sprite_animation)
														{
															RKString _sprite_name_source__ = __sprite_animation->Attribute("source");
															__object_dec->animation_dec->InsertSpriteFrameName(_sprite_name_source__);
															__sprite_animation = __sprite_animation->NextSiblingElement();
														}

													} while (__sprite_animation);
												}
											}
											else
											{
												_child_element_object = _object->FirstChildElement("Animature");
												if (_child_element_object)
												{
													__object_dec->animature_dec = new AnimatureDec();
													//
													auto __data_str = _child_element_object->Attribute("name");
													if (__data_str)
													{
														__object_dec->animature_dec->name = __data_str;
													}
													__data_str = _child_element_object->Attribute("SpeedScale");
													if (__data_str)
													{
														__object_dec->animature_dec->SpeedScale = atof(__data_str);
													}
													__data_str = _child_element_object->Attribute("playByIndex");
													if (__data_str)
													{
														__object_dec->animature_dec->idxPlay = atoi(__data_str);
													}
													auto _child_animature_object = _child_element_object->FirstChildElement("pathTexture");
													__object_dec->animature_dec->pathTexture = _child_animature_object->Attribute("path");

													_child_animature_object = _child_element_object->FirstChildElement("pathPlist");
													__object_dec->animature_dec->pathPlist = _child_animature_object->Attribute("path");

													_child_animature_object = _child_element_object->FirstChildElement("pathJson");
													__object_dec->animature_dec->PathJson = _child_animature_object->Attribute("path");
												}
												else
												{
													CC_ASSERT(false); //no object invalid !
												}
											}
											//get stickscreen
											_child_element_object = _object->FirstChildElement("StickScreen");
											if (_child_element_object)
											{
												__object_dec->StickScreen = atoi(_child_element_object->Attribute("value"));
											}

											//get size object
											_child_element_object = _object->FirstChildElement("Size");
											if (_child_element_object)
											{
												Vec2 value = ConvertToVector2(_child_element_object->Attribute("value"));
												__object_dec->Size_dec = new BasicDec();
												if (__object_dec->StickScreen == 1)
												{
													value.x = value.x / designSize.x;
													value.y = value.y / designSize.y;
												}
												else
												{
													value.x = value.x / designSize.x;
													value.y = value.y / designSize.x;
												}

												__object_dec->Size_dec->InsertDataVector2("origin",value);
											}
											else
											{
												Vec2 value = Vec2(50, 50);
												if (_layerData->p_list_layer_element_dec.find("Size") != _layerData->p_list_layer_element_dec.end())
												{
													value = _layerData->p_list_layer_element_dec.at("Size")->GetDataVector2("origin");
												}
												__object_dec->Size_dec = new BasicDec();
												__object_dec->Size_dec->InsertDataVector2("origin", value);
											}
											//get rotate
											_child_element_object = _object->FirstChildElement("Rotate");
											if (_child_element_object)
											{
												float value = atof(_child_element_object->Attribute("value"));
												__object_dec->Rotate_dec = new BasicDec();
												__object_dec->Rotate_dec->InsertDatafloat("origin", value);
											}
											else
											{
												float value = 0;
												if (_layerData->p_list_layer_element_dec.find("Rotate") != _layerData->p_list_layer_element_dec.end())
												{
													value = _layerData->p_list_layer_element_dec.at("Rotate")->GetDatafloat("origin");
												}
												__object_dec->Rotate_dec = new BasicDec();
												__object_dec->Rotate_dec->InsertDatafloat("origin", value);
											}
											//get opacity
											_child_element_object = _object->FirstChildElement("Opacity");
											if (_child_element_object)
											{
												RKUtils::BYTE value = atoi(_child_element_object->Attribute("value"));
												__object_dec->Opacity_dec = new BasicDec();
												__object_dec->Opacity_dec->InsertDataInt("origin", value);
											}
											else
											{
												RKUtils::BYTE value = 255;
												if (_layerData->p_list_layer_element_dec.find("Opacity") != _layerData->p_list_layer_element_dec.end())
												{
													value = _layerData->p_list_layer_element_dec.at("Opacity")->GetDataInt("origin");
												}
												__object_dec->Opacity_dec = new BasicDec();
												__object_dec->Opacity_dec->InsertDataInt("origin", value);
											}
											//get color
											_child_element_object = _object->FirstChildElement("Color");
											if (_child_element_object)
											{
												Color4B value = ConvertToColor(_child_element_object->Attribute("value"));
												__object_dec->Color_dec = new BasicDec();
												__object_dec->Color_dec->InsertDataColor("origin", value);
											}
											else
											{
												Color4B value = Color4B::WHITE;
												if (_layerData->p_list_layer_element_dec.find("Color") != _layerData->p_list_layer_element_dec.end())
												{
													value = _layerData->p_list_layer_element_dec.at("Color")->GetDataColor("origin");
												}
												__object_dec->Color_dec = new BasicDec();
												__object_dec->Color_dec->InsertDataColor("origin", value);
											}

											
											_layerData->InsertObjectBGDec(_name_object, __object_dec);

											_object = _object->NextSiblingElement();
										}
									} while (_object);
								}
								//
								_background->InsertLayerData(_id_layer + "#" + _name_layer, _layerData);
								_Layer = _Layer->NextSiblingElement();
							}

						} while (_Layer);


						InsertBackground(name, _background);

						BGStyle = BGStyle->NextSiblingElement();
					}

				} while (BGStyle);
			}
		}


		BasicDec * LoadDataXML::GenerateBasicDec(tinyxml2::XMLElement * _child_config_layer, char * name0, ...)
		{
			BasicDec * __dec = new BasicDec();
			va_list _var_list;
			char * next_arg = name0;
			//  name0 is the last argument specified; you must access 
			//  all others using the variable-argument macros.
			va_start(_var_list, name0);

			do
			{		
				if (!next_arg)
				{
					break; //run to end of list arg
				}
				auto _data_child = _child_config_layer->Attribute(next_arg);
				if (_data_child)
				{
                    RKString str = RKString(_data_child);
					if (!str.Contains("_"))
					{
                        bool isValue = IsValueNumber(str);
                        
						if(isValue)
						{
							if (!str.Contains("."))
							{
								int _get_data_int = atoi(_data_child);
								__dec->InsertDataInt(next_arg, _get_data_int);
							}
							else
							{
								float _get_data_float = atof(_data_child);
								__dec->InsertDatafloat(next_arg, _get_data_float);
							}
						}
						else
						{
							//the _data_child cannot convert to float or int so insert as char *
							__dec->InsertDataChar(next_arg, (char *)_data_child);
						}
					}
					else
					{
						//vector2 or color or vector3
						auto list__elelemt__ = RKString(_data_child).Split("_");
						if (list__elelemt__.Size() == 2)
						{
							Vec2 data_vector = ConvertToVector2(_data_child);
							__dec->InsertDataVector2(next_arg, data_vector);
						}
						else if (list__elelemt__.Size() == 4)
						{
							Color4B data_color = ConvertToColor(_data_child);
							__dec->InsertDataColor(next_arg, data_color);
						}
					}
				}
				next_arg = va_arg(_var_list, char *);

			} while (true);

			va_end(_var_list);

			return __dec;
		}

		/*
		-----------------------------------------------------
		*/

		UIMenuWidgetDec * LoadDataXML::GetUIWidgetByName(RKString str)
		{
			if (p_ListGameUIWidget.size() > 0 && p_ListGameUIWidget.find(str) != p_ListGameUIWidget.end())
				return p_ListGameUIWidget.at(str);
			return nullptr;
		}

		void LoadDataXML::RemoveUIMEnuWidgetDec(RKString str)
		{
			if (p_ListGameUIWidget.size() > 0 && p_ListGameUIWidget.find(str) != p_ListGameUIWidget.end())
			{
				delete p_ListGameUIWidget.at(str);
				p_ListGameUIWidget.erase(str);
			}
		}

		void LoadDataXML::InsertUIMenuWidgetDec(RKString str, UIMenuWidgetDec * c)
		{
			p_ListGameUIWidget.insert(std::pair<RKString, UIMenuWidgetDec*>(str, c));
		}

		void LoadDataXML::ReloadUIMEnuWidgetDec(RKString str, RKString path)
		{
			UIMenuWidgetDec* dec = nullptr;
			FILE * f = File::GetFile(path.GetString(), "rb");
			TiXmlDocument * pObjectXML = new TiXmlDocument();
			if (f)
			{
				
				pObjectXML->LoadFile(f);
				dec = OnLoadUIWidgetDetailXML(pObjectXML);
				fclose(f);
			}
			CC_SAFE_DELETE(pObjectXML);
			CC_SAFE_DELETE(p_ListGameUIWidget[str]);
			p_ListGameUIWidget[str] = dec;
		}

		UIMenuWidgetDec * LoadDataXML::OnLoadUIWidgetDetailXML(TiXmlDocument * objectXMl)
		{
			auto Root = objectXMl->FirstChildElement("GAME");
			if (!Root)
			{
				PASSERT2(false, "XML MenuWidget fail to load!");
				return nullptr;
			}

			auto UIMenuWidget = Root->FirstChildElement("MenuWidget");
			if (UIMenuWidget)
			{

#pragma region load_menu_widget_dec

				UIMenuWidgetDec * UIDec = new UIMenuWidgetDec();

				//get first element
                std::string name_ui = UIMenuWidget->Attribute("name");
				Vec2 design_size = GetGameDesignSize();
				if (UIMenuWidget->Attribute("designSize"))
				{
					design_size = ConvertToVector2(UIMenuWidget->Attribute("designSize"));
				}
				UIDec->DesignSize = design_size;
				UIDec->NameMenuWidget = name_ui;

				//get global fade process
				auto fade_process = UIMenuWidget->FirstChildElement("fade_process");
				sound_click_dec * global_id_sound_click = nullptr;
				if (fade_process)
				{
					auto str = fade_process->Attribute("max_idx_first");
					if (str)
					{
						UIDec->max_idx_first = atoi(str);
					}

					auto _sound_click = fade_process->FirstChildElement("sound_click");
					if (_sound_click)
					{
						global_id_sound_click = new sound_click_dec();
						auto string_sound_click = _sound_click->Attribute("value");
						if (string_sound_click)
						{
							global_id_sound_click->id_click = string_sound_click;
						}
						string_sound_click = _sound_click->Attribute("delayTime");
						if (string_sound_click)
						{
							global_id_sound_click->delayTime = atof(string_sound_click);
						}
						string_sound_click = _sound_click->Attribute("loop");
						if (string_sound_click)
						{
							global_id_sound_click->loop = atoi(string_sound_click);;
						}
					}

					// require
					auto _time_process = fade_process->FirstChildElement("time_process");
					do
					{
						if (_time_process)
						{
							float time_process = atof(_time_process->Attribute("val"));

							time_process_ui time_process__;
							time_process__.time_process = time_process;

							auto sound_process__ = _time_process->FirstChildElement("sound");
							if (sound_process__)
							{
								time_process__.ID_SOUND_FADE_IN = sound_process__->Attribute("FadeIn");
								time_process__.ID_SOUND_FADE_OUT = sound_process__->Attribute("FadeOut");
								if (sound_process__->Attribute("delayTime"))
								{
									time_process__.delayTime = atof(sound_process__->Attribute("delayTime"));
								}
								if (sound_process__->Attribute("loop"))
								{
									time_process__.loop = atoi(sound_process__->Attribute("loop"));
								}
							}
							UIDec->time_process.insert(UIDec->time_process.begin() + atoi(_time_process->Attribute("id")),time_process__);

							_time_process = _time_process->NextSiblingElement();
						}
					} while (_time_process);
				}

				//get the root UI element

				auto _widget_layer_root = UIMenuWidget->FirstChildElement("Widget");
				do
				{
					if (_widget_layer_root)
					{
						auto _widget_root = OnParseWidgetEntity(_widget_layer_root, nullptr, UIDec, global_id_sound_click);
						UIDec->InsertChildElement(_widget_root->NameWidget, _widget_root->index, _widget_root);
						UIDec->InsertNameToList(_widget_root->NameWidget);
						_widget_layer_root = _widget_layer_root->NextSiblingElement();
					}

				} while (_widget_layer_root);
#pragma endregion load_menu_widget_dec

				///InsertUIMenuWidgetDec(UIDec->NameMenuWidget, UIDec);

				return UIDec;
			}
			else
			{
				PASSERT2(UIMenuWidget != nullptr, "must defined menu widget!");
			}

			return nullptr;
		}

		std::vector<int> LoadDataXML::GetTableXMLLoad()
		{
			return p_table_screen_xml_load;
		}

		bool LoadDataXML::OnLoadUIWidgetDecTableXML(TiXmlDocument * objectXMl, RKString name_table)
		{
			auto Root = objectXMl->FirstChildElement("GAME");
			if (!Root)
			{
				PASSERT2(false, "XML MenuWidget fail to load!");
				return false;
			}

			auto rootUI = Root->FirstChildElement("list_menu_widget");
			do
			{
				if (rootUI)
				{
					if (name_table == rootUI->Attribute("name"))
					{
						auto UIMenuWidget = rootUI->FirstChildElement("UIWidget");
						do
						{
							if (UIMenuWidget)
							{
								auto path = UIMenuWidget->Attribute("path");
								auto name_ui = UIMenuWidget->Attribute("name");
								auto name_id = UIMenuWidget->Attribute("id");
								TiXmlDocument * p_ObjectXML = new TiXmlDocument();

								bool p_IsUseArkXML = false;
#ifdef USE_ARK_ARCHIVE
								p_IsUseArkXML = true;
#endif

#ifdef USE_ARK_ARCHIVE
								if (p_IsUseArkXML)
								{
									tinyxml2::XMLError error = p_ObjectXML->LoadFileArk(path);
									if (error == tinyxml2::XMLError::XML_NO_ERROR)
									{
										auto UIDec = OnLoadUIWidgetDetailXML(p_ObjectXML);
										PASSERT2(UIDec != nullptr, "UIdec is null, check the ui again !");
										InsertUIMenuWidgetDec(name_ui, UIDec);
									}
								}
								else
								{
									p_IsUseArkXML = false;
								}
#endif
								if (!p_IsUseArkXML)
								{
									FILE * f = File::GetFile(path, "rb");
									if (f)
									{
										p_ObjectXML->LoadFile(f);

										auto UIDec = OnLoadUIWidgetDetailXML(p_ObjectXML);
										PASSERT2(UIDec != nullptr, "UIdec is null, check the ui again !");
										InsertUIMenuWidgetDec(name_ui, UIDec);
										int id_xml_screen = atoi(name_id);
										fclose(f);

										p_table_screen_xml_load.push_back(id_xml_screen);
									}
									else
									{
										PASSERT2(false, "XML file cannot be read or found !");
									}
								}
								delete p_ObjectXML;
								p_ObjectXML = NULL;


								UIMenuWidget = UIMenuWidget->NextSiblingElement();
							}

						} while (UIMenuWidget);

						return true;
					}
					rootUI = rootUI->NextSiblingElement();
				}
			} while (rootUI);

			return false;
		}

		void LoadDataXML::OnLoadUIWidgetDecXML(TiXmlDocument * objectXMl)
		{
			auto Root = objectXMl->FirstChildElement("GAME");
			if (!Root)
			{
				PASSERT2(false, "XML MenuWidget fail to load!");
				return;
			}

			auto rootUI = Root->FirstChildElement("list_menu_widget");
			if (rootUI)
			{
				auto UIMenuWidget = rootUI->FirstChildElement("UIWidget");
				do
				{
					if (UIMenuWidget)
					{
						auto path = UIMenuWidget->Attribute("path");
						auto name_ui = UIMenuWidget->Attribute("name");

						TiXmlDocument * p_ObjectXML = new TiXmlDocument();

						bool p_IsUseArkXML = false;
#ifdef USE_ARK_ARCHIVE
						p_IsUseArkXML = true;
#endif

#ifdef USE_ARK_ARCHIVE
						if (p_IsUseArkXML)
						{
							tinyxml2::XMLError error = p_ObjectXML->LoadFileArk(path);
							if (error == tinyxml2::XMLError::XML_NO_ERROR)
							{
								auto UIDec = OnLoadUIWidgetDetailXML(p_ObjectXML);
								PASSERT2(UIDec != nullptr, "UIdec is null, check the ui again !");
								InsertUIMenuWidgetDec(name_ui, UIDec);
							}
						}
						else
						{
							p_IsUseArkXML = false;
						}
#endif
						if(!p_IsUseArkXML)
						{
							FILE * f = File::GetFile(path, "rb");
							if (f)
							{
								p_ObjectXML->LoadFile(f);

								auto UIDec = OnLoadUIWidgetDetailXML(p_ObjectXML);
								PASSERT2(UIDec != nullptr, "UIdec is null, check the ui again !");
								InsertUIMenuWidgetDec(name_ui, UIDec);

								fclose(f);
							}
							else
							{
								PASSERT2(false, "XML file cannot be read or found !");
							}
						}
						delete p_ObjectXML;
						p_ObjectXML = NULL;


						UIMenuWidget = UIMenuWidget->NextSiblingElement();
					}

				} while (UIMenuWidget);
			}
		}

		bool LoadDataXML::OnParseValueIntoXMLDec(RKString name_common_value, RKString common_value, xml::BasicDec * data, xml::UILayerWidgetDec * _widget)
		{
			if (data == nullptr || name_common_value == "" || common_value == "")
				return false;

			auto split_content = common_value.Split("_");

			bool isOnlyValue = true;
            RKString temp_str =split_content[0];
            isOnlyValue = IsValueNumber(temp_str);

			if (split_content.Size() > 1 && isOnlyValue)
			{
				if (split_content.Size() == 4) //color
				{
					Color4B tint = ConvertToColor(common_value);
					data->ReplaceDataColor(name_common_value.GetString(), tint);
				}
				else if (split_content.Size() == 2) // vector 2
				{
					Vec2  vector = ConvertToVector2(common_value);
					data->ReplaceDataVector2(name_common_value.GetString(), vector);
				}
			}
			else
			{

				bool IsStringValue = false;

				if ((name_common_value == "source" || (std::string(name_common_value.GetString())[0] == '[' )) &&
					_widget !=nullptr && 
					(_widget->typeValue == WIDGET_FONT || _widget->typeValue == WIDGET_TEXT_FIELD))
				{
					IsStringValue = true;
				}

				if (!IsStringValue)
				{
					if (!common_value.Contains(".")) //string or int value
					{
						bool isValue = IsValueNumber(common_value);

						if(isValue)
						{
							int value = atoi(common_value.GetString());
							data->ReplaceDataInt(name_common_value.GetString(), value);
						}
						else
						{
							IsStringValue = true;
						}
					}
					else
					{
                        
                        bool isfloatValue = IsValueNumber(common_value);
                        
						if(isfloatValue)
						{
							float value = atof(common_value.GetString());
							data->ReplaceDatafloat(name_common_value.GetString(), value);
						}
						else
						{
							IsStringValue = true;
						}
					}
				}

				if (IsStringValue)
				{
					RKString str_value = "";

					if (common_value.Contains("\\n"))
					{
						auto _l_str = common_value.Split("\\n");
						for (size_t ii = 0; ii < _l_str.Size(); ii++)
						{
							str_value += _l_str.GetAt(ii);
							if (ii != _l_str.Size() - 1)
							{
								str_value += "\n";
							}
						}
					}
					else
					{
						str_value = common_value;
					}

					_widget->GeneralValue->ReplaceDataChar(name_common_value.GetString(), (char*)str_value.GetString());
				}
			}

			return true;
		}

		UILayerWidgetDec * LoadDataXML::OnParseWidgetEntity(TiXmlElement * element_xml, UILayerWidgetDec* parent_widget, UIMenuWidgetDec * UIDec, sound_click_dec * global_id_sound_click)
		{
			if (!element_xml)
				return nullptr;

            RKString name_widget = element_xml->Attribute("name");
			UILayerWidgetDec * _widget = new UILayerWidgetDec();
            _widget->InitValue();
			_widget->p_parent_layer = parent_widget;
			_widget->p_menu_widget = UIDec;
			_widget->index = atoi(element_xml->Attribute("id"));
			auto type_res = element_xml->Attribute("typeRes");
			TYPE_UI_WIDGET ui_widget_type = GetUIWidget(type_res);
			PASSERT2(ui_widget_type != TYPE_UI_WIDGET::WIDGET_NONE, "ERROR: wrong widget dec!");

			_widget->typeValue = ui_widget_type;
            _widget->NameWidget = name_widget.GetString();

			/*PINFO("INFO: widget:(%s) with type:(%s) and parent is:(%s)",
				_widget->NameWidget.GetString(),
				element_xml->Attribute("typeRes"),
				parent_widget ? parent_widget->NameWidget.GetString() : "null"
				);*/
			//
			if (element_xml->Attribute("visible") != nullptr)
			{
				_widget->visible = atoi(element_xml->Attribute("visible"));
			}

			if (element_xml->Attribute("idx_appear") != nullptr)
			{
				_widget->index_appear = atoi(element_xml->Attribute("idx_appear"));
			}

			if (parent_widget && _widget->index_appear <= parent_widget->index_appear)
			{
                /*
				PINFO("WARNING : The idx_appear:(%d) of child:(%s) is smaller than idx:(%d) the parent:(%s)", _widget->index_appear, _widget->NameWidget.GetString(), parent_widget->index_appear, parent_widget->NameWidget.GetString());
				PWARN1("the idx_appear of child:(%s) will be recorrect to:(%d)", _widget->NameWidget.GetString(), parent_widget->index_appear + 1);
                */
				_widget->index_appear = parent_widget->index_appear + 1;
			}

			//time process
			if (element_xml->Attribute("time_process") != nullptr)
			{
				_widget->TimeProcessFade = atof(element_xml->Attribute("time_process"));
			}
			else
			{
					/*if (parent_widget)
					{
						PINFO("INFO: use the parent:(%s) widget time process", parent_widget->NameWidget.GetString())
						_widget->TimeProcessFade = parent_widget->TimeProcessFade;
					}
					else if (!parent_widget)*/
				{
				//	PINFO("INFO: use the UIMenu:(%s) widget time process", UIDec->NameMenuWidget.GetString())
						if (_widget->index_appear >= (int)UIDec->time_process.size())
						{
							_widget->TimeProcessFade = UIDec->time_process.at(0).time_process;
						}
						else {
							_widget->TimeProcessFade = UIDec->time_process.at(_widget->index_appear).time_process;
						}
					}
			}

			auto common_value = element_xml->FirstChildElement("common_value");
			if (!common_value)
			{
				PASSERT2(false, "ERROR: the common value was not found !");
				return nullptr;
			}

			auto common_obj = common_value->FirstChildElement("common");
			do
			{
				if (common_obj)
				{
					RKString name_common_value = common_obj->Attribute("name");

					RKString common_value = common_obj->Attribute("value");

					if (name_common_value == "sound_click")
					{
						_widget->sound_click = new sound_click_dec();
						_widget->sound_click->id_click = common_value;
						if (common_obj->Attribute("delay_time"))
						{
							_widget->sound_click->delayTime = atof(common_obj->Attribute("delay_time"));
						}
						else if (parent_widget && 
							(parent_widget->typeValue == WIDGET_LAYER || parent_widget->typeValue == WIDGET_LAYOUT) &&
							parent_widget->sound_click)
						{
							_widget->sound_click->delayTime = parent_widget->sound_click->delayTime;
						}
						else if (!parent_widget && global_id_sound_click)
						{
							_widget->sound_click->delayTime = global_id_sound_click->delayTime;
						}


						if (common_obj->Attribute("loop"))
						{
							_widget->sound_click->loop = atoi(common_obj->Attribute("loop"));
						}
						else if (parent_widget &&
							(parent_widget->typeValue == WIDGET_LAYER || parent_widget->typeValue == WIDGET_LAYOUT) &&
							parent_widget->sound_click)
						{
							_widget->sound_click->loop = parent_widget->sound_click->loop;
						}
						else if (!parent_widget && global_id_sound_click)
						{
							_widget->sound_click->loop = global_id_sound_click->loop;
						}
					}
					if (name_common_value == "source" || name_common_value == "title" || name_common_value =="cap_insert")
					{
						if (common_value.Contains("\\n"))
						{
							common_value.ReplaceAll("\\n", "\n");
						}

						_widget->GeneralValue->ReplaceDataChar(name_common_value.GetString(), (char*)common_value.GetString());
					}
					else
					{
						bool res = OnParseValueIntoXMLDec(name_common_value, common_value, _widget->GeneralValue, _widget);
						//PASSERT2(res == true, "false");
					}
					common_obj = common_obj->NextSiblingElement();
				}
			} while (common_obj);

			if ((_widget->typeValue == WIDGET_BUTTON || _widget->typeValue == WIDGET_CHECK_BOX || _widget->typeValue == WIDGET_SLIDER
				|| _widget->typeValue == WIDGET_LAYER || _widget->typeValue == WIDGET_LAYOUT) && _widget->sound_click == nullptr)
			{
				if (parent_widget &&
					(parent_widget->typeValue == WIDGET_LAYER || parent_widget->typeValue == WIDGET_LAYOUT) &&
					parent_widget->sound_click)
				{
					_widget->sound_click = parent_widget->sound_click;
				}
				else if (!parent_widget && global_id_sound_click)
				{
					_widget->sound_click = global_id_sound_click;
				}
				else
				{
					_widget->sound_click = new sound_click_dec();
				}
			}

			Vec2 design_size = UIDec->DesignSize;
			//parse position
			auto child_element = element_xml->FirstChildElement("pos");
			if (child_element)
			{
				const char * str_pos = child_element->Attribute("origin");
				if (!str_pos)
				{
					PASSERT2(false, "ERROR: NOT FOUND: origin position element");
				}

				Vec2 first_value, origin_value;

				Vec2 pos = ConvertToVector2(str_pos);


				origin_value = Vec2(pos.x / design_size.x, pos.y / design_size.y);
				first_value = origin_value;
				_widget->OriginValue->ReplaceDataVector2("pos_ratio", origin_value);
				//_widget->FirstValue->ReplaceDataVector2("pos_ratio", first_value);
				//
				str_pos = child_element->Attribute("first");
				if (str_pos)
				{
					pos = ConvertToVector2(str_pos);
					first_value = Vec2(pos.x / design_size.x, pos.y / design_size.y);
					_widget->FirstValue->InsertDataVector2("pos_ratio", first_value);
				}
				//
				bool not_fade = false;
				str_pos = child_element->Attribute("not_fade");
				if (str_pos)
				{
					not_fade = atoi(str_pos) == 1;
				}
				_widget->GeneralValue->ReplaceDataInt("not_fade", (int)not_fade);
				//
				if (first_value != origin_value && not_fade == false)
				{
					_widget->SetActionFade(UI_STATE_FADE_ACTION::FADE_MOVE);
				}
			}
			else
			{
				PASSERT2(false, "ERROR: NOT FOUND: position element");
			}

			//parse scale
			child_element = element_xml->FirstChildElement("Size");
			if (child_element)
			{
				const char * str_value = child_element->Attribute("origin");
				if (!str_value)
				{
					PASSERT2(false, "ERROR: NOT FOUND: origin size element");
				}

				Vec2 first_value, origin_value;

				Vec2 size = ConvertToVector2(str_value);

				if (_widget->typeValue == WIDGET_TEXT_FIELD || _widget->typeValue == WIDGET_FONT)
				{
					origin_value = first_value = size;
				}
				else
				{
					if (_widget->GeneralValue->GetDataInt("stick_screen") == 1)
						origin_value = Vec2(size.x / design_size.x, size.y / design_size.y);
					else
						origin_value = size;
					first_value = origin_value;
				}

				_widget->OriginValue->ReplaceDataVector2("scale_ratio", origin_value);
				//_widget->FirstValue->ReplaceDataVector2("scale_ratio", first_value);
				//
				str_value = child_element->Attribute("first");
				if (str_value)
				{
					size = ConvertToVector2(str_value);
					if (_widget->typeValue == WIDGET_TEXT_FIELD || _widget->typeValue == WIDGET_FONT)
					{
						first_value = size;
					}
					else
					{
						if (_widget->GeneralValue->GetDataInt("stick_screen") == 1)
							first_value = Vec2(size.x / design_size.x, size.y / design_size.y);
						else
							first_value = size;

						//first_value = Vec2(size.x / design_size.x, size.y / design_size.y);
					}
					_widget->FirstValue->InsertDataVector2("scale_ratio", first_value);
				}
				//
				bool not_fade = false;
				str_value = child_element->Attribute("not_fade");
				if (str_value)
				{
					not_fade = atoi(str_value) == 1;
				}
				_widget->GeneralValue->ReplaceDataInt("not_fade", (int)not_fade);
				//
				if (first_value != origin_value && not_fade == false)
				{
					_widget->SetActionFade(UI_STATE_FADE_ACTION::FADE_ZOOM);
				}
			}
			else
			{
				PASSERT2(false, "ERROR: NOT FOUND: position element");
			}

			//parse opacity
			child_element = element_xml->FirstChildElement("opacity");
			if (child_element)
			{
				const char * str_value = child_element->Attribute("origin");

				int first_value, origin_value;

				if (str_value)
				{
					first_value = origin_value = atoi(str_value);
				}
				else
				{
					first_value = origin_value = 255;
				}

				_widget->OriginValue->ReplaceDataInt("opacity", origin_value);
				//_widget->FirstValue->ReplaceDataInt("opacity", first_value);
				//
				str_value = child_element->Attribute("first");
				if (str_value)
				{
					first_value = atoi(str_value);
					_widget->FirstValue->InsertDataInt("opacity", first_value);
				}
				//
				bool not_fade = false;
				str_value = child_element->Attribute("not_fade");
				if (str_value)
				{
					not_fade = atoi(str_value) == 1;
				}
				_widget->GeneralValue->ReplaceDataInt("not_fade", (int)not_fade);
				//
				if (first_value != origin_value && not_fade == false)
				{
					_widget->SetActionFade(UI_STATE_FADE_ACTION::FADE_TRANS);
				}
			}

			//parse rotate
			child_element = element_xml->FirstChildElement("rotate");
			if (child_element)
			{
				const char * str_value = child_element->Attribute("origin");

				float first_value, origin_value;

				if (str_value)
				{
					first_value = origin_value = atof(str_value);
				}
				else
				{
					first_value = origin_value = 0.f;
				}

				_widget->OriginValue->ReplaceDatafloat("rotate", origin_value);
				//_widget->FirstValue->ReplaceDatafloat("rotate", first_value);
				//
				str_value = child_element->Attribute("first");
				if (str_value)
				{
					first_value = atof(str_value);
					_widget->FirstValue->InsertDatafloat("rotate", first_value);
				}
				//
				bool not_fade = false;
				str_value = child_element->Attribute("not_fade");
				if (str_value)
				{
					not_fade = atoi(str_value) == 1;
				}
				_widget->GeneralValue->ReplaceDataInt("not_fade", (int)not_fade);
				//
				if (first_value != origin_value && not_fade == false)
				{
					_widget->SetActionFade(UI_STATE_FADE_ACTION::FADE_ROTATE);
				}
			}
			
			child_element = element_xml->FirstChildElement("color");
			if (child_element)
			{
				const char * str_value = child_element->Attribute("origin");
				if (str_value)
				{
					auto color_tint = RKString(str_value).Split("_");
					_widget->GeneralValue->ReplaceDataColor("tint", Color4B(atoi(color_tint[0].GetString()), atoi(color_tint[1].GetString()), atoi(color_tint[2].GetString()), 255));
				}
			}

			//parse command

			child_element = element_xml->FirstChildElement("COMMAND");
			if (child_element)
			{
				auto command_xml = child_element->FirstChildElement("cmd");
				do
				{
					if (command_xml)
					{
                        std::string name_cmd = command_xml->Attribute("name");
						std::string contain_cmd = command_xml->GetText();
	//					contain_cmd.ReplaceAll("\t", "");
	//					contain_cmd.ReplaceAll("\n", "");
						
						_widget->InsertCommand(name_cmd, contain_cmd);
						command_xml = command_xml->NextSiblingElement();
					}
				} while (command_xml);
			}


			//parse the child widget into its parent

			auto _child_widget = element_xml->FirstChildElement("Widget");
			do
			{
				if (_child_widget)
				{
					auto _child_widget_dec = OnParseWidgetEntity(_child_widget, _widget, UIDec, global_id_sound_click);
					_widget->InsertChildWidget(_child_widget_dec);

					_child_widget = _child_widget->NextSiblingElement("Widget");
				}
			} while (_child_widget);

			return _widget;
		}

		TYPE_UI_WIDGET LoadDataXML::GetUIWidget(RKString name)
		{
			if (name == "button")
				return TYPE_UI_WIDGET::WIDGET_BUTTON;
			else if (name == "panel")
				return TYPE_UI_WIDGET::WIDGET_PANEL;
			else if (name == "font")
				return TYPE_UI_WIDGET::WIDGET_FONT;
			else if (name == "check_box")
				return TYPE_UI_WIDGET::WIDGET_CHECK_BOX;
			else if (name == "loading_bar")
				return TYPE_UI_WIDGET::WIDGET_LOADING_BAR;
			else if (name == "text_field")
				return TYPE_UI_WIDGET::WIDGET_TEXT_FIELD;
			else if (name == "slider")
				return TYPE_UI_WIDGET::WIDGET_SLIDER;
			else if (name == "list_view")
				return TYPE_UI_WIDGET::WIDGET_LIST_VIEW;
			else if (name == "scroll_view")
				return TYPE_UI_WIDGET::WIDGET_SCROLL_VIEW;
			else if (name == "page_view")
				return TYPE_UI_WIDGET::WIDGET_PAGE_VIEW;
			else if (name == "layer_widget")
				return TYPE_UI_WIDGET::WIDGET_LAYER;
			else if (name == "layout")
				return TYPE_UI_WIDGET::WIDGET_LAYOUT;
			else if (name == "radio_btn")
				return TYPE_UI_WIDGET::WIDGET_RADIO_BTN;
			else if (name == "panel_clipping")
				return TYPE_UI_WIDGET::WIDGET_PANEL_CLIPPING;
			else if (name == "edit_box")
				return TYPE_UI_WIDGET::WIDGET_EDIT_BOX;
			else if (name == "time_process")
				return TYPE_UI_WIDGET::WIDGET_TIME_PROCESS;
			else if (name == "list_view_table")
				return TYPE_UI_WIDGET::WIDGET_LIST_VIEW_TABLE;
			else if (name == "time_digital")
				return TYPE_UI_WIDGET::WIDGET_TIME_DIGITAL;
			else if (name == "animate")
				return TYPE_UI_WIDGET::WIDGET_ANIM;
			else if (name == "particle")
				return TYPE_UI_WIDGET::WIDGET_PARTICLE;
			return TYPE_UI_WIDGET::WIDGET_NONE;
		}
		/*
		-----------------------------------------------------
		*/

		Vec2 LoadDataXML::ConvertToVector2(RKString str)
		{
			auto val = str.Split("_");
			if (val.Size() != 2)
			{
				return Vec2(0, 0);
			}
			float x = atof(val.GetAt(0).GetString());
			float y = atof(val.GetAt(1).GetString());
			return Vec2(x, y);
		}

		Vec3 LoadDataXML::ConvertToVector3(RKString str)
		{
			auto val = str.Split("_");
			if (val.Size() != 3)
			{
				return Vec3(0, 0, 0);
			}
			float x = atof(val.GetAt(0).GetString());
			float y = atof(val.GetAt(1).GetString());
			float z = atof(val.GetAt(2).GetString());
			return Vec3(x, y, z);
		}

		Color4B LoadDataXML::ConvertToColor(RKString str)
		{
			auto val = str.Split("_");
			if (val.Size() != 4)
			{
				return Color4B::WHITE;
			}
			RKUtils::BYTE r = atoi(val.GetAt(0).GetString());
			RKUtils::BYTE g = atoi(val.GetAt(1).GetString());
			RKUtils::BYTE b = atoi(val.GetAt(2).GetString());
			RKUtils::BYTE a = atoi(val.GetAt(3).GetString());
			return Color4B(r, g, b, a);
		}

		cocos2d::Rect LoadDataXML::ConvertToRect(RKString str)
		{
			auto val = str.Split("_");
			if (val.Size() != 4)
			{
				return cocos2d::Rect(0, 0, 0, 0);
			}
			float x = atof(val.GetAt(0).GetString());
			float y = atof(val.GetAt(1).GetString());
			int w = atoi(val.GetAt(2).GetString());
			int h = atoi(val.GetAt(3).GetString());
			return cocos2d::Rect(x, y, w, h);
		}
		/*
		-----------------------------------------------------
		*/

		Wide_ObjectDec *  LoadDataXML::GetWideObjectByName(RKString str)
		{
			if (p_listWideObject.size() > 0 && p_listWideObject.find(str) != p_listWideObject.end())
			{
				return p_listWideObject.at(str);
			}
			return nullptr;
		}

		void LoadDataXML::InsertWideObjectDec(RKString str, Wide_ObjectDec * c)
		{
			p_listWideObject.insert(std::pair<RKString, Wide_ObjectDec*>(str, c));
		}

		Wide_ObjectDec * LoadDataXML::OnParseObjectWide(TiXmlElement * element_xml, Wide_ObjectDec* parent_object, Vec2 design_size)
		{
			if (element_xml)
			{
				Wide_ObjectDec * _object = new Wide_ObjectDec();

				_object->name_object = element_xml->Attribute("name");
				_object->parent_object = parent_object;
				_object->design_size = design_size;
				auto attribute = element_xml->FirstChildElement("Main");
				if (attribute)
				{
					_object->type_object = atoi(attribute->Attribute("type_object"));
					_object->CommonValue->ReplaceDataInt("stick_screen", atoi(attribute->Attribute("stick_screen")));

					_object->CommonValue->ReplaceDataInt("visible", atoi(attribute->Attribute("visible")));

					if (attribute->Attribute("anchor_point"))
					{
						_object->CommonValue->ReplaceDataVector2("anchor_point", ConvertToVector2(attribute->Attribute("anchor_point")));
					}
					if (attribute->Attribute("casca_color"))
					{
						_object->CommonValue->InsertDataInt("casca_color", atoi(attribute->Attribute("casca_color")));
					}
					if (attribute->Attribute("casca_opacity"))
					{
						_object->CommonValue->InsertDataInt("casca_opacity", atoi(attribute->Attribute("casca_opacity")));
					}
				}
				else
				{
					PASSERT2(attribute != nullptr, "ERROR : main attribute not found !");
					return nullptr;
				}

				if (_object->type_object == 0 || _object->type_object == 4 || _object->type_object == 5 || _object->type_object == 6) //sprite or process timer or prite_path
				{
					attribute = element_xml->FirstChildElement("Source");
					if (attribute)
					{
						_object->name_source = attribute->Attribute("name");
						if (_object->type_object == 5)
						{
							_object->CommonValue->InsertDataVector2("idx_sprite", ConvertToVector2(attribute->Attribute("idx_sprite")));
							_object->CommonValue->InsertDataVector2("number_sprite", ConvertToVector2(attribute->Attribute("number_sprite")));
						}
					}
					else
					{
						PASSERT2(attribute != nullptr, "ERROR : source attribute not found !");
						return nullptr;
					}
				}
				else if (_object->type_object == 1)
				{
					attribute = element_xml->FirstChildElement("Animature");
					if (attribute)
					{
						_object->animature_dec = new AnimatureDec();
						_object->animature_dec->name = attribute->Attribute("name");
						char * data_amanture = (char *)attribute->Attribute("speed_scale");
						if (data_amanture){
							_object->animature_dec->SpeedScale = atoi(data_amanture);
						}

						data_amanture = (char *)attribute->Attribute("playByIndex");
						if (data_amanture){
							_object->animature_dec->idxPlay = atoi(data_amanture);
						}

						_object->animature_dec->pathTexture = attribute->FirstChildElement("pathTexture")->Attribute("path");
						_object->animature_dec->pathPlist = attribute->FirstChildElement("pathPlist")->Attribute("path");
						_object->animature_dec->PathJson = attribute->FirstChildElement("pathJson")->Attribute("path");
					}
					else
					{
						PASSERT2(attribute != nullptr, "ERROR : Animature attribute not found !");
						return nullptr;
					}
				}
				else if (_object->type_object == 2)
				{
					attribute = element_xml->FirstChildElement("Animate");
					if (attribute)
					{
						_object->animation_dec = new AnimateDec();
						_object->animation_dec->time_delay = atof(attribute->Attribute("time_delay"));
						_object->animation_dec->loop = atoi(attribute->Attribute("loop"));
						_object->animation_dec->tag_action = atoi(attribute->Attribute("tag"));
						auto _sprite_element = attribute->FirstChildElement("sprite");
						do
						{
							if (_sprite_element)
							{
								auto source_sprite = _sprite_element->Attribute("source");
								auto rect_sprite = _sprite_element->Attribute("rect");
								auto slice_sprite = _sprite_element->Attribute("slice");
								if (rect_sprite || slice_sprite)
								{
									if (slice_sprite)
									{
										Vec2 slice_size = this->ConvertToVector2(slice_sprite);
										auto sp = SpriteFrameCache::getInstance()->getSpriteFrameByName(source_sprite);
										auto rect_texture = sp->getRect();
										cocos2d::Size size_each_slice = cocos2d::Size(rect_texture.size.width / slice_size.x, rect_texture.size.height / slice_size.y);
										Vec2 begin_pos = rect_texture.origin;
										for (int i = 0; i < slice_size.y; i++)
										{
											for (int j = 0; j < slice_size.x; j++)
											{
												cocos2d::Rect actual_rect = cocos2d::Rect(
													begin_pos.x + j* size_each_slice.width, begin_pos.y + i* size_each_slice.height,
													size_each_slice.width, size_each_slice.height
												);
												_object->animation_dec->InsertSpriteFrameNameRect(source_sprite , actual_rect);
											}
										}

									}
									else if (rect_sprite)
									{
										cocos2d::Rect actual_rect = this->ConvertToRect(rect_sprite);
										_object->animation_dec->InsertSpriteFrameNameRect(source_sprite, actual_rect);
									}

								}
								else
								{
									_object->animation_dec->InsertSpriteFrameName(source_sprite);
								}
								_sprite_element = _sprite_element->NextSiblingElement();
							}

						} while (_sprite_element);
					}
					else
					{
						PASSERT2(attribute != nullptr, "ERROR : animation attribute not found !");
						return nullptr;
					}
				}
				else if (_object->type_object == 3)
				{
					attribute = element_xml->FirstChildElement("Text");
					if (attribute)
					{
						_object->text_dec = new FontDec();

						_object->text_dec->name = attribute->Attribute("name");
						_object->text_dec->idx = atoi(attribute->Attribute("IdxFont"));
						_object->text_dec->IsUnicode = false;
						_object->text_dec->d_size = atoi(attribute->Attribute("size"));

						_object->text_dec->d_size = (((float)_object->text_dec->d_size / design_size.x));

						if (attribute->Attribute("border_w"))
						{
							_object->text_dec->d_width_border = atof(attribute->Attribute("border_w"));

							if (attribute->Attribute("border_color"))
							{
								Color4B tint_border = ConvertToColor(attribute->Attribute("border_color"));
								_object->text_dec->d_color_border[0] = tint_border.r;
								_object->text_dec->d_color_border[1] = tint_border.g;
								_object->text_dec->d_color_border[2] = tint_border.b;
								_object->text_dec->d_color_border[3] = tint_border.a;
							}
						}
						if (attribute->Attribute("color"))
						{
							Color4B tint_ = ConvertToColor(attribute->Attribute("color"));
							_object->text_dec->d_color[0] = tint_.r;
							_object->text_dec->d_color[1] = tint_.g;
							_object->text_dec->d_color[2] = tint_.b;
							_object->text_dec->d_color[3] = tint_.a;
						}

					}
					else
					{
						PASSERT2(attribute != nullptr, "ERROR : text attribute not found !");
						return nullptr;
					}
				}
				else if (_object->type_object == 7)
				{

				}
				//parse origin value

				attribute = element_xml->FirstChildElement("Pos");
				if (attribute)
				{
					Vec2 value = ConvertToVector2(attribute->Attribute("origin"));
					value = Vec2(value.x / design_size.x, value.y / design_size.y);
					_object->OriginValue->ReplaceDataVector2("pos_ratio", value);

					const char * first_value = attribute->Attribute("origin");
					if (attribute->Attribute("first"))
					{
						first_value = attribute->Attribute("first");
					}
					Vec2 vecfirst_value = ConvertToVector2(first_value);
					vecfirst_value = Vec2(vecfirst_value.x / design_size.x, vecfirst_value.y / design_size.y);
					_object->FirstValue->InsertDataVector2("pos_ratio", vecfirst_value);
				}

				attribute = element_xml->FirstChildElement("Size");
				if (attribute)
				{
					Vec2 value = ConvertToVector2(attribute->Attribute("origin"));
					value = Vec2(value.x / design_size.x, value.y / design_size.x);
					_object->OriginValue->ReplaceDataVector2("scale_ratio", value);

					const char * first_value = attribute->Attribute("origin");
					if (attribute->Attribute("first"))
					{
						first_value = attribute->Attribute("first");
					}
					Vec2 vecfirst_value = ConvertToVector2(first_value);
					vecfirst_value = Vec2(vecfirst_value.x / design_size.x, vecfirst_value.y / design_size.x);
					_object->FirstValue->InsertDataVector2("scale_ratio", vecfirst_value);
				}

				attribute = element_xml->FirstChildElement("Opacity");
				if (attribute)
				{
					int value = atoi(attribute->Attribute("origin"));
					_object->OriginValue->ReplaceDataInt("opacity", value);

					const char * first_value = attribute->Attribute("origin");
					if (attribute->Attribute("first"))
					{
						first_value = attribute->Attribute("first");
					}
					_object->FirstValue->InsertDataInt("opacity", atoi(first_value));
				}

				attribute = element_xml->FirstChildElement("Rotate");
				if (attribute)
				{
					float value = atof(attribute->Attribute("origin"));
					_object->OriginValue->ReplaceDatafloat("rotate", value);

					const char * first_value = attribute->Attribute("origin");
					if (attribute->Attribute("first"))
					{
						first_value = attribute->Attribute("first");
					}
					_object->FirstValue->InsertDatafloat("rotate", atof(first_value));
				}

				attribute = element_xml->FirstChildElement("Process");
				if (attribute)
				{
					int value = atoi(attribute->Attribute("origin"));
					_object->OriginValue->ReplaceDataInt("Process", value);

					const char * first_value = attribute->Attribute("origin");
					if (attribute->Attribute("first"))
					{
						first_value = attribute->Attribute("first");
					}
					_object->FirstValue->InsertDataInt("Process", atoi(first_value));
				}

				attribute = element_xml->FirstChildElement("Color");
				if (attribute)
				{
					Color4B value = ConvertToColor(attribute->Attribute("origin"));
					_object->OriginValue->ReplaceDataColor("tint", value);

					const char * first_value = attribute->Attribute("origin");
					if (attribute->Attribute("first"))
					{
						first_value = attribute->Attribute("first");
					}
					_object->FirstValue->InsertDataColor("tint", ConvertToColor(first_value));
				}

				//parse command

				attribute = element_xml->FirstChildElement("COMMAND");
				if (attribute)
				{
					auto command_xml = attribute->FirstChildElement("cmd");
					do
					{
						if (command_xml)
						{
							RKString name_cmd = command_xml->Attribute("name");
							RKString contain_cmd = command_xml->GetText();

							_object->InsertCommand(name_cmd, contain_cmd);
							command_xml = command_xml->NextSiblingElement();
						}
					} while (command_xml);
				}

				//parse object child
				attribute = element_xml->FirstChildElement("detailobject");
				do
				{
					if (attribute)
					{
						auto _child_dec = OnParseObjectWide(attribute, _object, design_size);
						_object->InsertWideObjectChild(_child_dec);

						attribute = attribute->NextSiblingElement("detailobject");
					}
				} while (attribute);

				return _object;
			}


			return nullptr;
		}

		void  LoadDataXML::OnLoadListWideObjectDecXML(TiXmlDocument * objectXMl)
		{
			auto Root = objectXMl->FirstChildElement("GAME");
			if (!Root)
				return;

			RKString name_object = "Object";

			auto rootObject = Root->FirstChildElement(name_object.GetString());
			if (rootObject)
			{
				Vec2 design_size = ConvertToVector2(rootObject->Attribute("designSize"));

				auto detailObject = rootObject->FirstChildElement("detailobject");
				do
				{
					if (detailObject)
					{			
						auto w_object = OnParseObjectWide(detailObject, nullptr, ConvertToVector2(rootObject->Attribute("designSize")));
						
						p_listWideObject.insert(std::pair<RKString, Wide_ObjectDec*>(w_object->name_object, w_object));

						detailObject = detailObject->NextSiblingElement();
					}
				} while (detailObject);

			}
		}

		void LoadDataXML::ReleaseAllListWideObjectDecXML()
		{
            int count = 0;
			for (auto it = p_listWideObject.begin(); it != p_listWideObject.end(); it++)
			{
                auto list_w = it->second;
                delete list_w;
                count++;
			}
			p_listWideObject.clear();
		}

		/*
		-----------------------------------------------------
		*/

		SoundDec * LoadDataXML::GetSoundDecByName(RKString str)
		{
			if (p_listSoundDec.size() > 0 && p_listSoundDec.contain(str))
			{
				return p_listSoundDec.at(str);
			}
			return nullptr;
		}
		
		void LoadDataXML::InsertSoundDec(RKString str, SoundDec * c)
		{
			p_listSoundDec.insert(str, c);
		}

		void LoadDataXML::OnLoadListSoundDecXML(TiXmlDocument * objectXMl)
		{
			auto Root = objectXMl->FirstChildElement("GAME");
			if (!Root)
				return;

			auto rootSound = Root->FirstChildElement("Sound");
			if (rootSound)
			{
				auto sound_detail = rootSound->FirstChildElement("sound_play");
				do
				{
					if (sound_detail)
					{
						SoundDec * __sound_dec = new SoundDec();

						RKString name_id = sound_detail->Attribute("name");
						__sound_dec->name_id = name_id;
						auto detail_element = sound_detail->FirstChildElement("path");
						if (detail_element)
						{
							__sound_dec->path = detail_element->Attribute("value");
						}
						detail_element = sound_detail->FirstChildElement("delay_first");
						if (detail_element)
						{
							__sound_dec->delay_first = atof(detail_element->Attribute("value"));
						}
						detail_element = sound_detail->FirstChildElement("loop");
						if (detail_element)
						{
							__sound_dec->loop = atoi(detail_element->Attribute("value"));
						}
						detail_element = sound_detail->FirstChildElement("type");
						if (detail_element)
						{
							__sound_dec->type_sound = detail_element->Attribute("value");
						}

						InsertSoundDec(name_id, __sound_dec);

						sound_detail = sound_detail->NextSiblingElement();
					}


				} while (sound_detail);

			}


		}

		/*
		-----------------------------------------------------
		*/

		LangDec * LoadDataXML::GetLangDecByType(LANGUAGE_TYPE type)
		{
			if (p_listLangDec.size() > 0 && p_listLangDec.find(type) != p_listLangDec.end())
			{
				return p_listLangDec.at(type);
			}
			return nullptr;
		}

		void LoadDataXML::InsertLangDec(LANGUAGE_TYPE type, LangDec * c)
		{
			if (p_listLangDec.find(type) != p_listLangDec.end()) //already insert !
				return;
			p_listLangDec.insert(std::pair<LANGUAGE_TYPE, LangDec*>(type, c));
		}

		void LoadDataXML::OnLoadListLangXML(TiXmlDocument * objectXMl)
		{
			auto Root = objectXMl->FirstChildElement("GAME");
			if (!Root)
				return;

			auto rootLang = Root->FirstChildElement("list_lang");
			if (rootLang)
			{
				auto lang_detail = rootLang->FirstChildElement("lang");
				do
				{
					if (lang_detail)
					{
						LangDec * __lang_dec = new LangDec();

						RKString name_id = lang_detail->Attribute("name");
						__lang_dec->name_id = name_id;
						__lang_dec->dec = lang_detail->Attribute("dec");
						auto detail_element = lang_detail->FirstChildElement("path");
						__lang_dec->path = detail_element->Attribute("value");

						LANGUAGE_TYPE type = LANGUAGE_TYPE::LANG_NONE;
						if (__lang_dec->name_id == "vi")
						{
							type = LANGUAGE_TYPE::LANG_VI;
						}
						else if (__lang_dec->name_id == "en")
						{
							type = LANGUAGE_TYPE::LANG_EN;
						}

						InsertLangDec(type, __lang_dec);

						lang_detail = lang_detail->NextSiblingElement();
					}


				} while (lang_detail);

			}
		}
        
        bool LoadDataXML::IsValueNumber(RKString str)
        {
            //return std::regex_match(std::string(str.GetString()), std::regex("^(\\-|\\+)?[0-9]*(\\.[0-9]+)?"));
            auto sep_number = str.Split(".");
            
            if(sep_number.Size() == 1)
            {
                int val;
                bool res = str.ParseValue(val,0);
                return res;
            }
            else if (sep_number.Size() == 2)
            {
                float val;
                bool res = str.ParseValue(val);
                return res;
            }
            else
            {
                return false;
            }
            
        }

	}
}
