/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   RunStack.cpp                                     *
* Usage:  Runtime stack for expression evaluation          *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

/***********************************************************
* Includes                                                 *
************************************************************/

#include "stdafx.h" // This is only here to speed up compiles
#include "RunStack.h"

/***********************************************************
* Globals                                                  *
************************************************************

/***********************************************************
* RunStack constructor                                     *
************************************************************/

RunStack::RunStack()
{
  TopOfStack = &Stack[RUN_STACK_SIZE-1];
}

/***********************************************************
* RunStack destructor                                      *
************************************************************/

RunStack::~RunStack()
{
}

/***********************************************************
* Pop a value off of the stack                             *
************************************************************/
float RunStack::Pop()
{
  TopOfStack++;
  return *TopOfStack;
}

/***********************************************************
* Push a value onto the stack                              *
************************************************************/
void RunStack::Push(float PushValue)
{
  if (TopOfStack >= &Stack[0])
  {
    *TopOfStack = PushValue;
    TopOfStack--;
  }
  else
  {
    LogBuf  = "Osi - RunStack::Push - Runtime stack size exceeded - ";
    LogBuf += ScriptFileName;
    LogIt(LogBuf);
    _endthread();
  }
}