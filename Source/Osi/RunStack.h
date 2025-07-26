/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   RunStack.h                                       *
* Usage:  Define RunStack class                            *
* Author: Steve Bryant                                     *
************************************************************/

#ifndef RUNSTACK_H
#define RUNSTACK_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Log.h"

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
    float           Stack[RUN_STACK_SIZE];
};

#endif