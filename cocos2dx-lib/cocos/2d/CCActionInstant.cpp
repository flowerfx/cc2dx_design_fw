/****************************************************************************
 Copyright (c) 2008-2010 Ricardo Quesada
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2011      Zynga Inc.
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 
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

#include "2d/CCActionInstant.h"
#include "2d/CCNode.h"
#include "2d/CCSprite.h"

#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif _MSC_VER >= 1400 //vs 2005 or higher
#pragma warning (push)
#pragma warning (disable: 4996)
#endif

NS_CC_BEGIN
//
// InstantAction
//
bool ActionInstant::isDone() const
{
    return true;
}

void ActionInstant::step(float dt)
{
    CC_UNUSED_PARAM(dt);
    float updateDt = 1;
#if CC_ENABLE_SCRIPT_BINDING
    if (_scriptType == kScriptTypeJavascript)
    {
        if (ScriptEngineManager::sendActionEventToJS(this, kActionUpdate, (void *)&updateDt))
            return;
    }
#endif
    update(updateDt);
}

void ActionInstant::update(float time)
{
    CC_UNUSED_PARAM(time);
    // nothing
}

//
// Show
//

Show* Show::create() 
{
    Show* ret = new (std::nothrow) Show();

    if (ret)
    {
        ret->autorelease();
    }

    return ret;
}

void Show::update(float time)
{
    CC_UNUSED_PARAM(time);
    _target->setVisible(true);
}

ActionInstant* Show::reverse() const
{
    return Hide::create();
}

Show* Show::clone() const
{
    // no copy constructor
    return Show::create();
}

//
// Hide
//
Hide * Hide::create() 
{
    Hide *ret = new (std::nothrow) Hide();

    if (ret)
    {
        ret->autorelease();
    }

    return ret;
}

void Hide::update(float time)
{
    CC_UNUSED_PARAM(time);
    _target->setVisible(false);
}

ActionInstant *Hide::reverse() const
{
    return Show::create();
}

Hide* Hide::clone() const
{
    // no copy constructor
    return Hide::create();
}

//
// ToggleVisibility
//
ToggleVisibility * ToggleVisibility::create()
{
    ToggleVisibility *ret = new (std::nothrow) ToggleVisibility();

    if (ret)
    {
        ret->autorelease();
    }

    return ret;
}

void ToggleVisibility::update(float time) 
{
    CC_UNUSED_PARAM(time);
    _target->setVisible(!_target->isVisible());
}

ToggleVisibility * ToggleVisibility::reverse() const
{
    return ToggleVisibility::create();
}

ToggleVisibility * ToggleVisibility::clone() const
{
    // no copy constructor
    return ToggleVisibility::create();
}

//
// Remove Self
//
RemoveSelf * RemoveSelf::create(bool isNeedCleanUp /*= true*/) 
{
    RemoveSelf *ret = new (std::nothrow) RemoveSelf();

    if (ret && ret->init(isNeedCleanUp))
    {
        ret->autorelease();
    }

    return ret;
}

bool RemoveSelf::init(bool isNeedCleanUp)
{
    _isNeedCleanUp = isNeedCleanUp;
    return true;
}

void RemoveSelf::update(float time)
{
    CC_UNUSED_PARAM(time);
    _target->removeFromParentAndCleanup(_isNeedCleanUp);
}

RemoveSelf *RemoveSelf::reverse() const
{
    return RemoveSelf::create(_isNeedCleanUp);
}

RemoveSelf * RemoveSelf::clone() const
{
    // no copy constructor
    return RemoveSelf::create(_isNeedCleanUp);
}

//
// FlipX
//

