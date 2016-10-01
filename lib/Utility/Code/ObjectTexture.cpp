#include "DisplayView.h"
#include "ObjectTexture.h"

namespace Utility
{
	namespace Texture
	{
		ObjectTexture * ObjectTexture::p_Instance = NULL;
		ObjectTexture::ObjectTexture()
		{
			p_levelMipMap = 0;
		}

		///////////////////////////////////////////////////////////////////////////////
		ObjectTexture::~ObjectTexture()
		{
			UnloadData();
		}
		
		///////////////////////////////////////////////////////////////////////////////
		Texture2D * ObjectTexture::GetTextureByName(RKString nameTexture)
		{
			if (p_listTexture.size() > 0)
			{
				if (p_listTexture.find(nameTexture) != p_listTexture.end())
				{
					return p_listTexture.at(nameTexture);
				}
			}
			return nullptr;
		}
		
		///////////////////////////////////////////////////////////////////////////////
		Sprite * ObjectTexture::GetSpriteByName(RKString nameSprite)
		{
#ifdef USE_SPRITE_FRAME
			Sprite * p = nullptr;
			p = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(nameSprite.GetString()));
			return p;
#else
			if (p_listSprite.size() > 0)
			{
				if (p_listSprite.find(nameSprite) != p_listSprite.end())
				{
					return p_listSprite.at(nameSprite);
				}
			}

			return 0;
#endif
			
		}
		
		///////////////////////////////////////////////////////////////////////////////
		Label * ObjectTexture::GetFontByName(RKString nameFont)
		{
			if (p_listFont.size() > 0)
			{
				if (p_listFont.find(nameFont) != p_listFont.end())
				{
					return p_listFont.at(nameFont)->label;
				}
			}
			return 0;
		}
		Label *	ObjectTexture::GetFontByIdx(int idx)
		{
			if (p_listFont.size() > 0)
			{
				for (auto iterator = p_listFont.begin(); iterator != p_listFont.end(); ++iterator)
				{
					if (iterator->second->idx == idx)
					{
						return iterator->second->label;
					}
				}
			}
			return 0;
		}
		///////////////////////////////////////////////////////////////////////////////
		void ObjectTexture::InsertTextureByName(RKString name, Texture2D * tex)
		{
			p_listTexture.insert(std::pair<RKString, Texture2D *>(name, tex));
		}
		
		///////////////////////////////////////////////////////////////////////////////
		void ObjectTexture::InsertSpriteByName(RKString name, Sprite * tex)
		{
			p_listSprite.insert(std::pair<RKString, Sprite *>(name, tex));
		}
		
		///////////////////////////////////////////////////////////////////////////////
		void ObjectTexture::InsertFontByName(RKString name, LabelDetail * tex)
		{
			p_listFont.insert(std::pair<RKString, LabelDetail *>(name, tex));
		}

		///////////////////////////////////////////////////////////////////////////////
		void ObjectTexture::UnloadData()
		{
			p_listTexture.clear();
			p_listSprite.clear();
			p_listFont.clear();
			p_listFontBmp.clear();
		}

		///////////////////////////////////////////////////////////////////////////////
		Texture2D * ObjectTexture::CreateTextureFromNamePath(RKString namepath, bool loadAA , int levelMipMap)
		{
			//create null image
			Image * p_AtlatSpriteObjectTemp = NULL;

			//create texture null
			Texture2D * p_tex = new Texture2D();

			//init image from namepath
			p_AtlatSpriteObjectTemp = new Image();
			p_AtlatSpriteObjectTemp->initWithImageFile(namepath.GetString());
			//
			if (p_AtlatSpriteObjectTemp->getNumberOfMipmaps() > 1)
			{
				int w_mip = p_AtlatSpriteObjectTemp->getWidth() >> levelMipMap;
				int h_mip = p_AtlatSpriteObjectTemp->getHeight() >> levelMipMap;
				//
				p_tex->initWithData(
					p_AtlatSpriteObjectTemp->getMipmaps()[levelMipMap].address, 
					p_AtlatSpriteObjectTemp->getMipmaps()[levelMipMap].len,
					p_AtlatSpriteObjectTemp->getRenderFormat(), 
					w_mip,
					h_mip,
                                    cocos2d::Size(w_mip, h_mip)
					);
			}
			else
			{
				p_tex->initWithImage(p_AtlatSpriteObjectTemp);				
			}

#ifdef USE_ANTIALIAS
			if (loadAA)
			{
				p_tex->generateMipmap();
				Texture2D::TexParams texParams = { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE };
				p_tex->setTexParameters(texParams);
			}
#endif

			CC_SAFE_DELETE(p_AtlatSpriteObjectTemp);

			return p_tex;
		}
		
