/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011 Zynga Inc.
 
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

#include "CCActionInterval.h"
#include "sprite_nodes/CCSprite.h"
#include "base_nodes/CCNode.h"
#include "support/CCPointExtension.h"
#include "CCStdC.h"
#include "CCActionInstant.h"
#include "cocoa/CCZone.h"
#include <stdarg.h>

NS_CC_BEGIN

//
// IntervalAction
//
CCActionInterval* CCActionInterval::actionWithDuration(float d)
{
    return CCActionInterval::create(d);
}

CCActionInterval* CCActionInterval::create(float d)
{
    CCActionInterval *pAction = new CCActionInterval();
    pAction->initWithDuration(d);
    pAction->autorelease();

    return pAction;
}

bool CCActionInterval::initWithDuration(float d)
{
    m_fDuration = d;

    // prevent division by 0
    // This comparison could be in step:, but it might decrease the performance
    // by 3% in heavy based action games.
    if (m_fDuration == 0)
    {
        m_fDuration = FLT_EPSILON;
    }

    m_elapsed = 0;
    m_bFirstTick = true;

    return true;
}

CCObject* CCActionInterval::copyWithZone(CCZone *pZone)
{
    CCZone* pNewZone = NULL;
    CCActionInterval* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (CCActionInterval*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new CCActionInterval();
        pZone = pNewZone = new CCZone(pCopy);
    }

    
    CCFiniteTimeAction::copyWithZone(pZone);

    CC_SAFE_DELETE(pNewZone);

    pCopy->initWithDuration(m_fDuration);

    return pCopy;
}

bool CCActionInterval::isDone(void)
{
    return m_elapsed >= m_fDuration;
}

void CCActionInterval::step(float dt)
{
    if (m_bFirstTick)
    {
        m_bFirstTick = false;
        m_elapsed = 0;
    }
    else
    {
        m_elapsed += dt;
    }
    
    this->update(MAX (0,                                  // needed for rewind. elapsed could be negative
                      MIN(1, m_elapsed /
                          MAX(m_fDuration, FLT_EPSILON)   // division by 0
                          )
                      )
                 );
}

void CCActionInterval::setAmplitudeRate(float amp)
{
    CC_UNUSED_PARAM(amp);
    // Abstract class needs implementation
    CCAssert(0, "");
}

float CCActionInterval::getAmplitudeRate(void)
{
    // Abstract class needs implementation
    CCAssert(0, "");

    return 0;
}

void CCActionInterval::startWithTarget(CCNode *pTarget)
{
    CCFiniteTimeAction::startWithTarget(pTarget);
    m_elapsed = 0.0f;
    m_bFirstTick = true;
}

CCActionInterval* CCActionInterval::reverse(void)
{
    CCAssert(false, "CCIntervalAction: reverse not implemented.");
    return NULL;
}

//
// Sequence
//
CCSequence* CCSequence::actionOneTwo(CCFiniteTimeAction *pActionOne, CCFiniteTimeAction *pActionTwo)
{
    return CCSequence::createWithTwoActions(pActionOne, pActionTwo);
}

CCSequence* CCSequence::createWithTwoActions(CCFiniteTimeAction *pActionOne, CCFiniteTimeAction *pActionTwo)
{
    CCSequence *pSequence = new CCSequence();
    pSequence->initWithTwoActions(pActionOne, pActionTwo);
    pSequence->autorelease();

    return pSequence;
}

CCFiniteTimeAction* CCSequence::actions(CCFiniteTimeAction *pAction1, ...)
{
    va_list params;
    va_start(params, pAction1);

    CCFiniteTimeAction *pNow;
    CCFiniteTimeAction *pPrev = pAction1;

    while (pAction1)
    {
        pNow = va_arg(params, CCFiniteTimeAction*);
        if (pNow)
        {
            pPrev = CCSequence::createWithTwoActions(pPrev, pNow);
        }
        else
        {
            break;
        }
    }

    va_end(params);
    return pPrev;
}

