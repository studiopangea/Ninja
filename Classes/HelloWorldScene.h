//
//  Created by Gonzalo Diaz Cruz on 04-09-12.
//  Copyright Studio Pangea 2012. All rights reserved.
//  http://www.studiopangea.com/
//

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
    
    virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch* touch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch* touch, CCEvent* event);
    virtual void registerWithTouchDispatcher();
    
    void gameLogic(float dt);
    void update(float dt);
    void spriteMoveFinished(CCNode* sender);

    virtual void keyBackClicked();

    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);

protected:
    int _projectilesDestroyed;
    int _goal;
    cocos2d::CCArray *_targets;
    cocos2d::CCArray *_projectiles;
};

#endif // __HELLOWORLD_SCENE_H__
