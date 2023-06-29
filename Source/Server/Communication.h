/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Communication.h                                  *
* Usage:  Define Communication class                       *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Color.h"
#include "Calendar.h"
#include "Descriptor.h"
#include "Dnode.h"
#include "Help.h"
#include "Log.h"
#include "Mobile.h"
#include "Object.h"
#include "Player.h"
#include "Room.h"
#include "Shop.h"
#include "Social.h"
#include "Utility.h"
#include "Violence.h"
#include "World.h"

/***********************************************************
* Define Communication class                               *
************************************************************/

class Communication
{

// Public functions static
  public:
    Communication();
    ~Communication();
    Dnode   static *GetTargetDnode(CString TargetName);
    Dnode   static *GetTargetDnode( string TargetName);
    bool    static  IsFighting();
    bool    static  IsSleeping();
    void    static  SendToAll(CString PlayerMsg, CString AllMsg);
    void    static  SendToAll(CString PlayerMsg,  string AllMsg);
    void    static  SendToAll( string PlayerMsg,  string AllMsg);
    void    static  SendToRoom(CString TargetRoomId, CString MsgText);
    void    static  SendToRoom(CString TargetRoomId,  string MsgText);
    void    static  SendToRoom( string TargetRoomId,  string MsgText);
    void    static  ShowPlayersInRoom(Dnode *pDnode);
    void    static  SockCheckForNewConnections();
    void    static  SockClosePort(int Port);
    void    static  SockOpenPort(int Port);
    void    static  SockRecv();

// Private functions static
  private:
    void    static  Color();
    void    static  CommandArrayLoad();
    string  static  CommandCheck(string MudCmdChk);
    void    static  CommandParse();
    void    static  DoAdvance();
    void    static  DoAfk();
    void    static  DoAssist();
    void    static  DoBuy();
    void    static  DoChat();
    void    static  DoColor();
    void    static  DoConsider();
    void    static  DoDelete();
    void    static  DoDestroy();
    void    static  DoDrink();
    void    static  DoDrop();
    void    static  DoEat();
    void    static  DoEmote();
    void    static  DoEquipment();
    void    static  DoExamine();
    void    static  DoFlee();
    void    static  DoFollow(Dnode *pDnode, CString CmdStr);
    void    static  DoGet();
    void    static  DoGive();
    void    static  DoGo();
    void    static  DoGoTo();
    void    static  DoGoToArrive();
    void    static  DoGoToDepart();
    void    static  DoGroup();
    void    static  DoGsay();
    void    static  DoHail();
    void    static  DoHelp();
    void    static  DoInventory();
    void    static  DoInvisible();
    void    static  DoKill();
    void    static  DoList();
    void    static  DoLoad();
    void    static  DoLogon();
    void    static  DoLook(CString CmdStr);
    void    static  DoMoney();
    void    static  DoMotd();
    void    static  DoOneWhack();
    void    static  DoPassword();
    void    static  DoPlayed();
    void    static  DoQuit();
    void    static  DoRefresh();
    void    static  DoRemove();
    void    static  DoRestore(CString CmdStr);
    void    static  DoRoomInfo();
    void    static  DoSave();
    void    static  DoSay();
    void    static  DoSell();
    void    static  DoShow();
    void    static  DoSit();
    void    static  DoSleep();
    void    static  DoStand();
    void    static  DoStatus();
    void    static  DoStop();
    void    static  DoTell();
    void    static  DoTime();
    void    static  DoTitle();
    void    static  DoTrain();
    void    static  DoWake();
    void    static  DoWear();
    void    static  DoWhere();
    void    static  DoWho();
    void    static  DoWield();
    void    static  GrpExperience(int MobileExpPoints, int MobileLevel);
    void    static  GrpLeave();
    void    static  GrpLeaveLeader();
    void    static  GrpLeaveMember();
    void    static  LogonGreeting();
    void    static  LogonWaitMaleFemale();
    void    static  LogonWaitName();
    void    static  LogonWaitNameConfirmation();
    void    static  LogonWaitNewCharacter();
    void    static  LogonWaitPassword();
    void    static  RepositionDnodeCursor();
    void    static  SockNewConnection();
    void    static  SockSend(const char *arg);
    void    static  UpdatePlayerStats();
    void    static  Violence();
    void    static  ViolenceMobile();
    void    static  ViolenceMobileDied(CString MobBeenWhacked, CString MobileDesc1, CString MobileId);
    void    static  ViolenceMobileLoot(CString Loot);
    bool    static  ViolenceMobileLootHandOut(CString Loot);
    void    static  ViolenceMobileMore();
    void    static  ViolencePlayer();
    void    static  ViolencePlayerDied(CString MobileDesc1);
};

#endif