FlipX *FlipX::create(bool x)
{
    FlipX *ret = new (std::nothrow) FlipX();

    if (ret && ret->initWithFlipX(x))
    {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool FlipX::initWithFlipX(bool x)
{
    _flipX = x;
    return true;
}

void FlipX::update(float time)
{
    CC_UNUSED_PARAM(time);
    static_cast<Sprite*>(_target)->setFlippedX(_flipX);
}

FlipX* FlipX::reverse() const
{
    return FlipX::create(!_flipX);
}

FlipX * FlipX::clone() const
{
    // no copy constructor
    return FlipX::create(_flipX);
}
//
// FlipY
//

FlipY * FlipY::create(bool y)
{
    FlipY *ret = new (std::nothrow) FlipY();

    if (ret && ret->initWithFlipY(y))
    {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool FlipY::initWithFlipY(bool y)
{
    _flipY = y;
    return true;
}

void FlipY::update(float time)
{
    CC_UNUSED_PARAM(time);
    static_cast<Sprite*>(_target)->setFlippedY(_flipY);
}

FlipY* FlipY::reverse() const
{
    return FlipY::create(!_flipY);
}

FlipY * FlipY::clone() const
{
    // no copy constructor
    return FlipY::create(_flipY);
}

//
// Place
//

Place* Place::create(const Vec2& pos)
{
    Place *ret = new (std::nothrow) Place();

    if (ret && ret->initWithPosition(pos))
    {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool Place::initWithPosition(const Vec2& pos)
{
    _position = pos;
    return true;
}

Place * Place::clone() const
{
    // no copy constructor
    return Place::create(_position);
}

Place * Place::reverse() const
{
    // no reverse, just clone
    return this->clone();
}

void Place::update(float time)
{
    CC_UNUSED_PARAM(time);
    _target->setPosition(_position);
}

//
// CallFunc
//

CallFunc * CallFunc::create(const std::function<void()> &func)
{
    CallFunc *ret = new (std::nothrow) CallFunc();

    if (ret && ret->initWithFunction(func) )
    {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

CallFunc * CallFunc::create(Ref* selectorTarget, SEL_CallFunc selector) 
{
    CallFunc *ret = new (std::nothrow) CallFunc();

    if (ret && ret->initWithTarget(selectorTarget))
    {
        ret->_callFunc = selector;
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool CallFunc::initWithFunction(const std::function<void()> &func)
{
    _function = func;
    return true;
}

bool CallFunc::initWithTarget(Ref* target)
{
    if (target)
    {
        target->retain();
    }

    if (_selectorTarget)
    {
        _selectorTarget->release();
    }

    _selectorTarget = target;
    return true;
}

CallFunc::~CallFunc()
{
    CC_SAFE_RELEASE(_selectorTarget);
}

CallFunc * CallFunc::clone() const
    {
    // no copy constructor
    auto a = new (std::nothrow) CallFunc();
    if( _selectorTarget)
    {
        a->initWithTarget(_selectorTarget);
        a->_callFunc = _callFunc;
    }
    else if( _function )
    {
        a->initWithFunction(_function);
    }

    a->autorelease();
    return a;
}

CallFunc * CallFunc::reverse() const
{
    // no reverse here, just return a clone
    return this->clone();
}

void CallFunc::update(float time)
{
    CC_UNUSED_PARAM(time);
    this->execute();
}

void CallFunc::execute()
{
    if (_callFunc)
    {
        (_selectorTarget->*_callFunc)();
    } 
    else if( _function )
    {
        _function();
    }
}

//
// CallFuncN
//

CallFuncN * CallFuncN::create(const std::function<void(Node*)> &func)
{
    auto ret = new (std::nothrow) CallFuncN();

    if (ret && ret->initWithFunction(func) )
    {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

// FIXME: deprecated
CallFuncN * CallFuncN::create(Ref* selectorTarget, SEL_CallFuncN selector)
{
    CallFuncN *ret = new (std::nothrow) CallFuncN();

    if (ret && ret->initWithTarget(selectorTarget, selector))
    {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

void CallFuncN::execute()
{
    if (_callFuncN)
    {
        (_selectorTarget->*_callFuncN)(_target);
    }
    else if (_functionN)
    {
        _functionN(_target);
    }
}

bool CallFuncN::initWithFunction(const std::function<void (Node *)> &func)
{
    _functionN = func;
    return true;
}

bool CallFuncN::initWithTarget(Ref* selectorTarget, SEL_CallFuncN selector)
{
    if (CallFunc::initWithTarget(selectorTarget))
    {
        _callFuncN = selector;
        return true;
    }

    return false;
}

CallFuncN * CallFuncN::clone() const
{
    // no copy constructor
    auto a = new (std::nothrow) CallFuncN();

    if( _selectorTarget)
    {
        a->initWithTarget(_selectorTarget, _callFuncN);
    }
    else if( _functionN ){
        a->initWithFunction(_functionN);
    }

    a->autorelease();
    return a;
}

//
// CallFuncND
//

__CCCallFuncND * __CCCallFuncND::create(Ref* selectorTarget, SEL_CallFuncND selector, void* d)
{
    __CCCallFuncND* ret = new (std::nothrow) __CCCallFuncND();
    
    if (ret && ret->initWithTarget(selectorTarget, selector, d))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool __CCCallFuncND::initWithTarget(Ref* selectorTarget, SEL_CallFuncND selector, void* d)
{
    if (CallFunc::initWithTarget(selectorTarget))
    {
        _data = d;
        _callFuncND = selector;
        return true;
    }
    
    return false;
}

void __CCCallFuncND::execute()
{
    if (_callFuncND)
    {
        (_selectorTarget->*_callFuncND)(_target, _data);
    }
}

__CCCallFuncND * __CCCallFuncND::clone() const
{
    // no copy constructor
    auto a = new (std::nothrow) __CCCallFuncND();
    
    if( _selectorTarget)
    {
        a->initWithTarget(_selectorTarget, _callFuncND, _data);
    }
    
    a->autorelease();
    return a;
}

//
// CallFuncO
//
__CCCallFuncO::__CCCallFuncO() :
_object(nullptr)
{
}

__CCCallFuncO::~__CCCallFuncO()
{
    CC_SAFE_RELEASE(_object);
}

void __CCCallFuncO::execute()
{
    if (_callFuncO)
    {
        (_selectorTarget->*_callFuncO)(_object);
    }
}

__CCCallFuncO * __CCCallFuncO::create(Ref* selectorTarget, SEL_CallFuncO selector, Ref* object)
{
    __CCCallFuncO *ret = new (std::nothrow) __CCCallFuncO();
    
    if (ret && ret->initWithTarget(selectorTarget, selector, object))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool __CCCallFuncO::initWithTarget(Ref* selectorTarget, SEL_CallFuncO selector, Ref* object)
{
    if (CallFunc::initWithTarget(selectorTarget))
    {
        _object = object;
        CC_SAFE_RETAIN(_object);
        
        _callFuncO = selector;
        return true;
    }
    
    return false;
}

__CCCallFuncO * __CCCallFuncO::clone() const
{
    // no copy constructor
    auto a = new (std::nothrow) __CCCallFuncO();
    
    if( _selectorTarget)
    {
        a->initWithTarget(_selectorTarget, _callFuncO, _object);
    }
    
    a->autorelease();
    return a;
}

Ref* __CCCallFuncO::getObject() const
{
    return _object;
}
    
void __CCCallFuncO::setObject(Ref* obj)
{
    if (obj != _object)
    {
        CC_SAFE_RELEASE(_object);
        _object = obj;
        CC_SAFE_RETAIN(_object);
    }
}

//chien todo :: add more action instant
//
// Place 3D
//

Place3D* Place3D::create(const Vec3& pos)
{
	Place3D *ret = new (std::nothrow) Place3D();

	if (ret && ret->initWithPosition(pos))
	{
		ret->autorelease();
		return ret;
	}

	delete ret;
	return nullptr;
}

bool Place3D::initWithPosition(const Vec3& pos)
{
	_position = pos;
	return true;
}

Place3D * Place3D::clone() const
{
	// no copy constructor
	return Place3D::create(_position);
}

Place3D * Place3D::reverse() const
{
	// no reverse, just clone
	return this->clone();
}

void Place3D::update(float time)
{
	CC_UNUSED_PARAM(time);
	_target->setPosition3D(_position);
}

//
// scale 2D
//

IScale* IScale::create(const Vec2& scale)
{
	IScale *ret = new (std::nothrow) IScale();

	if (ret && ret->initWithScale(scale))
	{
		ret->autorelease();
		return ret;
	}

	delete ret;
	return nullptr;
}

bool IScale::initWithScale(const Vec2& scale)
{
	_scale = scale;
	return true;
}

IScale * IScale::clone() const
{
	// no copy constructor
	return IScale::create(_scale);
}

IScale * IScale::reverse() const
{
	// no reverse, just clone
	return this->clone();
}

void IScale::update(float time)
{
	CC_UNUSED_PARAM(time);
	_target->setScale(_scale.x, _scale.y);
}

//
// opacity
//

IOpacity* IOpacity::create(const unsigned char opacity)
{
	IOpacity *ret = new (std::nothrow) IOpacity();

	if (ret && ret->initWithOpacity(opacity))
	{
		ret->autorelease();
		return ret;
	}

	delete ret;
	return nullptr;
}

bool IOpacity::initWithOpacity(const unsigned char opacity)
{
	_opacity = opacity;
	return true;
}

IOpacity * IOpacity::clone() const
{
	// no copy constructor
	return IOpacity::create(_opacity);
}

IOpacity * IOpacity::reverse() const
{
	// no reverse, just clone
	return this->clone();
}

void IOpacity::update(float time)
{
	CC_UNUSED_PARAM(time);
	_target->setOpacity(_opacity);
}

//
// color
//

IColor* IColor::create(Color4B color)
{
	IColor *ret = new (std::nothrow) IColor();

	if (ret && ret->initWithColor(color))
	{
		ret->autorelease();
		return ret;
	}

	delete ret;
	return nullptr;
}

bool IColor::initWithColor(Color4B color)
{
	_color = color;
	return true;
}

IColor * IColor::clone() const
{
	// no copy constructor
	return IColor::create(_color);
}

IColor * IColor::reverse() const
{
	// no reverse, just clone
	return this->clone();
}

void IColor::update(float time)
{
	CC_UNUSED_PARAM(time);
	_target->setColor(Color3B(_color.r,_color.g,_color.b));
}

//
// color
//

IAnchor* IAnchor::create(const Vec2& anchor)
{
	IAnchor *ret = new (std::nothrow) IAnchor();

	if (ret && ret->initWithAnchorPoint(anchor))
	{
		ret->autorelease();
		return ret;
	}

	delete ret;
	return nullptr;
}

bool IAnchor::initWithAnchorPoint(const Vec2& anchor)
{
	_anchor_point = anchor;
	return true;
}

IAnchor * IAnchor::clone() const
{
	// no copy constructor
	return IAnchor::create(_anchor_point);
}

IAnchor * IAnchor::reverse() const
{
	// no reverse, just clone
	return this->clone();
}

void IAnchor::update(float time)
{
	CC_UNUSED_PARAM(time);
	_target->setAnchorPoint(_anchor_point);
}

//
// rotate z
//

IRotateZ* IRotateZ::create(const float value)
{
	IRotateZ *ret = new (std::nothrow) IRotateZ();

	if (ret && ret->initWithRotateZ(value))
	{
		ret->autorelease();
		return ret;
	}

	delete ret;
	return nullptr;
}

bool IRotateZ::initWithRotateZ(const float value)
{
	_rotate_z = value;
	return true;
}

IRotateZ * IRotateZ::clone() const
{
	// no copy constructor
	return IRotateZ::create(_rotate_z);
}

IRotateZ * IRotateZ::reverse() const
{
	// no reverse, just clone
	return this->clone();
}

void IRotateZ::update(float time)
{
	CC_UNUSED_PARAM(time);
	//_target->setRotation(_rotate);
	auto current_rotate = _target->getRotation3D();
	_target->setRotation3D(Vec3(current_rotate.x, _rotate_z, current_rotate.y));
}

//
// rotate
//

IRotate* IRotate::create(const float value)
{
	IRotate *ret = new (std::nothrow) IRotate();

	if (ret && ret->initWithRotate(value))
	{
		ret->autorelease();
		return ret;
	}

	delete ret;
	return nullptr;
}

bool IRotate::initWithRotate(const float value)
{
	_rotate = value;
	return true;
}

IRotate * IRotate::clone() const
{
	// no copy constructor
	return IRotate::create(_rotate);
}

IRotate * IRotate::reverse() const
{
	// no reverse, just clone
	return this->clone();
}

void IRotate::update(float time)
{
	CC_UNUSED_PARAM(time);
	_target->setRotation(_rotate);
}

//chien end

NS_CC_END

#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#pragma GCC diagnostic warning "-Wdeprecated-declarations"
#elif _MSC_VER >= 1400 //vs 2005 or higher
#pragma warning (pop)
#endif
