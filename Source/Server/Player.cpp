/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Player.cpp                                       *
* Usage:  Manages players                                  *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

/***********************************************************
* Includes                                                 *
************************************************************/

#include "stdafx.h" // This is only here to speed up compiles
#include "Player.h"

/***********************************************************
* Globals                                                  *
************************************************************/

int Player::Count = 0;

/***********************************************************
* Constructor                                              *
************************************************************/

Player::Player()
{
  int i;

  // Player variables
  pDnode              = pDnodeActor;
  Count++;
  Output              = "";
  Stuff               = "";
  for (i = 0; i < GRP_LIMIT; i++)
  {
    pPlayerFollowers[i] = NULL;
    pPlayerGrpMember[i] = NULL;
  }
  // Player file variables
  Name                = "";
  Password            = "";
  Admin               = false;
  Afk                 = ""; // Controlled by PlayerStateAfk
  AllowAssist         = false;
  AllowGroup          = false;
  ArmorClass          = 0;
  Born                = 0;
  Color               = false;
  Experience          = 0;
  GoToArrive          = "";
  GoToDepart          = "";
  HitPoints           = PLAYER_HPT_PER_LEVEL;
  Hunger              = 0;
  Invisible           = false;
  Level               = 1;
  MovePoints          = 25;
  OneWhack            = false;
  Online              = ""; // Controlled by PlayerStatePlaying
  Position            = "stand";
  RoomId              = START_ROOM;
  RoomInfo            = false;
  Sex                 = "";
  Silver              = 0;
  SkillAxe            = 0;
  SkillClub           = 0;
  SkillDagger         = 0;
  SkillHammer         = 0;
  SkillSpear          = 0;
  SkillStaff          = 0;
  SkillSword          = 0;
  Thirst              = 0;
  TimePlayed          = 0;
  Title               = "";
  WeaponDamage        = PLAYER_DMG_HAND;
  WeaponDesc1         = "a pair of bare hands";
  WeaponType          = "Hand";
}

/***********************************************************
* Destructor                                               *
************************************************************/

Player::~Player()
{
  Count--;
}

////////////////////////////////////////////////////////////
// Public functions static                                //
////////////////////////////////////////////////////////////

/***********************************************************
 * Calculate adjusted experience points                    *
 ***********************************************************/

int Player::CalcAdjustedExpPoints(int PlayerLevel, int MobileLevel, int ExpPoints)
{
  int PctOfExpPoints;
  int AdjustedExpPoints;

  PctOfExpPoints = 100-(PlayerLevel-MobileLevel-2)*20;
  if (PctOfExpPoints < 1)
  { // Stop from going negative
    PctOfExpPoints = 0;
  }
  if (PctOfExpPoints > 100)
  { // Stop from being more than 100
    PctOfExpPoints = 100;
  }
  AdjustedExpPoints = int(ceil(ExpPoints*PctOfExpPoints/100.0));
  return AdjustedExpPoints;
}

/***********************************************************
* Calculate experience needed to obtain the next level     *
************************************************************/

float Player::CalcLevelExperience(int Level)
{
  float AddExp;
  float BaseExp;
  float TotalExp;

  BaseExp = CalcLevelExperienceBase(Level);
  AddExp  = CalcLevelExperienceAdd(Level, BaseExp);
  TotalExp = BaseExp + AddExp;
  return TotalExp;
}

float Player::CalcLevelExperienceAdd(int Level, float BaseExp)
{
  float AddExp;
  float LogLevel;

  LogLevel = (float) log10(Level+20);
  AddExp   = (float) pow(BaseExp,LogLevel)*(Level/10000.0f);
  return AddExp;
}

float Player::CalcLevelExperienceBase(int Level)
{ // Recursive
  // Assuming PLAYER_EXP_PER_LEVEL = 1000
  // experience needed to get to level 5 is:
  // 14000 = 5 * 1000 + 4 * 1000 + 3 * 1000 + 2 * 1000
  if (Level < 2)
  {
    return 0;
  }
  return (Level * PLAYER_EXP_PER_LEVEL) + CalcLevelExperienceBase(Level-1);
}

/***********************************************************
* Is this a valid Player?                                  *
************************************************************/

bool Player::IsPlayer(CString PlayerName)
{
  CString    PlayerFileName;
  CStdioFile PlayerFile;
  int        Success;
  
  PlayerFileName = PLAYER_DIR;
  PlayerFileName += PlayerName + ".txt";
  Success = PlayerFile.Open(PlayerFileName,
            CFile::modeRead |
            CFile::typeText);
  if(Success)
  {
    return true;
    PlayerFile.Close();
  }
  else
  {
    return false;
  }
}

/***********************************************************
* Return player count                                      *
************************************************************/

