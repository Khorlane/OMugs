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

#include "stdafx.h"
#include "WhoIsOnline.h"

/***********************************************************
* WhoIsOnline constructor                                  *
************************************************************/

WhoIsOnline::WhoIsOnline(CString HomeDir)
{
  GetPlayers(HomeDir);
}

/***********************************************************
* Log destructor                                           *
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
  StatsWhoFile.WriteString(Stuff);
  StatsWhoFile.Close();
}

/***********************************************************
 * Open StatsWho file                                      *
 ***********************************************************/
void WhoIsOnline::OpenStatsWho()
{
  StatsWhoFileName  = WEB_SITE_DIR;
  StatsWhoFileName += "StatsWho.xml";
  Success = StatsWhoFile.Open(StatsWhoFileName,
                   CFile::modeCreate |
                   CFile::modeWrite  |
                   CFile::typeText);
  if(!Success)
  { // Create file failed
    AfxMessageBox("WhoIsOnline::OpenStatsWho - Create StatsWho file failed", MB_ICONSTOP);
    _endthread();
  }
  // Write first line of xml
  Stuff = "<webstats>";
  Stuff += "\n";
  StatsWhoFile.WriteString(Stuff);
}

/***********************************************************
 * Get players                                             *
 ***********************************************************/

void WhoIsOnline::GetPlayers(CString HomeDir)
{
  OpenStatsWho();
  // Change to home directory so that following change to player directory will work
  if (ChgDir((LPCTSTR)HomeDir))
  { // Change directory failed
    AfxMessageBox("WhoIsOnline::GetPlayers - Change to home directory failed", MB_ICONSTOP);
    _endthread();
  }
  // Change to player directory to get file list
  if (ChgDir(PLAYER_DIR))
  { // Change directory failed
    AfxMessageBox("WhoIsOnline::GetPlayers - Change to player directory failed", MB_ICONSTOP);
    _endthread();
  }
  MoreFiles = FileList.FindFile("*.*");
  // Change back to home directory
  if (ChgDir((LPCTSTR)HomeDir))
  { // Change directory failed
    AfxMessageBox("WhoIsOnline::GetPlayers - Change to home directory failed", MB_ICONSTOP);
    _endthread();
  }
  // FileList now contains a list of all players
  while (MoreFiles)
  { // For each player file
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    { // Skip directories
      continue;
    }
    PlayerFileName = FileList.GetFileName();
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
  Success = PlayerFile.Open(PlayerFileName,
                 CFile::modeRead |
                 CFile::typeText);
  if(!Success)
  { // We don't care, just return
    return;
  }
  Name      = "";
  Afk       = "";
  Invisible = "";
  Level     = "";
  Online    = "";
  Title     = "";
  PlayerFile.ReadString(Stuff);
  while (Stuff != "")
  { // Name
    if (Stuff.Left(5) == "Name:")
    {
      Name = Stuff.Right(Stuff.GetLength()-5);
    }
    else
    // AFK
    if(Stuff.Left(4) == "AFK:")
    {
      Afk = Stuff.Right(Stuff.GetLength()-4);
    }
    else
    // Invisible
    if (Stuff.Left(10) == "Invisible:")
    {
      Invisible = Stuff.Right(Stuff.GetLength()-10);
    }
    else
    // Level
    if (Stuff.Left(6) == "Level:")
    {
      Level = Stuff.Right(Stuff.GetLength()-6);
    }
    else
    // Online
    if (Stuff.Left(7) == "Online:")
    {
      Online = Stuff.Right(Stuff.GetLength()-7);
    }
    else
    // Title
    if (Stuff.Left(6) == "Title:")
    {
      Title = Stuff.Right(Stuff.GetLength()-6);
    }
    PlayerFile.ReadString(Stuff);
  }
  PlayerFile.Close();
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
  if (Title.GetLength() < 1)
  { // No title
    Stuff += "_";
  }
  else
  { // Title - strip out color codes
    Title.Replace("&N", "");
    Title.Replace("&K", "");
    Title.Replace("&R", "");
    Title.Replace("&G", "");
    Title.Replace("&Y", "");
    Title.Replace("&B", "");
    Title.Replace("&M", "");
    Title.Replace("&C", "");
    Title.Replace("&W", "");
    Stuff += Title;
  }
  Stuff += "</title>";
  Stuff += "\n";
  // End stat
  Stuff += "</stat>";
  Stuff += "\n";
  // Write it
  StatsWhoFile.WriteString(Stuff);
}