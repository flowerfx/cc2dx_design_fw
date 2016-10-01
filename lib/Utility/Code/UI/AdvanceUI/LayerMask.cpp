/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
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

#include <stdarg.h>
#include "LayerMask.h"
#include "base/CCScriptSupport.h"
#include "platform/CCDevice.h"
#include "renderer/CCRenderer.h"
#include "renderer/ccGLStateCache.h"
#include "renderer/CCGLProgramState.h"
#include "base/CCDirector.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventListenerTouch.h"
#include "base/CCEventTouch.h"
#include "base/CCEventKeyboard.h"
#include "base/CCEventListenerKeyboard.h"
#include "base/CCEventAcceleration.h"
#include "base/CCEventListenerAcceleration.h"


#include "deprecated/CCString.h"

NS_CC_BEGIN

// LayerMask
LayerMask::LayerMask()
: _touchEnabled(false)
, _accelerometerEnabled(false)
, _keyboardEnabled(false)
, _touchListener(nullptr)
, _keyboardListener(nullptr)
, _accelerationListener(nullptr)
, _touchMode(Touch::DispatchMode::ALL_AT_ONCE)
, _swallowsTouches(true)
{
    _ignoreAnchorPointForPosition = true;
    setAnchorPoint(Vec2(0.5f, 0.5f));
}

LayerMask::~LayerMask()
{

}

bool LayerMask::init()
{
    Director * director = Director::getInstance();
    setContentSize(director->getWinSize());
    return true;
}

LayerMask *LayerMask::create()
{
    LayerMask *ret = new (std::nothrow) LayerMask();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    else
    {
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
}

int LayerMask::executeScriptTouchHandler(EventTouch::EventCode eventType, Touch* touch, Event* event)
{
#if CC_ENABLE_SCRIPT_BINDING
    if (kScriptTypeLua == _scriptType)
    {
        TouchScriptData data(eventType, this, touch, event);
        ScriptEvent scriptEvent(kTouchEvent, &data);
        return ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&scriptEvent);
    }
#endif
    //can not reach it
    return 0;
}

int LayerMask::executeScriptTouchesHandler(EventTouch::EventCode eventType, const std::vector<Touch*>& touches, Event* event)
{
#if CC_ENABLE_SCRIPT_BINDING
    if (kScriptTypeLua == _scriptType)
    {
        TouchesScriptData data(eventType, this, touches, event);
        ScriptEvent scriptEvent(kTouchesEvent, &data);
        return ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&scriptEvent);
    }
#endif
    return 0;
}

#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif _MSC_VER >= 1400 //vs 2005 or higher
#pragma warning (push)
#pragma warning (disable: 4996)
#endif

/// isTouchEnabled getter
bool LayerMask::isTouchEnabled() const
{
    return _touchEnabled;
}

/// isTouchEnabled setter
void LayerMask::setTouchEnabled(bool enabled)
{
    if (_touchEnabled != enabled)
    {
        _touchEnabled = enabled;
        if (enabled)
        {
            if (_touchListener != nullptr)
                return;

            if( _touchMode == Touch::DispatchMode::ALL_AT_ONCE )
            {
                // Register Touch Event
                auto listener = EventListenerTouchAllAtOnce::create();

                listener->onTouchesBegan = CC_CALLBACK_2(LayerMask::onTouchesBegan, this);
                listener->onTouchesMoved = CC_CALLBACK_2(LayerMask::onTouchesMoved, this);
                listener->onTouchesEnded = CC_CALLBACK_2(LayerMask::onTouchesEnded, this);
                listener->onTouchesCancelled = CC_CALLBACK_2(LayerMask::onTouchesCancelled, this);

                _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
                _touchListener = listener;
            }
            else
            {
                // Register Touch Event
                auto listener = EventListenerTouchOneByOne::create();
                listener->setSwallowTouches(_swallowsTouches);

                listener->onTouchBegan = CC_CALLBACK_2(LayerMask::onTouchBegan, this);
                listener->onTouchMoved = CC_CALLBACK_2(LayerMask::onTouchMoved, this);
                listener->onTouchEnded = CC_CALLBACK_2(LayerMask::onTouchEnded, this);
                listener->onTouchCancelled = CC_CALLBACK_2(LayerMask::onTouchCancelled, this);

                _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
                _touchListener = listener;
            }
        }
        else
        {
            _eventDispatcher->removeEventListener(_touchListener);
            _touchListener = nullptr;
        }
    }
}

void LayerMask::setTouchMode(Touch::DispatchMode mode)
{
    if(_touchMode != mode)
    {
        _touchMode = mode;

        if( _touchEnabled)
        {
            setTouchEnabled(false);
            setTouchEnabled(true);
        }
    }
}

void LayerMask::setSwallowsTouches(bool swallowsTouches)
{
    if (_swallowsTouches != swallowsTouches)
    {
        _swallowsTouches = swallowsTouches;

        if( _touchEnabled)
        {
            setTouchEnabled(false);
            setTouchEnabled(true);
        }
    }
}

Touch::DispatchMode LayerMask::getTouchMode() const
{
    return _touchMode;
}

bool LayerMask::isSwallowsTouches() const
{
    return _swallowsTouches;
}

/// isAccelerometerEnabled getter
bool LayerMask::isAccelerometerEnabled() const
{
    return _accelerometerEnabled;
}
/// isAccelerometerEnabled setter
void LayerMask::setAccelerometerEnabled(bool enabled)
{
    if (enabled != _accelerometerEnabled)
    {
        _accelerometerEnabled = enabled;

        Device::setAccelerometerEnabled(enabled);

        _eventDispatcher->removeEventListener(_accelerationListener);
        _accelerationListener = nullptr;

        if (enabled)
        {
            _accelerationListener = EventListenerAcceleration::create(CC_CALLBACK_2(LayerMask::onAcceleration, this));
            _eventDispatcher->addEventListenerWithSceneGraphPriority(_accelerationListener, this);
        }
    }
}

