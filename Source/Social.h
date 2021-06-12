/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   Social.h                                         *
* Usage:  Define Social class                              *
* Author: Steve Bryant (khorlane@www.holyquest.org)        *
*                                                          *
* This program belongs to Stephen L. Bryant.               *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef SOCIAL_H
#define SOCIAL_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "stdafx.h"
#include "Dnode.h"

/***********************************************************
* Define Social class                                      *
************************************************************/

class Social
{

// Public functions
  public:
    Social();
    ~Social();
    bool          Social::IsSocial(Dnode *pDnode1, CString CmdStr, CString MudCmd);

// Private functions
  private:
    void          CloseFile();
    bool          OpenFile();
	  bool          PositionNotOk(Dnode *pDnode1);
    int           PosNbr(CString Position);
    void          ReadLine();
	  void          SendToPlayer(Dnode *pDnode1);
	  void          SendToTarget(Dnode *pDnode1);
	  void          Socialize(Dnode *pDnode1, CString CmdStr, CString MudCmd);
	  void          Substitute(Dnode *pDnode1, Dnode *pDnode2);

// Private variables
  private:
    CStdioFile    SocialFile;
	  CString       MsgText;
    CString       MinPos;
};

#endif