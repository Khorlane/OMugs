/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   Communication.h                                  *
* Usage:  Define Communication class                       *
* Author: Steve Bryant (khorlane@www.holyquest.org)        *
*                                                          *
* This program belongs to Stephen L. Bryant.               *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "stdafx.h"
#include "Dnode.h"

/***********************************************************
* Define Communication class                               *
************************************************************/

class Communication
{

// Public functions
  public:
    Communication(int port);
    ~Communication();
	  Dnode         *GetTargetDnode(CString TargetName);
    void          CheckForNewConnections();
    void          RecvIt();
	  void          SendToRoom(Dnode *pDnode1, Dnode *pDnode2, CString MsgText);

// Private functions
  private:
    void          CloseIt(Dnode *pDnode1);
	  void          Color(Dnode *pDnode1);
	  void          DoColor(Dnode *pDnode1, CString CmdStr);
    void          DoGo(Dnode *pDnode1, CString CmdStr);
	  void          DoGroup(Dnode *pDnode1, CString CmdStr);
	  void          DoGsay(Dnode *pDnode1, CString CmdStr);
    void          DoHelp(Dnode *pDnode1, CString CmdStr);
    void          DoLogon(Dnode *pDnode1, CString CmdStr);
    void          DoLook(Dnode *pDnode1, CString CmdStr);
    void          DoMoney(Dnode *pDnode1);
    void          DoQuit(Dnode *pDnode1);
	  void          DoSay(Dnode *pDnode1, CString CmdStr);
	  void          DoSit(Dnode *pDnode1);
	  void          DoStand(Dnode *pDnode1);
	  void          DoStatus(Dnode *pDnode1);
    void          DoStop();
	  void          DoTell(Dnode *pDnode1, CString CmdStr);
	  void          DoTitle(Dnode *pDnode1, CString CmdStr);
    void          DoWho(Dnode *pDnode1);
	  void          GrpLeave(Dnode *pDnode1);
	  void          GrpLeaveMember(Dnode *pDnode1);
	  void          GrpLeaveLeader(Dnode *pDnode1);
	  void          LogonWaitNewCharacter(Dnode *pDnode1, CString CmdStr);
	  void          LogonWaitName(Dnode *pDnode1, CString CmdStr);
	  void          LogonWaitNameConfirmation(Dnode *pDnode1, CString CmdStr);
	  void          LogonWaitPassword(Dnode *pDnode1, CString CmdStr);
	  void          LogonWaitMaleFemale(Dnode *pDnode1, CString CmdStr);
    void          NewConnection(int ListenSocket);
    void          ParseIt(Dnode *pDnode1);
    void          ProcessCommands(Dnode *pDnode1);
    void          SendIt(Dnode *pDnode1, char *arg);

// Private variables
  private:
	  void DoFollow(Dnode *pDnode1, CString CmdStr);
    Descriptor    *FdList;
    Descriptor    *CnList;
    fd_set        ExcSet;
    fd_set        InpSet;
    int           ListenSocket;
    fd_set        OutSet;
};

#endif