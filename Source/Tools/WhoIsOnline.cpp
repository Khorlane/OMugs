/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   WhoIsOnline.cpp                                  *
* Usage:  Read Player files and create statswho.xml        *
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
#include "WhoIsOnline.h"

/***********************************************************
* WhoIsOnline constructor                                  *
************************************************************/

WhoIsOnline::WhoIsOnline(string HomeDir1)
{
  HomeDir = HomeDir1;
  GetPlayers(HomeDir);
}

/***********************************************************
* WhoIsOnline destructor                                   *
************************************************************/

WhoIsOnline::~WhoIsOnline()
{
}

/***********************************************************
 * Close StatsWho file                                     *
 ***********************************************************/

void WhoIsOnline::CloseStatsWho()
{
  // Write last line of xml
  Stuff  = "</webstats>";
  Stuff += "\n";
  copy(Stuff.begin(), Stuff.end(), Buffer);
  Buffer[Stuff.size()] = '\0';
  StatsWhoFile.write(Buffer, strlen(Buffer));
  StatsWhoFile.close();
}

/***********************************************************
 * Open StatsWho file                                      *
 ***********************************************************/

void WhoIsOnline::OpenStatsWho()
{
  StatsWhoFileName  = WEB_SITE_DIR;
  StatsWhoFileName += "StatsWho.xml";
  StatsWhoFile.open(StatsWhoFileName);
  if (!StatsWhoFile.is_open())
  { // Create file failed
    LogIt("WhoIsOnline::OpenStatsWho - Create StatsWho file failed");
    _endthread();
  }
  // Write first line of xml
  Stuff = "<webstats>";
  Stuff += "\n";
  copy(Stuff.begin(), Stuff.end(), Buffer);
  Buffer[Stuff.size()] = '\0';
  StatsWhoFile.write(Buffer, strlen(Buffer));
}

/***********************************************************
 * Get players                                             *
 ***********************************************************/

void WhoIsOnline::GetPlayers(string HomeDir1)
{
  HomeDir = HomeDir1;
  OpenStatsWho();
  for (const auto &entry : filesystem::directory_iterator(PLAYER_DIR))
  {
    PlayerFileName = entry.path().filename().string();
    ParsePlayer();
  }
  CloseStatsWho();
}

/***********************************************************
 * Parse player                                            *
 ***********************************************************/

void WhoIsOnline::ParsePlayer()
{ 
  PlayerFileName = PLAYER_DIR + PlayerFileName;
  PlayerFile.open(PlayerFileName);
  if (!PlayerFile.is_open())
  { // We don't care, just return
    return;
  }
  Name      = "";
  Afk       = "";
  Invisible = "";
  Level     = "";
  Online    = "";
  Title     = "";
  getline(PlayerFile, Stuff);
  while (PlayerFile.peek() != EOF)
  { // Name
    if (StrLeft(Stuff, 5) == "Name:")
    {
      Name = StrRight(Stuff, StrGetLength(Stuff) - 5);
    }
    else
    // AFK
    if (StrLeft(Stuff, 4) == "AFK:")
    {
      Afk = StrRight(Stuff, StrGetLength(Stuff) - 4);
    }
    else
    // Invisible
    if (StrLeft(Stuff, 10) == "Invisible:")
    {
      Invisible = StrRight(Stuff, StrGetLength(Stuff) - 10);
    }
    else
    // Level
    if (StrLeft(Stuff, 6) == "Level:")
    {
      Level = StrRight(Stuff, StrGetLength(Stuff) - 6);
    }
    else
    // Online
    if (StrLeft(Stuff, 7) == "Online:")
    {
      Online = StrRight(Stuff, StrGetLength(Stuff) - 7);
    }
    else
    // Title
    if (StrLeft(Stuff, 6) == "Title:")
    {
      Title = StrRight(Stuff, StrGetLength(Stuff) - 6);
    }
    getline(PlayerFile, Stuff);
  }
  PlayerFile.close();
  if (Online != "Yes")
  {
    return;
  }
  if (Invisible == "Yes")
  {
    return;
  }
  // Begin stat
  Stuff  = "<stat>";
  Stuff += "\n";
  // Name
  Stuff += "<name>";
  Stuff += Name;
  Stuff += "</name>";
  Stuff += "\n";
  // Level
  Stuff += "<Level>";
  Stuff += Level;
  Stuff += "</Level>";
  Stuff += "\n";
  // AFK
  Stuff += "<afk>";
  if (Afk == "Yes")
  { // Player AFK
    Stuff += "AFK";        
  }
  else
  { // Player not AFK
    Stuff += "_";
  }
  Stuff += "</afk>";
  Stuff += "\n";
  // Title
  Stuff += "<title>";
  if (StrGetLength(Title) < 1)
  { // No title
    Stuff += "_";
  }
  else
  { // Title - strip out color codes
    StrReplace(Title, "&N", "");
    StrReplace(Title, "&K", "");
    StrReplace(Title, "&R", "");
    StrReplace(Title, "&G", "");
    StrReplace(Title, "&Y", "");
    StrReplace(Title, "&B", "");
    StrReplace(Title, "&M", "");
    StrReplace(Title, "&C", "");
    StrReplace(Title, "&W", "");
    Stuff += Title;
  }
  Stuff += "</title>";
  Stuff += "\n";
  // End stat
  Stuff += "</stat>";
  Stuff += "\n";
  // Write it
  copy(Stuff.begin(), Stuff.end(), Buffer);
  Buffer[Stuff.size()] = '\0';
  StatsWhoFile.write(Buffer, strlen(Buffer));
}