		///////////////////////////////////////////////////////////////////////////////
		Sprite * ObjectTexture::CreateSpriteFromTexture(Texture2D * tex, xml::AtlasChild* nameSprite, int levelMipMap)
		{
			cocos2d::Rect bound;
			bound.setRect(
				float((int)nameSprite->Position.x >> levelMipMap),
				float((int)nameSprite->Position.y >> levelMipMap),
				float((int)nameSprite->Size.x >> levelMipMap),
				float((int)nameSprite->Size.y >> levelMipMap)
				);

			auto sprite = Sprite::createWithTexture(tex, bound);

			return sprite;

		}

		void ObjectTexture::LoadTextureAtlas(RKString name, bool loadAA)
		{
			bool IsUseArk = false;
#ifdef USE_ARK_ARCHIVE
			IsUseArk = true;
			g_IsUseArk = IsUseArk;
#endif
			if (XMLMgr->GetHarwardProfile() == xml::HARDWARE_HIGH || XMLMgr->GetHarwardProfile() == xml::HARDWARE_MED)
			{
				p_levelMipMap = 0;
			}
			else
			{
				p_levelMipMap = 1;
			}

			auto zone_tex_atlas = XMLMgr->GetTextureAtlasXML()->GetTexAtlasByName(name);

			for (auto tex_detail : zone_tex_atlas.list_texture)
			{
				if (tex_detail.IsValid())
				{
					RKString name_fix = tex_detail.name_sub_fix;
                    RKString atlas_file_path = tex_detail.texture_atlas_path;
					RKString name_path = tex_detail.name_global_path;
					ReadDataFromAtlasPath(atlas_file_path, zone_tex_atlas.subfix, zone_tex_atlas.endfix, name_fix, name_path);
				}
			}

		}

		void ObjectTexture::LoadTextureFrame(RKString name, bool loadAA )
		{
			bool IsUseArk = false;
#ifdef USE_ARK_ARCHIVE
			IsUseArk = true;
			g_IsUseArk = IsUseArk;
#endif
			if (XMLMgr->GetHarwardProfile() == xml::HARDWARE_HIGH || XMLMgr->GetHarwardProfile() == xml::HARDWARE_MED)
			{
				p_levelMipMap = 0;
			}
			else
			{
				p_levelMipMap = 1;
			}

			auto tex_frame = XMLMgr->GetTextureFrameXML()->GetTexFrameByName(name);

			Texture2D * tex = nullptr;
			for (auto it : tex_frame.list_tex_frame_sheet)
			{
				RKString name_frame = it.first;
				auto framedetail = it.second;
				RKString name_tex = framedetail.texture_path;

				if (GetTextureByName(name_tex) == nullptr)
				{
					tex = CreateTextureFromNamePath(File::ResolveNamePath(name_tex), false, 0);
					InsertTextureByName(name_tex, tex);
				}

				Vec2 begin = framedetail.value.GetDataVector2("begin");
				Vec2 end = framedetail.value.GetDataVector2("end");
				if (end.x < 0)
				{
					end.x = p_listTexture.at(name_tex)->getPixelsWide();
				}
				if (end.y < 0)
				{
					end.y = p_listTexture.at(name_tex)->getPixelsHigh();
				}
				cocos2d::Size size = cocos2d::Size(end.x - begin.x, end.y - begin.y);

				SpriteFrame * fr = SpriteFrame::createWithTexture(
					p_listTexture.at(name_tex),
					cocos2d::Rect(begin.x, begin.y, size.width, size.height)
				);

				SpriteFrameCache::getInstance()->addSpriteFrame(fr, name_frame.GetString());
			}
		}

