#ifndef __TEXT_WIDGET_H__
#define __TEXT_WIDGET_H__

#include "WidgetEntity.h"
#include "RKString_Code/RKString.h"
#include "2d/CCActionInterval.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
		class NumberToEffect : public ActionInterval
		{
		public:
			static NumberToEffect* create(float duration, long long numberTo);
			//
			// Overrides
			//
			virtual NumberToEffect* clone() const override;
			virtual NumberToEffect* reverse(void) const override;
			/**
			* @param time In seconds.
			*/
			virtual void update(float time) override;

			void SetFunctionConvert(const std::function<std::string(long long)> & func);
			void SetStartNumber(long long number);
		CC_CONSTRUCTOR_ACCESS:
			NumberToEffect();
			virtual ~NumberToEffect();

			/** initializes the action with duration and color */
			bool initWithDuration(float duration, long long numberTo);

		protected:
			long long _to;
			long long _from;
			long long _current_number;
			std::function<std::string(long long)> _function_convert;
		private:
			CC_DISALLOW_COPY_AND_ASSIGN(NumberToEffect);
		};

		class TextWidget : public WidgetEntity
		{
		protected:
			int			p_border_width;
			Color4B		p_border_color;
			bool		p_size_use_scale;

			bool		p_use_bmp_text;
			int			p_size_font_bmp;
			int			p_use_origin_size;
			Vec2		p_size_area;

			RKString	p_current_text;
			void		SetText();

			bool		p_isLinebreak;
		public:
			TextWidget();
			virtual ~TextWidget();

			virtual void SetSize(Vec2 s)												override;
			virtual void InitParam(RKString name, xml::UILayerWidgetDec * xml_value)	override;
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value)					override;
			virtual void SetColor(Color3B p)											override;
			virtual WidgetEntity * Clone()												override;
			virtual void CopyValue(WidgetEntity * value)								override;
			virtual void ReloadText()													override;

			RKString GetText();
			void	SetText(RKString text , bool force = false);

			void	TextRunEffect(	long long to_number, 
									long long current_number, 
									float time, std::function<std::string(long long)> func = nullptr, 
									CallFunc * call_back = nullptr);
			Vec2	GetTextAreaSize();
			void	SetTextAreaSize(Vec2 size);

			void	SetLineBreakWithoutSpace(bool value);
			int		GetNumberOfLine();
			float	GetFontSize();

			Vec2	GetActualContainSize();
		};

	}
}

#endif //__TEXT_WIDGET_H__