CCFiniteTimeAction* CCSequence::create(CCFiniteTimeAction *pAction1, ...)
{
    va_list params;
    va_start(params, pAction1);

    CCFiniteTimeAction *pNow;
    CCFiniteTimeAction *pPrev = pAction1;

    while (pAction1)
    {
        pNow = va_arg(params, CCFiniteTimeAction*);
        if (pNow)
        {
            pPrev = createWithTwoActions(pPrev, pNow);
        }
        else
        {
            break;
        }
    }

    va_end(params);
    return pPrev;
}

CCFiniteTimeAction* CCSequence::actionWithArray(CCArray* arrayOfActions)
{
    return CCSequence::create(arrayOfActions);
}

CCFiniteTimeAction* CCSequence::create(CCArray* arrayOfActions)
{
    CCFiniteTimeAction* prev = (CCFiniteTimeAction*)arrayOfActions->objectAtIndex(0);

    for (unsigned int i = 1; i < arrayOfActions->count(); ++i)
    {
        prev = createWithTwoActions(prev, (CCFiniteTimeAction*)arrayOfActions->objectAtIndex(i));
    }

    return prev;
}

bool CCSequence::initWithTwoActions(CCFiniteTimeAction *pActionOne, CCFiniteTimeAction *pActionTwo)
{
    CCAssert(pActionOne != NULL, "");
    CCAssert(pActionTwo != NULL, "");

    float d = pActionOne->getDuration() + pActionTwo->getDuration();
    CCActionInterval::initWithDuration(d);

    m_pActions[0] = pActionOne;
    pActionOne->retain();

    m_pActions[1] = pActionTwo;
    pActionTwo->retain();

    return true;
}

