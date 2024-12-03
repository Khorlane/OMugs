/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Violence.cpp                                     *
* Usage:  Violence related fucntions                       *
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
#include "Violence.h"

/***********************************************************
* Globals                                                  *
************************************************************/

////////////////////////////////////////////////////////////
// Public functions static                                //
////////////////////////////////////////////////////////////

/***********************************************************
 * Calculate damage to mobile                              *
 ***********************************************************/

int Violence::CalcDamageToMobile(int Damage, int WeaponSkill)    
{
  int CalcDamage;
  int Deduction;
  int Percent;
  int SkillFactor;

  SkillFactor  = int(WeaponSkill/1.66);
  Percent      = GetRandomNumber(PLAYER_DMG_PCT-SkillFactor);
  Deduction    = int(ceil(Damage*(Percent/100.0)));
  CalcDamage   = Damage - Deduction;
  
  return CalcDamage;
}

/***********************************************************
 * Calculate damage to player                              *
 ***********************************************************/

int Violence::CalcDamageToPlayer(int Damage, int PAC)
{
  int CalcDamage;
  int Deduction;
  int Percent;

  Percent    = GetRandomNumber(PLAYER_DMG_PCT);
  Deduction  = int(ceil(Damage*(Percent/100.0)));
  CalcDamage = Damage - Deduction;
  CalcDamage = int(floor(CalcDamage-(PAC*PACMN*CalcDamage)));
  return CalcDamage;
}

/***********************************************************
 * Calculate damage to player                              *
 ***********************************************************/

string Violence::CalcHealthPct(int HitPoints, int HitPointsMax)
{
  string  HealthPct;
  int     Percent;
  string  TmpStr;

  if (HitPoints < 1)
  { // Skip percent calculation
    Percent = 0;
  }
  else
  { // Calculate percent
    Percent = CalcPct(HitPoints, HitPointsMax);
  }
  if (Percent > 75)
  {
    HealthPct = "&C";
  }
  else
  if (Percent > 50)
  {
    HealthPct = "&Y";
  }
  else
  if (Percent > 25)
  {
    HealthPct = "&M";
  }
  else
  {
    HealthPct = "&R";
  }
  sprintf(Buf, "%3d", int(Percent));
  TmpStr = Buf;
  HealthPct += TmpStr;
  HealthPct += "&N";
  return HealthPct;
}

/***********************************************************
 * Get mobile Armor                                        *
 ***********************************************************/

