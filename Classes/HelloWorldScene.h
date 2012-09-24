#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

#include "GameOverScene.h"

using namespace cocos2d;

class HelloWorld : public cocos2d::CCLayerColor
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    HelloWorld();
    virtual ~HelloWorld();

    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);

    void addTarget();
    void ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);

    void gameLogic(float dt);
    void update(float dt);
    void spriteMoveFinished(CCNode* sender);

    // implement the "static node()" method manually
    LAYER_CREATE_FUNC(HelloWorld);

protected:
    int _projectilesDestroyed;
    int _goal;
    cocos2d::CCArray *_targets;
    cocos2d::CCArray *_projectiles;
};

#endif // __HELLOWORLD_SCENE_H__
