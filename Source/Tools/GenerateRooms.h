/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   GenerateRooms.h                                  *
* Usage:  Define the GenerateRooms class                   *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef GENERATE_ROOMS_H
#define GENERATE_ROOMS_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Utility.h"

/***********************************************************
* Define GenerateRooms class                               *
************************************************************/

class GenerateRooms
{

// Public functions
  public:
	  GenerateRooms();
	  ~GenerateRooms();

// Private functions
  private:
	  void            CloseRecordsets();
    void            Final();
    void            GetFields1();
    void            GetFields2();
    void            Init();
	  void            OpenRecordsets();
    void            OpenRoomFile();
    void            Parse1();
    void            Parse2();
	  void            Read1();
	  void            Read2();

// Private variables
  private:
    // Work variables
    string          AlternateExit;
    int             CurrentRoomNbr;
    string          DatabaseName;
    ofstream        GenRoomFile;
    bool            LastStuff1;
    bool            LastStuff2;
    BOOL            MoreFiles;
    int             RowCount1;
    int             RowCount2;
    string          Message;
    bool            MoreDesc;
    bool            MoreRooms;
    int             n;
    int             RoomCount;
    string          RoomFileName;
    string          Stuff;
    string          ToRoomName;
    string          ToRoomNbr;
    string          TmpStr;

    // Dao stuff
    COleVariant     OleVar;
    string          SqlStmt1;
    string          SqlStmt2;

    // Rooms table columns
    int             iRoomNbr;
    string          iRoomName;
    string          iRoomType;
    string          iRoomDesc;

    // Exits table columns
    int             iFromRoomNbr;
    int             iDirection;
    string          iAlternateExit;
    string          iExitDesc;
    int             iToRoomNbr;
    string          iToRoomName;

    // Room fields
    string          oRoomId;
    string          oRoomType;
    string          oTerrain;
    string          oRoomName;
    string          oRoomDesc;
    string          oExitName;
    string          oExitDesc;
    string          oExitToRoomId;
};

#endif