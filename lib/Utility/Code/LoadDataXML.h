#ifndef LOAD_DATA_XML_H
#define LOAD_DATA_XML_H

#define USE_TINY_XML2
#include "math/CCGeometry.h"
#include "base/CCVector.h"
#ifdef USE_TINY_XML2
#include "external/tinyxml2/tinyxml2.h"
#else
#include "../../external/tinyxml/tinyxml.h"
#endif
#include "FileManager.h"
#include "DisplayView.h"
#include "RKString_Code/RKString.h"

#include "wTool.h"
#ifdef USE_TINY_XML2

#define TiXmlDocument	tinyxml2::XMLDocument
#define TiXmlNode		tinyxml2::XMLNode
#define TiXmlElement	tinyxml2::XMLElement
#endif
using namespace RKUtils;
USING_NS_CC;
namespace Utility
{
	enum UI_STATE_FADE_ACTION
	{
		FADE_MOVE = 1,
		FADE_TRANS = FADE_MOVE << 1,
		FADE_ROTATE = FADE_TRANS << 1,
		FADE_ZOOM = FADE_ROTATE << 1,
		FADE_DELAY = FADE_ZOOM<<1,
		FADE_BENZIER = FADE_DELAY <<1,
		FADE_JUMP = FADE_BENZIER<<1,
		FADE_ROTATE_AROUND = FADE_JUMP<<1,
		FADE_SCALE = FADE_ROTATE_AROUND << 1,
		FADE_TINT = FADE_SCALE<<1,
		FADE_COUNT = 4096
	};

	enum LANGUAGE_TYPE
	{
		LANG_NONE = 0,
		LANG_VI,
		LANG_EN,
		LANG_COUNT
	};

	namespace xml
	{
		enum HARDWARD_PROFILE
		{
			HARDWARE_LOW = 0,
			HARDWARE_MED,
			HARDWARE_HIGH
		};

		enum TYPE_UI_WIDGET
		{
			WIDGET_NONE = 0,
			WIDGET_BUTTON,
			WIDGET_PANEL,
			WIDGET_FONT,
			WIDGET_CHECK_BOX,
			WIDGET_LOADING_BAR,
			WIDGET_TEXT_FIELD,
			WIDGET_SLIDER,
			WIDGET_LIST_VIEW,
			WIDGET_SCROLL_VIEW,
			WIDGET_PAGE_VIEW,
			WIDGET_LAYER,
			WIDGET_LAYOUT,
			WIDGET_RADIO_BTN,
			WIDGET_EDIT_BOX,
			WIDGET_PANEL_CLIPPING,
			WIDGET_TIME_PROCESS,
			WIDGET_LIST_VIEW_TABLE,
			WIDGET_TIME_DIGITAL,
			WIDGET_ANIM,
			WIDGET_PARTICLE,
			WIDGET_COUNT
		};

		struct V2
		{
			float x;
			float y;

			V2()
			{
				x = y = 0;
			}

			V2(Vec2 v)
			{
				x = v.x;
				y = v.y;
			}

			V2(float _x, float _y)
			{
				x = _x;
				y = _y;
			}

			Vec2 ToVec2()const
			{
				return Vec2(x, y);
			}

			virtual ~V2()
			{
			}
		};
		struct V3
		{
			float x;
			float y;
			float z;

			V3()
			{
				x = y = z = 0;
			}

			V3(Vec3 v)
			{
				x = v.x;
				y = v.y;
				z = v.z;
			}

			V3(float _x , float _y, float _z)
			{
				x = _x;
				y = _y;
				z = _z;
			}

			Vec3 ToVec3()const
			{
				return Vec3(x, y, z);
			}

			virtual ~V3()
			{
			}

		};
		struct V4
		{
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;

			V4()
			{
				r = g = b = a = 0;
			}

			V4(Color4B v)
			{
				r = v.r;
				g = v.g;
				b = v.b;
				a = v.a;
			}

			V4(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
			{
				r = _r;
				g = _g;
				b = _b;
				a = _a;
			}
			virtual ~V4()
			{
			}

			Color4B ToColor()const 
			{
				return Color4B(r, g, b,a);
			}
		};

		class BasicDec
		{
		private:
    
			wMap<std::string, V4>			__name_color_;

