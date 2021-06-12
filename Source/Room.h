/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   Room.h                                           *
* Usage:  Define Room class                                *
* Author: Steve Bryant (khorlane@www.holyquest.org)        *
*                                                          *
* This program belongs to Stephen L. Bryant.               *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef ROOM_H
#define ROOM_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "stdafx.h"
#include "Dnode.h"

/***********************************************************
* Define Room class                                        *
************************************************************/

class Room
{

// Public functions
  public:
    Room();
    ~Room();
    void          ShowRoom(Dnode *pDnode1);
    bool          IsExit(Dnode *pDnode1, CString CmdStr, CString MudCmd);

// Private functions
  private:
    void          CloseFile();
    void          Move(Dnode *pDnode1);
    bool          OpenFile(Dnode *pDnode1);
    void          ReadLine();
    void          ShowDesc(Dnode *pDnode1);
    void          ShowExitDesc(Dnode *pDnode1);
    void          ShowExits(Dnode *pDnode1);
    void          ShowName(Dnode *pDnode1);
	  CString       TranslateExit(CString ExitName);

// Private variables
  private:
    CString       Stuff;
    CStdioFile    RoomFile;
};

#endif