int Player::GetCount()
{
  return Count;
}

/***********************************************************
* Validate player name                                     *
************************************************************/

bool Player::IsNameValid(CString Name)
{
  CString    NameIn;
  int        Success;
  CStdioFile ValidNameFile;
  CString    ValidNamesFileName;

  ValidNamesFileName  = VALID_NAMES_DIR;
  ValidNamesFileName += "ValidNames.txt";

  Success = ValidNameFile.Open(ValidNamesFileName,
                    CFile::modeRead |
                    CFile::typeText);
  if(!Success)
  { // Ok, who deleted the valid names file?
    AfxMessageBox("Player::IsNameValid - Error opening valid name file, it may not exist", MB_ICONSTOP);
    _endthread();
  }
  Name.MakeLower();
  ValidNameFile.ReadString(NameIn);
  NameIn.MakeLower();
  while (NameIn != "")
  { // Read all names
    if (Name == NameIn)
    { // Name is valid
      ValidNameFile.Close();
      return true;
    }
    ValidNameFile.ReadString(NameIn);
  }
  ValidNameFile.Close();
  return false;
}


////////////////////////////////////////////////////////////
// Public functions                                       //
////////////////////////////////////////////////////////////

/***********************************************************
* Create player prompt                                     *
************************************************************/

void Player::CreatePrompt()
{
  CString TmpStr;

  Output  = "\r\n";
  sprintf(Buf, "%d", HitPoints);
  TmpStr = ConvertStringToCString(Buf);
  Output += TmpStr + "H ";
  sprintf(Buf, "%d", MovePoints);
  TmpStr = ConvertStringToCString(Buf);
  Output += TmpStr + "M ";
  Output += "> ";
}

/***********************************************************
* Player drink                                             *
************************************************************/

void Player::Drink(int Percent)
{
  Thirst -= Percent;
  if (Thirst <= 0)
  { //  Not thirsty
    Thirst = 0;
    Output  = "You are no longer thirsty, not even a little bit.";
    Output += "\r\n";
    return;
  }
  else
  if (Thirst < 20)
  { // A little bit thirsty
    Output  = "You are a little bit thirsty.";
    Output += "\r\n";
    return;
  }
  else
  if (Thirst < 40)
  { // Lip balm
    Output  = "You need some lip balm.";
    Output += "\r\n";
    return;
  }
  else
  if (Thirst < 60)
  { // Thirsty
    Output  = "You are thirsty.";
    Output += "\r\n";
    return;
  }
  else
  if (Thirst < 80)
  { // Parched
    Output  = "Your throat is parched!";
    Output += "\r\n";
    return;
  }
  else
  if (Thirst < 100)
  { // Extreme thirst
    Output  = "You are extremely thirsty!!!";
    Output += "\r\n";
    return;
  }
}

/***********************************************************
* Player eat                                               *
************************************************************/

void Player::Eat(int Percent)
{
  Hunger -= Percent;
  if (Hunger <= 0)
  { //  Not hungry
    Hunger = 0;
    Output  = "You are no longer hungry, not even a little bit.";
    Output += "\r\n";
    return;
  }
  else
  if (Hunger < 20)
  { // A little bit hungry
    Output  = "You are a little bit hungry.";
    Output += "\r\n";
    return;
  }
  else
  if (Hunger < 40)
  { // Stomach growling
    Output  = "Your stomach is growling.";
    Output += "\r\n";
    return;
  }
  else
  if (Hunger < 60)
  { // Hungry
    Output  = "You are hungry.";
    Output += "\r\n";
    return;
  }
  else
  if (Hunger < 80)
  { // Eat a horse
    Output  = "You could eat a horse!";
    Output += "\r\n";
    return;
  }
  else
  if (Hunger < 100)
  { // Extreme hunger
    Output  = "You are extremely hungry!!!";
    Output += "\r\n";
    return;
  }
}

/***********************************************************
* Player gains some experience                             *
************************************************************/

void Player::GainExperience(Dnode *pDnode, int ExperienceToBeGained)
{
  float   LevelExperience;
  CString LogBuf;
  CString TmpStr;

  Experience += ExperienceToBeGained;
  LevelExperience = CalcLevelExperience(Level+1);
  if (Experience >= LevelExperience)
  { // Player has enough experience to gain a level
    pDnode->PlayerOut += "&Y";
    pDnode->PlayerOut += "You gain a LEVEL!!!";
    pDnode->PlayerOut += "&N";
    pDnode->PlayerOut += "\r\n";
    Level++;
    LogBuf  = pDnode->PlayerName;
    LogBuf += " has gained level ";
    sprintf(Buf, "%d",Level);
    TmpStr = ConvertStringToCString(Buf);
    LogBuf += TmpStr;
    LogBuf += "!";
    LogIt(LogBuf);
  }
}