			wMap<std::string, V2>			__name_vector_;

			wMap<std::string, V3>			__name_vector3_;

			wMap<std::string, std::string>	__name_string_;

			wMap<std::string, float>		__name_float_;

			wMap<std::string, int>			__name_int_;
		public:
			BasicDec();
			virtual ~BasicDec();
			void clearAllData();
			void operator=(const BasicDec & Ba);
			//clone
			BasicDec * Clone();
			//insert
			void InsertDataInt(std::string name, const int data) ;
			void InsertDatafloat(std::string name, const float data) ;
			void InsertDataChar(std::string name, const std::string data) ;
			void InsertDataVector2(std::string name, const Vec2 data) ;
			void InsertDataVector3(std::string name, const Vec3 data) ;
			void InsertDataColor(std::string name, const Color4B data) ;

			//check find data
			bool HaveDataInt(std::string name);
			bool HaveDatafloat(std::string name);
			bool HaveDataChar(std::string name);
			bool HaveDataVector2(std::string name);
			bool HaveDataVector3(std::string name);
			bool HaveDataColor(std::string name);

			//get data
			const int GetDataInt(std::string name) ;
			const float GetDatafloat(std::string name) ;
            const std::string GetDataChar(std::string name) ;
			const Vec2 GetDataVector2(std::string name) ;
			const Vec3 GetDataVector3(std::string name) ;
			const Color4B GetDataColor(std::string name) ;
			//replace 
			void ReplaceDataInt(std::string name, const int data) ;
			void ReplaceDatafloat(std::string name, const float data) ;
			void ReplaceDataChar(std::string name, const std::string data) ;
			void ReplaceDataVector2(std::string name, const Vec2 data) ;
			void ReplaceDataVector3(std::string name, const Vec3 data) ;
			void ReplaceDataColor(std::string name, const Color4B data) ;

			//get specific data
			std::map<std::string, std::string> GetListChar();
			std::map<std::string, float>	 GetListFloat();
			std::map<std::string, int>		 GetListInt();

		};


		/*

			use for load atlas texture

			*/

		struct AtlasChild
		{
			Vec2  Size;
			Vec2  Position;
			RKString nameSprite;
		public:
			AtlasChild();
			virtual ~AtlasChild();
			bool IsValidPosition();
		};

		struct AtlasTexture
		{
			std::map<RKString, AtlasChild > dDataSize;
			RKString NameImage;
			RKString NamePath;
		public:
			AtlasTexture();
			virtual ~AtlasTexture();
			AtlasChild GetAtlasChild(RKString str);		
			void InsertAtlasChild(RKString str, AtlasChild d);			
			int GetNumberAtlas();
			
		};
        
        struct TextPackerDetail
        {
            RKString texture_path;
            RKString texture_plist;
        public:
			TextPackerDetail();
			TextPackerDetail(RKString path, RKString plist);
			bool IsValid();
        };
        
        struct TextPacker
        {
            std::map<RKString , std::vector<TextPackerDetail>> p_list_texture_packer;
            Vec2 SizeFit;
            
        public:
			TextPacker();
			virtual ~TextPacker();
			TextPackerDetail GetTexPackByName(RKString str, int idx = 0);
			void InsertTexPack(RKString str, std::vector<TextPackerDetail> d);
			int GetNumberTextureByName(RKString name);
			int GetNumberTexPack();   
        };

		struct TextAtlasDetail
		{
			RKString				name_sub_fix;
			RKString				name_global_path;
			RKString				texture_atlas_path;
			std::vector<RKString>	list_texture_path;
		public:
			TextAtlasDetail();
			TextAtlasDetail(RKString _sub_fix , RKString path, RKString path_atlas, std::vector<RKString> list_texture_path);
			virtual ~TextAtlasDetail();
			bool IsValid();
		};

		struct zone_textAtlas_detail
		{
			std::vector<TextAtlasDetail>	list_texture;
			RKString subfix;
			RKString endfix;
		public:
			zone_textAtlas_detail();
			zone_textAtlas_detail(RKString _subfix, RKString _refix, std::vector<TextAtlasDetail> _list_texture);
			virtual ~zone_textAtlas_detail();
		};
        