void LayerMask::setAccelerometerInterval(double interval) {
    if (_accelerometerEnabled)
    {
        if (_running)
        {
            Device::setAccelerometerInterval(interval);
        }
    }
}

void LayerMask::onAcceleration(Acceleration* acc, Event* unused_event)
{
    CC_UNUSED_PARAM(acc);
    CC_UNUSED_PARAM(unused_event);
#if CC_ENABLE_SCRIPT_BINDING
    if(kScriptTypeNone != _scriptType)
    {
        BasicScriptData data(this,(void*)acc);
        ScriptEvent event(kAccelerometerEvent,&data);
        ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&event);
    }
#endif
}

void LayerMask::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* unused_event)
{
    CC_UNUSED_PARAM(keyCode);
    CC_UNUSED_PARAM(unused_event);
}

void LayerMask::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* unused_event)
{
    CC_UNUSED_PARAM(unused_event);
#if CC_ENABLE_SCRIPT_BINDING
    if(kScriptTypeNone != _scriptType)
    {
        KeypadScriptData data(keyCode, this);
        ScriptEvent event(kKeypadEvent,&data);
        ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&event);
    }
#endif
}

/// isKeyboardEnabled getter
bool LayerMask::isKeyboardEnabled() const
{
    return _keyboardEnabled;
}
/// isKeyboardEnabled setter
void LayerMask::setKeyboardEnabled(bool enabled)
{
    if (enabled != _keyboardEnabled)
    {
        _keyboardEnabled = enabled;

        _eventDispatcher->removeEventListener(_keyboardListener);
        _keyboardListener = nullptr;

        if (enabled)
        {
            auto listener = EventListenerKeyboard::create();
            listener->onKeyPressed = CC_CALLBACK_2(LayerMask::onKeyPressed, this);
            listener->onKeyReleased = CC_CALLBACK_2(LayerMask::onKeyReleased, this);

            _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
            _keyboardListener = listener;
        }
    }
}

void LayerMask::setKeypadEnabled(bool enabled)
{
    setKeyboardEnabled(enabled);
}
/// Callbacks

bool LayerMask::onTouchBegan(Touch *touch, Event *event)
{
#if CC_ENABLE_SCRIPT_BINDING
    if (kScriptTypeLua == _scriptType)
    {
        return executeScriptTouchHandler(EventTouch::EventCode::BEGAN, touch, event) == 0 ? false : true;
    }
#endif
    CC_UNUSED_PARAM(event);
    CCASSERT(false, "LayerMask#ccTouchBegan override me");
    return true;
}

void LayerMask::onTouchMoved(Touch *touch, Event *event)
{
#if CC_ENABLE_SCRIPT_BINDING
    if (kScriptTypeLua == _scriptType)
    {
        executeScriptTouchHandler(EventTouch::EventCode::MOVED, touch, event);
        return;
    }
#endif
    
    CC_UNUSED_PARAM(event);
}

void LayerMask::onTouchEnded(Touch *touch, Event *event)
{
#if CC_ENABLE_SCRIPT_BINDING
    if (kScriptTypeLua == _scriptType)
    {
        executeScriptTouchHandler(EventTouch::EventCode::ENDED, touch, event);
        return;
    }
#endif
    
    CC_UNUSED_PARAM(event);
}

void LayerMask::onTouchCancelled(Touch *touch, Event *event)
{
#if CC_ENABLE_SCRIPT_BINDING
    if (kScriptTypeLua == _scriptType)
    {
        executeScriptTouchHandler(EventTouch::EventCode::CANCELLED, touch, event);
        return;
    }
#endif
    
    CC_UNUSED_PARAM(event);
}    

void LayerMask::onTouchesBegan(const std::vector<Touch*>& touches, Event *event)
{
#if CC_ENABLE_SCRIPT_BINDING
    if (kScriptTypeLua == _scriptType)
    {
        executeScriptTouchesHandler(EventTouch::EventCode::BEGAN, touches, event);
        return;
    }
#endif
    CC_UNUSED_PARAM(event);
}

void LayerMask::onTouchesMoved(const std::vector<Touch*>& touches, Event *event)
{
#if CC_ENABLE_SCRIPT_BINDING
    if (kScriptTypeLua == _scriptType)
    {
        executeScriptTouchesHandler(EventTouch::EventCode::MOVED, touches, event);
        return;
    }
#endif
    
    CC_UNUSED_PARAM(event);
}

void LayerMask::onTouchesEnded(const std::vector<Touch*>& touches, Event *event)
{
#if CC_ENABLE_SCRIPT_BINDING
    if (kScriptTypeLua == _scriptType)
    {
        executeScriptTouchesHandler(EventTouch::EventCode::ENDED, touches, event);
        return;
    }
#endif
    CC_UNUSED_PARAM(event);
}

void LayerMask::onTouchesCancelled(const std::vector<Touch*>& touches, Event *event)
{
#if CC_ENABLE_SCRIPT_BINDING
    if (kScriptTypeLua == _scriptType)
    {
        executeScriptTouchesHandler(EventTouch::EventCode::CANCELLED, touches, event);
        return;
    }
#endif
    CC_UNUSED_PARAM(event);
}

std::string LayerMask::getDescription() const
{
    return StringUtils::format("<LayerMask | Tag = %d>", _tag);
}

NS_CC_END
