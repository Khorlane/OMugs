/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Player.h                                         *
* Usage:  Define Player class                              *
* Author: Steve Bryant                                     *
************************************************************/

#ifndef PLAYER_H
#define PLAYER_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Dnode.h"
#include "Log.h"
#include "Object.h"
#include "Utility.h"

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
    static  bool    IsPlayer(string PlayerName);
    static  int     GetCount();
    static  bool    IsNameValid(string Name);

// Public functions
  public:
    Player();
    ~Player();
    void            CreatePrompt();
    void            Drink(int Percent);
    void            Eat(int Percent);
    void            GainExperience(Dnode *pDnode, int ExperienceToBeGained);
    string          GetOutput();
    int             GetWeaponSkill();
    void            ParsePlayerStuff();
    bool            PlayerRoomHasNotBeenHere();
    void            Save();
    void            SetMoney(char PlusMinus, int Amount, string Metal);
    void            ShowMoney();
    void            ShowStatus();

// Private functions
  private:
    void            CloseFile();
    bool            OpenFile(string Name, string Mode);
    void            PlayerRoomBitsToCharConvert();
    void            PlayerRoomCharToBitsConvert();
    void            PlayerRoomStringRead();
    void            PlayerRoomStringWrite();
    void            ReadLine();
    void            WriteLine(string Stuff);

// Public variables
  public:
    Player         *pPlayerGrpMember[GRP_LIMIT];
    Player         *pPlayerFollowers[GRP_LIMIT];
    int             SessionTime;
    string          RoomIdBeforeMove;

// Private variables static
  private:
    static  int     Count;

// Private variables  
  private:
    Dnode          *pDnode;
    string          Output;
    fstream         PlayerFile;

    // Exploration tracking variables
    int             PlayerRoomBitPos;
    bitset<8>       PlayerRoomBits;
    unsigned char   PlayerRoomChar;
    int             PlayerRoomCharPos;
    vector<unsigned char> PlayerRoomVector;

// Player file variables
  public:
    string          Name;
    string          Password;
    bool            Admin;
    string          Afk;
    bool            AllowAssist;
    bool            AllowGroup;
    int             ArmorClass;
    int             Born;
    bool            Color;
    float           Experience;
    string          GoToArrive;
    string          GoToDepart;
    int             HitPoints;
    int             Hunger;
    bool            Invisible;
    int             Level;
    int             MovePoints;
    bool            OneWhack;
    string          Online;
    string          Position;
    string          RoomId;
    bool            RoomInfo;
    string          Sex;
    int             Silver;
    int             SkillAxe;
    int             SkillClub;
    int             SkillDagger;
    int             SkillHammer;
    int             SkillSpear;
    int             SkillStaff;
    int             SkillSword;
    int             Thirst;
    int             TimePlayed;
    string          Title;
    int             WeaponDamage;
    string          WeaponDesc1;
    string          WeaponType;
};

#endif