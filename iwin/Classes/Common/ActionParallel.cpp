#include "ActionParallel.h"
#include "2d/CCActionInterval.h"
USING_NS_CC;

//
// ActionParallel
//

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
ActionParallel* ActionParallel::variadicCreate(FiniteTimeAction *action1, ...)
{
	va_list params;
	va_start(params, action1);

	ActionParallel *ret = ActionParallel::createWithVariableList(action1, params);

	va_end(params);

	return ret;
}
#else
ActionParallel* ActionParallel::create(FiniteTimeAction *action1, ...)
{
	va_list params;
	va_start(params, action1);

	ActionParallel *ret = new ActionParallel();
	ret->initWithListAction(action1, params);

	va_end(params);

	return ret;
}
#endif

void ActionParallel::initWithListAction(FiniteTimeAction *action1, va_list args)
{
	_allDone = false;
	FiniteTimeAction *now;
	_ls_action.push_back(action1);
	action1->retain();
	while (action1)
	{
		now = va_arg(args, FiniteTimeAction*);
		if (now)
		{
			_ls_action.push_back(now);
			now->retain();
		}
		else
		{
			break;
		}
	}
}

ActionParallel* ActionParallel::clone() const
{
	// no copy constructor
	return nullptr;
}

ActionParallel::~ActionParallel(void)
{
	for (FiniteTimeAction* action : _ls_action)
	{
		action->release();
	}
}

void ActionParallel::startWithTarget(Node *target)
{
	
	FiniteTimeAction::startWithTarget(target);
	for (FiniteTimeAction* action : _ls_action)
	{
		//action->startWithTarget(target);
		target->runAction(action);
	}

}

void ActionParallel::stop(void)
{
	for (FiniteTimeAction* action : _ls_action)
	{
		action->stop();
	}

	FiniteTimeAction::stop();
}

void ActionParallel::update(float t)
{
	if (_allDone) return;
	bool allDone = true;
	for (FiniteTimeAction* action : _ls_action)
	{
		if (!action->isDone())
		{
			allDone = false;
			break;
		}
	}
	if (allDone)
	{
		_allDone = allDone;
		int a = 0;
		a = 1;
	}
}


bool ActionParallel::isDone() const 
{
	return _allDone;
}

ActionParallel* ActionParallel::reverse() const
{
	return nullptr;
}