CCObject* CCSequence::copyWithZone(CCZone *pZone)
{
    CCZone* pNewZone = NULL;
    CCSequence* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (CCSequence*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new CCSequence();
        pZone = pNewZone = new CCZone(pCopy);
    }

    CCActionInterval::copyWithZone(pZone);

    pCopy->initWithTwoActions((CCFiniteTimeAction*)(m_pActions[0]->copy()->autorelease()), 
                (CCFiniteTimeAction*)(m_pActions[1]->copy()->autorelease()));

    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

CCSequence::~CCSequence(void)
{
    CC_SAFE_RELEASE(m_pActions[0]);
    CC_SAFE_RELEASE(m_pActions[1]);
}

void CCSequence::startWithTarget(CCNode *pTarget)
{
    CCActionInterval::startWithTarget(pTarget);
    m_split = m_pActions[0]->getDuration() / m_fDuration;
    m_last = -1;
}

void CCSequence::stop(void)
{
    // Issue #1305
    if( m_last != - 1)
    {
        m_pActions[m_last]->stop();
    }

    CCActionInterval::stop();
}

void CCSequence::update(float t)
{
    int found = 0;
    float new_t = 0.0f;

    if( t < m_split ) {
        // action[0]
        found = 0;
        if( m_split != 0 )
            new_t = t / m_split;
        else
            new_t = 1;

    } else {
        // action[1]
        found = 1;
        if ( m_split == 1 )
            new_t = 1;
        else
            new_t = (t-m_split) / (1 - m_split );
    }

    if ( found==1 ) {

        if( m_last == -1 ) {
            // action[0] was skipped, execute it.
            m_pActions[0]->startWithTarget(m_pTarget);
            m_pActions[0]->update(1.0f);
            m_pActions[0]->stop();
        }
        else if( m_last == 0 )
        {
            // switching to action 1. stop action 0.
            m_pActions[0]->update(1.0f);
            m_pActions[0]->stop();
        }
    }

    // Last action found and it is done.
    if( found == m_last && m_pActions[found]->isDone() )
    {
        return;
    }

    // New action. Start it.
    if( found != m_last )
    {
        m_pActions[found]->startWithTarget(m_pTarget);
    }

    m_pActions[found]->update(new_t);
    m_last = found;
}

CCActionInterval* CCSequence::reverse(void)
{
    return CCSequence::createWithTwoActions(m_pActions[1]->reverse(), m_pActions[0]->reverse());
}

//
// Repeat
//
CCRepeat* CCRepeat::actionWithAction(CCFiniteTimeAction *pAction, unsigned int times)
{
    return CCRepeat::create(pAction, times);
}

CCRepeat* CCRepeat::create(CCFiniteTimeAction *pAction, unsigned int times)
{
    CCRepeat* pRepeat = new CCRepeat();
    pRepeat->initWithAction(pAction, times);
    pRepeat->autorelease();

    return pRepeat;
}

bool CCRepeat::initWithAction(CCFiniteTimeAction *pAction, unsigned int times)
{
    float d = pAction->getDuration() * times;

    if (CCActionInterval::initWithDuration(d))
    {
        m_uTimes = times;
        m_pInnerAction = pAction;
        pAction->retain();

        m_bActionInstant = dynamic_cast<CCActionInstant*>(pAction) ? true : false;
        //an instant action needs to be executed one time less in the update method since it uses startWithTarget to execute the action
        if (m_bActionInstant) 
        {
            m_uTimes -=1;
        }
        m_uTotal = 0;

        return true;
    }

    return false;
}

CCObject* CCRepeat::copyWithZone(CCZone *pZone)
{
    
    CCZone* pNewZone = NULL;
    CCRepeat* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (CCRepeat*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new CCRepeat();
        pZone = pNewZone = new CCZone(pCopy);
    }

    CCActionInterval::copyWithZone(pZone);

    pCopy->initWithAction((CCFiniteTimeAction*)(m_pInnerAction->copy()->autorelease()), m_uTimes);

    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

CCRepeat::~CCRepeat(void)
{
    CC_SAFE_RELEASE(m_pInnerAction);
}

void CCRepeat::startWithTarget(CCNode *pTarget)
{
    m_uTotal = 0;
    m_fNextDt = m_pInnerAction->getDuration()/m_fDuration;
    CCActionInterval::startWithTarget(pTarget);
    m_pInnerAction->startWithTarget(pTarget);
}

void CCRepeat::stop(void)
{
    m_pInnerAction->stop();
    CCActionInterval::stop();
}

// issue #80. Instead of hooking step:, hook update: since it can be called by any 
// container action like CCRepeat, CCSequence, CCEase, etc..
void CCRepeat::update(float dt)
{
    if (dt >= m_fNextDt)
    {
        while (dt > m_fNextDt && m_uTotal < m_uTimes)
        {

            m_pInnerAction->update(1.0f);
            m_uTotal++;

            m_pInnerAction->stop();
            m_pInnerAction->startWithTarget(m_pTarget);
            m_fNextDt += m_pInnerAction->getDuration()/m_fDuration;
        }

        // fix for issue #1288, incorrect end value of repeat
        if(dt >= 1.0f && m_uTotal < m_uTimes) 
        {
            m_uTotal++;
        }

        // don't set an instant action back or update it, it has no use because it has no duration
        if (!m_bActionInstant)
        {
            if (m_uTotal == m_uTimes)
            {
                m_pInnerAction->update(1);
                m_pInnerAction->stop();
            }
            else
            {
                // issue #390 prevent jerk, use right update
                m_pInnerAction->update(dt - (m_fNextDt - m_pInnerAction->getDuration()/m_fDuration));
            }
        }
    }
    else
    {
        m_pInnerAction->update(fmodf(dt * m_uTimes,1.0f));
    }
}

bool CCRepeat::isDone(void)
{
    return m_uTotal == m_uTimes;
}

CCActionInterval* CCRepeat::reverse(void)
{
    return CCRepeat::create(m_pInnerAction->reverse(), m_uTimes);
}

//
// RepeatForever
//
CCRepeatForever::~CCRepeatForever()
{
    CC_SAFE_RELEASE(m_pInnerAction);
}

CCRepeatForever *CCRepeatForever::actionWithAction(CCActionInterval *pAction)
{
    return CCRepeatForever::create(pAction);
}

CCRepeatForever *CCRepeatForever::create(CCActionInterval *pAction)
{
    CCRepeatForever *pRet = new CCRepeatForever();
    if (pRet && pRet->initWithAction(pAction))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}

bool CCRepeatForever::initWithAction(CCActionInterval *pAction)
{
    CCAssert(pAction != NULL, "");
    pAction->retain();
    m_pInnerAction = pAction;
    return true;
}
CCObject* CCRepeatForever::copyWithZone(CCZone *pZone)
{
    CCZone* pNewZone = NULL;
    CCRepeatForever* pRet = NULL;
    if(pZone && pZone->m_pCopyObject) //in case of being called at sub class
    {
        pRet = (CCRepeatForever*)(pZone->m_pCopyObject);
    }
    else
    {
        pRet = new CCRepeatForever();
        pZone = pNewZone = new CCZone(pRet);
    }
    CCActionInterval::copyWithZone(pZone);
    // win32 : use the m_pOther's copy object.
    pRet->initWithAction((CCActionInterval*)(m_pInnerAction->copy()->autorelease())); 
    CC_SAFE_DELETE(pNewZone);
    return pRet;
}

void CCRepeatForever::startWithTarget(CCNode* pTarget)
{
    CCActionInterval::startWithTarget(pTarget);
    m_pInnerAction->startWithTarget(pTarget);
}

void CCRepeatForever::step(float dt)
{
    m_pInnerAction->step(dt);
    if (m_pInnerAction->isDone())
    {
        float diff = m_pInnerAction->getElapsed() - m_pInnerAction->getDuration();
        m_pInnerAction->startWithTarget(m_pTarget);
        // to prevent jerk. issue #390, 1247
        m_pInnerAction->step(0.0f);
        m_pInnerAction->step(diff);
    }
}

bool CCRepeatForever::isDone()
{
    return false;
}

CCActionInterval *CCRepeatForever::reverse()
{
    return (CCActionInterval*)(CCRepeatForever::create(m_pInnerAction->reverse()));
}

//
// Spawn
//
CCFiniteTimeAction* CCSpawn::actions(CCFiniteTimeAction *pAction1, ...)
{
    va_list params;
    va_start(params, pAction1);

    CCFiniteTimeAction *pNow;
    CCFiniteTimeAction *pPrev = pAction1;

    while (pAction1)
    {
        pNow = va_arg(params, CCFiniteTimeAction*);
        if (pNow)
        {
            pPrev = CCSpawn::createWithTwoActions(pPrev, pNow);
        }
        else
        {
            break;
        }
    }

    va_end(params);
    return pPrev;
}

CCFiniteTimeAction* CCSpawn::create(CCFiniteTimeAction *pAction1, ...)
{
    va_list params;
    va_start(params, pAction1);

    CCFiniteTimeAction *pNow;
    CCFiniteTimeAction *pPrev = pAction1;

    while (pAction1)
    {
        pNow = va_arg(params, CCFiniteTimeAction*);
        if (pNow)
        {
            pPrev = createWithTwoActions(pPrev, pNow);
        }
        else
        {
            break;
        }
    }

    va_end(params);
    return pPrev;
}

CCFiniteTimeAction* CCSpawn::actionWithArray(CCArray *arrayOfActions)
{
    return CCSpawn::create(arrayOfActions);
}

CCFiniteTimeAction* CCSpawn::create(CCArray *arrayOfActions)
{
    CCFiniteTimeAction* prev = (CCFiniteTimeAction*)arrayOfActions->objectAtIndex(0);

    for (unsigned int i = 1; i < arrayOfActions->count(); ++i)
    {
        prev = createWithTwoActions(prev, (CCFiniteTimeAction*)arrayOfActions->objectAtIndex(i));
    }

    return prev;
}

CCSpawn* CCSpawn::actionOneTwo(CCFiniteTimeAction *pAction1, CCFiniteTimeAction *pAction2)
{
    return CCSpawn::createWithTwoActions(pAction1, pAction2);
}

CCSpawn* CCSpawn::createWithTwoActions(CCFiniteTimeAction *pAction1, CCFiniteTimeAction *pAction2)
{
    CCSpawn *pSpawn = new CCSpawn();
    pSpawn->initWithTwoActions(pAction1, pAction2);
    pSpawn->autorelease();

    return pSpawn;
}

bool CCSpawn:: initWithTwoActions(CCFiniteTimeAction *pAction1, CCFiniteTimeAction *pAction2)
{
    CCAssert(pAction1 != NULL, "");
    CCAssert(pAction2 != NULL, "");

    bool bRet = false;

    float d1 = pAction1->getDuration();
    float d2 = pAction2->getDuration();

    if (CCActionInterval::initWithDuration(MAX(d1, d2)))
    {
        m_pOne = pAction1;
        m_pTwo = pAction2;

        if (d1 > d2)
        {
            m_pTwo = CCSequence::createWithTwoActions(pAction2, CCDelayTime::create(d1 - d2));
        } 
        else if (d1 < d2)
        {
            m_pOne = CCSequence::createWithTwoActions(pAction1, CCDelayTime::create(d2 - d1));
        }

        m_pOne->retain();
        m_pTwo->retain();

        bRet = true;
    }

    
    return bRet;
}

CCObject* CCSpawn::copyWithZone(CCZone *pZone)
{
    CCZone* pNewZone = NULL;
    CCSpawn* pCopy = NULL;

    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (CCSpawn*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new CCSpawn();
        pZone = pNewZone = new CCZone(pCopy);
    }

    CCActionInterval::copyWithZone(pZone);

    pCopy->initWithTwoActions((CCFiniteTimeAction*)(m_pOne->copy()->autorelease()), 
                    (CCFiniteTimeAction*)(m_pTwo->copy()->autorelease()));

    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

CCSpawn::~CCSpawn(void)
{
    CC_SAFE_RELEASE(m_pOne);
    CC_SAFE_RELEASE(m_pTwo);
}

void CCSpawn::startWithTarget(CCNode *pTarget)
{
    CCActionInterval::startWithTarget(pTarget);
    m_pOne->startWithTarget(pTarget);
    m_pTwo->startWithTarget(pTarget);
}

void CCSpawn::stop(void)
{
    m_pOne->stop();
    m_pTwo->stop();
    CCActionInterval::stop();
}

void CCSpawn::update(float time)
{
    if (m_pOne)
    {
        m_pOne->update(time);
    }
    if (m_pTwo)
    {
        m_pTwo->update(time);
    }
}

CCActionInterval* CCSpawn::reverse(void)
{
    return CCSpawn::createWithTwoActions(m_pOne->reverse(), m_pTwo->reverse());
}

//
// RotateTo
//
CCRotateTo* CCRotateTo::actionWithDuration(float duration, float fDeltaAngle)
{
    return CCRotateTo::create(duration, fDeltaAngle);
}

CCRotateTo* CCRotateTo::create(float duration, float fDeltaAngle)
{
    CCRotateTo* pRotateTo = new CCRotateTo();
    pRotateTo->initWithDuration(duration, fDeltaAngle);
    pRotateTo->autorelease();

    return pRotateTo;
}

bool CCRotateTo::initWithDuration(float duration, float fDeltaAngle)
{
    if (CCActionInterval::initWithDuration(duration))
    {
        m_fDstAngle = fDeltaAngle;
        return true;
    }

    return false;
}

CCObject* CCRotateTo::copyWithZone(CCZone *pZone)
{
    CCZone* pNewZone = NULL;
    CCRotateTo* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)
    {
        //in case of being called at sub class
        pCopy = (CCRotateTo*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new CCRotateTo();
        pZone = pNewZone = new CCZone(pCopy);
    }

    CCActionInterval::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_fDstAngle);

    //Action *copy = [[[self class] allocWithZone: zone] initWithDuration:[self duration] angle: angle];
    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

void CCRotateTo::startWithTarget(CCNode *pTarget)
{
    CCActionInterval::startWithTarget(pTarget);

    m_fStartAngle = pTarget->getRotation();

    if (m_fStartAngle > 0)
    {
        m_fStartAngle = fmodf(m_fStartAngle, 360.0f);
    }
    else
    {
        m_fStartAngle = fmodf(m_fStartAngle, -360.0f);
    }

    m_fDiffAngle = m_fDstAngle - m_fStartAngle;
    if (m_fDiffAngle > 180)
    {
        m_fDiffAngle -= 360;
    }

    if (m_fDiffAngle < -180)
    {
        m_fDiffAngle += 360;
    }
}

void CCRotateTo::update(float time)
{
    if (m_pTarget)
    {
        m_pTarget->setRotation(m_fStartAngle + m_fDiffAngle * time);
    }
}

//
// RotateBy
//
CCRotateBy* CCRotateBy::actionWithDuration(float duration, float fDeltaAngle)
{
    return CCRotateBy::create(duration, fDeltaAngle);
}

CCRotateBy* CCRotateBy::create(float duration, float fDeltaAngle)
{
    CCRotateBy *pRotateBy = new CCRotateBy();
    pRotateBy->initWithDuration(duration, fDeltaAngle);
    pRotateBy->autorelease();

    return pRotateBy;
}

bool CCRotateBy::initWithDuration(float duration, float fDeltaAngle)
{
    if (CCActionInterval::initWithDuration(duration))
    {
        m_fAngle = fDeltaAngle;
        return true;
    }

    return false;
}

CCObject* CCRotateBy::copyWithZone(CCZone *pZone)
{
    CCZone* pNewZone = NULL;
    CCRotateBy* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (CCRotateBy*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new CCRotateBy();
        pZone = pNewZone = new CCZone(pCopy);
    }

    CCActionInterval::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_fAngle);

    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

void CCRotateBy::startWithTarget(CCNode *pTarget)
{
    CCActionInterval::startWithTarget(pTarget);
    m_fStartAngle = pTarget->getRotation();
}

void CCRotateBy::update(float time)
{
    // XXX: shall I add % 360
    if (m_pTarget)
    {
        m_pTarget->setRotation(m_fStartAngle + m_fAngle * time);
    }
}

CCActionInterval* CCRotateBy::reverse(void)
{
    return CCRotateBy::create(m_fDuration, -m_fAngle);
}

//
// MoveTo
//
CCMoveTo* CCMoveTo::actionWithDuration(float duration, const CCPoint& position)
{
    return CCMoveTo::create(duration, position);
}

CCMoveTo* CCMoveTo::create(float duration, const CCPoint& position)
{
    CCMoveTo *pMoveTo = new CCMoveTo();
    pMoveTo->initWithDuration(duration, position);
    pMoveTo->autorelease();

    return pMoveTo;
}

bool CCMoveTo::initWithDuration(float duration, const CCPoint& position)
{
    if (CCActionInterval::initWithDuration(duration))
    {
        m_endPosition = position;
        return true;
    }

    return false;
}

CCObject* CCMoveTo::copyWithZone(CCZone *pZone)
{
    CCZone* pNewZone = NULL;
    CCMoveTo* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (CCMoveTo*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new CCMoveTo();
        pZone = pNewZone = new CCZone(pCopy);
    }

    CCActionInterval::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_endPosition);

    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

void CCMoveTo::startWithTarget(CCNode *pTarget)
{
    CCActionInterval::startWithTarget(pTarget);
    m_startPosition = pTarget->getPosition();
    m_delta = ccpSub(m_endPosition, m_startPosition);
}

void CCMoveTo::update(float time)
{
    if (m_pTarget)
    {
        m_pTarget->setPosition(ccp(m_startPosition.x + m_delta.x * time,
            m_startPosition.y + m_delta.y * time));
    }
}

//
// MoveBy
//
CCMoveBy* CCMoveBy::actionWithDuration(float duration, const CCPoint& position)
{
    return CCMoveBy::create(duration, position);
}

CCMoveBy* CCMoveBy::create(float duration, const CCPoint& position)
{
    CCMoveBy *pMoveBy = new CCMoveBy();
    pMoveBy->initWithDuration(duration, position);
    pMoveBy->autorelease();

    return pMoveBy;
}

bool CCMoveBy::initWithDuration(float duration, const CCPoint& position)
{
    if (CCActionInterval::initWithDuration(duration))
    {
        m_delta = position;
        return true;
    }

    return false;
}

CCObject* CCMoveBy::copyWithZone(CCZone *pZone)
{
    CCZone* pNewZone = NULL;
    CCMoveBy* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (CCMoveBy*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new CCMoveBy();
        pZone = pNewZone = new CCZone(pCopy);
    }

    CCMoveTo::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_delta);
    
    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

void CCMoveBy::startWithTarget(CCNode *pTarget)
{
    CCPoint dTmp = m_delta;
    CCMoveTo::startWithTarget(pTarget);
    m_delta = dTmp;
}

CCActionInterval* CCMoveBy::reverse(void)
{
    return CCMoveBy::create(m_fDuration, ccp(-m_delta.x, -m_delta.y));
}

//
// CCSkewTo
//
CCSkewTo* CCSkewTo::actionWithDuration(float t, float sx, float sy)
{
    return CCSkewTo::create(t, sx, sy);
}

CCSkewTo* CCSkewTo::create(float t, float sx, float sy)
{
    CCSkewTo *pSkewTo = new CCSkewTo();
    if (pSkewTo)
    {
        if (pSkewTo->initWithDuration(t, sx, sy))
        {
            pSkewTo->autorelease();
        }
        else
        {
            CC_SAFE_DELETE(pSkewTo);
        }
    }

    return pSkewTo;
}

bool CCSkewTo::initWithDuration(float t, float sx, float sy)
{
    bool bRet = false;

    if (CCActionInterval::initWithDuration(t))
    {
        m_fEndSkewX = sx;
        m_fEndSkewY = sy;

        bRet = true;
    }

    return bRet;
}

CCObject* CCSkewTo::copyWithZone(CCZone* pZone)
{
    CCZone* pNewZone = NULL;
    CCSkewTo* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (CCSkewTo*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new CCSkewTo();
        pZone = pNewZone = new CCZone(pCopy);
    }

    CCActionInterval::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_fEndSkewX, m_fEndSkewY);

    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

void CCSkewTo::startWithTarget(CCNode *pTarget)
{
    CCActionInterval::startWithTarget(pTarget);

    m_fStartSkewX = pTarget->getSkewX();

    if (m_fStartSkewX > 0)
    {
        m_fStartSkewX = fmodf(m_fStartSkewX, 180.f);
    }
    else
    {
        m_fStartSkewX = fmodf(m_fStartSkewX, -180.f);
    }

    m_fDeltaX = m_fEndSkewX - m_fStartSkewX;

    if (m_fDeltaX > 180)
    {
        m_fDeltaX -= 360;
    }
    if (m_fDeltaX < -180)
    {
        m_fDeltaX += 360;
    }

    m_fStartSkewY = pTarget->getSkewY();

    if (m_fStartSkewY > 0)
    {
        m_fStartSkewY = fmodf(m_fStartSkewY, 360.f);
    }
    else
    {
        m_fStartSkewY = fmodf(m_fStartSkewY, -360.f);
    }

    m_fDeltaY = m_fEndSkewY - m_fStartSkewY;

    if (m_fDeltaY > 180)
    {
        m_fDeltaY -= 360;
    }
    if (m_fDeltaY < -180)
    {
        m_fDeltaY += 360;
    }
}

void CCSkewTo::update(float t)
{
    m_pTarget->setSkewX(m_fStartSkewX + m_fDeltaX * t);
    m_pTarget->setSkewY(m_fStartSkewY + m_fDeltaY * t);
}

CCSkewTo::CCSkewTo()
: m_fSkewX(0.0)
, m_fSkewY(0.0)
, m_fStartSkewX(0.0)
, m_fStartSkewY(0.0)
, m_fEndSkewX(0.0)
, m_fEndSkewY(0.0)
, m_fDeltaX(0.0)
, m_fDeltaY(0.0)
{
}

//
// CCSkewBy
//
CCSkewBy* CCSkewBy::actionWithDuration(float t, float sx, float sy)
{
    return CCSkewBy::create(t, sx, sy);
}

CCSkewBy* CCSkewBy::create(float t, float sx, float sy)
{
    CCSkewBy *pSkewBy = new CCSkewBy();
    if (pSkewBy)
    {
        if (pSkewBy->initWithDuration(t, sx, sy))
        {
            pSkewBy->autorelease();
        }
        else
        {
            CC_SAFE_DELETE(pSkewBy);
        }
    }

    return pSkewBy;
}

bool CCSkewBy::initWithDuration(float t, float deltaSkewX, float deltaSkewY)
{
    bool bRet = false;

    if (CCSkewTo::initWithDuration(t, deltaSkewX, deltaSkewY))
    {
        m_fSkewX = deltaSkewX;
        m_fSkewY = deltaSkewY;

        bRet = true;
    }

    return bRet;
}

void CCSkewBy::startWithTarget(CCNode *pTarget)
{
    CCSkewTo::startWithTarget(pTarget);
    m_fDeltaX = m_fSkewX;
    m_fDeltaY = m_fSkewY;
    m_fEndSkewX = m_fStartSkewX + m_fDeltaX;
    m_fEndSkewY = m_fStartSkewY + m_fDeltaY;
}

CCActionInterval* CCSkewBy::reverse()
{
    return create(m_fDuration, -m_fSkewX, -m_fSkewY);
}

//
// JumpBy
//
CCJumpBy* CCJumpBy::actionWithDuration(float duration, const CCPoint& position, float height, unsigned int jumps)
{
    return CCJumpBy::create(duration, position, height, jumps);
}

CCJumpBy* CCJumpBy::create(float duration, const CCPoint& position, float height, unsigned int jumps)
{
    CCJumpBy *pJumpBy = new CCJumpBy();
    pJumpBy->initWithDuration(duration, position, height, jumps);
    pJumpBy->autorelease();

    return pJumpBy;
}

bool CCJumpBy::initWithDuration(float duration, const CCPoint& position, float height, unsigned int jumps)
{
    if (CCActionInterval::initWithDuration(duration))
    {
        m_delta = position;
        m_height = height;
        m_nJumps = jumps;

        return true;
    }

    return false;
}

CCObject* CCJumpBy::copyWithZone(CCZone *pZone)
{
    CCZone* pNewZone = NULL;
    CCJumpBy* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (CCJumpBy*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new CCJumpBy();
        pZone = pNewZone = new CCZone(pCopy);
    }

    CCActionInterval::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_delta, m_height, m_nJumps);
    
    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

void CCJumpBy::startWithTarget(CCNode *pTarget)
{
    CCActionInterval::startWithTarget(pTarget);
    m_startPosition = pTarget->getPosition();
}

void CCJumpBy::update(float time)
{
    // parabolic jump (since v0.8.2)
    if (m_pTarget)
    {
        float frac = fmodf(time * m_nJumps, 1.0f);
        float y = m_height * 4 * frac * (1 - frac);
        y += m_delta.y * time;
        float x = m_delta.x * time;
        m_pTarget->setPosition(ccp(m_startPosition.x + x, m_startPosition.y + y));
    }
}

CCActionInterval* CCJumpBy::reverse(void)
{
    return CCJumpBy::create(m_fDuration, ccp(-m_delta.x, -m_delta.y),
        m_height, m_nJumps);
}

//
// JumpTo
//
CCJumpTo* CCJumpTo::actionWithDuration(float duration, const CCPoint& position, float height, int jumps)
{
    return CCJumpTo::create(duration, position, height, jumps);
}

CCJumpTo* CCJumpTo::create(float duration, const CCPoint& position, float height, int jumps)
{
    CCJumpTo *pJumpTo = new CCJumpTo();
    pJumpTo->initWithDuration(duration, position, height, jumps);
    pJumpTo->autorelease();

    return pJumpTo;
}

CCObject* CCJumpTo::copyWithZone(CCZone* pZone)
{
    CCZone* pNewZone = NULL;
    CCJumpTo* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)
    {
        //in case of being called at sub class
        pCopy = (CCJumpTo*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new CCJumpTo();
        pZone = pNewZone = new CCZone(pCopy);
    }

    CCJumpBy::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_delta, m_height, m_nJumps);
    
    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

void CCJumpTo::startWithTarget(CCNode *pTarget)
{
    CCJumpBy::startWithTarget(pTarget);
    m_delta = ccp(m_delta.x - m_startPosition.x, m_delta.y - m_startPosition.y);
}

// Bezier cubic formula:
//    ((1 - t) + t)3 = 1 