		struct TextAtlas
		{
			std::map<RKString, zone_textAtlas_detail> p_list_texture;
			Vec2 SizeFit;

		public:
			TextAtlas();
			virtual ~TextAtlas();
			zone_textAtlas_detail GetTexAtlasByName(RKString str);
			void InsertTexAtlas(RKString str, zone_textAtlas_detail d);
			int GetNumberTexAtlasByName(RKString name);
			int GetNumberTexAtlas();
		};

		struct TexFrameDetail
		{
			RKString texture_path;
			xml::BasicDec value;
		public:
			TexFrameDetail();
			TexFrameDetail(RKString path, xml::BasicDec value);
			virtual ~TexFrameDetail();
		};

		struct TexFrame
		{
			std::map<RKString, TexFrameDetail> list_tex_frame_sheet;
		public:
			TexFrame();
			virtual ~TexFrame();
			TexFrameDetail GetTexFrameDetailByName(RKString str);
			void InsertTexFrameDetail(RKString str, TexFrameDetail d);
			int GetNumberTexFrameDetail();
		};

		struct ListTexFrame
		{
			std::map<RKString, TexFrame> list_tex_frame;
			Vec2 SizeFit;
		public:
			ListTexFrame();
			virtual ~ListTexFrame();
			TexFrame GetTexFrameByName(RKString str);
			void InsertTexFrame(RKString str, TexFrame d);
			int GetNumberTexFrame();
		};
		/*

		use for load font

		*/

		struct FontDec
		{
			int idx;

			float d_size;
			float d_width_border;
            RKUtils::BYTE d_color[4];
            RKUtils::BYTE d_color_border[4];

			RKString path;
			RKString name;
			bool IsUnicode;
			bool IsBitmap;
		public:
			FontDec();
			virtual ~FontDec();
		};


		/*

		use for load abstract basic object

		*/
		
		struct AnimatureDec
		{
			RKString name;
			RKString pathTexture;
			RKString pathPlist;
			RKString PathJson;

			float SpeedScale;
			int idxPlay;
		public:
			AnimatureDec();
			virtual ~AnimatureDec();		
		};

		struct SpriteDetail
		{
			RKString str_img;
			cocos2d::Rect rect_img;
		public:
			SpriteDetail(RKString _str_img, cocos2d::Rect _rect_img);
			SpriteDetail(RKString _str_img);
			SpriteDetail();
			~SpriteDetail();
		};

		struct AnimateDec
		{
			std::vector<SpriteDetail> p_SpriteFrame;
			float time_delay;
			int loop;
			int tag_action;
		public:
			AnimateDec();	
			virtual ~AnimateDec();
			void InsertSpriteFrameName(RKString name);
			void InsertSpriteFrameNameRect(RKString name, cocos2d::Rect);
			SpriteDetail GetSpriteFrameNameAtIdx(int idx);
			int GetNumberSprite();
		};

		struct ObjectDec
		{
			int idx;
			RKString name;
			//
			char *			name_source;
			AnimatureDec *	animature_dec;
			AnimateDec *	animation_dec;
			FontDec *		text_dec;
			//
			BasicDec * Move_delay_dec;
			//
			BasicDec * Position_dec;
			//
			BasicDec * Size_dec;
			//
			BasicDec * Opacity_dec;
			//
			BasicDec * Rotate_dec;
			//
			BasicDec * Rotate_around_dec;
			//
			BasicDec * Color_dec;
			//
			BasicDec * Process_dec;
			//
			int		   StickScreen;
		public:
			ObjectDec();
			virtual ~ObjectDec();

		};

		struct Wide_ObjectDec
		{
			RKString name_object;

			int type_object;
			//
			RKString		name_source;
			AnimatureDec *	animature_dec;
			AnimateDec *	animation_dec;
			FontDec *		text_dec;
			//
			BasicDec * OriginValue;
			BasicDec * FirstValue;
			BasicDec * CommonValue;
			//
			std::map<RKString, RKString> p_list_command;
			//
			Wide_ObjectDec *			 parent_object;
			std::vector<Wide_ObjectDec*> list_child_object;

			Vec2 design_size;