int Violence::GetMobileArmor(string MobileId)
{
  int        MobileArmor;
  ifstream   MobStatsArmorFile;
  string     MobStatsArmorFileName;
  string     Stuff;

  // Read mobile stats Armor file
  MobStatsArmorFileName = MOB_STATS_ARM_DIR;
  MobStatsArmorFileName += MobileId;
  MobStatsArmorFileName += ".txt";
  MobStatsArmorFile.open(MobStatsArmorFileName);
  if (!MobStatsArmorFile.is_open())
  {
    // Mobile Armor is not implemented, so for now, we just return zero
    MobileArmor = 0;
    return MobileArmor;
    // This code is currently unreachable, on purpose.
    AfxMessageBox("Violence::GetArmor - Open MobStatsArmorFile file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  getline(MobStatsArmorFile, Stuff);;
  MobStatsArmorFile.close();
  // Return mobile's Armor
  Stuff = StrTrimLeft(Stuff);
  Stuff = StrTrimRight(Stuff);
  MobileArmor = stoi(Stuff);
  return MobileArmor;
}

/***********************************************************
 * Get mobile Attack                                       *
 ***********************************************************/

string Violence::GetMobileAttack(string MobileId)
{
  string     MobileAttack;
  ifstream   MobStatsAttackFile;
  string     MobStatsAttackFileName;
  string     Stuff;

  // Read mobile stats Attack file
  MobStatsAttackFileName = MOB_STATS_ATK_DIR;
  MobStatsAttackFileName += MobileId;
  MobStatsAttackFileName += ".txt";
  MobStatsAttackFile.open(MobStatsAttackFileName);
  if (!MobStatsAttackFile.is_open())
  {
    AfxMessageBox("Violence::GetMobileAttack - Open MobStatsAttack file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  getline(MobStatsAttackFile, Stuff);
  MobStatsAttackFile.close();
  // Return mobile's Attack
  Stuff = StrTrimLeft(Stuff);
  Stuff = StrTrimRight(Stuff);
  MobileAttack = Stuff;
  return MobileAttack;
}

/***********************************************************
 * Get mobile Damage                                       *
 ***********************************************************/

int Violence::GetMobileDamage(string MobileId)
{
  int        MobileDamage;
  ifstream   MobStatsDamageFile;
  string     MobStatsDamageFileName;
  string     Stuff;

  // Read mobile stats Damage file
  MobStatsDamageFileName = MOB_STATS_DMG_DIR;
  MobStatsDamageFileName += MobileId;
  MobStatsDamageFileName += ".txt";
  MobStatsDamageFile.open(MobStatsDamageFileName);
  if (!MobStatsDamageFile.is_open())
  {
    AfxMessageBox("Violence::GetMobileDamage - Open MobStatsDamageFile file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  getline(MobStatsDamageFile, Stuff);
  MobStatsDamageFile.close();
  // Return mobile's Damage
  Stuff = StrTrimLeft(Stuff);
  Stuff = StrTrimRight(Stuff);
  MobileDamage = stoi(Stuff);
  return MobileDamage;
}

/***********************************************************
 * Get mobile Desc1                                        *
 ***********************************************************/

string Violence::GetMobileDesc1(string MobileId)
{
  string     MobileDesc1;
  ifstream   MobStatsDesc1File;
  string     MobStatsDesc1FileName;
  string     Stuff;

  // Read mobile stats Desc1 file
  MobStatsDesc1FileName = MOB_STATS_DSC_DIR;
  MobStatsDesc1FileName += MobileId;
  MobStatsDesc1FileName += ".txt";
  MobStatsDesc1File.open(MobStatsDesc1FileName);
  if(!MobStatsDesc1File.is_open())
  {
    AfxMessageBox("Violence::GetMobileDesc1 - Open MobStatsDesc1 file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  getline(MobStatsDesc1File, Stuff);
  MobStatsDesc1File.close();
  // Return mobile's Desc1
  Stuff = StrTrimLeft(Stuff);
  Stuff = StrTrimRight(Stuff);
  MobileDesc1 = Stuff;
  return MobileDesc1;
}

/***********************************************************
 * Get mobile Experience Points                            *
 ***********************************************************/

string Violence::GetMobileExpPointsLevel(string MobileId)
{
  ifstream   MobStatsExpPointsFile;
  string     MobStatsExpPointsFileName;
  string     Stuff;

  // Read mobile stats ExpPoints and Level file
  MobStatsExpPointsFileName = MOB_STATS_EXP_DIR;
  MobStatsExpPointsFileName += MobileId;
  MobStatsExpPointsFileName += ".txt";
  MobStatsExpPointsFile.open(MobStatsExpPointsFileName);
  if(!MobStatsExpPointsFile.is_open())
  {
    AfxMessageBox("Violence::GetMobileExpPointsLevel - Open MobStatsExpPointsFile file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  getline(MobStatsExpPointsFile, Stuff);
  MobStatsExpPointsFile.close();
  // Return mobile's ExpPoints
  Stuff = StrTrimLeft(Stuff);
  Stuff = StrTrimRight(Stuff);
  return Stuff;
}

/***********************************************************
 * Get mobile Hit Points                                   *
 ***********************************************************/

string Violence::GetMobileHitPoints(string MobileId)
{
  ifstream   MobStatsHitPointsFile;
  string     MobStatsHitPointsFileName;
  string     MobHitPoints;
  string     Stuff;

  // Read mobile stats hit points file
  MobStatsHitPointsFileName = MOB_STATS_HPT_DIR;
  MobStatsHitPointsFileName += MobileId;
  MobStatsHitPointsFileName += ".txt";
  MobStatsHitPointsFile.open(MobStatsHitPointsFileName);
  if(!MobStatsHitPointsFile.is_open())
  {
    AfxMessageBox("Violence::WhackMobile - Open MobStatsHitPointsFile file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  getline(MobStatsHitPointsFile, Stuff);
  MobStatsHitPointsFile.close();
  MobHitPoints = Stuff;
  return Stuff;
}

/***********************************************************
 * Get mobile Loot                                         *
 ***********************************************************/

string Violence::GetMobileLoot(string MobileId)
{
  string     MobileLoot;
  ifstream   MobStatsLootFile;
  string     MobStatsLootFileName;
  string     Stuff;

  // Read mobile stats Loot file
  MobStatsLootFileName = MOB_STATS_LOOT_DIR;
  MobStatsLootFileName += MobileId;
  MobStatsLootFileName += ".txt";
  MobStatsLootFile.open(MobStatsLootFileName);
  if(!MobStatsLootFile.is_open())
  {
    AfxMessageBox("Violence::GetMobileLoot - Open MobStatsLoot file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  getline(MobStatsLootFile, Stuff);
  MobStatsLootFile.close();
  // Return mobile's Loot
  Stuff = StrTrimLeft(Stuff);
  Stuff = StrTrimRight(Stuff);
  MobileLoot = Stuff;
  return MobileLoot;
}

/***********************************************************
 * Get mobile Room                                         *
 ***********************************************************/

string Violence::GetMobileRoom(string MobileId)
{
  string     MobileRoom;
  ifstream   MobStatsRoomFile;
  string     MobStatsRoomFileName;
  string     Stuff;

  // Read mobile stats Loot file
  MobStatsRoomFileName = MOB_STATS_ROOM_DIR;
  MobStatsRoomFileName += MobileId;
  MobStatsRoomFileName += ".txt";
  MobStatsRoomFile.open(MobStatsRoomFileName);
  if(!MobStatsRoomFile.is_open())
  {
    AfxMessageBox("Violence::GetMobileRoom - Open MobStatsRoom file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  getline(MobStatsRoomFile, Stuff);
  MobStatsRoomFile.close();
  // Return mobile's Room
  Stuff = StrTrimLeft(Stuff);
  Stuff = StrTrimRight(Stuff);
  MobileRoom = Stuff;
  return MobileRoom;
}

/***********************************************************
 * Get MobPlayer MobileId                                  *
 ***********************************************************/

string Violence::GetMobPlayerMobileId(string PlayerName, int i)
{
  int        j;
  string     MobileId;
  ifstream   MobPlayerFile;
  string     MobPlayerFileName;

  MobPlayerFileName =  MOB_PLAYER_DIR;
  MobPlayerFileName += PlayerName;
  MobPlayerFileName += ".txt";
  MobPlayerFile.open(MobPlayerFileName);
  if(!MobPlayerFile.is_open())
  {
    MobileId = "No more mobiles";
    return MobileId;
  }
  for (j = 1; j <= i; j++)
  {
    getline(MobPlayerFile, MobileId);
  }
  MobPlayerFile.close();
  MobileId = StrTrimLeft(MobileId);
  MobileId = StrTrimRight(MobileId);
  if (MobileId == "")
  {
    MobileId = "No more mobiles";
  }
  return MobileId;
}

/***********************************************************
 * Get PlayerMob MobileId                                  *
 ***********************************************************/

string Violence::GetPlayerMobMobileId(string PlayerName)
{
  string     MobileId;
  ifstream   PlayerMobFile;
  string     PlayerMobFileName;

  PlayerMobFileName =  PLAYER_MOB_DIR;
  PlayerMobFileName += PlayerName;
  PlayerMobFileName += ".txt";
  PlayerMobFile.open(PlayerMobFileName);
  if(!PlayerMobFile.is_open())
  {
    AfxMessageBox("Violence::GetPlayerMobMobileId - Open PlayerMob file failed", MB_ICONSTOP);
    _endthread();
  }
  getline(PlayerMobFile, MobileId);
  PlayerMobFile.close();
  MobileId = StrTrimLeft(MobileId);
  MobileId = StrTrimRight(MobileId);
  return MobileId;
}

/***********************************************************
 * Whack the mobile - do some damage!                      *
 ***********************************************************/

string Violence::WhackMobile(string MobileId, int DamageToMobile, string MobileDesc1, string WeaponType)
{
  string     DamageAmount;
  int        DamageMagnitude;
  string     ExtraDamageMsg;
  string     MobileBeenWhacked;
  string     MobHealthPct;
  int        MobHealthPctNew;
  int        MobHealthPctOld;
  string     MobHitPointsInfo;
  int        MobHitPointsLeft;
  int        MobHitPointsTotal;
  ofstream   MobStatsHitPointsFile;
  string     MobStatsHitPointsFileName;
  string     Stuff;
  string     TmpStr;
  string     WeaponAction;

  // Get mobile's total hit points and hit points left
  MobHitPointsInfo  = GetMobileHitPoints(MobileId);
  MobHitPointsTotal = stoi(StrGetWord(MobHitPointsInfo, 1));
  MobHitPointsLeft  = stoi(StrGetWord(MobHitPointsInfo, 2));
  MobHealthPctOld   = CalcPct(MobHitPointsLeft, MobHitPointsTotal);
  // Reduce mobile's hit points by damage done and write to file
  MobHitPointsLeft = MobHitPointsLeft - DamageToMobile;
  if (MobHitPointsLeft < 0)
  { // Keep MobHitPointsLeft from going negative
    MobHitPointsLeft = 0;
  }
  MobStatsHitPointsFileName = MOB_STATS_HPT_DIR;
  MobStatsHitPointsFileName += MobileId;
  MobStatsHitPointsFileName += ".txt";
  MobStatsHitPointsFile.open(MobStatsHitPointsFileName);
  if(!MobStatsHitPointsFile.is_open())
  { // Open failed - very bad
    AfxMessageBox("Violence::WhackMobile - Open MobStatsHitPointsFile file failed (write)", MB_ICONSTOP);
    _endthread();
  }
  sprintf(Buf, "%d", MobHitPointsTotal);
  TmpStr = Buf;
  Stuff  = TmpStr;
  Stuff += " ";
  sprintf(Buf, "%d", MobHitPointsLeft);
  TmpStr = Buf;
  Stuff += TmpStr;
  MobStatsHitPointsFile << Stuff << endl;
  MobStatsHitPointsFile.close();
  // Format whack message
  MobHealthPct = CalcHealthPct(MobHitPointsLeft, MobHitPointsTotal);
  WeaponType = StrMakeLower(WeaponType);
  WeaponAction = TranslateWord(WeaponType);
  sprintf(Buf, "%d", DamageToMobile);
  DamageAmount = Buf;
  MobHealthPctNew = CalcPct(MobHitPointsLeft, MobHitPointsTotal);
  DamageMagnitude = MobHealthPctOld - MobHealthPctNew;
  if (DamageMagnitude > 70)
  {
    ExtraDamageMsg = "&RALMOST OBLITERATE&N";
  }
  else
  if (DamageMagnitude > 60)
  {
    ExtraDamageMsg = "&RSMACK DOWN&N";
  }
  else
  if (DamageMagnitude > 50)
  {
    ExtraDamageMsg = "&MSEVERLY WOUND&N";
  }
  else
  if (DamageMagnitude > 40)
  {
    ExtraDamageMsg = "&YREALLY HURT&N";
  }
  if (MobHitPointsLeft > 0)
  { // Mobile is still alive
    MobileBeenWhacked  = "alive";
    MobileBeenWhacked += " ";
    MobileBeenWhacked += MobHealthPct;
    MobileBeenWhacked += " ";
    if (DamageMagnitude > 40)
    {
      MobileBeenWhacked += "You";
      MobileBeenWhacked += " ";
      MobileBeenWhacked += ExtraDamageMsg;
      MobileBeenWhacked += " ";
      MobileBeenWhacked += MobileDesc1;
      MobileBeenWhacked += " ";
      MobileBeenWhacked += "with your";
      MobileBeenWhacked += " ";
      MobileBeenWhacked += WeaponAction;
      MobileBeenWhacked += " ";
      MobileBeenWhacked += "of";
      MobileBeenWhacked += " ";
      MobileBeenWhacked += DamageAmount;
      MobileBeenWhacked += " ";
      MobileBeenWhacked += "points of damage.";
    }
    else
    {
      MobileBeenWhacked += "You";
      MobileBeenWhacked += " ";
      MobileBeenWhacked += WeaponAction;
      MobileBeenWhacked += " ";
      MobileBeenWhacked += MobileDesc1;
      MobileBeenWhacked += " ";
      MobileBeenWhacked += "for";
      MobileBeenWhacked += " ";
      MobileBeenWhacked += DamageAmount;
      MobileBeenWhacked += " ";
      MobileBeenWhacked += "points of damage.";
    }
  }
  else
  { // Mobile is dead
    MobileBeenWhacked  = "dead";
    MobileBeenWhacked += " ";
    if (MobHealthPctOld == 100 && MobHealthPctNew == 0)
    { // Mob was killed with one hit
      MobileBeenWhacked += "You";
      MobileBeenWhacked += " ";
      MobileBeenWhacked += "&ROBLITERATED&N";
      MobileBeenWhacked += " ";
      MobileBeenWhacked += MobileDesc1;
      MobileBeenWhacked += " ";
      MobileBeenWhacked += "with a";
      MobileBeenWhacked += " ";
      MobileBeenWhacked += WeaponAction;
      MobileBeenWhacked += " ";
      MobileBeenWhacked += "that did";
      MobileBeenWhacked += " ";
      MobileBeenWhacked += DamageAmount;
      MobileBeenWhacked += " ";
      MobileBeenWhacked += "points of damage.";
    }
    else
    { // Mob died from more than one hit
      MobileBeenWhacked += "You";
      MobileBeenWhacked += " ";
      MobileBeenWhacked += "vanquish";
      MobileBeenWhacked += " ";
      MobileBeenWhacked += MobileDesc1;
      MobileBeenWhacked += " ";
      MobileBeenWhacked += "with a";
      MobileBeenWhacked += " ";
      MobileBeenWhacked += WeaponAction;
      MobileBeenWhacked += " ";
      MobileBeenWhacked += "that did";
      MobileBeenWhacked += " ";
      MobileBeenWhacked += DamageAmount;
      MobileBeenWhacked += " ";
      MobileBeenWhacked += "points of damage.";
    }
  }
  return MobileBeenWhacked;
}

/***********************************************************
 * Whack the player - do some damage!                      *
 ***********************************************************/

string Violence::WhackPlayer(string MobileDesc1, string MobileAttack, int DamageToPlayer)
{
  string PlayerBeenWhacked;
  string TmpStr;

  // Capitalize first leter of first word of MobileDesc1
  MobileDesc1 = StrMakeFirstUpper(MobileDesc1);
  // Format damage message
  sprintf(Buf, "%d", DamageToPlayer);
  TmpStr = Buf;
  PlayerBeenWhacked =  MobileDesc1;
  PlayerBeenWhacked += " ";
  PlayerBeenWhacked += MobileAttack;
  PlayerBeenWhacked += " ";
  PlayerBeenWhacked += "you for";
  PlayerBeenWhacked += " ";
  PlayerBeenWhacked += TmpStr;
  PlayerBeenWhacked += " ";
  PlayerBeenWhacked += "points of damage.";
  return PlayerBeenWhacked;
}