#include "AppDelegate.h"

#include <vector>
#include <string>

//#include "HelloWorldScene.h"
#include "MainGame.h"
#include "Features/IapMgr.h"
#include "Features/FacebookMgr.h"

#include "AppMacros.h"

USING_NS_CC;
using namespace std;
float scaleFactorTouch = 1.0f;
AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

void AppDelegate::initGLContextAttrs()
{
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {

	GetFacebookMgr()->init();
	GetIapMgr()->init();

    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if defined _WIN32_
		glview = GLViewImpl::createWithRect("iWin Online", Rect(0, 0, 960, 540));
#else
        glview = GLViewImpl::create("iWin Online");
#endif
        director->setOpenGLView(glview);
    }

    director->setOpenGLView(glview);
    
    designResolutionSize = glview->getDesignResolutionSize();
    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);

    cocos2d::Size frameSize = glview->getFrameSize();
    
    vector<string> searchPath;

    // In this demo, we select resource according to the frame's height.
    // If the resource size is different from design resolution size, you need to set contentScaleFactor.
    // We use the ratio of resource's height to the height of design resolution,
    // this can make sure that the resource's height could fit for the height of design resolution.

    // if the frame's height is larger than the height of medium resource size, select large resource.
	if (frameSize.height > mediumResource.size.height)
	{
		//director->setContentScaleFactor(MIN(largeResource.size.height / designResolutionSize.height, largeResource.size.width / designResolutionSize.width));
	}
	// if the frame's height is larger than the height of small resource size, select medium resource.
	else 
	{
		director->setContentScaleFactor(1.f);
	}
	director->setContentScaleFactor(1.f);
#ifdef _WIN32_
	searchPath.push_back("../../../iwin/Resources");
#endif

	// set searching path
	FileUtils::getInstance()->setSearchPaths(searchPath);

	// turn on display FPS
#ifdef DEBUG 
	director->setDisplayStats(true);
#else
	director->setDisplayStats(false);
#endif


	// set FPS. the default value is 1.0/60 if you don't call this
	director->setAnimationInterval(1.0f / 60);

	// create a scene. it's an autorelease object
	MainGame::GetInstance()->InitForFirstScence(director);

	//auto scene = HelloWorld::scene();

	// run
	//director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}