		public:
            Wide_ObjectDec();
            virtual ~Wide_ObjectDec();
            void InsertWideObjectChild(Wide_ObjectDec * child_object);
            const char* GetCommandByName(RKString name) const;
            void InsertCommand(RKString name, RKString command);
            unsigned int GetCommandSize();
            std::pair<RKString, RKString> GetIterCommandAtIdx(int idx);
		};

		/*

		use for load UI

		*/

		struct sound_click_dec
		{
			RKString id_click;
			float delayTime;
			int loop;
		public:
			sound_click_dec();
			sound_click_dec(sound_click_dec * value);
			virtual ~sound_click_dec();
		};

		struct time_process_ui
		{
			float time_process;
			RKString ID_SOUND_FADE_IN;
			RKString ID_SOUND_FADE_OUT;
			float delayTime;
			int loop;
		public:
			time_process_ui();
			virtual ~time_process_ui();
		};
		
		struct UILayerWidgetDec;
		struct UIMenuWidgetDec;

		struct UIWidgetDec
		{

			TYPE_UI_WIDGET typeValue;

			BasicDec * OriginValue;
			BasicDec * FirstValue;
			BasicDec * GeneralValue;
			float TimeProcessFade;
			//
			int index;
			int visible;
			int index_appear;

			sound_click_dec * sound_click;

			UILayerWidgetDec * p_parent_layer;
			UIMenuWidgetDec * p_menu_widget;

            std::map<std::string, std::string> p_list_command;
            std::vector<UI_STATE_FADE_ACTION> actionFade;
            std::string NameWidget;
		public:
			UIWidgetDec();
			virtual ~UIWidgetDec();
			void SetActionFade(UI_STATE_FADE_ACTION af);
			const char* GetCommandByName(std::string name);
			void InsertCommand(std::string name, std::string command);
			unsigned int GetCommandSize();
			std::pair<std::string, std::string> GetIterCommandAtIdx(int idx);
            void InitValue();

		};

		struct UILayerWidgetDec : public UIWidgetDec
		{
			std::vector<UILayerWidgetDec *> p_list_child_widget;

		public:
			UILayerWidgetDec();
			UILayerWidgetDec * Clone();		
			virtual ~UILayerWidgetDec();
			void InsertChildWidget(UILayerWidgetDec * widget);
			UILayerWidgetDec * GetChildWidgetByName(std::string name);
			bool IsTopLayerMenu();

			void RecorrectLayerParent(UILayerWidgetDec * parent_layer);
			void RecorrectMenuParent(UIMenuWidgetDec * menu_widget);
		};

		struct UIMenuWidgetDec
		{
			Vec2 DesignSize;
			int max_idx_first;

            std::string NameMenuWidget;
            
            std::vector<time_process_ui>			time_process;
            std::vector<std::string>					p_listChildElementByName;
			std::map<std::string, UILayerWidgetDec*>	p_listElementChild;
		public:
			UIMenuWidgetDec();
			UIMenuWidgetDec(std::string Name, Vec2 size);
			virtual ~UIMenuWidgetDec();
			UILayerWidgetDec * GetElementChildByName(std::string str);
			void InsertChildElement(std::string str, int idx, UILayerWidgetDec* e);
            std::vector<std::string> GetListChildName();
			void InsertNameToList(std::string name);
			time_process_ui GetTimeProcess(int idx);
			int GetNumberTimeProcess();
			
		};

		/*

			use for load background data

		*/

		struct LayerData
		{
			RKString p_NameLayer;
			int		p_index_layer;
			int     p_loopAction;


			Vec2  p_DistanceEachObject;
			Vec2  p_delta_distance_x;
			Vec2  p_delta_distance_y;
			//
			float p_TimeDelay;
			float p_processTime;
			Vec2  p_number_object;
			//

			bool  p_IsObjectPosRandom; // the pos each object in layer random in screen not squence together
			bool  p_IsScaleRandom;
			bool  p_IsRotateRandom;
			bool  p_IsOpacityRandom;
			bool  p_IsColorRandom;
			bool  p_IsProcessRandom;

			bool  p_isObjectRandom;

			int  p_idxAppearRandomOnEdgeBound;
			Vec2  p_TimeMoveRandom;
            cocos2d::Rect  p_BoundRandomPos;
			Vec2  p_randomScale;
			Vec2  p_randomRotate;
			Vec2  p_randomOpacity;
			Vec2  p_randomColor_r;
			Vec2  p_randomColor_g;
			Vec2  p_randomColor_b;
			Vec2  p_timeDelayRandom;

