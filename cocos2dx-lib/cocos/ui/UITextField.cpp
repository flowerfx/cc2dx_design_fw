/****************************************************************************
Copyright (c) 2013-2014 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "ui/UITextField.h"
#include "platform/CCFileUtils.h"
#include "ui/UIHelper.h"
#include "base/ccUTF8.h"
#include "2d/CCCamera.h"

NS_CC_BEGIN

namespace ui {

UICCTextField::UICCTextField()
: _maxLengthEnabled(false)
, _maxLength(0)
, _attachWithIME(false)
, _detachWithIME(false)
, _insertText(false)
, _deleteBackward(false)
{
	p_current_type_text = TYPE_TEXT_FIELD::TEXT_AND_NUMBER;
	_text_area = Vec2(0, 0);
}

UICCTextField::~UICCTextField()
{
}

UICCTextField * UICCTextField::create(const std::string& placeholder, const std::string& fontName, float fontSize)
{
    UICCTextField *pRet = new (std::nothrow) UICCTextField();
    
    if(pRet && pRet->initWithPlaceHolder("", fontName, fontSize))
    {
        pRet->autorelease();
        if (!placeholder.empty())
        {
            pRet->setPlaceHolder(placeholder);
        }
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    
    return nullptr;
}

void UICCTextField::onEnter()
{
    TextFieldTTF::onEnter();
    TextFieldTTF::setDelegate(this);
}


bool UICCTextField::onTextFieldAttachWithIME(TextFieldTTF *pSender)
{
    setAttachWithIME(true);
    return false;
}

bool UICCTextField::onTextFieldInsertText(TextFieldTTF *pSender, const char *text, size_t nLen)
{
    if (nLen == 1 && strcmp(text, "\n") == 0)
    {
        return false;
    }
    setInsertText(true);
    if (_maxLengthEnabled)
    {
        if (static_cast<int>(TextFieldTTF::getCharCount()) >= _maxLength)
        {
            return true;
        }
    }
    
    return false;
}

bool UICCTextField::onTextFieldDeleteBackward(TextFieldTTF *pSender, const char *delText, size_t nLen)
{
    setDeleteBackward(true);
    return false;
}

bool UICCTextField::onTextFieldDetachWithIME(TextFieldTTF *pSender)
{
    setDetachWithIME(true);
    return false;
}

void UICCTextField::insertText(const char*  text, size_t len)
{
    std::string input_text = text;
    
    if (strcmp(text, "\n") != 0)
    {
        if (_maxLengthEnabled)
        {
            long text_count = StringUtils::getCharacterCountInUTF8String(getString());
            if (text_count >= _maxLength)
            {
                // password
                if (this->isSecureTextEntry())
                {
                    setPasswordText(getString());
                }
                return;
            }
            
            long input_count = StringUtils::getCharacterCountInUTF8String(text);
            long total = text_count + input_count;
            
            if (total > _maxLength)
            {
                long length = _maxLength - text_count;
                
                input_text = Helper::getSubStringOfUTF8String(input_text, 0, length);
                len  = input_text.length();
            }
        }
    }

	input_text = GetValidString(input_text);

	if (input_text != "")
	{
		TextFieldTTF::insertText(input_text.c_str(), len);
	}

    // password
    if (this->isSecureTextEntry())
    {
        if (TextFieldTTF::getCharCount() > 0)
        {
            setPasswordText(getString());
        }
    }

	if (_text_area != Vec2(0, 0))
	{
		std::string current_string = getString();
		std::string current_store_text = getStoreText();


		std::u16string utf16_current_string;
		std::u16string utf16_current_store_text;

		StringUtils::UTF8ToUTF16(current_string, utf16_current_string);
		StringUtils::UTF8ToUTF16(current_store_text, utf16_current_store_text);

		do
		{
			if (this->getContentSize().width >= _text_area.x)
			{
				utf16_current_store_text += utf16_current_string[0];
				utf16_current_string.erase(utf16_current_string.begin());

				StringUtils::UTF16ToUTF8(utf16_current_string, current_string);
				StringUtils::UTF16ToUTF8(utf16_current_store_text, current_store_text);

				setStoreText(current_store_text);
				setString(current_string);
			}
			else
			{
				break;
			}
		} while (true);
		
	}
}

void UICCTextField::deleteBackward()
{
    TextFieldTTF::deleteBackward();
    
    if (TextFieldTTF::getCharCount() > 0)
    {
        // password
        if (this->isSecureTextEntry())
        {
            setPasswordText(_inputText);
        }
    }


	if (_text_area != Vec2(0, 0))
	{
		std::string current_string = getString();
		std::string current_store_text = getStoreText();

		std::u16string utf16_current_string ;
		std::u16string utf16_current_store_text ;

		StringUtils::UTF8ToUTF16(current_string, utf16_current_string);
		StringUtils::UTF8ToUTF16(current_store_text, utf16_current_store_text);
		do
		{
			if (utf16_current_store_text.length() > 0)
			{
				utf16_current_string = utf16_current_store_text[utf16_current_store_text.length() - 1] + utf16_current_string;
				utf16_current_store_text.erase(utf16_current_store_text.length() - 1);

				StringUtils::UTF16ToUTF8(utf16_current_string, current_string);
				StringUtils::UTF16ToUTF8(utf16_current_store_text, current_store_text);

				setStoreText(current_store_text);
				setString(current_string);
			}
		} while (false);
	}
}

void UICCTextField::openIME()
{
    TextFieldTTF::attachWithIME();
}

void UICCTextField::closeIME()
{
    TextFieldTTF::detachWithIME();
}

void UICCTextField::setMaxLengthEnabled(bool enable)
{
    _maxLengthEnabled = enable;
}

bool UICCTextField::isMaxLengthEnabled()const
{
    return _maxLengthEnabled;
}

void UICCTextField::setMaxLength(int length)
{
    _maxLength = length;
}

int UICCTextField::getMaxLength()const
{
    return _maxLength;
}

std::size_t UICCTextField::getCharCount()const
{
    return TextFieldTTF::getCharCount();
}

void UICCTextField::setPasswordEnabled(bool enable)
{
    this->setSecureTextEntry(enable);
}

bool UICCTextField::isPasswordEnabled()const
{
    return this->isSecureTextEntry();
}

void UICCTextField::setPasswordStyleText(const std::string& styleText)
{
    this->setPasswordTextStyle(styleText);
}

void UICCTextField::setPasswordText(const std::string& text)
{
    std::string tempStr = "";
    long text_count = StringUtils::getCharacterCountInUTF8String(text);
    long max = text_count;
    
    if (_maxLengthEnabled)
    {
        if (text_count > _maxLength)
        {
            max = _maxLength;
        }
    }
    
    for (int i = 0; i < max; ++i)
    {
        tempStr.append(_passwordStyleText);
    }
    
    Label::setString(tempStr);
}

void UICCTextField::setAttachWithIME(bool attach)
{
    _attachWithIME = attach;
}

bool UICCTextField::getAttachWithIME()const
{
    return _attachWithIME;
}

void UICCTextField::setDetachWithIME(bool detach)
{
    _detachWithIME = detach;
}

bool UICCTextField::getDetachWithIME()const
{
    return _detachWithIME;
}

void UICCTextField::setInsertText(bool insert)
{
    _insertText = insert;
}

bool UICCTextField::getInsertText()const
{
    return _insertText;
}

void UICCTextField::setDeleteBackward(bool deleteBackward)
{
    _deleteBackward = deleteBackward;
}

bool UICCTextField::getDeleteBackward()const
{
    return _deleteBackward;
}

void UICCTextField::SetTypeTextField(TYPE_TEXT_FIELD type)
{
	p_current_type_text = type;
}

std::string UICCTextField::GetValidString(std::string str)
{
	if (str == "")
		return "";
	std::string final_str = "";
	bool IsUnicode = str.size() >= 2;
	if (p_current_type_text == TYPE_TEXT_FIELD::NUMBER_ONLY)
	{
		for (int i = 0; i < str.length(); i++)
		{
			if (str[i] >= 48 && str[i] <= 57)
			{
				final_str += str[i];
			}
		}
		return final_str;
	}
	else if (p_current_type_text == TYPE_TEXT_FIELD::TEXT_ONLY)
	{
		for (int i = 0; i < str.length(); i++)
		{
			if (((str[i] >= 56 && str[i] <= 90) || (str[i] >= 97 && str[i] <= 122)) || str[i] == 32 || (IsUnicode))
			{
				final_str += str[i];
			}
		}
		return final_str;
	}
	return str;

}

void UICCTextField::setTextArea(Vec2 size)
{
	if (_text_area == size)
		return;

	bool increase_text = true;
	if (_text_area.x > size.x)
	{
		increase_text = false;
	}

	bool is_first_text_set = (_text_area.x == 0);

	_text_area = size;

	if (is_first_text_set)
		return;

	//if (_text_area)
	{
		std::string current_string = getString();
		std::string current_store_text = getStoreText();


		std::u16string utf16_current_string;
		std::u16string utf16_current_store_text;

		StringUtils::UTF8ToUTF16(current_string, utf16_current_string);
		StringUtils::UTF8ToUTF16(current_store_text, utf16_current_store_text);

		do
		{
			if (!increase_text)
			{
				if (this->getContentSize().width >= _text_area.x && utf16_current_string.size() > 0)
				{
					utf16_current_store_text += utf16_current_string[0];
					utf16_current_string.erase(utf16_current_string.begin());

					StringUtils::UTF16ToUTF8(utf16_current_string, current_string);
					StringUtils::UTF16ToUTF8(utf16_current_store_text, current_store_text);

					setStoreText(current_store_text);
					setString(current_string);
				}
				else
				{	
					break;
				}
			}
			else
			{
				if (this->getContentSize().width <= _text_area.x && utf16_current_store_text.size() > 0)
				{
					utf16_current_string = utf16_current_store_text[utf16_current_store_text.size() - 1] + utf16_current_string;

					utf16_current_store_text.pop_back();

					StringUtils::UTF16ToUTF8(utf16_current_string, current_string);
					StringUtils::UTF16ToUTF8(utf16_current_store_text, current_store_text);

					setStoreText(current_store_text);
					setString(current_string);
				}
				else
				{
					break;
				}
			}
		} while (true);

		cocos2d::Size _contain_size = this->getContentSize();
		this->setPositionX(_contain_size.width * this->getAnchorPoint().x);

	}
}
static const int TEXTFIELD_RENDERER_Z = (-1);

IMPLEMENT_CLASS_GUI_INFO(TextField)
    
    
TextField::TextField():
_textFieldRenderer(nullptr),
_touchWidth(0.0f),
_touchHeight(0.0f),
_useTouchArea(false),
_textFieldEventListener(nullptr),
_textFieldEventSelector(nullptr),
_eventCallback(nullptr),
_textFieldRendererAdaptDirty(true),
_fontName("Thonburi"),
_fontSize(10),
_fontType(FontType::SYSTEM)
{
}

TextField::~TextField()
{
    _textFieldEventListener = nullptr;
    _textFieldEventSelector = nullptr;
}

TextField* TextField::create()
{
    TextField* widget = new (std::nothrow) TextField();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return nullptr;
}
    
TextField* TextField::create(const std::string &placeholder, const std::string &fontName, int fontSize)
{
    TextField* widget = new (std::nothrow) TextField();
    if (widget && widget->init())
    {
        widget->setFontName(fontName);
        widget->setFontSize(fontSize);
        widget->setPlaceHolder(placeholder);
        widget->autorelease();
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return nullptr;
}
    
bool TextField::init()
{
    if (Widget::init())
    {
        setTouchEnabled(true);
        return true;
    }
    return false;
}
    
void TextField::onEnter()
{
#if CC_ENABLE_SCRIPT_BINDING
    if (_scriptType == kScriptTypeJavascript)
    {
        if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnEnter))
            return;
    }
#endif
    
    Widget::onEnter();
    scheduleUpdate();
}

void TextField::initRenderer()
{
    _textFieldRenderer = UICCTextField::create("input words here", "Thonburi", 20);
    addProtectedChild(_textFieldRenderer, TEXTFIELD_RENDERER_Z, -1);
}

void TextField::setTouchSize(const Size &size)
{
    _touchWidth = size.width;
    _touchHeight = size.height;
}
    
void TextField::setTouchAreaEnabled(bool enable)
{
    _useTouchArea = enable;
}
    
bool TextField::hitTest(const Vec2 &pt, const Camera* camera, Vec3 *p) const
{
    if (false == _useTouchArea)
    {
        return Widget::hitTest(pt, camera, nullptr);
    }

    auto size = getContentSize();
    auto anch = getAnchorPoint();
    Rect rect((size.width - _touchWidth) * anch.x, (size.height - _touchHeight) * anch.y, _touchWidth, _touchHeight);
    return isScreenPointInRect(pt, camera, getWorldToNodeTransform(), rect, nullptr);
}

Size TextField::getTouchSize()const
{
    return Size(_touchWidth, _touchHeight);
}

void TextField::setString(const std::string& text)
{
    std::string strText(text);
    
    if (isMaxLengthEnabled())
    {
        int max = _textFieldRenderer->getMaxLength();
        long text_count = StringUtils::getCharacterCountInUTF8String(text);
        if (text_count > max)
        {
            strText = Helper::getSubStringOfUTF8String(strText, 0, max);
        }
    }
    
    if (isPasswordEnabled())
    {
        _textFieldRenderer->setPasswordText(strText);
        _textFieldRenderer->setString("");
        _textFieldRenderer->insertText(strText.c_str(), strText.size());
    }
    else
    {
        _textFieldRenderer->setString(strText);
    }
    _textFieldRendererAdaptDirty = true;
    updateContentSizeWithTextureSize(_textFieldRenderer->getContentSize());
}

void TextField::setPlaceHolder(const std::string& value)
{
    _textFieldRenderer->setPlaceHolder(value);
    _textFieldRendererAdaptDirty = true;
    updateContentSizeWithTextureSize(_textFieldRenderer->getContentSize());
}
    
const std::string& TextField::getPlaceHolder()const
{
    return _textFieldRenderer->getPlaceHolder();
}
    
const Color4B& TextField::getPlaceHolderColor()const
{
    return _textFieldRenderer->getColorSpaceHolder();
}
    
void TextField::setPlaceHolderColor(const cocos2d::Color3B &color)
{
    _textFieldRenderer->setColorSpaceHolder(color);
}
    
void TextField::setPlaceHolderColor(const cocos2d::Color4B &color)
{
    _textFieldRenderer->setColorSpaceHolder(color);
}

const Color4B& TextField::getTextColor()const
{
    return _textFieldRenderer->getTextColor();
}

void TextField::setTextColor(const cocos2d::Color4B &textColor)
{
    _textFieldRenderer->setTextColor(textColor);
}

void TextField::setFontSize(int size)
{
    if (_fontType == FontType::SYSTEM)
    {
        _textFieldRenderer->setSystemFontSize(size);
    }
    else
    {
        TTFConfig config = _textFieldRenderer->getTTFConfig();
        config.fontSize = size;
        _textFieldRenderer->setTTFConfig(config);
    }
    _fontSize = size;
    _textFieldRendererAdaptDirty = true;
    updateContentSizeWithTextureSize(_textFieldRenderer->getContentSize());
}
    
int TextField::getFontSize()const
{
    return _fontSize;
}

void TextField::setFontName(const std::string& name)
{
    if(FileUtils::getInstance()->isFileExist(name))
    {
        TTFConfig config = _textFieldRenderer->getTTFConfig();
        config.fontFilePath = name;
        config.fontSize = _fontSize;
        _textFieldRenderer->setTTFConfig(config);
        _fontType = FontType::TTF;
    }
    else
    {
        _textFieldRenderer->setSystemFontName(name);
        if (_fontType == FontType::TTF)
        {
            _textFieldRenderer->requestSystemFontRefresh();
        }
        _fontType = FontType::SYSTEM;
    }
    _fontName = name;
    _textFieldRendererAdaptDirty = true;
    updateContentSizeWithTextureSize(_textFieldRenderer->getContentSize());
}
    
const std::string& TextField::getFontName()const
{
    return _fontName;
}

void TextField::didNotSelectSelf()
{
    _textFieldRenderer->detachWithIME();
}

const std::string& TextField::getString()const
{
    return _textFieldRenderer->getString();
}
    
int TextField::getStringLength() const 
{
	std::u16string utf16_current_store_text;
	StringUtils::UTF8ToUTF16(_textFieldRenderer->getStoreText(), utf16_current_store_text);

    return _textFieldRenderer->getStringLength() + utf16_current_store_text.length();
}


bool TextField::onTouchBegan(Touch *touch, Event *unusedEvent)
{
    bool pass = Widget::onTouchBegan(touch, unusedEvent);
    if (_hitted)
    {
        if (isFocusEnabled())
        {
            requestFocus();
        }

        _textFieldRenderer->attachWithIME();
    }
    else
    {
        this->didNotSelectSelf();
    }
    return pass;
}

void TextField::setMaxLengthEnabled(bool enable)
{
    _textFieldRenderer->setMaxLengthEnabled(enable);
}

bool TextField::isMaxLengthEnabled()const
{
    return _textFieldRenderer->isMaxLengthEnabled();
}

void TextField::setMaxLength(int length)
{
    _textFieldRenderer->setMaxLength(length);
    
    setString(getString());
}

int TextField::getMaxLength()const
{
    return _textFieldRenderer->getMaxLength();
}

void TextField::setPasswordEnabled(bool enable)
{
    _textFieldRenderer->setPasswordEnabled(enable);
}

bool TextField::isPasswordEnabled()const
{
    return _textFieldRenderer->isPasswordEnabled();
}

void TextField::setPasswordStyleText(const char *styleText)
{
    _textFieldRenderer->setPasswordStyleText(styleText);
    
    setString(getString());
}
    
const char* TextField::getPasswordStyleText()const
{
    return _textFieldRenderer->getPasswordTextStyle().c_str();
}

void TextField::update(float dt)
{
    if (getDetachWithIME())
    {
        detachWithIMEEvent();
        setDetachWithIME(false);
    }
    
    if (getAttachWithIME())
    {
        attachWithIMEEvent();
        setAttachWithIME(false);
    }

    if (getDeleteBackward())
    {
        _textFieldRendererAdaptDirty = true;
        updateContentSizeWithTextureSize(_textFieldRenderer->getContentSize());

        deleteBackwardEvent();
        setDeleteBackward(false);
    }

    if (getInsertText())
    {
        //we update the content size first such that when user call getContentSize() in event callback won't be wrong
        _textFieldRendererAdaptDirty = true;
        updateContentSizeWithTextureSize(_textFieldRenderer->getContentSize());
        
        insertTextEvent();
        setInsertText(false);
    }
	_textFieldRenderer->update(dt);
}

bool TextField::getAttachWithIME()const
{
    return _textFieldRenderer->getAttachWithIME();
}

void TextField::setAttachWithIME(bool attach)
{
    _textFieldRenderer->setAttachWithIME(attach);
}

bool TextField::getDetachWithIME()const
{
    return _textFieldRenderer->getDetachWithIME();
}

void TextField::setDetachWithIME(bool detach)
{
    _textFieldRenderer->setDetachWithIME(detach);
}

bool TextField::getInsertText()const
{
    return _textFieldRenderer->getInsertText();
}

void TextField::setInsertText(bool insertText)
{
    _textFieldRenderer->setInsertText(insertText);
}

bool TextField::getDeleteBackward()const
{
    return _textFieldRenderer->getDeleteBackward();
}

void TextField::setDeleteBackward(bool deleteBackward)
{
    _textFieldRenderer->setDeleteBackward(deleteBackward);
}

void TextField::attachWithIMEEvent()
{
    this->retain();
    if (_textFieldEventListener && _textFieldEventSelector)
    {
        (_textFieldEventListener->*_textFieldEventSelector)(this, TEXTFIELD_EVENT_ATTACH_WITH_IME);
    }
    if (_eventCallback) {
        _eventCallback(this, EventType::ATTACH_WITH_IME);
    }
    if (_ccEventCallback)
    {
        _ccEventCallback(this, static_cast<int>(EventType::ATTACH_WITH_IME));
    }
    this->release();
}

void TextField::detachWithIMEEvent()
{
    this->retain();
    if (_textFieldEventListener && _textFieldEventSelector)
    {
        (_textFieldEventListener->*_textFieldEventSelector)(this, TEXTFIELD_EVENT_DETACH_WITH_IME);
    }
    if (_eventCallback)
    {
        _eventCallback(this, EventType::DETACH_WITH_IME);
    }
    if (_ccEventCallback)
    {
        _ccEventCallback(this, static_cast<int>(EventType::DETACH_WITH_IME));
    }
    this->release();
}

void TextField::insertTextEvent()
{
    this->retain();
    if (_textFieldEventListener && _textFieldEventSelector)
    {
        (_textFieldEventListener->*_textFieldEventSelector)(this, TEXTFIELD_EVENT_INSERT_TEXT);
    }
    if (_eventCallback)
    {
        _eventCallback(this, EventType::INSERT_TEXT);
    }
    if (_ccEventCallback)
    {
        _ccEventCallback(this, static_cast<int>(EventType::INSERT_TEXT));
    }
    this->release();
}

void TextField::deleteBackwardEvent()
{
    this->retain();
    if (_textFieldEventListener && _textFieldEventSelector)
    {
        (_textFieldEventListener->*_textFieldEventSelector)(this, TEXTFIELD_EVENT_DELETE_BACKWARD);
    }
    if (_eventCallback)
    {
        _eventCallback(this, EventType::DELETE_BACKWARD);
    }
    if (_ccEventCallback)
    {
        _ccEventCallback(this, static_cast<int>(EventType::DELETE_BACKWARD));
    }
    this->release();
}

void TextField::addEventListenerTextField(Ref *target, SEL_TextFieldEvent selecor)
{
    _textFieldEventListener = target;
    _textFieldEventSelector = selecor;
}
    
void TextField::addEventListener(const ccTextFieldCallback& callback)
{
    _eventCallback = callback;
}

void TextField::onSizeChanged()
{
    Widget::onSizeChanged();
    _textFieldRendererAdaptDirty = true;
}
    
void TextField::adaptRenderers()
{
    if (_textFieldRendererAdaptDirty)
    {
        textfieldRendererScaleChangedWithSize();
        _textFieldRendererAdaptDirty = false;
    }
}

void TextField::textfieldRendererScaleChangedWithSize()
{
    if (!_ignoreSize)
    {
        _textFieldRenderer->setDimensions(_contentSize.width, _contentSize.height);
    }
    _textFieldRenderer->setPosition(_contentSize.width / 2.0f, _contentSize.height / 2.0f);
}

Size TextField::getAutoRenderSize()
{
    Size virtualSize = _textFieldRenderer->getContentSize();
    if (!_ignoreSize)
    {
        _textFieldRenderer->setDimensions(0, 0);
        virtualSize = _textFieldRenderer->getContentSize();
        _textFieldRenderer->setDimensions(_contentSize.width, _contentSize.height);
    }

    return virtualSize;
}

Size TextField::getVirtualRendererSize() const
{
    return _textFieldRenderer->getContentSize();
}

Node* TextField::getVirtualRenderer()
{
    return _textFieldRenderer;
}

std::string TextField::getDescription() const
{
    return "TextField";
}

void TextField::attachWithIME()
{
    _textFieldRenderer->attachWithIME();
}

Widget* TextField::createCloneInstance()
{
    return TextField::create();
}

void TextField::copySpecialProperties(Widget *widget)
{
    TextField* textField = dynamic_cast<TextField*>(widget);
    if (textField)
    {
        setString(textField->_textFieldRenderer->getString());
        setPlaceHolder(textField->getString());
        setFontSize(textField->_fontSize);
        setFontName(textField->_fontName);
        setMaxLengthEnabled(textField->isMaxLengthEnabled());
        setMaxLength(textField->getMaxLength());
        setPasswordEnabled(textField->isPasswordEnabled());
        setPasswordStyleText(textField->getPasswordStyleText());
        setAttachWithIME(textField->getAttachWithIME());
        setDetachWithIME(textField->getDetachWithIME());
        setInsertText(textField->getInsertText());
        setDeleteBackward(textField->getDeleteBackward());
        _eventCallback = textField->_eventCallback;
        _ccEventCallback = textField->_ccEventCallback;
        _textFieldEventListener = textField->_textFieldEventListener;
        _textFieldEventSelector = textField->_textFieldEventSelector;
    }
}
    
void TextField::setTextAreaSize(const Size &size)
{
    this->setContentSize(size);
}

void TextField::setTextHorizontalAlignment(TextHAlignment alignment)
{
    _textFieldRenderer->setHorizontalAlignment(alignment);
}

TextHAlignment TextField::getTextHorizontalAlignment() const
{
    return _textFieldRenderer->getHorizontalAlignment();
}

void TextField::setTextVerticalAlignment(TextVAlignment alignment)
{
    _textFieldRenderer->setVerticalAlignment(alignment);
}

TextVAlignment TextField::getTextVerticalAlignment() const
{
    return _textFieldRenderer->getVerticalAlignment();
}

void TextField::SetTypeTextField(TYPE_TEXT_FIELD type)
{
	_textFieldRenderer->SetTypeTextField(type);
}

void TextField::SetTextArea(Vec2 pos)
{
	_textFieldRenderer->setTextArea(pos);
}

void TextField::SetStoreText(std::string store_tex)
{
	_textFieldRenderer->setStoreText(store_tex);
}

void TextField::setCursorEnabled(bool enabled)
{
    _textFieldRenderer->setCursorEnabled(enabled);
}
 
bool TextField::isCursorEnable()
{
	return _textFieldRenderer->isCursorEnable();
}

void TextField::setCursorChar(char cursor)
{
    _textFieldRenderer->setCursorChar(cursor);
}

void TextField::setCursorPosition(std::size_t cursorPosition)
{
    _textFieldRenderer->setCursorPosition(cursorPosition);
}

void TextField::setCursorFromPoint(const Vec2 &point, const Camera* camera)
{
    _textFieldRenderer->setCursorFromPoint(point, camera);
}
}

NS_CC_END
