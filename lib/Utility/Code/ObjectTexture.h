#ifndef _OBJECT_TEXTURE_H_
#define _OBJECT_TEXTURE_H_

#include "math/CCGeometry.h"
#include "base/CCVector.h"
#include "cocos2d.h"
#include "renderer/CCTexture2D.h"
#include "LoadDataXML.h"
USING_NS_CC;
#define USE_SPRITE_STORE 1
using namespace RKUtils;
namespace Utility
{
	namespace Texture
	{
		struct LabelDetail
		{
			Label		* label;
			int			  idx;
		public:
			LabelDetail()
			{
				label = NULL;
				idx = 0;
			}
			LabelDetail(Label* l, int id)
			{
				label = l;
				idx = id;
			}
			virtual ~LabelDetail()
			{
				if (label)
				{
					delete label;
					label = NULL;
				}
				idx = -1;
			}
		};
		
		struct TextBMPDetail
		{
			RKString path_fnt;
			int		 size_font;
			int		 idx;
			TextBMPDetail()
			{
				path_fnt = "";
				size_font = 1;
				idx = -1;
			}
		};

		class ObjectTexture
		{
		public:
			void Load(bool loadAA = false);
			void LoadTexturePacker(RKString name, bool loadAA = false);
			void LoadTextureAtlas(RKString name, bool loadAA = false);
			void LoadTextureFrame(RKString name, bool loadAA = false);
			void LoadFont();
			void UnloadData();

			Texture2D	*	GetTextureByName(RKString nameImage);
			Sprite		*	GetSpriteByName(RKString nameImage);
			Label		*	GetFontByName(RKString nameImage);
			Label		*	GetFontByIdx(int idx);

			TextBMPDetail * GetFontBmpByName(RKString nameFont);
			TextBMPDetail *	GetFontBmpByIdx(int idx);

			Texture2D * CreateTextureFromNamePath(RKString namepath, bool loadAA = false, int levelMipMap = 0);
			Sprite *	CreateSpriteFromTexture(Texture2D * tex, xml::AtlasChild* nameSprite, int levelMipMap = 0);
		protected:
			

			void InsertTextureByName(RKString name, Texture2D * tex);
			void InsertSpriteByName(RKString name, Sprite * tex);
			void InsertFontByName(RKString name, LabelDetail * tex);

			void ReadDataFromAtlasPath(RKString path_atlas, 
				RKString subfix, 
				RKString endfix, 
				RKString name_fix,
				RKString global_path_tex);
		private:

			//texture
			std::map<RKString, Texture2D *>			p_listTexture;
			
			//label for font
			std::map<RKString, LabelDetail * >		p_listFont;

			//text bmp for font
			std::map<RKString, TextBMPDetail *>		p_listFontBmp;
			
			//sprite
			std::map<RKString, Sprite *>			p_listSprite;

			static ObjectTexture * p_Instance;

			int	p_levelMipMap;
		public:
			static ObjectTexture * GetInstance()
			{
				if (!p_Instance)
				{
					p_Instance = new ObjectTexture();
				}
				return p_Instance;
			}
			ObjectTexture();
			virtual ~ObjectTexture();

			int GetLevelMipMap() { return p_levelMipMap; }

		};
	}
#define TextureMgr Texture::ObjectTexture::GetInstance()

}
#endif