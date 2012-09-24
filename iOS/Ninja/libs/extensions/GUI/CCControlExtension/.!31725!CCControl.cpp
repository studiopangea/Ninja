/*
 * CCControl.m
 *
 * Copyright 2011 Yannick Loriot.
 * http://yannickloriot.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *
 * converted to c++ / cocos2d-x by Angus C
 */

#include "CCControl.h"
#include "CCDirector.h"
#include "touch_dispatcher/CCTouchDispatcher.h"
#include "menu_nodes/CCMenu.h"
#include "touch_dispatcher/CCTouch.h"

NS_CC_EXT_BEGIN

CCControl::CCControl()
{

}

bool CCControl::init()
{
    if (CCLayer::init())
    {
        //this->setTouchEnabled(true);
        //m_bIsTouchEnabled=true;
        // Initialise instance variables
        m_nState=CCControlStateNormal;
        m_bEnabled=true;
        m_bSelected=false;
        m_bHighlighted=false;

        // Set the touch dispatcher priority by default to 1
        m_nDefaultTouchPriority = 1;
        this->setDefaultTouchPriority(m_nDefaultTouchPriority);
        // Initialise the tables
        dispatchTable=new CCDictionary();    
        //dispatchTable->autorelease();
        //   dispatchTable_ = [[NSMutableDictionary alloc] initWithCapacity:1];
        return true;
    }
    else
        return false;
}

CCControl::~CCControl()
{
    CC_SAFE_RELEASE(dispatchTable);
}

    //Menu - Events
void CCControl::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority, true);
}

void CCControl::onEnter()
{
    //CCTouchDispatcher::sharedDispatcher()->addTargetedDelegate(this, m_nDefaultTouchPriority, true);
    CCLayer::onEnter();
}

void CCControl::onExit()
{
    //CCTouchDispatcher::sharedDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void CCControl::sendActionsForControlEvents(CCControlEvent controlEvents)
{
    // For each control events
    for (int i = 0; i < CONTROL_EVENT_TOTAL_NUMBER; i++)
    {
        // If the given controlEvents bitmask contains the curent event
        if ((controlEvents & (1 << i)))
        {
            // Call invocations
            // <CCInvocation*>
            CCArray* invocationList=CCControl::dispatchListforControlEvent(1<<i);
            CCObject* pObj = NULL;
            CCARRAY_FOREACH(invocationList, pObj)
            {
                CCInvocation* invocation = (CCInvocation*)pObj;
                invocation->invoke(this);
            }                 
        }
    }
}
void CCControl::addTargetWithActionForControlEvents(CCObject* target, SEL_CCControlHandler action, CCControlEvent controlEvents)
{
    // For each control events
    for (int i = 0; i < CONTROL_EVENT_TOTAL_NUMBER; i++)
    {
        // If the given controlEvents bitmask contains the curent event
        if ((controlEvents & (1 << i)))
        {
            CCControl::addTargetWithActionForControlEvent(target, action, 1<<i);            
        }
    }
}



/**
 * Adds a target and action for a particular event to an internal dispatch 
 * table.
 * The action message may optionnaly include the sender and the event as 
 * parameters, in that order.
 * When you call this method, target is not retained.
 *