/***********************************************************
* Get player output                                        *
************************************************************/

CString Player::GetOutput()
{
  return Output;
}

/***********************************************************
* Get skill for current weapon                             *
************************************************************/

int Player::GetWeaponSkill()
{
  int WeaponSkill;

  WeaponSkill = 0;
  WeaponType.MakeLower();
  if (WeaponType == "axe")
  { // Axe
    WeaponSkill = SkillAxe;
  }
  else
  if (WeaponType == "club")
  { // Club
    WeaponSkill = SkillClub;
  }
  else
  if (WeaponType == "dagger")
  { // Dagger
    WeaponSkill = SkillDagger;
  }
  else
  if (WeaponType == "hammer")
  { // Hammer
    WeaponSkill = SkillHammer;
  }
  else
  if (WeaponType == "Spear")
  { // Spear
    WeaponSkill = SkillSpear;
  }
  else
  if (WeaponType == "Staff")
  { // Staff
    WeaponSkill = SkillStaff;
  }
  else
  if (WeaponType == "Sword")
  { // Sword
    WeaponSkill = SkillSword;
  }
  return WeaponSkill;
}

/***********************************************************
* Parse player stuff                                       *
************************************************************/

void Player::ParsePlayerStuff()
{
  int     Amount;
  CString LogBuf;
  CString Name;
  CString TmpStr;

  Name = pDnodeActor->PlayerName;
  if (!OpenFile(Name, "Read"))
  {
    AfxMessageBox("Player::Save - Error opening player file for read, Players directory may not exist", MB_ICONSTOP);
    _endthread();
  }
  ReadLine();
  while (Stuff != "")
  { // Name
    if (Stuff.Left(5) == "Name:")
    {
      TmpStr = ""; // Already got the name
    }
    else
    // Password
    if (Stuff.Left(9) == "Password:")
    { 
      Password = Stuff.Right(Stuff.GetLength()-9);
    }
    else
    // Admin
    if (Stuff.Left(6) == "Admin:")
    {
      TmpStr = Stuff.Right(Stuff.GetLength()-6);
      TmpStr.MakeLower();
      if (TmpStr == "yes")
      {
        Admin = true;
      }
      else
      {
        Admin = false;
      }
    }
    else
    // AFK
    if(Stuff.Left(4) == "AFK:")
    {
      TmpStr = ""; // Does not matter what is the file
    }
    else
    // AllowAssist
    if (Stuff.Left(12) == "AllowAssist:")
    {
      TmpStr = Stuff.Right(Stuff.GetLength()-12);
      TmpStr.MakeLower();
      if (TmpStr == "yes")
      {
        AllowAssist = true;
      }
      else
      {
        AllowAssist = false;
      }
    }
    else
    // AllowGroup
    if (Stuff.Left(11) == "AllowGroup:")
    {
      TmpStr = Stuff.Right(Stuff.GetLength()-11);
      TmpStr.MakeLower();
      if (TmpStr == "yes")
      {
        AllowGroup = true;
      }
      else
      {
        AllowGroup = false;
      }
    }
    else
    // ArmorClass
    if (Stuff.Left(11) == "ArmorClass:")
    {
      ArmorClass = Object::CalcPlayerArmorClass();
    }
    else
    // Born
    if (Stuff.Left(5) == "Born:")
    {
      Born = atol(Stuff.Right(Stuff.GetLength()-5));
    }
    else
    // Color
    if (Stuff.Left(6) == "Color:")
    {
      TmpStr = Stuff.Right(Stuff.GetLength()-6);
      TmpStr.MakeLower();
      if (TmpStr == "yes")
      {
        Color = true;
      }
      else
      {
        Color = false;
      }
    }
    else
    // Experience
    if (Stuff.Left(11) == "Experience:")
    {
      Experience = (float) atof(Stuff.Right(Stuff.GetLength()-11));
    }
    else
    // GoToArrive
    if (Stuff.Left(11) == "GoToArrive:")
    {
      GoToArrive = Stuff.Right(Stuff.GetLength()-11);
      if (Admin)
      { // Player is an Admin
        if (GoToArrive == "")
        { // With no departure message
          GoToArrive = "arrives!";
        }
      }
    }
    else
    // GoToDepart
    if (Stuff.Left(11) == "GoToDepart:")
    {
      GoToDepart = Stuff.Right(Stuff.GetLength()-11);
      if (Admin)
      { // Player is an Admin
        if (GoToDepart == "")
        { // With no departure message
          GoToDepart = "leaves!";
        }
      }
    }
    else
    // HitPoints
    if (Stuff.Left(10) == "HitPoints:")
    {
      HitPoints = atoi(Stuff.Right(Stuff.GetLength()-10));
    }
    else
    // Hunger
    if (Stuff.Left(7) == "Hunger:")
    {
      Hunger = atoi(Stuff.Right(Stuff.GetLength()-7));
    }
    else
    // Invisible
    if (Stuff.Left(10) == "Invisible:")
    {
      TmpStr = Stuff.Right(Stuff.GetLength()-10);
      TmpStr.MakeLower();
      if (TmpStr == "yes")
      {
        Invisible = true;
        pDnodeActor->PlayerStateInvisible = true;
      }
      else
      {
        Invisible = false;
        pDnodeActor->PlayerStateInvisible = false;
      }
    }
    else
    // Level
    if (Stuff.Left(6) == "Level:")
    {
      Level = atoi(Stuff.Right(Stuff.GetLength()-6));
    }
    else
    // MovePoints
    if (Stuff.Left(11) == "MovePoints:")
    {
      MovePoints = atoi(Stuff.Right(Stuff.GetLength()-11));
    }
    else
    // OneWhack
    if (Stuff.Left(9) == "OneWhack:")
    {
      TmpStr = Stuff.Right(Stuff.GetLength()-9);
      TmpStr.MakeLower();
      if (TmpStr == "yes")
      {
        OneWhack = true;
      }
      else
      {
        OneWhack = false;
      }
    }
    else
    // Online
    if (Stuff.Left(7) == "Online:")
    {
      TmpStr = ""; // Does not matter what is the file
    }
    else
    // Position
    if (Stuff.Left(9) == "Position:")
    {
      Position = Stuff.Right(Stuff.GetLength()-9);
    }
    else
    // RoomId
    if (Stuff.Left(7) == "RoomId:")
    {
      RoomId = Stuff.Right(Stuff.GetLength()-7);
    }
    else
    // RoomInfo
    if (Stuff.Left(9) == "RoomInfo:")
    {
      TmpStr = Stuff.Right(Stuff.GetLength()-9);
      TmpStr.MakeLower();
      if (TmpStr == "yes")
      {
        RoomInfo = true;
      }
      else
      {
        RoomInfo = false;
      }
    }
    else
    // Sex
    if (Stuff.Left(4) == "Sex:")
    {
      Sex = Stuff.Right(Stuff.GetLength()-4);
      Sex.MakeUpper();
    }
    else
    // Sivler
    if (Stuff.Left(7) == "Silver:")
    {
      Amount = atoi(Stuff.Right(Stuff.GetLength()-7));
      SetMoney('+', Amount, "Silver");
    }
    else
    // SkillAxe
    if (Stuff.Left(9) == "SkillAxe:")
    {
      SkillAxe = atoi(Stuff.Right(Stuff.GetLength()-9));
    }
    else
    // SkillClub
    if (Stuff.Left(10) == "SkillClub:")
    {
      SkillClub = atoi(Stuff.Right(Stuff.GetLength()-10));
    }
    else
    // SkillDagger
    if (Stuff.Left(12) == "SkillDagger:")
    {
      SkillDagger = atoi(Stuff.Right(Stuff.GetLength()-12));
    }
    else
    // SkillHammer
    if (Stuff.Left(12) == "SkillHammer:")
    {
      SkillHammer = atoi(Stuff.Right(Stuff.GetLength()-12));
    }
    else
    // SkillSpear
    if (Stuff.Left(11) == "SkillSpear:")
    {
      SkillSpear = atoi(Stuff.Right(Stuff.GetLength()-11));
    }
    else
    // SkillStaff
    if (Stuff.Left(11) == "SkillStaff:")
    {
      SkillStaff = atoi(Stuff.Right(Stuff.GetLength()-11));
    }
    else
    // SkillSword
    if (Stuff.Left(11) == "SkillSword:")
    {
      SkillSword = atoi(Stuff.Right(Stuff.GetLength()-11));
    }
    else
    // Thirst
    if (Stuff.Left(7) == "Thirst:")
    {
      Thirst = atoi(Stuff.Right(Stuff.GetLength()-7));
    }
    else
    // TimePlayed
    if (Stuff.Left(11) == "TimePlayed:")
    {
      TimePlayed = atol(Stuff.Right(Stuff.GetLength()-11));
    }
    else
    // Title
    if (Stuff.Left(6) == "Title:")
    {
      Title = Stuff.Right(Stuff.GetLength()-6);
    }
    else
    // WeaponDamage
    if (Stuff.Left(13) == "WeaponDamage:")
    {
      WeaponDamage = atoi(Stuff.Right(Stuff.GetLength()-13));
    }
    else
    // WeaponDesc1
    if (Stuff.Left(12) == "WeaponDesc1:")
    {
      WeaponDesc1 = Stuff.Right(Stuff.GetLength()-12);
    }
    else
    // WeaponType
    if (Stuff.Left(11) == "WeaponType:")
    {
      WeaponType = Stuff.Right(Stuff.GetLength()-11);
      WeaponType.MakeLower();
    }
    else
    // Unidentified field in the player's file
    {
      LogBuf  = Name;
      LogBuf += " has an unidentified player file field";
      LogIt(LogBuf);
      LogBuf  = Stuff;
      LogIt(LogBuf);
    }
    if (WeaponType == "hand")
    {
      WeaponDamage = PLAYER_DMG_HAND;
    }
    // Read the next line
    ReadLine();
  }
  CloseFile();
}

