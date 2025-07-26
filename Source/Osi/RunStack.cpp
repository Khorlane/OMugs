/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   RunStack.cpp                                     *
* Usage:  Runtime stack for expression evaluation          *
* Author: Steve Bryant                                     *
************************************************************/

/***********************************************************
* Includes                                                 *
************************************************************/

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