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

CString Violence::CalcHealthPct(int HitPoints, int HitPointsMax)
{
  CString HealthPct;
  int     Percent;
  CString TmpStr;

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
  TmpStr = ConvertStringToCString(Buf);
  HealthPct += TmpStr;
  HealthPct += "&N";
  return HealthPct;
}

/***********************************************************
 * Get mobile Armor                                        *
 ***********************************************************/

int Violence::GetMobileArmor(CString MobileId)
{
  int        MobileArmor;
  CStdioFile MobStatsArmorFile;
  CString    MobStatsArmorFileName;
  CString    Stuff;
  int        Success;

  // Read mobile stats Armor file
  MobStatsArmorFileName = MOB_STATS_ARM_DIR;
  MobStatsArmorFileName += MobileId;
  MobStatsArmorFileName += ".txt";
  Success = MobStatsArmorFile.Open(MobStatsArmorFileName,
                        CFile::modeRead |
                        CFile::typeText);
  if(!Success)
  {
    // Mobile Armor is not implemented, so for now, we just return zero
    MobileArmor = 0;
    return MobileArmor;
    // This code is currently unreachable, on purpose.
    AfxMessageBox("Violence::GetArmor - Open MobStatsArmorFile file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  MobStatsArmorFile.ReadString(Stuff);
  MobStatsArmorFile.Close();
  // Return mobile's Armor
  Stuff = StrTrimLeft(Stuff);
  Stuff = StrTrimRight(Stuff);
  MobileArmor = atoi(Stuff);
  return MobileArmor;
}

/***********************************************************
 * Get mobile Attack                                       *
 ***********************************************************/

CString Violence::GetMobileAttack(CString MobileId)
{
  CString    MobileAttack;
  CStdioFile MobStatsAttackFile;
  CString    MobStatsAttackFileName;
  CString    Stuff;
  int        Success;

  // Read mobile stats Attack file
  MobStatsAttackFileName = MOB_STATS_ATK_DIR;
  MobStatsAttackFileName += MobileId;
  MobStatsAttackFileName += ".txt";
  Success = MobStatsAttackFile.Open(MobStatsAttackFileName,
                        CFile::modeRead |
                        CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Violence::GetMobileAttack - Open MobStatsAttack file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  MobStatsAttackFile.ReadString(Stuff);
  MobStatsAttackFile.Close();
  // Return mobile's Attack
  Stuff = StrTrimLeft(Stuff);
  Stuff = StrTrimRight(Stuff);
  MobileAttack = Stuff;
  return MobileAttack;
}

/***********************************************************
 * Get mobile Damage                                       *
 ***********************************************************/

int Violence::GetMobileDamage(CString MobileId)
{
  int        MobileDamage;
  CStdioFile MobStatsDamageFile;
  CString    MobStatsDamageFileName;
  CString    Stuff;
  int        Success;

  // Read mobile stats Damage file
  MobStatsDamageFileName = MOB_STATS_DMG_DIR;
  MobStatsDamageFileName += MobileId;
  MobStatsDamageFileName += ".txt";
  Success = MobStatsDamageFile.Open(MobStatsDamageFileName,
                         CFile::modeRead |
                         CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Violence::GetMobileDamage - Open MobStatsDamageFile file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  MobStatsDamageFile.ReadString(Stuff);
  MobStatsDamageFile.Close();
  // Return mobile's Damage
  Stuff = StrTrimLeft(Stuff);
  Stuff = StrTrimRight(Stuff);
  MobileDamage = atoi(Stuff);
  return MobileDamage;
}

/***********************************************************
 * Get mobile Desc1                                        *
 ***********************************************************/

CString Violence::GetMobileDesc1(CString MobileId)
{
  CString    MobileDesc1;
  CStdioFile MobStatsDesc1File;
  CString    MobStatsDesc1FileName;
  CString    Stuff;
  int        Success;

  // Read mobile stats Desc1 file
  MobStatsDesc1FileName = MOB_STATS_DSC_DIR;
  MobStatsDesc1FileName += MobileId;
  MobStatsDesc1FileName += ".txt";
  Success = MobStatsDesc1File.Open(MobStatsDesc1FileName,
                        CFile::modeRead |
                        CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Violence::GetMobileDesc1 - Open MobStatsDesc1 file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  MobStatsDesc1File.ReadString(Stuff);
  MobStatsDesc1File.Close();
  // Return mobile's Desc1
  Stuff = StrTrimLeft(Stuff);
  Stuff = StrTrimRight(Stuff);
  MobileDesc1 = Stuff;
  return MobileDesc1;
}

/***********************************************************
 * Get mobile Experience Points                            *
 ***********************************************************/

CString Violence::GetMobileExpPointsLevel(CString MobileId)
{
  CStdioFile MobStatsExpPointsFile;
  CString    MobStatsExpPointsFileName;
  CString    Stuff;
  int        Success;

  // Read mobile stats ExpPoints and Level file
  MobStatsExpPointsFileName = MOB_STATS_EXP_DIR;
  MobStatsExpPointsFileName += MobileId;
  MobStatsExpPointsFileName += ".txt";
  Success = MobStatsExpPointsFile.Open(MobStatsExpPointsFileName,
                        CFile::modeRead |
                        CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Violence::GetMobileExpPointsLevel - Open MobStatsExpPointsFile file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  MobStatsExpPointsFile.ReadString(Stuff);
  MobStatsExpPointsFile.Close();
  // Return mobile's ExpPoints
  Stuff = StrTrimLeft(Stuff);
  Stuff = StrTrimRight(Stuff);
  return Stuff;
}

/***********************************************************
 * Get mobile Hit Points                                   *
 ***********************************************************/

CString Violence::GetMobileHitPoints(CString MobileId)
{
  CStdioFile MobStatsHitPointsFile;
  CString    MobStatsHitPointsFileName;
  CString    MobHitPoints;
  CString    Stuff;
  int        Success;

  // Read mobile stats hit points file
  MobStatsHitPointsFileName = MOB_STATS_HPT_DIR;
  MobStatsHitPointsFileName += MobileId;
  MobStatsHitPointsFileName += ".txt";
  Success = MobStatsHitPointsFile.Open(MobStatsHitPointsFileName,
                            CFile::modeRead |
                            CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Violence::WhackMobile - Open MobStatsHitPointsFile file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  MobStatsHitPointsFile.ReadString(Stuff);
  MobStatsHitPointsFile.Close();
  MobHitPoints = Stuff;
  return Stuff;
}

/***********************************************************
 * Get mobile Loot                                         *
 ***********************************************************/

CString Violence::GetMobileLoot(CString MobileId)
{
  CString    MobileLoot;
  CStdioFile MobStatsLootFile;
  CString    MobStatsLootFileName;
  CString    Stuff;
  int        Success;

  // Read mobile stats Loot file
  MobStatsLootFileName = MOB_STATS_LOOT_DIR;
  MobStatsLootFileName += MobileId;
  MobStatsLootFileName += ".txt";
  Success = MobStatsLootFile.Open(MobStatsLootFileName,
                        CFile::modeRead |
                        CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Violence::GetMobileLoot - Open MobStatsLoot file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  MobStatsLootFile.ReadString(Stuff);
  MobStatsLootFile.Close();
  // Return mobile's Loot
  Stuff = StrTrimLeft(Stuff);
  Stuff = StrTrimRight(Stuff);
  MobileLoot = Stuff;
  return MobileLoot;
}

/***********************************************************
 * Get mobile Room                                         *
 ***********************************************************/

CString Violence::GetMobileRoom(CString MobileId)
{
  CString    MobileRoom;
  CStdioFile MobStatsRoomFile;
  CString    MobStatsRoomFileName;
  CString    Stuff;
  int        Success;

  // Read mobile stats Loot file
  MobStatsRoomFileName = MOB_STATS_ROOM_DIR;
  MobStatsRoomFileName += MobileId;
  MobStatsRoomFileName += ".txt";
  Success = MobStatsRoomFile.Open(MobStatsRoomFileName,
                        CFile::modeRead |
                        CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Violence::GetMobileRoom - Open MobStatsRoom file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  MobStatsRoomFile.ReadString(Stuff);
  MobStatsRoomFile.Close();
  // Return mobile's Room
  Stuff = StrTrimLeft(Stuff);
  Stuff = StrTrimRight(Stuff);
  MobileRoom = Stuff;
  return MobileRoom;
}

/***********************************************************
 * Get MobPlayer MobileId                                  *
 ***********************************************************/

CString Violence::GetMobPlayerMobileId(CString PlayerName, int i)
{
  int        j;
  CString    MobileId;
  CStdioFile MobPlayerFile;
  CString    MobPlayerFileName;
  int        Success;

  MobPlayerFileName =  MOB_PLAYER_DIR;
  MobPlayerFileName += PlayerName;
  MobPlayerFileName += ".txt";
  Success = MobPlayerFile.Open(MobPlayerFileName,
                    CFile::modeRead |
                    CFile::typeText);
  if(!Success)
  {
    MobileId = "No more mobiles";
    return MobileId;
  }
  for (j = 1; j <= i; j++)
  {
    MobPlayerFile.ReadString(MobileId);
  }
  MobPlayerFile.Close();
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

CString Violence::GetPlayerMobMobileId(CString PlayerName)
{
  CString    MobileId;
  CStdioFile PlayerMobFile;
  CString    PlayerMobFileName;
  int        Success;

  PlayerMobFileName =  PLAYER_MOB_DIR;
  PlayerMobFileName += PlayerName;
  PlayerMobFileName += ".txt";
  Success = PlayerMobFile.Open(PlayerMobFileName,
                    CFile::modeRead |
                    CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Violence::GetPlayerMobMobileId - Open PlayerMob file failed", MB_ICONSTOP);
    _endthread();
  }
  PlayerMobFile.ReadString(MobileId);
  PlayerMobFile.Close();
  MobileId = StrTrimLeft(MobileId);
  MobileId = StrTrimRight(MobileId);
  return MobileId;
}

/***********************************************************
 * Whack the mobile - do some damage!                      *
 ***********************************************************/

CString Violence::WhackMobile(CString MobileId,
                              int     DamageToMobile,
                              CString MobileDesc1,
                              CString WeaponType)
{
  CString    DamageAmount;
  int        DamageMagnitude;
  CString    ExtraDamageMsg;
  CString    MobileBeenWhacked;
  CString    MobHealthPct;
  int        MobHealthPctNew;
  int        MobHealthPctOld;
  CString    MobHitPointsInfo;
  int        MobHitPointsLeft;
  int        MobHitPointsTotal;
  CStdioFile MobStatsHitPointsFile;
  CString    MobStatsHitPointsFileName;
  CString    Stuff;
  int        Success;
  CString    TmpStr;
  CString    WeaponAction;

  // Get mobile's total hit points and hit points left
  MobHitPointsInfo  = GetMobileHitPoints(MobileId);
  MobHitPointsTotal = atoi(GetWord(MobHitPointsInfo, 1));
  MobHitPointsLeft  = atoi(GetWord(MobHitPointsInfo, 2));
  MobHealthPctOld = CalcPct(MobHitPointsLeft, MobHitPointsTotal);
  // Reduce mobile's hit points by damage done and write to file
  MobHitPointsLeft = MobHitPointsLeft - DamageToMobile;
  if (MobHitPointsLeft < 0)
  { // Keep MobHitPointsLeft from going negative
    MobHitPointsLeft = 0;
  }
  MobStatsHitPointsFileName = MOB_STATS_HPT_DIR;
  MobStatsHitPointsFileName += MobileId;
  MobStatsHitPointsFileName += ".txt";
  Success = MobStatsHitPointsFile.Open(MobStatsHitPointsFileName,
                            CFile::modeWrite |
                            CFile::typeText);
  if(!Success)
  { // Open failed - very bad
    AfxMessageBox("Violence::WhackMobile - Open MobStatsHitPointsFile file failed (write)", MB_ICONSTOP);
    _endthread();
  }
  sprintf(Buf, "%d", MobHitPointsTotal);
  TmpStr = ConvertStringToCString(Buf);
  Stuff  = TmpStr;
  Stuff += " ";
  sprintf(Buf, "%d", MobHitPointsLeft);
  TmpStr = ConvertStringToCString(Buf);
  Stuff += TmpStr;
  MobStatsHitPointsFile.WriteString(Stuff);
  MobStatsHitPointsFile.SetLength(Stuff.GetLength());
  MobStatsHitPointsFile.Close();
  // Format whack message
  MobHealthPct = CalcHealthPct(MobHitPointsLeft, MobHitPointsTotal);
  WeaponType.MakeLower();
  WeaponAction = TranslateWord(WeaponType);
  sprintf(Buf, "%d", DamageToMobile);
  DamageAmount = ConvertStringToCString(Buf);
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

CString Violence::WhackPlayer(CString MobileDesc1,
                              CString MobileAttack,
                              int     DamageToPlayer)
{
  CString PlayerBeenWhacked;
  CString TmpStr;

  // Capitalize first leter of first word of MobileDesc1
  MobileDesc1 = MakeFirstUpper(MobileDesc1);
  // Format damage message
  sprintf(Buf, "%d", DamageToPlayer);
  TmpStr = ConvertStringToCString(Buf);
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