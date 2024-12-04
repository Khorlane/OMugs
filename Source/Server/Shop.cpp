/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Shop.cpp                                         *
* Usage:  Handles all shop related activity                *
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
#include "Shop.h"

/***********************************************************
* Globals                                                  *
************************************************************/

////////////////////////////////////////////////////////////
// Public functions static                                //
////////////////////////////////////////////////////////////

/***********************************************************
 * Is a valid shop?                                        *
 ***********************************************************/

bool Shop::IsShop(string RoomId)
{
  string     ShopFileName;
  ifstream   ShopFile;
  
  ShopFileName = SHOPS_DIR;
  ShopFileName += RoomId + ".txt";
  ShopFile.open(ShopFileName);
  if (!ShopFile.is_open())
  {
    return true;
    ShopFile.close();
  }
  else
  {
    return false;
  }
}

/***********************************************************
 * Is this shop buying and selling this object?            *
 ***********************************************************/

void Shop::IsShopObj(string RoomId, string ObjectName)
{
  string      LogBuf;
  string      NamesCheck;
  string      ObjectId;
  int         Result;
  string      ShopFileName;
  ifstream    ShopFile;
  string      ShopText;
  string      Stuff;

  ShopFileName = SHOPS_DIR;
  ShopFileName += RoomId + ".txt";
  ShopFile.open(ShopFileName);
  if (!ShopFile.is_open())
  { // No such file???, But there should be, This is bad!
    AfxMessageBox("Shop::IsShopObj - Shop does not exist", MB_ICONSTOP);
    _endthread();
  }
  getline(ShopFile, Stuff);
  while (Stuff != "End of Items")
  { // Read 'item' lines in ShopFile
    Stuff = StrTrimLeft(Stuff);
    Stuff = StrTrimRight(Stuff);
    Stuff = StrMakeLower(Stuff);
    if (StrGetWord(Stuff, 1) == "item:")
    { // Found an item
      ObjectId = StrGetWord(Stuff, 2);
      ObjectName = StrMakeLower(ObjectName);
      if (ObjectName == ObjectId)
      { // Found a match
        pObject = new Object(ObjectId);
        if (pObject)
        { // Object exists
          return;
        }
        else
        { // Object does not exist, Log it
          LogBuf =  ObjectId;
          LogBuf += " is an invalid shop item - ";
          LogBuf += "Shop::IsShopObj";
          LogIt(LogBuf);
          delete pObject;
          pObject = NULL;
          return;
        }
      }
    }
    getline(ShopFile, Stuff);;
  }
  // Object not found in shop item list
  ShopFile.close();
  //***************************************************
  //* No match found, try getting match using 'names' *
  //***************************************************
  ShopFile.open(ShopFileName);
  if (!ShopFile.is_open())
  { // No such file???, But there should be, This is bad!
    AfxMessageBox("Shop::IsShopObj - Shop does not exist", MB_ICONSTOP);
    _endthread();
  }
  getline(ShopFile, Stuff);
  while (Stuff != "End of Items")
  { // Read 'item' lines in ShopFile
    Stuff = StrTrimLeft(Stuff);
    Stuff = StrTrimRight(Stuff);
    Stuff = StrMakeLower(Stuff);
    if (StrGetWord(Stuff, 1) == "item:")
    { // Found an item
      ObjectId = StrGetWord(Stuff, 2);
      pObject = new Object(ObjectId);
      if (pObject)
      { // Check for a match
        NamesCheck = pObject->Names;
        NamesCheck = StrMakeLower(NamesCheck);
        Result = StrFind(NamesCheck, ObjectName);
        if (Result != -1)
        { // Match, Object found in this shop
          ShopFile.close();
          return;
        }
        else
        {
          delete pObject;
          pObject = NULL;
        }
      }
      else
      { // Object does not exist, Log it
        LogBuf =  ObjectId;
        LogBuf += " is an invalid shop item - ";
        LogBuf += "Shop::IsShopObj";
        LogIt(LogBuf);
        delete pObject;
        pObject = NULL;
      }
    }
    getline(ShopFile, Stuff);
  }
  ShopFile.close();
  // No match found, Object is not buyable from this shop
  return;
}

/***********************************************************
 * List objects that can be bought and sold                *
 ***********************************************************/

void Shop::ListObjects()
{
  int         i;
  int         j;
  string      LogBuf;
  string      ObjectId;
  string      ShopFileName;
  ifstream    ShopFile;
  string      ShopText;
  string      Stuff;
  string      TmpStr;
  
  ShopFileName = SHOPS_DIR;
  ShopFileName += pDnodeActor->pPlayer->RoomId + ".txt";
  ShopFile.open(ShopFileName);
  if (!ShopFile.is_open())
  { // No such file???, But there should be, This is bad!
    AfxMessageBox("Shop::ListObjects - Shop does not exist", MB_ICONSTOP);
    _endthread();
  }
  // Shop welcome message
  getline(ShopFile, Stuff);
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "&W";
  pDnodeActor->PlayerOut += Stuff;
  pDnodeActor->PlayerOut += "&N";
  pDnodeActor->PlayerOut += "\r\n";
  // Headings
  pDnodeActor->PlayerOut += "\r\n";
  // Line one
  sprintf(Buf, "%-45s", "Items you may buy and sell");
  ShopText = Buf;
  pDnodeActor->PlayerOut += ShopText;
  pDnodeActor->PlayerOut += " ";
  sprintf(Buf, "%-6s", "Amount");
  ShopText = Buf;
  pDnodeActor->PlayerOut += ShopText;
  pDnodeActor->PlayerOut += "\r\n";
  // Line two
  sprintf(Buf, "%-45s", " ");
  ShopText = Buf;
  StrReplace(ShopText, " ", "-");
  pDnodeActor->PlayerOut += ShopText;
  pDnodeActor->PlayerOut += " ";
  sprintf(Buf, "%-6s", " ");
  ShopText = Buf;
  StrReplace(ShopText, " ", "-");
  pDnodeActor->PlayerOut += ShopText;
  pDnodeActor->PlayerOut += "\r\n";
  // List items for trade
  getline(ShopFile, Stuff);
  while (Stuff != "End of Items")
  { // Read 'item' lines in ShopFile
    Stuff = StrTrimLeft(Stuff);
    Stuff = StrTrimRight(Stuff);
    Stuff = StrMakeLower(Stuff);
    if (StrGetWord(Stuff, 1) == "item:")
    { // Found an item
      ObjectId = StrGetWord(Stuff, 2);
      pObject = NULL;
      IsObject(ObjectId); // Sets pObject
      if (pObject)
      { // Format shop item text
        sprintf(Buf, "%-45s", pObject->Desc1.c_str());
        ShopText = Buf;
        TmpStr = ShopText;
        i = StrCountChar(TmpStr, '&');
        i = i * 2;
        for (j = 1; j <= i; j++)
        { // Color codes will be removed, so adjust length
          ShopText += " ";
        }
        pDnodeActor->PlayerOut += ShopText;
        pDnodeActor->PlayerOut += " ";
        sprintf(Buf, "%6d", pObject->Cost);
        ShopText = Buf;
        pDnodeActor->PlayerOut += ShopText;
        pDnodeActor->PlayerOut += "\r\n";
        // Done with object
        delete pObject;
        pObject = NULL;
      }
      else
      { // Object does not exist, Log it
        LogBuf =  ObjectId;
        LogBuf += " is an invalid shop item - ";
        LogBuf += "Shop::ListObjects";
        LogIt(LogBuf);
      }
    }
    getline(ShopFile, Stuff);
  }
  ShopFile.close();
}