		void ObjectTexture::ReadDataFromAtlasPath(
			RKString path, 
			RKString subfix, 
			RKString endfix , 
			RKString name_fix,
			RKString global_path_tex)
		{
			const char * data = File::GetDataFromFile(path);
			if (data == nullptr)
			{
				PASSERT2(data, "file not found or data couldnot read!");
			}

			//int idx_texture = 0;
			auto list_template = RKString(data).Split("\n");
			Texture2D * tex = nullptr;
			RKString current_tex_name = "";
			for (size_t i = 0; i < list_template.Size();)
			{      
				RKString detail_str = list_template.GetAt(i);
                detail_str.ReplaceAll("\r", "");
				if (detail_str.Contains(".png")&& detail_str!= "")
				{
					RKString tex_path = File::ResolveNamePath(global_path_tex + detail_str);
					tex = CreateTextureFromNamePath(tex_path, false, 0);
					current_tex_name = detail_str;
					InsertTextureByName(detail_str, tex);
					i += 4;
					//idx_texture++;
				}
				else if (!detail_str.Contains(":") && detail_str!= "")
				{
                    i += 1;
					RKString step = list_template.GetAt(i);
                    bool rotate = false;
                    if(step.Contains("rotate"))
                    {
                        auto t = step.Split(":");
                        PASSERT2(t.Size() > 1 , "have problem");
                        rotate = (t.GetAt(1) == "true");
                    }
                    else
                    {
                        PASSERT2(false , "have problem");
                        i--;
                        continue;
                    }

					step = list_template.GetAt(i += 1);
					Vec2 pos_xy = [step]() -> Vec2 {
						Vec2 pos(0, 0);
						RKString pos_str = step.Split(":").GetAt(1);
						auto split = pos_str.Split(",");
						pos.x = atof(split.GetAt(0).GetString());
						pos.y = atof(split.GetAt(1).GetString());
						return pos;
					}();

					step = (list_template.GetAt(i += 1));
					Vec2 size = [step]() -> Vec2 {
						Vec2 pos(0, 0);
						RKString pos_str = step.Split(":").GetAt(1);
						auto split = pos_str.Split(",");
						pos.x = atof(split.GetAt(0).GetString());
						pos.y = atof(split.GetAt(1).GetString());
						return pos;
					}();

					cocos2d::Rect split(0, 0, 0, 0);
					cocos2d::Rect pad(0, 0, 0, 0);

					bool is_nine_path_texture = false;
					bool have_pad = false;
					step = (list_template.GetAt(i += 1));
					if (step.Contains("split"))
					{
						split = [step]() -> cocos2d::Rect {
							cocos2d::Rect value(0, 0, 0, 0);
							RKString value_str = step.Split(":").GetAt(1);
							auto split = value_str.Split(",");
							value.origin.x = atof(split.GetAt(0).GetString());
							value.origin.y = atof(split.GetAt(1).GetString());
							value.size.width = atof(split.GetAt(2).GetString());
							value.size.height = atof(split.GetAt(3).GetString());
							return value;
						}();
						is_nine_path_texture = true;
					}
					else
					{
						i -= 1;
					}

					step = (list_template.GetAt(i += 1));
					if (step.Contains("pad"))
					{
						pad = [step]() -> cocos2d::Rect {
							cocos2d::Rect value(0, 0, 0, 0);
							RKString value_str = step.Split(":").GetAt(1);
							auto split = value_str.Split(",");
							value.origin.x = atof(split.GetAt(0).GetString());
							value.origin.y = atof(split.GetAt(1).GetString());
							value.size.width = atof(split.GetAt(2).GetString());
							value.size.height = atof(split.GetAt(3).GetString());
							return value;
						}();
						have_pad = true;
					}
					else
					{
						i -= 1;
					}

					step = (list_template.GetAt(i += 1));
					Vec2 origin = [step]() -> Vec2 {
						Vec2 pos(0, 0);
						RKString pos_str = step.Split(":").GetAt(1);
						auto split = pos_str.Split(",");
						pos.x = atof(split.GetAt(0).GetString());
						pos.y = atof(split.GetAt(1).GetString());
						return pos;
					}();

					step = (list_template.GetAt(i += 1));
					Vec2 offset = [step]() -> Vec2 {
						Vec2 pos(0, 0);
						RKString pos_str = step.Split(":").GetAt(1);
						auto split = pos_str.Split(",");
						pos.x = atof(split.GetAt(0).GetString());
						pos.y = atof(split.GetAt(1).GetString());
						return pos;
					}();

					PASSERT2(current_tex_name != "", "ERROR ! some thing wrong in file struct !");

					SpriteFrame * fr = SpriteFrame::createWithTexture(
						p_listTexture.at(current_tex_name),
						cocos2d::Rect(pos_xy.x, pos_xy.y, size.x, size.y),
						rotate,
						offset,
						cocos2d::Size(origin.x, origin.y)
					);

					if (is_nine_path_texture)
					{
						if (!detail_str.Contains(".9"))
						{
							detail_str = detail_str + ".9";
						}
					}
					RKString full_name = subfix + name_fix + detail_str + endfix;
					if (full_name == "default-select.9.png")
					{
						int ttt = 0;
					}
					if (is_nine_path_texture)
					{
						cocos2d::Rect cap_insert = cocos2d::Rect(
							split.origin.x, split.size.width,
							
							abs((size.x - split.origin.y) - split.origin.x), abs((size.y - split.size.height) - split.size.width)
						);

						if (have_pad && (pad.origin.x < 0 || pad.origin.y < 0) )
						{
							if (rotate)
							{
								cap_insert.origin.x = pad.size.width;
								cap_insert.size.width = size.x + pad.origin.x;
							}
							else
							{
								cap_insert.origin.y = pad.size.height;
								cap_insert.size.height = size.y + pad.origin.y ;
							}
						}

						fr->SetCapInsert(cap_insert);
					}
					SpriteFrameCache::getInstance()->addSpriteFrame(fr, full_name.GetString());

					i += 1;
				}
				else
				{
					i++;
				}
			}

			delete []data;
			data = nullptr;

		}

