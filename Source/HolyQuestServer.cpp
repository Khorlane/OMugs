/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   HolyQuestServer.cpp                              *
* Usage:  Starting point for all HolyQuest stuff.          *
* Author: Steve Bryant (khorlane@www.holyquest.org)        *
*                                                          *
* This program belongs to Stephen L. Bryant.               *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

/***********************************************************
* Includes                                                 *
************************************************************/

#include "stdafx.h"
#include "Config.h"
#include "Log.h"
#include "Communication.h"
#include "Descriptor.h"
#include "Dnode.h"
#include "Social.h"

/***********************************************************
* Globals                                                  *
************************************************************/

bool          StateRunning;
bool          StateConnections;
Communication *pComm;
Help          *pHelp;
Log           *pLog;
Room          *pRoom;
Social        *pSocial;

/************************************************************
 * HolyQuest Server                                         *
 ************************************************************/

int HolyQuestServer()
{
  int MilliSecondsToSleep;

  pLog                = new Log;
  StateRunning        = true;
  MilliSecondsToSleep = 250;
  pLog->OpenFile();
  pLog->LogIt("HolyQuest server has started");
  pComm = new Communication(PORT_NBR);
  while (StateRunning)
  {
    Sleep(MilliSecondsToSleep);
    pComm->CheckForNewConnections();
    pComm->RecvIt();
  }
  pLog->LogIt("HolyQuest server has stopped");
  pLog->CloseFile();
	return 0;
}