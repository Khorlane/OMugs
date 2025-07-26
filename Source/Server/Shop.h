/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Shop.h                                           *
* Usage:  Handles all shop related activity                *
* Author: Steve Bryant                                     *
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
    static  bool    IsShop(string RoomId);
    static  void    IsShopObj(string RoomId, string ObjectName);
    static  void    ListObjects();
};

#endif