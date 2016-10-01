#include "cocos2d.h"

#ifndef __PARTICLE_CHIP_H__
#define __PARTICLE_CHIP_H__

class ChipParticle
{

private:
	static ChipParticle* _instance;
	cocos2d::Layer* p_layout;
	cocos2d::ParticleSystemQuad* particel;
public:
	static ChipParticle* getInstance()
	{
		if (_instance == nullptr)
		{
			_instance = new ChipParticle();
		}
		return _instance;
	}
	ChipParticle();
	~ChipParticle();
	void visiable(bool isVisiable);
	
};

#endif // __PARTICLE_CHIP_H__