			Vec2  p_timeProcessRandom;
			Vec2  p_percentProcessRandom;

			Vec2  p_direct_move_random_x;
			Vec2  p_direct_move_random_y;

			std::map<RKString, ObjectDec *> p_listObject;

			std::map<RKString, BasicDec *>   p_list_layer_element_dec;
		public:

			LayerData();
			virtual ~LayerData();
			ObjectDec * GetObjectBGDecByName(RKString name);
			ObjectDec * GetObjectBGDecByIdx(int idx);
			void InsertObjectBGDec(RKString name, ObjectDec * val);
			int GetSizeObjDec();
			float GetRandomObject(Vec2 range, int div = 0);
			int GetNumberObject();
			int GetDistanceObject();
			float direct_x_Obj();
			float direct_y_Obj();
			float GetScaleRandom();
			float GetRotateRandom();
			RKUtils::BYTE GetOpacityRandom();
			Color4B GetColorRandom();
			cocos2d::Rect GetBoundRandomPos();
			float GetTimeMoveRandom();
			float GetDelayTimeRandom();
			Vec2 GetVectorMoveRandom();
			float GetPercentRandom();
			float GetTimeProcessRandom();
		};

		class Background
		{
		protected:
			std::map<RKString, LayerData*> p_ListObject;

			RKString p_Name;
		public:
			Background();
			~Background();

			LayerData * GetLayerData(RKString str);

			LayerData * GetLayerDataByIdx(int idx);

			void InsertLayerData(RKString str, LayerData * c);
			RKString GetNameBGLayer();
			int GetNumberLayer();
		};

		
		/*
			use for load sound dec
		*/

		struct SoundDec
		{
			RKString name_id;
			RKString path;
			float delay_first;
			int loop;
			RKString type_sound; //sound or music

			SoundDec();
			
			virtual ~SoundDec();
			
		};

		/*
		use for load language dec
		*/

		struct LangDec
		{
			RKString name_id;
			RKString path;
			RKString dec;

			LangDec();
			
			virtual ~LangDec();
			
		};

		//

		class LoadDataXML
		{
		private:

			static LoadDataXML *		 p_Instance;

			HARDWARD_PROFILE			 p_HProfile;
            
            TextPacker *                            p_texture_packer_xml;
			TextAtlas  *							p_texture_atlas_xml;
			ListTexFrame   *						p_texture_frame_xml;

			std::map<RKString, AtlasTexture *>		p_ListTextureXML;

			std::map<RKString, FontDec*>			p_list_font;

			std::map<RKString, Background*>			p_ListBackground;

			std::map<RKString, UIMenuWidgetDec*>	p_ListGameUIWidget;

			wMap<RKString, SoundDec *>				p_listSoundDec;

			std::map<RKString, Wide_ObjectDec *>	p_listWideObject;

			std::map<LANGUAGE_TYPE, LangDec *>		p_listLangDec;

			std::map<RKString, RKString>			p_xml_loader_config;

			std::vector<int>						p_table_screen_xml_load;

			template<class T>
			void OnLoadXML(std::function<void(T&, TiXmlDocument*)> func, T * refer, RKString path_xml)
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
					func(*refer, p_ObjectXML);				
				}
				else
				{
					PASSERT2(IsLoadXMLSucceed == true, "couldnot load xml loader correctly !");
				}
				delete p_ObjectXML;
				p_ObjectXML = NULL;
			}

			void OnLoadXML1(std::function<void(TiXmlDocument*)> func, RKString path_xml);
		public:

			static LoadDataXML *		 GetInstance()
			{
				if (!p_Instance)
				{
					p_Instance = new LoadDataXML();
				}
				return p_Instance;
			}
			LoadDataXML();
			virtual ~LoadDataXML();

			void OnLoaderXML();

			template<class T>
			void OnLoadXMLData(RKString name_loader, std::function<void(T&, TiXmlDocument*)> func, T * refer)
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
				OnLoadXML<T>(func, refer, TargetXML);
			}
			
