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
    WhoIsOnline(CString HomeDir);
    ~WhoIsOnline();

// Private functions
  private:
    void            CloseStatsWho();
    void            OpenStatsWho();
    void            ParsePlayer();
    void            GetPlayers(CString HomeDir);

// Private variables
  private:
    CString         Afk;
    CFileFind       FileList;
    CString         Invisible;
    CString         Level;
    BOOL            MoreFiles;
    CString         Name;
    CString         Online;
    CStdioFile      PlayerFile;
    CString         PlayerFileName;
    CStdioFile      StatsWhoFile;
    CString         StatsWhoFileName;
    CString         Stuff;
    int             Success;
    CString         Title;
};

#endif