/***********************************************************
* Save player stuff                                        *
************************************************************/

void Player::Save()
{
  CString TmpStr;

  if (!OpenFile(Name, "Write"))
  {
    AfxMessageBox("Player::Save - Error opening player file for write, Players directory may not exist", MB_ICONSTOP);
    _endthread();
  }
  // Name
  Stuff = "Name:" + Name;
  WriteLine(Stuff);
  // Password
  Stuff = "Password:" + ConvertStringToCString(Password);
  WriteLine(Stuff);
  // Admin
  if (Admin)
  {
    Stuff = "Admin:Yes";
    WriteLine(Stuff);
  }
  else
  {
    Stuff = "Admin:No";
    WriteLine(Stuff);
  }
  // AFK
  if (pDnode->PlayerStateAfk)
  {
    Stuff = "AFK:Yes";
    WriteLine(Stuff);
  }
  else
  {
    Stuff = "AFK:No";
    WriteLine(Stuff);
  }
  // AllowAssist
  if (AllowAssist)
  {
    Stuff = "AllowAssist:Yes";
    WriteLine(Stuff);
  }
  else
  {
    Stuff = "AllowAssist:No";
    WriteLine(Stuff);
  }
  // AllowGroup
  if (AllowGroup)
  {
    Stuff = "AllowGroup:Yes";
    WriteLine(Stuff);
  }
  else
  {
    Stuff = "AllowGroup:No";
    WriteLine(Stuff);
  }
  // ArmorClass - save only, ParsePlayerStuff calls CalcPlayerArmorClass
  sprintf(Buf, "%d", ArmorClass);
  TmpStr = ConvertStringToCString(Buf);
  Stuff = "ArmorClass:" + TmpStr;
  WriteLine(Stuff);
  // Born
  sprintf(Buf, "%d", Born);
  TmpStr = ConvertStringToCString(Buf);
  Stuff = "Born:" + TmpStr;
  WriteLine(Stuff);
  // Color
  if (Color)
  {
    Stuff = "Color:Yes";
    WriteLine(Stuff);
  }
  else
  {
    Stuff = "Color:No";
    WriteLine(Stuff);
  }
  // Experience
  sprintf(Buf, "%f15.0", Experience);
  TmpStr = ConvertStringToCString(Buf);
  Stuff = "Experience:" + TmpStr;
  WriteLine(Stuff);
  // GoToArrive
  Stuff = "GoToArrive:" + GoToArrive;
  WriteLine(Stuff);
  // GoToDepart
  Stuff = "GoToDepart:" + GoToDepart;
  WriteLine(Stuff);
  // HitPoints
  sprintf(Buf, "%d", HitPoints);
  TmpStr = ConvertStringToCString(Buf);
  Stuff = "HitPoints:" + TmpStr;
  WriteLine(Stuff);
  // Hunger
  sprintf(Buf, "%d", Hunger);
  TmpStr = ConvertStringToCString(Buf);
  Stuff = "Hunger:" + TmpStr;
  WriteLine(Stuff);
  // Invisible
  if (Invisible)
  {
    Stuff = "Invisible:Yes";
    WriteLine(Stuff);
  }
  else
  {
    Stuff = "Invisible:No";
    WriteLine(Stuff);
  }
  // Level
  sprintf(Buf, "%d", Level);
  TmpStr = ConvertStringToCString(Buf);
  Stuff = "Level:" + TmpStr;
  WriteLine(Stuff);
  // MovePoints
  sprintf(Buf, "%d", MovePoints);
  TmpStr = ConvertStringToCString(Buf);
  Stuff = "MovePoints:" + TmpStr;
  WriteLine(Stuff);
  // OneWhack
  if (OneWhack)
  {
    Stuff = "OneWhack:Yes";
    WriteLine(Stuff);
  }
  else
  {
    Stuff = "OneWhack:No";
    WriteLine(Stuff);
  }
  // Online
  if (pDnode->PlayerStatePlaying)
  {
    Stuff = "Online:Yes";
    WriteLine(Stuff);
  }
  else
  {
    Stuff = "Online:No";
    WriteLine(Stuff);
  }
  // Position
  Stuff = "Position:" + Position;
  WriteLine(Stuff);
  // RoomId
  Stuff = "RoomId:" + RoomId;
  WriteLine(Stuff);
  // RoomInfo
  if (RoomInfo)
  {
    Stuff = "RoomInfo:Yes";
    WriteLine(Stuff);
  }
  else
  {
    Stuff = "RoomInfo:No";
    WriteLine(Stuff);
  }
  // Sex
  Stuff = "Sex:" + Sex;
  WriteLine(Stuff);
  // Silver
  sprintf(Buf, "%d", Silver);
  TmpStr = ConvertStringToCString(Buf);
  Stuff = "Silver:" + TmpStr;
  WriteLine(Stuff);
  // SkillAxe
  sprintf(Buf, "%d", SkillAxe);
  TmpStr = ConvertStringToCString(Buf);
  Stuff = "SkillAxe:" + TmpStr;
  WriteLine(Stuff);
  // SkillClub
  sprintf(Buf, "%d", SkillClub);
  TmpStr = ConvertStringToCString(Buf);
  Stuff = "SkillClub:" + TmpStr;
  WriteLine(Stuff);
  // SkillDagger
  sprintf(Buf, "%d", SkillDagger);
  TmpStr = ConvertStringToCString(Buf);
  Stuff = "SkillDagger:" + TmpStr;
  WriteLine(Stuff);
  // SkillHammer
  sprintf(Buf, "%d", SkillHammer);
  TmpStr = ConvertStringToCString(Buf);
  Stuff = "SkillHammer:" + TmpStr;
  WriteLine(Stuff);
  // SkillSpear
  sprintf(Buf, "%d", SkillSpear);
  TmpStr = ConvertStringToCString(Buf);
  Stuff = "SkillSpear:" + TmpStr;
  WriteLine(Stuff);
  // SkillStaff
  sprintf(Buf, "%d", SkillStaff);
  TmpStr = ConvertStringToCString(Buf);
  Stuff = "SkillStaff:" + TmpStr;
  WriteLine(Stuff);
  // SkillSword
  sprintf(Buf, "%d", SkillSword);
  TmpStr = ConvertStringToCString(Buf);
  Stuff = "SkillSword:" + TmpStr;
  WriteLine(Stuff);
  // Thirst
  sprintf(Buf, "%d", Thirst);
  TmpStr = ConvertStringToCString(Buf);
  Stuff = "Thirst:" + TmpStr;
  WriteLine(Stuff);
  // TimePlayed
  if (pDnode->PlayerStatePlaying)
  { // Don't update TimePlayed if player is not 'playing'
    TimePlayed += GetTimeSeconds() - SessionTime;
    SessionTime = GetTimeSeconds();
  }
  sprintf(Buf, "%d", TimePlayed);
  TmpStr = ConvertStringToCString(Buf);
  Stuff = "TimePlayed:" + TmpStr;
  WriteLine(Stuff);
  // Title
  Stuff = "Title:" + Title;
  WriteLine(Stuff);
  // WeaponDamage
  sprintf(Buf, "%d", WeaponDamage);
  TmpStr = ConvertStringToCString(Buf);
  Stuff = "WeaponDamage:" + TmpStr;
  WriteLine(Stuff);
  // WeaponDesc1
  Stuff = "WeaponDesc1:" + WeaponDesc1;
  WriteLine(Stuff);
  // WeaponType
  Stuff = "WeaponType:" + WeaponType;
  WriteLine(Stuff);
  // Done
  CloseFile();
}

