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
    static  int     GetMobileArmor(CString MobileId);
    static  CString GetMobileAttack(CString MobileId);
    static  int     GetMobileDamage(CString MobileId);
    static  CString GetMobileDesc1(CString MobileId);
    static  CString GetMobileExpPointsLevel(CString MobileId);
    static  CString GetMobileHitPoints(CString MobileId);
    static  CString GetMobileLoot(CString MobileId);
    static  CString GetMobileRoom(CString MobileId);
    static  CString GetMobPlayerMobileId(CString PlayerName, int i);
    static  CString GetPlayerMobMobileId(CString PlayerName);
    static  CString WhackMobile(CString PlayerName, int DamageToMobile, CString MobileDesc1, CString WeaponType);
    static  CString WhackPlayer(CString MobileDesc1, CString MobileAttack, int DamageToPlayer);
};

#endif