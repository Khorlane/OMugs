/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   RunStack.h                                       *
* Usage:  Define RunStack class                            *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef RUNSTACK_H
#define RUNSTACK_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Log.h"

using namespace std;

/***********************************************************
* Define RunStack class                                    *
************************************************************/

class RunStack
{

// Public functions
  public:
    RunStack();
    ~RunStack();
    float           Pop();
    void            Push(float PushValue);

// Private variables
  private:
    float          *TopOfStack;
    CString         LogBuf;
    float           Stack[RUN_STACK_SIZE];
};

#endif