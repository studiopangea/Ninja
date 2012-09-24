#include "HelloWorldScene.h"

USING_NS_CC;

// Initialize arrays
// _targets = new CCMutableArray<CCSprite*>;
// _projectiles = new CCMutableArray<CCSprite*>;

HelloWorld::HelloWorld() : _targets(NULL), _projectiles(NULL)
{
	_projectilesDestroyed = 0;
	_goal = 10;
}

HelloWorld::~HelloWorld()
{

  if (_targets)
  {
    _targets->release();
    _targets = NULL;
  }

  if (_projectiles)
  {
    _projectiles->release();
    _projectiles = NULL;
  }

  // cpp don't need to call super dealloc
  // virtual destructor will do this
}


CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();

    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}


// on "init" you need to initialize your instance
bool HelloWorld::init()
{

    //////////////////////////////
    // 1. super init first
	if ( !CCLayerColor::initWithColor( ccc4(255,255,255,255) ) )
	{
	    return false;
	}

	this->setTouchEnabled(true);
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic(
	"background-music-aac.wav", true);

	_targets = new CCArray();
	_projectiles = new CCArray();

	//CCDirector::sharedDirector()->setDisplayFPS(false);

    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(HelloWorld::menuCloseCallback));
    if (CCApplication::sharedApplication().isIos() && !CCApplication::sharedApplication().isIpad())
    {
        pCloseItem->setPosition(ccp(visibleSize.width - 20 + origin.x, 20 + origin.y));
    }
    else
    {
        pCloseItem->setPosition(ccp(visibleSize.width - 40 + origin.x, 40 + origin.y));
    }

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);

    /////////////////////////////
    // 3. add your codes below...
    /*
    // add a label shows "Hello World"
    // create and initialize a label
    CCLabelTTF* pLabel = CCLabelTTF::create("Hello World", "Arial", 24);
    pLabel->setColor(ccc3(0, 0, 255));

    // position the label on the center of the screen
    pLabel->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height - 50 + origin.y));

    // add the label as a child to this layer
    this->addChild(pLabel, 1);

    // add "HelloWorld" splash screen"

    CCSprite* pSprite = CCSprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    pSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(pSprite, 0);
    */
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    CCSprite *player = CCSprite::create("Player.png",
                                                CCRectMake(0, 0, 27, 40) );
    player->setPosition( ccp(player->getContentSize().width/2, winSize.height/2) );
    this->addChild(player);

    this->schedule( schedule_selector(HelloWorld::gameLogic), 1.0 );
    this->schedule( schedule_selector(HelloWorld::update) );

    return true;
}

// cpp with cocos2d-x
void HelloWorld::addTarget()
{

    CCSprite *target = CCSprite::create("Target.png",
        CCRectMake(0,0,27,40) );

    target->setTag(1);
    _targets->addObject(target);

    // Determine where to spawn the target along the Y axis
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    int minY = target->getContentSize().height/2;
    int maxY = winSize.height
                          -  target->getContentSize().height/2;
    int rangeY = maxY - minY;
    // srand( TimGetTicks() );
    int actualY = ( rand() % rangeY ) + minY;

    // Create the target slightly off-screen along the right edge,
    // and along a random position along the Y axis as calculated
    target->setPosition(
        ccp(winSize.width + (target->getContentSize().width/2),
        actualY) );
    this->addChild(target);

    // Determine speed of the target
    int minDuration = (int)2.0;
    int maxDuration = (int)4.0;
    int rangeDuration = maxDuration - minDuration;
    // srand( TimGetTicks() );
    int actualDuration = ( rand() % rangeDuration )
                                        + minDuration;

    // Create the actions
    CCFiniteTimeAction* actionMove =
        CCMoveTo::create( (float)actualDuration,
        ccp(0 - target->getContentSize().width/2, actualY) );
    CCFiniteTimeAction* actionMoveDone =
        CCCallFuncN::create( this,
        callfuncN_selector(HelloWorld::spriteMoveFinished));
    target->runAction( CCSequence::create(actionMove,
        actionMoveDone, NULL) );

}

