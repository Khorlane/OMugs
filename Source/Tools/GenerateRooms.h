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
    CString         AlternateExit;
    int             CurrentRoomNbr;
    CString         DatabaseName;
    CFileFind       FileList;
    CStdioFile      GenRoomFile;
    bool            LastStuff1;
    bool            LastStuff2;
    BOOL            MoreFiles;
    int             RowCount1;
    int             RowCount2;
    CString         Message;
    bool            MoreDesc;
    bool            MoreRooms;
    int             n;
    int             RoomCount;
    CString         RoomFileName;
    int             Success;
    CString         Stuff;
    CString         ToRoomName;
    CString         ToRoomNbr;
    CString         TmpStr;

    // Dao stuff
    COleVariant     OleVar;
    CString         SqlStmt1;
    CString         SqlStmt2;

    // Rooms table columns
    int             iRoomNbr;
    CString         iRoomName;
    CString         iRoomType;
    CString         iRoomDesc;

    // Exits table columns
    int             iFromRoomNbr;
    int             iDirection;
    CString         iAlternateExit;
    CString         iExitDesc;
    int             iToRoomNbr;
    CString         iToRoomName;

    // Room fields
    CString         oRoomId;
    CString         oRoomType;
    CString         oTerrain;
    CString         oRoomName;
    CString         oRoomDesc;
    CString         oExitName;
    CString         oExitDesc;
    CString         oExitToRoomId;
};

#endif