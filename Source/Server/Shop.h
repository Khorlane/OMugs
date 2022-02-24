/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Shop.h                                           *
* Usage:  Handles all shop related activity                *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef SHOP_H
#define SHOP_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Dnode.h"
#include "Log.h"
#include "Object.h"

/***********************************************************
* Define Shop class                                        *
************************************************************/

class Shop
{

// Public functions static
  public:
    static  bool    IsShop(CString RoomId);
    static  void    IsShopObj(CString RoomId, CString ObjectName);
    static  void    ListObjects();
};

#endif