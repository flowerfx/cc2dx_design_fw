#ifndef __ACTION_PARALLEL_H__
#define __ACTION_PARALLEL_H__ 
#include "cocos2d.h"

/** @class ActionParallel
* @brief Runs actions sequentially, one after another.
*/
 class ActionParallel : public cocos2d::FiniteTimeAction
{
public:
	std::vector<FiniteTimeAction*> _ls_action;
	bool _allDone;
	/** Helper constructor to create an array of ActionParallelable actions.
	*
	* @return An autoreleased ActionParallel object.
	*/
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	// VS2013 does not support nullptr in variable args lists and variadic templates are also not supported
	typedef FiniteTimeAction* M;
	static ActionParallel* create(M m1, std::nullptr_t listEnd) { return variadicCreate(m1, NULL); }
	static ActionParallel* create(M m1, M m2, std::nullptr_t listEnd) { return variadicCreate(m1, m2, NULL); }
	static ActionParallel* create(M m1, M m2, M m3, std::nullptr_t listEnd) { return variadicCreate(m1, m2, m3, NULL); }
	static ActionParallel* create(M m1, M m2, M m3, M m4, std::nullptr_t listEnd) { return variadicCreate(m1, m2, m3, m4, NULL); }
	static ActionParallel* create(M m1, M m2, M m3, M m4, M m5, std::nullptr_t listEnd) { return variadicCreate(m1, m2, m3, m4, m5, NULL); }
	static ActionParallel* create(M m1, M m2, M m3, M m4, M m5, M m6, std::nullptr_t listEnd) { return variadicCreate(m1, m2, m3, m4, m5, m6, NULL); }
	static ActionParallel* create(M m1, M m2, M m3, M m4, M m5, M m6, M m7, std::nullptr_t listEnd) { return variadicCreate(m1, m2, m3, m4, m5, m6, m7, NULL); }
	static ActionParallel* create(M m1, M m2, M m3, M m4, M m5, M m6, M m7, M m8, std::nullptr_t listEnd) { return variadicCreate(m1, m2, m3, m4, m5, m6, m7, m8, NULL); }
	static ActionParallel* create(M m1, M m2, M m3, M m4, M m5, M m6, M m7, M m8, M m9, std::nullptr_t listEnd) { return variadicCreate(m1, m2, m3, m4, m5, m6, m7, m8, m9, NULL); }
	static ActionParallel* create(M m1, M m2, M m3, M m4, M m5, M m6, M m7, M m8, M m9, M m10, std::nullptr_t listEnd) { return variadicCreate(m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, NULL); }

	// On WP8 for variable argument lists longer than 10 items, use the other create functions or variadicCreate with NULL as the last argument
	static ActionParallel* variadicCreate(FiniteTimeAction* item, ...);
#else
	static ActionParallel* create(FiniteTimeAction *action1, ...) CC_REQUIRES_NULL_TERMINATION;
#endif


	void initWithListAction(FiniteTimeAction *action1, va_list args);
	//
	// Overrides
	//
	virtual ActionParallel* clone() const override;
	virtual ActionParallel* reverse() const override;
	virtual void startWithTarget(cocos2d::Node *target) override;
	virtual void stop(void) override;
	virtual bool isDone()const override;
	/**
	* @param t In seconds.
	*/
	virtual void update(float t) override;

CC_CONSTRUCTOR_ACCESS:
	ActionParallel() {}
	virtual ~ActionParallel(void);

	/** initializes the action */
	bool initWithTwoActions(FiniteTimeAction *pActionOne, FiniteTimeAction *pActionTwo);
	bool init(const cocos2d::Vector<FiniteTimeAction*>& arrayOfActions);

protected:
	FiniteTimeAction *_actions[2];
	float _split;
	int _last;

private:
	CC_DISALLOW_COPY_AND_ASSIGN(ActionParallel);
};

/** @class Repeat
* @brief Repeats an action a number of times.
* To repeat an action forever use the RepeatForever action.
*/

#endif //__ACTION_PARALLEL_H__ 