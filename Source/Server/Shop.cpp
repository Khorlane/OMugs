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

bool Shop::IsShop(CString RoomId)
{
  CString    ShopFileName;
  CStdioFile ShopFile;
  int        Success;
  
  ShopFileName = SHOPS_DIR;
  ShopFileName += RoomId + ".txt";
  Success = ShopFile.Open(ShopFileName,
            CFile::modeRead |
            CFile::typeText);
  if(Success)
  {
    return true;
    ShopFile.Close();
  }
  else
  {
    return false;
  }
}

/***********************************************************
 * Is this shop buying and selling this object?            *
 ***********************************************************/

Object *Shop::IsShopObj(CString RoomId, CString ObjectName)
{
  Object     *pObject;
  CString     LogBuf;
  CString     NamesCheck;
  CString     ObjectId;
  CString     ShopFileName;
  CStdioFile  ShopFile;
  CString     ShopText;
  CString     Stuff;
  int         Success;

  ShopFileName = SHOPS_DIR;
  ShopFileName += RoomId + ".txt";
  Success = ShopFile.Open(ShopFileName,
               CFile::modeRead |
               CFile::typeText);
  if(!Success)
  { // No such file???, But there should be, This is bad!
    AfxMessageBox("Shop::IsShopObj - Shop does not exist", MB_ICONSTOP);
    _endthread();
  }
  ShopFile.ReadString(Stuff);
  while (Stuff != "End of Items")
  { // Read 'item' lines in ShopFile
    Stuff.TrimLeft();
    Stuff.TrimRight();
    Stuff.MakeLower();
    if (GetWord(Stuff, 1) == "item:")
    { // Found an item
      ObjectId = GetWord(Stuff, 2);
      ObjectName.MakeLower();
      if (ObjectName == ObjectId)
      { // Found a match
        pObject = new Object(ObjectId);
        if (pObject)
        { // Object exists
          return pObject;
        }
        else
        { // Object does not exist, Log it
          LogBuf =  ObjectId;
          LogBuf += " is an invalid shop item - ";
          LogBuf += "Shop::IsShopObj";
          LogIt(LogBuf);
          return NULL;
        }
      }
    }
    ShopFile.ReadString(Stuff);
  }
  // Object not found in shop item list
  ShopFile.Close();
  //***************************************************
  //* No match found, try getting match using 'names' *
  //***************************************************
  Success = ShopFile.Open(ShopFileName,
               CFile::modeRead |
               CFile::typeText);
  if(!Success)
  { // No such file???, But there should be, This is bad!
    AfxMessageBox("Shop::IsShopObj - Shop does not exist", MB_ICONSTOP);
    _endthread();
  }
  ShopFile.ReadString(Stuff);
  while (Stuff != "End of Items")
  { // Read 'item' lines in ShopFile
    Stuff.TrimLeft();
    Stuff.TrimRight();
    Stuff.MakeLower();
    if (GetWord(Stuff, 1) == "item:")
    { // Found an item
      ObjectId = GetWord(Stuff, 2);
      pObject = new Object(ObjectId);
      if (pObject)
      { // Check for a match
        NamesCheck = pObject->Names;
        NamesCheck.MakeLower();

        Success = NamesCheck.Find(ObjectName);
        if (Success != -1)
        { // Match, Object found in this shop
          ShopFile.Close();
          return pObject;
        }
      }
      else
      { // Object does not exist, Log it
        LogBuf =  ObjectId;
        LogBuf += " is an invalid shop item - ";
        LogBuf += "Shop::IsShopObj";
        LogIt(LogBuf);
      }
    }
    ShopFile.ReadString(Stuff);
  }
  ShopFile.Close();
  // No match found, Object is not buyable from this shop
  return NULL;
}

/***********************************************************
 * List objects that can be bought and sold                *
 ***********************************************************/

void Shop::ListObjects()
{
  Object     *pObject;
  int         i;
  int         j;
  CString     LogBuf;
  CString     ObjectId;
  CString     ShopFileName;
  CStdioFile  ShopFile;
  CString     ShopText;
  CString     Stuff;
  int         Success;
  CString     TmpStr;
  
  ShopFileName = SHOPS_DIR;
  ShopFileName += pDnodeActor->pPlayer->RoomId + ".txt";
  Success = ShopFile.Open(ShopFileName,
            CFile::modeRead |
            CFile::typeText);
  if(!Success)
  { // No such file???, But there should be, This is bad!
    AfxMessageBox("Shop::ListObjects - Shop does not exist", MB_ICONSTOP);
    _endthread();
  }
  // Shop welcome message
  ShopFile.ReadString(Stuff);
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "&W";
  pDnodeActor->PlayerOut += Stuff;
  pDnodeActor->PlayerOut += "&N";
  pDnodeActor->PlayerOut += "\r\n";
  // Headings
  pDnodeActor->PlayerOut += "\r\n";
  // Line one
  sprintf(Buf, "%-45s", "Items you may buy and sell");
  ShopText = ConvertStringToCString(Buf);
  pDnodeActor->PlayerOut += ShopText;
  pDnodeActor->PlayerOut += " ";
  sprintf(Buf, "%-6s", "Amount");
  ShopText = ConvertStringToCString(Buf);
  pDnodeActor->PlayerOut += ShopText;
  pDnodeActor->PlayerOut += "\r\n";
  // Line two
  sprintf(Buf, "%-45s", " ");
  ShopText = ConvertStringToCString(Buf);
  ShopText.Replace(" ", "-");
  pDnodeActor->PlayerOut += ShopText;
  pDnodeActor->PlayerOut += " ";
  sprintf(Buf, "%-6s", " ");
  ShopText = ConvertStringToCString(Buf);
  ShopText.Replace(" ", "-");
  pDnodeActor->PlayerOut += ShopText;
  pDnodeActor->PlayerOut += "\r\n";
  // List items for trade
  ShopFile.ReadString(Stuff);
  while (Stuff != "End of Items")
  { // Read 'item' lines in ShopFile
    Stuff.TrimLeft();
    Stuff.TrimRight();
    Stuff.MakeLower();
    if (GetWord(Stuff, 1) == "item:")
    { // Found an item
      ObjectId = GetWord(Stuff, 2);
      pObject = Object::IsObject(ObjectId);
      if (pObject)
      { // Format shop item text
        sprintf(Buf, "%-45s", (LPCSTR) pObject->Desc1);
        ShopText = ConvertStringToCString(Buf);
        TmpStr = ShopText;
        i = TmpStr.Remove('&');
        i = i * 2;
        for (j = 1; j <= i; j++)
        { // Color codes will be removed, so adjust length
          ShopText += " ";
        }
        pDnodeActor->PlayerOut += ShopText;
        pDnodeActor->PlayerOut += " ";
        sprintf(Buf, "%6d", pObject->Cost);
        ShopText = ConvertStringToCString(Buf);
        pDnodeActor->PlayerOut += ShopText;
        pDnodeActor->PlayerOut += "\r\n";
        // Done with object
        delete pObject;
      }
      else
      { // Object does not exist, Log it
        LogBuf =  ObjectId;
        LogBuf += " is an invalid shop item - ";
        LogBuf += "Shop::ListObjects";
        LogIt(LogBuf);
      }
    }
    ShopFile.ReadString(Stuff);
  }
  ShopFile.Close();
}