/***********************************************************
* Manipulate player money                                  *
************************************************************/

void Player::SetMoney(char PlusMinus, int Amount, CString Metal)
{
  if (PlusMinus == '-')
    Amount = Amount * -1;

  if (Metal == "Silver")
  {
    Silver = Silver + Amount;
  }
}

/***********************************************************
* Show player money                                        *
************************************************************/

void Player::ShowMoney()
{
  CString TmpStr;

  sprintf(Buf, "%d", Silver);
  TmpStr = ConvertStringToCString(Buf);
  Output = "Silver: " + TmpStr + "\r\n";
}

/***********************************************************
* Show player status                                       *
************************************************************/

void Player::ShowStatus()
{
  CString Exp1; // Current Experience
  CString Exp2; // Experience needed for next level
  CString TmpStr;

  Output = "\r\n";
  // Name
  Output += "Name:         ";
  Output += Name;
  Output += "\r\n";
  // Sex
  Output += "Sex:          ";
  Output += Sex;
  Output += "\r\n";
  // Level
  sprintf(Buf, "%d", Level);
  TmpStr = ConvertStringToCString(Buf);
  Output += "Level:        ";
  Output += TmpStr;
  Output += "\r\n";
  // Hit Points
  Output += "Hit Points:   ";
  sprintf(Buf, "%d", HitPoints);
  TmpStr = ConvertStringToCString(Buf);
  Output += TmpStr;
  Output += "/";
  sprintf(Buf, "%d", Level * PLAYER_HPT_PER_LEVEL);
  TmpStr = ConvertStringToCString(Buf);
  Output += TmpStr;
  Output += "\r\n";
  // Current Experience and Experience needed for next level
  sprintf(Buf, "%f15.0", Experience);
  TmpStr = ConvertStringToCString(Buf);
  TmpStr = TmpStr.Left(TmpStr.Find('.'));
  Exp1   = FormatCommas(TmpStr);

  sprintf(Buf, "%f15.0", CalcLevelExperience(Level + 1));
  TmpStr = ConvertStringToCString(Buf);
  TmpStr = TmpStr.Left(TmpStr.Find('.'));
  Exp2   = FormatCommas(TmpStr);
  while (Exp1.GetLength() < Exp2.GetLength())
  {
    Exp1.Insert(0,' ');
  }
  Output += "Experience:   ";
  Output += Exp1;
  Output += "\r\n";
  Output += "Next level:   ";
  Output += Exp2;
  Output += "\r\n";
  // Armor Class
  sprintf(Buf, "%d", ArmorClass);
  TmpStr = ConvertStringToCString(Buf);
  Output += "Armor Class:  ";
  Output += TmpStr;
  Output += "\r\n";
  // Color
  if (Color)
  {
    TmpStr = "On";
  }
  else
  {
    TmpStr = "Off";
  }
  Output += "Color:        ";
  Output += TmpStr;
  Output += "\r\n";
  // AllowGroup
  if (AllowGroup)
  {
    TmpStr = "On";
  }
  else
  {
    TmpStr = "Off";
  }
  Output += "Allow Group:  ";
  Output += TmpStr;
  Output += "\r\n";
  // AllowAssist
  if (AllowAssist)
  {
    TmpStr = "On";
  }
  else
  {
    TmpStr = "Off";
  }
  Output += "Allow Assist: ";
  Output += TmpStr;
  Output += "\r\n";
  // Position
  Output += "Position:     ";
  TmpStr = Position;
  TmpStr.Replace("s", "S");
  if (TmpStr == "Sit")
  { // Add extra 't'
    TmpStr += "t";
  }
  TmpStr += "ing";
  Output += TmpStr;
  Output += "\r\n";
  // Silver
  sprintf(Buf, "%d", Silver);
  TmpStr = ConvertStringToCString(Buf);
  Output += "Silver:       ";
  Output += TmpStr;
  Output += "\r\n";
  // Hunger
  sprintf(Buf, "%d", Hunger);
  TmpStr = ConvertStringToCString(Buf);
  Output += "Hunger:       ";
  Output += TmpStr;
  Output += "\r\n";
  // Thirst
  sprintf(Buf, "%d", Thirst);
  TmpStr = ConvertStringToCString(Buf);
  Output += "Thirst:       ";
  Output += TmpStr;
  Output += "\r\n";
}

