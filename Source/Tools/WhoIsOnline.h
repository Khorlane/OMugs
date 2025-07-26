/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   WhoIsOnline.h                                    *
* Usage:  Read Player files and create statswho.xml        *
* Author: Steve Bryant                                     *
************************************************************/

#ifndef WHO_IS_ONLINE_H
#define WHO_IS_ONLINE_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include <string>           // For std::string
#include <fstream>          // For file operations (ifstream, ofstream)
#include <filesystem>       // For std::filesystem
#include <thread>           // For _endthread
#include <algorithm>        // For std::copy
#include <cstring>          // For strlen
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
    string          Title;
};

#endif