void HelloWorld::spriteMoveFinished(CCNode* sender)
{
  CCSprite *sprite = (CCSprite *)sender;
  this->removeChild(sprite, true);

  if (sprite->getTag() == 1)  // target
  {
    _targets->removeObject(sprite);
    GameOverScene *gameOverScene = GameOverScene::create();
    gameOverScene->getLayer()->getLabel()->setString("You Lose :[");
    CCDirector::sharedDirector()->replaceScene(gameOverScene);
  }
  else if (sprite->getTag() == 2) // projectile
  {
    _projectiles->removeObject(sprite);
  }
}

void HelloWorld::gameLogic(float dt)
{
    this->addTarget();
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::ccTouchesEnded(CCSet* touches, CCEvent* event)
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(
	"pew-pew-lei.wav");

    // Choose one of the touches to work with
    CCTouch* touch = (CCTouch*)( touches->anyObject() );
    CCPoint location = touch->getLocationInView();
    location = CCDirector::sharedDirector()->convertToGL(location);

    // Set up initial location of projectile
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    CCSprite *projectile = CCSprite::create("Projectile.png",
        CCRectMake(0, 0, 20, 20));
    projectile->setPosition( ccp(20, winSize.height/2) );

    projectile->setTag(2);
    _projectiles->addObject(projectile);

    // Determinie offset of location to projectile
    int offX = location.x - projectile->getPosition().x;
    int offY = location.y - projectile->getPosition().y;

    // Bail out if we are shooting down or backwards
    if (offX <= 0) return;

    // Ok to add now - we've double checked position
    this->addChild(projectile);

    // Determine where we wish to shoot the projectile to
    int realX = winSize.width
                         + (projectile->getContentSize().width/2);
    float ratio = (float)offY / (float)offX;
    int realY = (realX * ratio) + projectile->getPosition().y;
    CCPoint realDest = ccp(realX, realY);

    // Determine the length of how far we're shooting
    int offRealX = realX - projectile->getPosition().x;
    int offRealY = realY - projectile->getPosition().y;
    float length = sqrtf((offRealX * offRealX)
                                        + (offRealY*offRealY));
    float velocity = 480/1; // 480pixels/1sec
    float realMoveDuration = length/velocity;

    // Move projectile to actual endpoint
    projectile->runAction( CCSequence::create(
        CCMoveTo::create(realMoveDuration, realDest),
        CCCallFuncN::create(this,

        callfuncN_selector(HelloWorld::spriteMoveFinished)),
        NULL) );
}


void HelloWorld::update(float dt)
{
  CCArray *projectilesToDelete = CCArray::create();
  CCObject* it = NULL;
  CCObject* jt = NULL;
  CCSprite* projectile = NULL;

  CCARRAY_FOREACH(_projectiles, it)
  {
	  projectile = (CCSprite*)it;

      if(!projectile)
          break;

      // do something
      CCRect projectileRect = CCRectMake(
          projectile->getPosition().x
                            - (projectile->getContentSize().width/2),
          projectile->getPosition().y
                            - (projectile->getContentSize().height/2),
          projectile->getContentSize().width,
          projectile->getContentSize().height);


      CCArray *targetsToDelete = CCArray::create();

      CCARRAY_FOREACH(_targets, jt)
      {
    	  CCSprite *target = (CCSprite*)jt;
    	        CCRect targetRect = CCRectMake(
    	          target->getPosition().x - (target->getContentSize().width/2),
    	          target->getPosition().y - (target->getContentSize().height/2),
    	          target->getContentSize().width,
    	          target->getContentSize().height);

    	        if (projectileRect.intersectsRect(targetRect))
    	        {
    	          targetsToDelete->addObject(target);
    	        }
      }

      CCARRAY_FOREACH(targetsToDelete, jt)
      {
        CCSprite *target = (CCSprite*) jt;
        _targets->removeObject(target);
        this->removeChild(target, true);

        _projectilesDestroyed++;
		  if (_projectilesDestroyed > _goal)
		  {
			GameOverScene *gameOverScene = GameOverScene::create();
			gameOverScene->getLayer()->getLabel()->setString("You Win!");
			CCDirector::sharedDirector()->replaceScene(gameOverScene);
		  }
      }

      if (targetsToDelete->count() >0)
      {
        projectilesToDelete->addObject(projectile);
      }
      targetsToDelete->release();
  }

      CCARRAY_FOREACH(projectilesToDelete, it)
	  {
    	    CCSprite* projectile =(CCSprite*)it;
    	    _projectiles->removeObject(projectile);
    	    this->removeChild(projectile, true);
	  }

      projectilesToDelete->release();


}

