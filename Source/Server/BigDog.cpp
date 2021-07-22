/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   BigDog.cpp                                       *
* Usage:  Starting point for all OMugs stuff.              *
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
#include "BigDog.h"

/***********************************************************
 * BigDog                                                  *
 ***********************************************************/

void BigDog()
{
  WhoIsOnline *pWhoIsOnline;
  int          EventTick;
  string       GoGoGoFileName;
  string       LogBuf;
  int          MobHealTick;
  string       StopItFileName;
  int          WhoIsOnlineTick;

  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("BigDog - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  // Set Go Stop, force go status
  StopItFileName  = CONTROL_DIR;
  StopItFileName += "StopIt";
  GoGoGoFileName  = CONTROL_DIR;
  GoGoGoFileName += "GoGoGo";
  if (FileExist(StopItFileName))
  { // If StopIt file exists, Rename it to GoGoGo
    Rename(StopItFileName, GoGoGoFileName);
  }
  // Log game startup
  Log::OpenLogFile();
  LogBuf  = "OMugs version ";
  LogBuf += VERSION ;
  LogBuf += " has started";
  Log::LogIt(LogBuf);
  LogBuf  = "Home directory is ";
  LogBuf += HomeDir;
  Log::LogIt(LogBuf);
  // Initialize
  EventTick        = EVENT_TICK;    // Force first tick at startup
  MobHealTick      = 0;
  WhoIsOnlineTick  = 0;
  StateConnections = true;
  StateRunning     = true;
  StateStopping    = false;
  srand((unsigned)time(NULL)); // Seed random number generator
  PACMN = 1.0f / MAC * MDRP / 100.0f;
  // Validate library files
  ValErr = Validate::ValidateIt("All");
  if (ValErr)
  { // Validation failed
    LogBuf = "OMugs has stopped";
    Log::LogIt(LogBuf);
    Log::CloseLogFile();
    return;
  }
  // Validation was ok, so open port, init, play on
  Communication::SockOpenPort(PORT_NBR);
  Descriptor::InitDescriptor();
  pCalendar = new Calendar;
  while (StateRunning)
  { // Game runs until it is stopped
    Sleep(MILLI_SECONDS_TO_SLEEP);
    pCalendar->AdvanceTime();
    if (!StateStopping)
    { // Game is not stopping, but should it be?
      if (FileExist(StopItFileName))
      { // StopIt file was found, Stop the game
        StateStopping = true;
        LogBuf = "Game is stopping";
        Log::LogIt(LogBuf);
      }
    }
    if (!StateStopping)
    { // No new connections after stop command
      Communication::SockCheckForNewConnections();
      if (StateConnections && Dnode::GetCount() == 1)
      { // No players connected
        LogBuf = "No Connections - going to sleep";
        Log::LogIt(LogBuf);
        StateConnections = false;
      }
    }
    if (StateConnections)
    { // One or more players are connected
      Communication::SockRecv();
      EventTick++;
      if (EventTick >= EVENT_TICK)
      { // Time to process events
        EventTick = 0;
        World::Events();
      }
      MobHealTick++;
      if (MobHealTick >= MOB_HEAL_TICK)
      { // Time to process events
        MobHealTick = 0;
        World::HealMobiles();
      }
    }
    else
    { // No connections
      if (StateStopping)
      { // Game is stopping
        StateRunning = false;
      }
    }
    // Who is online?
    WhoIsOnlineTick++;
    if (WhoIsOnlineTick >= WHO_IS_ONLINE_TICK)
    { // Time to see who is online
      WhoIsOnlineTick = 0;
      pWhoIsOnline = new WhoIsOnline(HomeDir);
      delete pWhoIsOnline;
    }
  }
  // Game has stopped so clean up
  Descriptor::ClearDescriptor();
  Communication::SockClosePort(PORT_NBR);
  pWhoIsOnline = new WhoIsOnline(HomeDir);
  delete pWhoIsOnline;
  delete pCalendar;
  LogBuf = "OMugs has stopped";
  Log::LogIt(LogBuf);
  Log::CloseLogFile();
}