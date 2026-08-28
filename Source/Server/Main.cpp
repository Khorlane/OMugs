/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Main.cpp                                         *
* Usage:  Command-line entry point for the OMugs server    *
* Author: Steve Bryant                                     *
************************************************************/

/***********************************************************
* Includes                                                 *
************************************************************/

#include "BigDog.h"

/***********************************************************
* Main                                                     *
************************************************************/

int main()
{
  AppTestCode();
  HomeDir = GetHomeDir();
  BigDog();
  return 0;
}