////////////////////////////////////////////////////////////
// Private functions                                      //
////////////////////////////////////////////////////////////

/***********************************************************
* Close player file                                        *
************************************************************/

void Player::CloseFile()
{
  PlayerFile.Close();
}

/***********************************************************
* Open player file                                         *
************************************************************/

bool Player::OpenFile(CString Name, CString Mode)
{
  CString PlayerFileName;
  int     Success;

  PlayerFileName = PLAYER_DIR;
  PlayerFileName += Name + ".txt";

  if (Mode == "Read")
  {
    Success = PlayerFile.Open(PlayerFileName,
              CFile::modeRead |
              CFile::typeText);
    if(!Success)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
  else
  if (Mode == "Write")
  {
    Success = PlayerFile.Open(PlayerFileName,
                   CFile::modeCreate |
                   CFile::modeWrite  |
                   CFile::typeText);
    if(!Success)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
  else
  {
    AfxMessageBox("Player::OpenFile - Mode is not 'Read' or 'Write'", MB_ICONSTOP);
    _endthread();
    return false; // Never will execute this return, but stops warning message
  }
}

/***********************************************************
* Read a line from player file                             *
************************************************************/

void Player::ReadLine()
{
  PlayerFile.ReadString(Stuff);
}

/***********************************************************
* Write a line to player file                              *
************************************************************/

void Player::WriteLine(CString Stuff)
{
  Stuff = Stuff + "\n";
  PlayerFile.WriteString(Stuff);
  PlayerFile.Flush();
}

/***********************************************************
* Check whether or not player has been in the current room *
************************************************************/

bool Player::PlayerRoomHasNotBeenHere()
{
  char    Char;
  int     CharPos;
  int     RoomNbr;
  CString RoomNbrStr;

  if (PlayerRoomString.GetLength() == 0)
  {
    PlayerRoomStringRead();
  }
  // Get RoomNbr from RoomId
  CharPos = RoomId.GetLength()-1;
  Char    = RoomId.GetAt(CharPos);
  while (isdigit(Char))
  {
    RoomNbrStr.Insert(0, Char);
    CharPos--;
    Char        = RoomId.GetAt(CharPos);
  }
  RoomNbr = atoi(RoomNbrStr);
  // Has player been here?
  PlayerRoomCharPos = (int) ceil(RoomNbr/8.0)-1;
  PlayerRoomBitPos  = RoomNbr-(PlayerRoomCharPos*8)-1;
  PlayerRoomChar    = PlayerRoomString.GetAt(PlayerRoomCharPos);
  PlayerRoomCharToBitsConvert();

  if (PlayerRoomBits.test(PlayerRoomBitPos))
  { // Player has been here
    return false;
  }
  else
  { // Player has not been here
    PlayerRoomBits.set(PlayerRoomBitPos);
    PlayerRoomBitsToCharConvert();
    PlayerRoomString.SetAt(PlayerRoomCharPos, PlayerRoomChar);
    PlayerRoomStringWrite();
    return true;
  }
}

/***********************************************************
* Convert from PlayerRoom char to PlayerRoom bits          *
************************************************************/

void Player::PlayerRoomCharToBitsConvert()
{
  int BitPos;
  int Char;
  int Remainder;

  Char = PlayerRoomChar;
  PlayerRoomBits.reset();
  for (BitPos=7; BitPos>=0; BitPos--)
  { // For each bit
    Remainder = (int) Char - (int) pow(2,BitPos);
    if (Remainder > -1)
    { // Set bit on
      PlayerRoomBits.set(BitPos);
      Char = Remainder;
    }
  }
}

/***********************************************************
* Convert from PlayerRoom bits to PlayerRoom char          *
************************************************************/

void Player::PlayerRoomBitsToCharConvert()
{
  int BitPos;

  PlayerRoomChar = 0;
  for (BitPos=7; BitPos>=0; BitPos--)
  { // For each bit
    if (PlayerRoomBits.test(BitPos))
    { // Bit is set
      PlayerRoomChar = (int) PlayerRoomChar + (unsigned char) pow(2,BitPos);
    }
  }
}

/***********************************************************
* Read PlayerRoomString from disk                          *
************************************************************/

void Player::PlayerRoomStringRead()
{
  CStdioFile BitsetFile;
  CString    BitsetFileName;
  char       PlayerRoomBuffer[MAX_ROOMS_CHAR];
  int        Success;

  // Initialize exloration tracking string
  for (PlayerRoomCharPos=0; PlayerRoomCharPos<MAX_ROOMS_CHAR; PlayerRoomCharPos++)
  {
    PlayerRoomString.Insert(PlayerRoomCharPos, '\x00');
  }
  BitsetFileName =  PLAYER_ROOM_DIR;
  BitsetFileName += Name;
  BitsetFileName += ".txt";
  Success = BitsetFile.Open(BitsetFileName,
                 CFile::modeRead |
                 CFile::typeText);
  if(!Success)
  {  // Save exloration tracking string and exit
    PlayerRoomStringWrite();
    return;
  }
  BitsetFile.Read(PlayerRoomBuffer, MAX_ROOMS_CHAR);
  for (PlayerRoomCharPos=0; PlayerRoomCharPos<MAX_ROOMS_CHAR; PlayerRoomCharPos++)
  {
    PlayerRoomString.SetAt(PlayerRoomCharPos, PlayerRoomBuffer[PlayerRoomCharPos]);
  }
  BitsetFile.Close();
}


/***********************************************************
* Write PlayerRoomString to disk                           *
************************************************************/

void Player::PlayerRoomStringWrite()
{
  CStdioFile BitsetFile;
  CString    BitsetFileName;
  int        Success;

  BitsetFileName =  PLAYER_ROOM_DIR;
  BitsetFileName += Name;
  BitsetFileName += ".txt";
  Success = BitsetFile.Open(BitsetFileName,
                 CFile::modeCreate |
                 CFile::modeWrite  |
                 CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Player::PlayerRoomStringWrite -  file failed to open", MB_ICONSTOP);
    _endthread();
  }
  BitsetFile.Write((LPCTSTR) PlayerRoomString, MAX_ROOMS_CHAR);
  BitsetFile.Close();
}