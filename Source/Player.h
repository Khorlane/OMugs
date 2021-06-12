/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   Player.h                                         *
* Usage:  Define Player class                              *
* Author: Steve Bryant (khorlane@www.holyquest.org)        *
*                                                          *
* This program belongs to Stephen L. Bryant.               *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef PLAYER_H
#define PLAYER_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "stdafx.h"
#include "Config.h"

/***********************************************************
* Define Player class                                      *
************************************************************/

class Player
{

// Friend classes
  friend class PlayerList;

// Public functions static
  public:
	  static  bool  IsNew(CString InpName);
    static  int   GetCount();
    static  bool  ValidName(CString Name);

// Public functions
  public:
    Player();
    ~Player();
	  void          CreatePrompt();
	  CString       GetOutput();
    void          ParseStuff(CString Name);
    void          Save();
    void          SetMoney(char PlusMinus, int Amount, CString Metal);
    void          ShowMoney();
	  void          ShowStatus();

// Private functions
  private:
    void          CloseFile();
    bool          OpenFile(CString Name, CString Mode);
    CString       ReadLine();
    void          WriteLine(CString Stuff);

// Public variables
  public:
    CString       Position;
    CString       RoomId;
    Player        *pPlayerGrpMember[GRP_LIMIT];
    Player        *pPlayerFollowers[GRP_LIMIT];

// Private variables static
  private:
    static  int   Count;

// Private variables  
  private:
    Player        *pPlayerNext;
    Player        *pPlayerPrev;
    CString       Output;
    CStdioFile    PlayerFile;
    int           PlayerId;
    CString       Stuff;

// Player file variables
  private:
    int           Gold;
    int           Silver;
    int           Bronze;
    int           Copper;
    int           HitPoints;
    int           MovePoints;
  public:
    CString       Name;
    CString       Password;
    CString       Sex;
    int           Level;
    int           Experience;
	  bool          Color;
    bool          Group;
    CString       Title;
};

#endif