			void OnLoadXMLData1(RKString name_loader, std::function<void(TiXmlDocument*)> func);
			
			bool OnParseValueIntoXMLDec(RKString name_value, RKString value, xml::BasicDec * data, xml::UILayerWidgetDec * _widget = nullptr);
			/*
			-----------------------------------------------------
			*/

			void OnLoadXMLLoader(TiXmlDocument * objectXMl);
			/*
			-----------------------------------------------------
			*/

            void OnLoadTexurePackerDecXML(TiXmlDocument * objectXMl);
			void OnLoadTextureAtlasDecXML(TiXmlDocument * objectXMl);
			TextPacker		* GetTexturePackerXML();
			TextAtlas		* GetTextureAtlasXML();
			ListTexFrame	* GetTextureFrameXML();

			void OnLoadTextureDecXML(TiXmlDocument * objectXMl);
			AtlasTexture * GetAtlastTextureByName(RKString name);
			AtlasTexture * GetAtlasTextureByIdx(int idx);
			int	GetNumberTexture() { return (int)p_ListTextureXML.size(); }

			void InsertFontToList(RKString name, FontDec* ddc);
			FontDec* GetFontDecByName(RKString name);
			FontDec* GetFontDecByIdx(int idx);
			std::map<RKString, FontDec*> GetListFontDec() { return p_list_font; }
			int GetNumberFont() { return (int)p_list_font.size(); }
			/*
			-----------------------------------------------------
			*/
			Background * GetBackground(RKString str);
			void InsertBackground(RKString str, Background * c);
			void OnLoadBackgroundDecXML(TiXmlDocument * objectXMl);
			/*
			-----------------------------------------------------
			*/
			std::vector<int>	GetTableXMLLoad();

			UIMenuWidgetDec * GetUIWidgetByName(RKString str);
			void ReloadUIMEnuWidgetDec(RKString str, RKString path);
			void InsertUIMenuWidgetDec(RKString str, UIMenuWidgetDec * c);
			void RemoveUIMEnuWidgetDec(RKString str);
			void OnLoadUIWidgetDecXML(TiXmlDocument * objectXMl);
			bool OnLoadUIWidgetDecTableXML(TiXmlDocument * objectXMl, RKString name_table);
			UIMenuWidgetDec * OnLoadUIWidgetDetailXML(TiXmlDocument * objectXMl);
			UILayerWidgetDec * OnParseWidgetEntity(TiXmlElement * element_xml, UILayerWidgetDec* parent_widget, UIMenuWidgetDec * UIDec, sound_click_dec * global_id_sound_click);
			TYPE_UI_WIDGET GetUIWidget(RKString name);
			/*
			-----------------------------------------------------
			*/
			Wide_ObjectDec * GetWideObjectByName(RKString str);
			void InsertWideObjectDec(RKString str, Wide_ObjectDec * c);
			Wide_ObjectDec * OnParseObjectWide(TiXmlElement * element_xml, Wide_ObjectDec* parent_object , Vec2 design_size);
			void OnLoadListWideObjectDecXML(TiXmlDocument * objectXMl);
			void ReleaseAllListWideObjectDecXML();
			/*
			-----------------------------------------------------
			*/
			SoundDec * GetSoundDecByName(RKString str);
			void InsertSoundDec(RKString str, SoundDec * c);
			void OnLoadListSoundDecXML(TiXmlDocument * objectXMl);
			/*
			-----------------------------------------------------
			*/
			LangDec * GetLangDecByType(LANGUAGE_TYPE type);
			void InsertLangDec(LANGUAGE_TYPE type, LangDec * c);
			void OnLoadListLangXML(TiXmlDocument * objectXMl);

			HARDWARD_PROFILE GetHarwardProfile();

			Vec2 ConvertToVector2(RKString str);
			Vec3 ConvertToVector3(RKString str);
			Color4B ConvertToColor(RKString str);
			cocos2d::Rect ConvertToRect(RKString str);

			BasicDec * GenerateBasicDec(tinyxml2::XMLElement * element, char * name0, ...);
            
            bool IsValueNumber(RKString str);
		private:
			void InsertTextureXML(RKString str, AtlasTexture * te);
		};

	}

    #define XMLMgr xml::LoadDataXML::GetInstance()
}
#endif
