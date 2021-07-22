/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   WhoIsOnline.h                                    *
* Usage:  Read Player files and create statswho.xml        *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef WHO_IS_ONLINE_H
#define WHO_IS_ONLINE_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"

/***********************************************************
* Define Log class                                         *
************************************************************/

class WhoIsOnline
{

// Public functions
  public:
    WhoIsOnline(string HomeDir);
    ~WhoIsOnline();

// Private functions
  private:
    void            CloseStatsWho();
    void            OpenStatsWho();
    void            ParsePlayer();
    void            GetPlayers(string HomeDir);

// Private variables
  private:
    string          Afk;
    char            Buffer[100];
    string          Invisible;
    string          Level;
    string          Name;
    string          Online;
    ifstream        PlayerFile;
    string          PlayerFileName;
    ofstream        StatsWhoFile;
    string          StatsWhoFileName;
    string          Stuff;
    string          Title;
};

#endif