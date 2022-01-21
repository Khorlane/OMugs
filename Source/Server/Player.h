/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Player.h                                         *
* Usage:  Define Player class                              *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef PLAYER_H
#define PLAYER_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Communication.h"
#include "Config.h"
#include "Dnode.h"
#include "Log.h"
#include "Object.h"

/***********************************************************
* Define Player class                                      *
************************************************************/

class Player
{

// Public functions static
  public:
    static  int     CalcAdjustedExpPoints(int PlayerLevel, int MobileLevel, int ExpPoints);
    static  float   CalcLevelExperience(int Level);
    static  float   CalcLevelExperienceAdd(int Level, float BaseExp);
    static  float   CalcLevelExperienceBase(int Level);
    static  bool    IsPlayer(CString PlayerName);
    static  int     GetCount();
    static  bool    IsNameValid(CString Name);

// Public functions
  public:
    Player();
    ~Player();
    void            CreatePrompt();
    void            Drink(int Percent);
    void            Eat(int Percent);
    void            GainExperience(Dnode *pDnode, int ExperienceToBeGained);
    CString         GetOutput();
    int             GetWeaponSkill();
    void            ParsePlayerStuff();
    bool            PlayerRoomHasNotBeenHere();
    void            Save();
    void            SetMoney(char PlusMinus, int Amount, CString Metal);
    void            ShowMoney();
    void            ShowStatus();

// Private functions
  private:
    void            CloseFile();
    bool            OpenFile(CString Name, CString Mode);
    void            PlayerRoomBitsToCharConvert();
    void            PlayerRoomCharToBitsConvert();
    void            PlayerRoomStringRead();
    void            PlayerRoomStringWrite();
    void            ReadLine();
    void            WriteLine(CString Stuff);

// Public variables
  public:
    Player         *pPlayerGrpMember[GRP_LIMIT];
    Player         *pPlayerFollowers[GRP_LIMIT];
    CTime           SessionTime;
    CString         RoomIdBeforeMove;

// Private variables static
  private:
    static  int     Count;

// Private variables  
  private:
    Dnode          *pDnode;
    CString         Output;
    CStdioFile      PlayerFile;
    CString         Stuff;

    // Exploration tracking variables
    int             PlayerRoomBitPos;
    bitset<8>       PlayerRoomBits;
    unsigned char   PlayerRoomChar;
    int             PlayerRoomCharPos;
    CString         PlayerRoomString;

// Player file variables
  public:
    CString         Name;
    string          Password;
    bool            Admin;
    CString         Afk;
    bool            AllowAssist;
    bool            AllowGroup;
    int             ArmorClass;
    CTime           Born;
    bool            Color;
    float           Experience;
    CString         GoToArrive;
    CString         GoToDepart;
    int             HitPoints;
    int             Hunger;
    bool            Invisible;
    int             Level;
    int             MovePoints;
    bool            OneWhack;
    CString         Online;
    CString         Position;
    CString         RoomId;
    bool            RoomInfo;
    CString         Sex;
    int             Silver;
    int             SkillAxe;
    int             SkillClub;
    int             SkillDagger;
    int             SkillHammer;
    int             SkillSpear;
    int             SkillStaff;
    int             SkillSword;
    int             Thirst;
    CTimeSpan       TimePlayed;
    CString         Title;
    int             WeaponDamage;
    CString         WeaponDesc1;
    CString         WeaponType;
};

#endif