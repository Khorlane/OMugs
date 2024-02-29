/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Violence.h                                       *
* Usage:  Violence related fucntions                       *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef VIOLENCE_H
#define VIOLENCE_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Utility.h"

/***********************************************************
* Define Voilence class                                    *
************************************************************/

class Violence
{

// Public functions static
  public:
    static  int     CalcDamageToMobile(int Damage, int WeaponSkill);
    static  int     CalcDamageToPlayer(int Damage, int PAC);
    static  CString CalcHealthPct(int HitPoints, int HitPointsMax);
    static  int     GetMobileArmor(string MobileId);
    static  string  GetMobileAttack(string MobileId);
    static  int     GetMobileDamage(string MobileId);
    static  string  GetMobileDesc1(string MobileId);
    static  string  GetMobileExpPointsLevel(string MobileId);
    static  string  GetMobileHitPoints(string MobileId);
    static  string  GetMobileLoot(string MobileId);
    static  string  GetMobileRoom(string MobileId);
    static  string  GetMobPlayerMobileId(string PlayerName, int i);
    static  string  GetPlayerMobMobileId(string PlayerName);
    static  string  WhackMobile(string PlayerName, int DamageToMobile, string MobileDesc1, string WeaponType);
    static  string  WhackPlayer(string MobileDesc1, string MobileAttack, int DamageToPlayer);
};

#endif