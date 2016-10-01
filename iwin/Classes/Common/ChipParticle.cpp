#include "ChipParticle.h"
#include "DisplayView.h"
USING_NS_CC;
ChipParticle* ChipParticle::_instance = nullptr;
ChipParticle::ChipParticle()
{
	p_layout = nullptr;
}

ChipParticle::~ChipParticle()
{
	p_layout = nullptr;
}

void ChipParticle::visiable(bool isVisiable)
{
	if (!p_layout)
	{
		Scene * scene = Director::getInstance()->getRunningScene();
		p_layout = Layer::create();
		particel = ParticleSystemQuad::create("iWin/Anims/chip.plist");
		particel->setPosition(Vec2(cocos2d::Director::getInstance()->getWinSize().width/2, cocos2d::Director::getInstance()->getWinSize().height*0.8));
		p_layout->addChild(particel);
		p_layout->setZOrder(100);
		scene->addChild(p_layout);
		
	}
	p_layout->setVisible(isVisiable);
	particel->resetSystem();
}