		void ObjectTexture::LoadTexturePacker(RKString name, bool loadAA)
		{
			bool IsUseArk = false;
#ifdef USE_ARK_ARCHIVE
			IsUseArk = true;
			g_IsUseArk = IsUseArk;
#endif
			if (XMLMgr->GetHarwardProfile() == xml::HARDWARE_HIGH || XMLMgr->GetHarwardProfile() == xml::HARDWARE_MED)
			{
				p_levelMipMap = 0;
			}
			else
			{
				p_levelMipMap = 1;
			}
			RKString texture_path = "";
			RKString plist_path = "";

			int number_texture = XMLMgr->GetTexturePackerXML()->GetNumberTextureByName(name);
			while (number_texture > 0)
			{
				auto tex_pack = XMLMgr->GetTexturePackerXML()->GetTexPackByName(name , number_texture - 1);

				if (tex_pack.IsValid())
				{
					texture_path = File::ResolveNamePath(tex_pack.texture_path);
					plist_path = File::ResolveNamePath(tex_pack.texture_plist);
				}
				if (texture_path != "")
				{
					cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist_path.GetString(), texture_path.GetString());
				}

				number_texture--;
			}
		

		}

		///////////////////////////////////////////////////////////////////////////////
		void ObjectTexture::Load(bool loadAA)
		{
			bool IsUseArk = false;
#ifdef USE_ARK_ARCHIVE
			IsUseArk = true;
			g_IsUseArk = IsUseArk;
#endif
			if (XMLMgr->GetHarwardProfile() == xml::HARDWARE_HIGH || XMLMgr->GetHarwardProfile() == xml::HARDWARE_MED)
			{
				p_levelMipMap = 0;
			}
			else
			{
				p_levelMipMap = 1;
			}

			//loop through all the texture data store in xml
			int number_image = XMLMgr->GetNumberTexture();
			while (number_image > 0)
			{
				number_image--;
				//process each image
				auto textureXml = XMLMgr->GetAtlasTextureByIdx(number_image);

				RKString namepath = textureXml->NamePath;
                
				if (p_levelMipMap > 0 && namepath.Contains(".png"))
				{
					namepath = File::GenerateMipName(namepath);
				}
				RKString ImagePath = File::ResolveNamePath(namepath);

				// create texture and insert to game
				auto tex = CreateTextureFromNamePath(ImagePath, loadAA, p_levelMipMap);
				InsertTextureByName(textureXml->NameImage, tex);

				//create list sprite from tex and insert to game
				for (auto it = textureXml->dDataSize.begin(); it != textureXml->dDataSize.end(); it++)
				{
					auto sprite = CreateSpriteFromTexture(tex, &it->second, p_levelMipMap);
#ifdef USE_SPRITE_FRAME
					//store into the sprite frame
					SpriteFrame * sp = SpriteFrame::createWithTexture(sprite->getTexture(), sprite->getTextureRect());
					SpriteFrameCache::getInstance()->addSpriteFrame(sp, it->first.GetString());
#else
                    //store into the sprite frame
                    SpriteFrame * sp = SpriteFrame::createWithTexture(sprite->getTexture(), sprite->getTextureRect());
                    SpriteFrameCache::getInstance()->addSpriteFrame(sp, it->first.GetString());
                    
					sprite->retain();
					InsertSpriteByName(it->first, sprite);
#endif
				}
				//
				tex = NULL;
			}

			//
			//
			//load font
			//
			p_listFont.clear();
			size_t numberfont = XMLMgr->GetNumberFont();
			for (size_t i = 0; i < numberfont; i++)
			{
				auto font_dec = XMLMgr->GetFontDecByIdx((int)i);
				Label* string = NULL;
				string = Label::createWithTTF(						
						"",
						File::ResolveNamePath(font_dec->path).GetString(),
						font_dec->d_size
						);
				string->setColor(Color3B(font_dec->d_color[0], font_dec->d_color[1], font_dec->d_color[2]));
				string->setOpacity(font_dec->d_color[3]);
				if (font_dec->d_width_border > 0)
				{
					string->enableOutline(Color4B(font_dec->d_color_border[0], font_dec->d_color_border[1], font_dec->d_color_border[2], font_dec->d_color_border[3]), font_dec->d_width_border);
				}
				string->retain();
				LabelDetail * l_de = new LabelDetail(string, font_dec->idx);

				InsertFontByName(font_dec->name, l_de);
			}
#ifdef USE_ARK_ARCHIVE
			IsUseArk = false;
			g_IsUseArk = IsUseArk;
#endif
		}

		///////////////////////////////////////////////////////////////////////////////
		void ObjectTexture::LoadFont()
		{
			p_listFont.clear();
			auto font_dec = XMLMgr->GetListFontDec();
			for (auto it = font_dec.begin(); it != font_dec.end(); ++it)
			{
				auto font_dec = it->second;

				if (font_dec->IsBitmap)
				{
					TextBMPDetail * font_bmp_detail = new TextBMPDetail();
					font_bmp_detail->idx = font_dec->idx;
					font_bmp_detail->path_fnt = font_dec->path;
					font_bmp_detail->size_font = font_dec->d_size;

					p_listFontBmp.insert(std::pair<RKString, TextBMPDetail*>(font_dec->name, font_bmp_detail));

				}
				else
				{
					Label* string = Label::createWithTTF(
						"",
						File::ResolveNamePath(font_dec->path).GetString(),
						font_dec->d_size
					);
					string->setColor(Color3B(font_dec->d_color[0], font_dec->d_color[1], font_dec->d_color[2]));
					string->setOpacity(font_dec->d_color[3]);
					if (font_dec->d_width_border > 0)
					{
						string->enableOutline(Color4B(font_dec->d_color_border[0], font_dec->d_color_border[1], font_dec->d_color_border[2], font_dec->d_color_border[3]), font_dec->d_width_border);
					}
					string->retain();
					LabelDetail * l_de = new LabelDetail(string, font_dec->idx);

					InsertFontByName(font_dec->name, l_de);
				}
			}
		}
		///////////////////////////////////////////////////////////////////////////////
		TextBMPDetail * ObjectTexture::GetFontBmpByName(RKString nameFont)
		{
			if (p_listFontBmp.size() > 0)
			{
				if (p_listFontBmp.find(nameFont) != p_listFontBmp.end())
				{
					return p_listFontBmp.at(nameFont);
				}
			}
			return 0;
		}
		TextBMPDetail *	ObjectTexture::GetFontBmpByIdx(int idx)
		{
			if (p_listFontBmp.size() > 0)
			{
				for (auto iterator = p_listFontBmp.begin(); iterator != p_listFontBmp.end(); ++iterator)
				{
					if (iterator->second->idx == idx)
					{
						return iterator->second;
					}
				}
			}
			return 0;
		}

	}
}