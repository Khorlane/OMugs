/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Object.cpp                                       *
* Usage:  Manages objects                                  *
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

#include "stdafx.h"         // precompiled headers
#include "Object.h"

/***********************************************************
* Globals                                                  *
************************************************************/

/***********************************************************
* Object constructor                                       *
************************************************************/

Object::Object(CString ObjectId)
{
  // Init variables
  ArmorValue        = 0;
  ArmorWear         = "";
  ContainerCapacity = 0;
  Cost              = 0;
  Count             = "1";
  Desc1             = "";
  Desc2             = "";
  Desc3             = "";
  DrinkPct          = 0;
  FoodPct           = 0;
  LightHours        = 0;
  Names             = "";
  Type              = "";
  WeaponType        = "";
  WeaponDamage      = 0;
  WearPosition      = "";
  Weight            = 0;
  // Construct object
  OpenFile(ObjectId);
  ParseStuff();
  CloseFile();
}

/***********************************************************
* Object destructor                                        *
************************************************************/

Object::~Object()
{
}

////////////////////////////////////////////////////////////
// Public functions static                                //
////////////////////////////////////////////////////////////

/***********************************************************
* Add an object to player's equipment                      *
************************************************************/

bool Object::AddObjToPlayerEqu(CString WearPosition, CString ObjectId)
{
  int        BytesInFile;
  bool       NewPlayerEquFile;
  bool       ObjectIdAdded;
  CString    PlayerEquFileName;
  CString    PlayerEquFileNameTmp;
  CStdioFile PlayerEquFile;
  CStdioFile PlayerEquFileTmp;
  CString    Stuff;
  int        Success;
  CString    TmpStr;
  CString    WearPositionCheck;
  bool       WearWieldFailed;

  WearWieldFailed = false;
  ObjectId.MakeLower();
  // Open PlayerEqu file
  PlayerEquFileName =  PLAYER_EQU_DIR;
  PlayerEquFileName += pDnodeActor->PlayerName;
  PlayerEquFileName += ".txt";
  NewPlayerEquFile = false;
  Success = PlayerEquFile.Open(PlayerEquFileName,
                    CFile::modeRead |
                    CFile::typeText);
  if(!Success)
  {
    NewPlayerEquFile = true;
  }
  // Open temp PlayerEqu file
  PlayerEquFileNameTmp =  PLAYER_EQU_DIR;
  PlayerEquFileNameTmp += pDnodeActor->PlayerName;
  PlayerEquFileNameTmp += ".tmp.txt";
  Success = PlayerEquFileTmp.Open(PlayerEquFileNameTmp,
                    CFile::modeCreate |
                    CFile::modeWrite  |
                    CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Object::AddObjToPlayerEqu - Open PlayerEqu temp file failed", MB_ICONSTOP);
    _endthread();
  }
  WearPosition = Utility::TranslateWord(WearPosition);
  if (NewPlayerEquFile)
  { // New player equipment file, write the object and return
    ObjectId = WearPosition + " " + ObjectId;
    ObjectId += "\n";
    PlayerEquFileTmp.WriteString(ObjectId);
    PlayerEquFileTmp.Close();
    CFile::Rename(PlayerEquFileNameTmp, PlayerEquFileName);
    return WearWieldFailed;
  }
  // Write temp PlayerEqu file
  ObjectIdAdded = false;
  PlayerEquFile.ReadString(Stuff);
  while (Stuff != "")
  {
    if (ObjectIdAdded)
    { // New object has been written, just write the rest of the objects
      Stuff += "\n";
      PlayerEquFileTmp.WriteString(Stuff);
      PlayerEquFile.ReadString(Stuff);
      continue;
    }
    WearPositionCheck = Utility::GetWord(Stuff, 1);
    if (WearPosition < WearPositionCheck)
    { // Add new object in alphabetical order by translated WearPosition
      ObjectId = WearPosition + " " + ObjectId;
      ObjectId += "\n";
      PlayerEquFileTmp.WriteString(ObjectId);
      ObjectIdAdded = true;
      Stuff += "\n";
      PlayerEquFileTmp.WriteString(Stuff);
      PlayerEquFile.ReadString(Stuff);
      continue;
    }
    if (WearPosition == WearPositionCheck)
    { // Already wearing an object in that position
      WearWieldFailed = true;
      ObjectIdAdded = true; // Not really added
      Stuff += "\n";
      PlayerEquFileTmp.WriteString(Stuff);
      PlayerEquFile.ReadString(Stuff);
      continue;
    }
    // None of the above conditions satisfied, just write it
    Stuff += "\n";
    PlayerEquFileTmp.WriteString(Stuff);
    PlayerEquFile.ReadString(Stuff);
  }
  if (!ObjectIdAdded)
  { // New object is alphabetically last
    ObjectId = WearPosition + " " + ObjectId;
    ObjectId += "\n";
    PlayerEquFileTmp.WriteString(ObjectId);
    ObjectIdAdded = true;
  }
  BytesInFile = PlayerEquFileNameTmp.GetLength();
  PlayerEquFile.Close();
  PlayerEquFileTmp.Close();
  CFile::Remove(PlayerEquFileName);
  if (BytesInFile > 0)
  { // If the file is not empty, rename it
    CFile::Rename(PlayerEquFileNameTmp, PlayerEquFileName);
  }
  else
  {
    CFile::Remove(PlayerEquFileNameTmp);
  }
  return WearWieldFailed;
}

/***********************************************************
* Add an object to player's inventory                      *
************************************************************/

void Object::AddObjToPlayerInv(Dnode *pDnodeTgt, CString ObjectId)
{
  int        BytesInFile;
  bool       NewPlayerObjFile;
  bool       ObjectIdAdded;
  CString    ObjectIdCheck;
  int        ObjCount;
  CString    PlayerObjFileName;
  CString    PlayerObjFileNameTmp;
  CStdioFile PlayerObjFile;
  CStdioFile PlayerObjFileTmp;
  CString    Stuff;
  int        Success;
  CString    TmpStr;

  ObjectId.MakeLower();
  // Open PlayerObj file
  PlayerObjFileName =  PLAYER_OBJ_DIR;
  PlayerObjFileName += pDnodeTgt->PlayerName;
  PlayerObjFileName += ".txt";
  NewPlayerObjFile = false;
  Success = PlayerObjFile.Open(PlayerObjFileName,
                    CFile::modeRead |
                    CFile::typeText);
  if(!Success)
  {
    NewPlayerObjFile = true;
  }
  // Open temp PlayerObj file
  PlayerObjFileNameTmp =  PLAYER_OBJ_DIR;
  PlayerObjFileNameTmp += pDnodeTgt->PlayerName;
  PlayerObjFileNameTmp += ".tmp.txt";
  Success = PlayerObjFileTmp.Open(PlayerObjFileNameTmp,
                    CFile::modeCreate |
                    CFile::modeWrite  |
                    CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Object::AddObjToPlayerInv - Open PlayerObj temp file failed", MB_ICONSTOP);
    _endthread();
  }
  if (NewPlayerObjFile)
  { // New player inventory file, write the object and return
    ObjectId = "1 " + ObjectId;
    ObjectId += "\n";
    PlayerObjFileTmp.WriteString(ObjectId);
    PlayerObjFileTmp.Close();
    CFile::Rename(PlayerObjFileNameTmp, PlayerObjFileName);
    return;
  }
  // Write temp PlayerObj file
  ObjectIdAdded = false;
  PlayerObjFile.ReadString(Stuff);
  while (Stuff != "")
  {
    if (ObjectIdAdded)
    { // New object has been written, just write the rest of the objects
      Stuff += "\n";
      PlayerObjFileTmp.WriteString(Stuff);
      PlayerObjFile.ReadString(Stuff);
      continue;
    }
    ObjectIdCheck = Utility::GetWord(Stuff, 2);
    if (ObjectId < ObjectIdCheck)
    { // Add new object in alphabetical order
      ObjectId = "1 " + ObjectId;
      ObjectId += "\n";
      PlayerObjFileTmp.WriteString(ObjectId);
      ObjectIdAdded = true;
      Stuff += "\n";
      PlayerObjFileTmp.WriteString(Stuff);
      PlayerObjFile.ReadString(Stuff);
      continue;
    }
    if (ObjectId == ObjectIdCheck)
    { // Existing object same as new object, add 1 to count
      ObjCount = atoi(Utility::GetWord(Stuff, 1));
      ObjCount++;
      TmpStr.Format("%d", ObjCount);
      ObjectId = TmpStr + " " + ObjectId;
      ObjectId += "\n";
      PlayerObjFileTmp.WriteString(ObjectId);
      ObjectIdAdded = true;
      PlayerObjFile.ReadString(Stuff);
      continue;
    }
    // None of the above conditions satisfied, just write it
    Stuff += "\n";
    PlayerObjFileTmp.WriteString(Stuff);
    PlayerObjFile.ReadString(Stuff);
  }
  if (!ObjectIdAdded)
  { // New object is alphabetically last
    ObjectId = "1 " + ObjectId;
    ObjectId += "\n";
    PlayerObjFileTmp.WriteString(ObjectId);
    ObjectIdAdded = true;
  }
  BytesInFile = PlayerObjFileNameTmp.GetLength();
  PlayerObjFile.Close();
  PlayerObjFileTmp.Close();
  CFile::Remove(PlayerObjFileName);
  if (BytesInFile > 0)
  { // If the file is not empty, rename it
    CFile::Rename(PlayerObjFileNameTmp, PlayerObjFileName);
  }
  else
  { // If the file is empty, remove it
    CFile::Remove(PlayerObjFileNameTmp);
  }
}

/***********************************************************
* Add an object to room                                    *
************************************************************/

void Object::AddObjToRoom(CString RoomId, CString ObjectId)
{
  int        BytesInFile;
  bool       NewRoomObjFile;
  bool       ObjectIdAdded;
  CString    ObjectIdCheck;
  int        ObjCount;
  CString    RoomObjFileName;
  CString    RoomObjFileNameTmp;
  CStdioFile RoomObjFile;
  CStdioFile RoomObjFileTmp;
  CString    Stuff;
  int        Success;
  CString    TmpStr;

  ObjectId.MakeLower();
  // Open RoomObj file
  RoomObjFileName =  ROOM_OBJ_DIR;
  RoomObjFileName += RoomId;
  RoomObjFileName += ".txt";
  NewRoomObjFile = false;
  Success = RoomObjFile.Open(RoomObjFileName,
                  CFile::modeRead |
                  CFile::typeText);
  if(!Success)
  {
    NewRoomObjFile = true;
  }
  // Open temp RoomObj file
  RoomObjFileNameTmp =  ROOM_OBJ_DIR;
  RoomObjFileNameTmp += RoomId;
  RoomObjFileNameTmp += ".tmp.txt";
  Success = RoomObjFileTmp.Open(RoomObjFileNameTmp,
                  CFile::modeCreate |
                  CFile::modeWrite  |
                  CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Object::AddObjToRoom - Open RoomObj temp file failed", MB_ICONSTOP);
    _endthread();
  }
  if (NewRoomObjFile)
  { // New room object file, write the object and return
    ObjectId = "1 " + ObjectId;
    ObjectId += "\n";
    RoomObjFileTmp.WriteString(ObjectId);
    RoomObjFileTmp.Close();
    CFile::Rename(RoomObjFileNameTmp, RoomObjFileName);
    return;
  }
  // Write temp RoomObj file
  ObjectIdAdded = false;
  RoomObjFile.ReadString(Stuff);
  while (Stuff != "")
  {
    if (ObjectIdAdded)
    { // New object has been written, just write the rest of the objects
      Stuff += "\n";
      RoomObjFileTmp.WriteString(Stuff);
      RoomObjFile.ReadString(Stuff);
      continue;
    }
    ObjectIdCheck = Utility::GetWord(Stuff, 2);
    if (ObjectId < ObjectIdCheck)
    { // Add new object in alphabetical order
      ObjectId = "1 " + ObjectId;
      ObjectId += "\n";
      RoomObjFileTmp.WriteString(ObjectId);
      ObjectIdAdded = true;
      Stuff += "\n";
      RoomObjFileTmp.WriteString(Stuff);
      RoomObjFile.ReadString(Stuff);
      continue;
    }
    if (ObjectId == ObjectIdCheck)
    { // Existing object same as new object, add 1 to count
      ObjCount = atoi(Utility::GetWord(Stuff, 1));
      ObjCount++;
      TmpStr.Format("%d", ObjCount);
      ObjectId = TmpStr + " " + ObjectId;
      ObjectId += "\n";
      RoomObjFileTmp.WriteString(ObjectId);
      ObjectIdAdded = true;
      RoomObjFile.ReadString(Stuff);
      continue;
    }
    // None of the above conditions satisfied, just write it
    Stuff += "\n";
    RoomObjFileTmp.WriteString(Stuff);
    RoomObjFile.ReadString(Stuff);
  }
  if (!ObjectIdAdded)
  { // New object is alphabetically last
    ObjectId = "1 " + ObjectId;
    ObjectId += "\n";
    RoomObjFileTmp.WriteString(ObjectId);
    ObjectIdAdded = true;
  }
  BytesInFile = RoomObjFileNameTmp.GetLength();
  RoomObjFile.Close();
  RoomObjFileTmp.Close();
  CFile::Remove(RoomObjFileName);
  if (BytesInFile > 0)
  { // If the file is not empty, rename it
    CFile::Rename(RoomObjFileNameTmp, RoomObjFileName);
  }
  else
  { // If the file is empty, delete it
    CFile::Remove(RoomObjFileNameTmp);
  }
}

/***********************************************************
* Calculate player armor class                             *
************************************************************/

int Object::CalcPlayerArmorClass()
{
  Object     *pObject;
  int         ArmorClass;
  CString     ObjectId;
  CStdioFile  PlayerEquFile;
  CString     PlayerEquFileName;
  CString     Stuff;
  int         Success;
  CString     WearPosition;

  ArmorClass = 0;
  // Open PlayerObj file
  PlayerEquFileName =  PLAYER_EQU_DIR;
  PlayerEquFileName += pDnodeActor->PlayerName;
  PlayerEquFileName += ".txt";
  Success = PlayerEquFile.Open(PlayerEquFileName,
                    CFile::modeRead |
                    CFile::typeText);
  if(!Success)
  { // No player equipment
    return ArmorClass;
  }
  PlayerEquFile.ReadString(Stuff);
  while (Stuff != "")
  {
    ObjectId = Utility::GetWord(Stuff, 2);
    pObject = new Object(ObjectId);
    ArmorClass += pObject->ArmorValue;
    delete pObject;
    PlayerEquFile.ReadString(Stuff);
  }
  PlayerEquFile.Close();
  return ArmorClass;
}

/***********************************************************
* Is object in player's equipment?                         *
************************************************************/

Object *Object::IsObjInPlayerEqu(CString ObjectName)
{
  Object     *pObject;
  CString     LogBuf;
  CString     NamesCheck;
  CString     ObjectId;
  CString     ObjectIdCheck;
  CString     ObjectNameCheck;
  CString     PlayerEquFileName;
  CStdioFile  PlayerEquFile;
  CString     Stuff;
  int         Success;

  // Open PlayerEqu file
  PlayerEquFileName =  PLAYER_EQU_DIR;
  PlayerEquFileName += pDnodeActor->PlayerName;
  PlayerEquFileName += ".txt";
  //*******************************
  //* Try matching using ObjectId *
  //*******************************
  Success = PlayerEquFile.Open(PlayerEquFileName,
                    CFile::modeRead |
                    CFile::typeText);
  if(!Success)
  { // Player has no objects
    return NULL;
  }
  PlayerEquFile.ReadString(Stuff);
  while (Stuff != "")
  { // For each player equipment object 
    ObjectId = Utility::GetWord(Stuff, 2);
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
        LogBuf  = ObjectId;
        LogBuf += " is an invalid item found in player equipment - ";
        LogBuf += "Object::IsObjInPlayerEqu";
        Log::LogIt(LogBuf);
        return NULL;
      }
    }
    PlayerEquFile.ReadString(Stuff);
  }
  PlayerEquFile.Close();
  //***************************************************
  //* No match found, try getting match using 'names' *
  //***************************************************
  Success = PlayerEquFile.Open(PlayerEquFileName,
                    CFile::modeRead |
                    CFile::typeText);
  if(!Success)
  { // Player has no objects
    return NULL;
  }
  PlayerEquFile.ReadString(Stuff);
  while (Stuff != "")
  { // For each player equipment object 
    ObjectId = Utility::GetWord(Stuff, 2);
    pObject = new Object(ObjectId);
    if (!pObject)
    { // Object does not exist, Log it
      LogBuf  = ObjectId;
      LogBuf += " is an invalid item found in player equipment - ";
      LogBuf += "Object::IsObjInPlayerEqu";
      Log::LogIt(LogBuf);
      return NULL;
    }
    NamesCheck = pObject->Names;
    NamesCheck.MakeLower();
    if (Utility::IsWord(ObjectName, NamesCheck))
    { // Match
      return pObject;
    }
    else
    { // No match
      delete pObject;
    }
    PlayerEquFile.ReadString(Stuff);
  }
  PlayerEquFile.Close();
  // Object not found in player's inventory
  return NULL;
}

/***********************************************************
* Is object in player's inventory?                         *
************************************************************/

Object *Object::IsObjInPlayerInv(CString ObjectName)
{
  Object     *pObject;
  CString     LogBuf;
  CString     NamesCheck;
  CString     ObjectId;
  CString     ObjectIdCheck;
  CString     ObjectNameCheck;
  CString     PlayerObjFileName;
  CStdioFile  PlayerObjFile;
  CString     Stuff;
  int         Success;

  // Open PlayerObj file
  PlayerObjFileName =  PLAYER_OBJ_DIR;
  PlayerObjFileName += pDnodeActor->PlayerName;
  PlayerObjFileName += ".txt";
  //*******************************
  //* Try matching using ObjectId *
  //*******************************
  Success = PlayerObjFile.Open(PlayerObjFileName,
                    CFile::modeRead |
                    CFile::typeText);
  if(!Success)
  { // Player has no objects
    return NULL;
  }
  PlayerObjFile.ReadString(Stuff);
  while (Stuff != "")
  { // For all items in player inventory
    ObjectId = Utility::GetWord(Stuff, 2);
    ObjectName.MakeLower();
    if (ObjectName == ObjectId)
    { // Found a match
      pObject = new Object(ObjectId);
      if (pObject)
      { // Object exists
        pObject->Count = Utility::GetWord(Stuff, 1);
        return pObject;
      }
      else
      { // Object does not exist, Log it
        LogBuf  = ObjectId;
        LogBuf += " is an invalid item found in player inventory - ";
        LogBuf += "Object::IsObjInPlayerInv";
        Log::LogIt(LogBuf);
        return NULL;
      }
    }
    PlayerObjFile.ReadString(Stuff);
  }
  PlayerObjFile.Close();
  //***************************************************
  //* No match found, try getting match using 'names' *
  //***************************************************
  Success = PlayerObjFile.Open(PlayerObjFileName,
                    CFile::modeRead |
                    CFile::typeText);
  if(!Success)
  { // Player has no objects
    return NULL;
  }
  PlayerObjFile.ReadString(Stuff);
  while (Stuff != "")
  { // For all items in player inventory
    ObjectId = Utility::GetWord(Stuff, 2);
    pObject  = new Object(ObjectId);
    if (!pObject)
    { // Object does not exist, Log it
      LogBuf  = ObjectId;
      LogBuf += " is an invalid item found in player inventory - ";
      LogBuf += "Object::IsObjInPlayerInv";
      Log::LogIt(LogBuf);
      return NULL;
    }
    pObject->Count = Utility::GetWord(Stuff, 1);
    NamesCheck     = pObject->Names;
    NamesCheck.MakeLower();
    if (Utility::IsWord(ObjectName, NamesCheck))
    { // Match
      return pObject;
    }
    else
    { // No match
      delete pObject;
    }
    PlayerObjFile.ReadString(Stuff);
  }
  PlayerObjFile.Close();
  // Object not found in player's inventory
  return NULL;
}

/***********************************************************
* Is object in room                                        *
************************************************************/

Object *Object::IsObjInRoom(CString ObjectName)
{
  Object     *pObject;
  CString     LogBuf;
  CString     NamesCheck;
  CString     ObjectId;
  CString     ObjectIdCheck;
  CString     ObjectNameCheck;
  CString     RoomObjFileName;
  CStdioFile  RoomObjFile;
  CString     Stuff;
  int         Success;

  // Open RoomObj file
  RoomObjFileName =  ROOM_OBJ_DIR;
  RoomObjFileName += pDnodeActor->pPlayer->RoomId;
  RoomObjFileName += ".txt";
  //*******************************
  //* Try matching using ObjectId *
  //*******************************
  Success = RoomObjFile.Open(RoomObjFileName,
                  CFile::modeRead |
                  CFile::typeText);
  if(!Success)
  { // Room has no objects
    return NULL;
  }
  RoomObjFile.ReadString(Stuff);
  while (Stuff != "")
  { // For each item in room
    ObjectId = Utility::GetWord(Stuff, 2);
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
        LogBuf  = ObjectId;
        LogBuf += " is an invalid item found in room - ";
        LogBuf += "Object::IsObjInRoom";
        Log::LogIt(LogBuf);
        return NULL;
      }
    }
    RoomObjFile.ReadString(Stuff);
  }
  RoomObjFile.Close();
  //***************************************************
  //* No match found, try getting match using 'names' *
  //***************************************************
  Success = RoomObjFile.Open(RoomObjFileName,
                  CFile::modeRead |
                  CFile::typeText);
  if(!Success)
  { // Room has no objects
    return NULL;
  }
  RoomObjFile.ReadString(Stuff);
  while (Stuff != "")
  { // For each item in room
    ObjectId = Utility::GetWord(Stuff, 2);
    pObject = new Object(ObjectId);
    if (!pObject)
    { // Object does not exist, Log it
      LogBuf  = ObjectId;
      LogBuf += " is an invalid item found in room - ";
      LogBuf += "Object::IsObjInRoom";
      Log::LogIt(LogBuf);
      return NULL;
    }
    NamesCheck = pObject->Names;
    NamesCheck.MakeLower();
    if (Utility::IsWord(ObjectName, NamesCheck))
    { // Match
      return pObject;
    }
    else
    { // No match
      delete pObject;
    }
    RoomObjFile.ReadString(Stuff);
  }
  RoomObjFile.Close();
  // Object not found in room
  return NULL;
}

/***********************************************************
* Is this a valid object?                                  *
************************************************************/

Object *Object::IsObject(CString ObjectId)
{
  Object     *pObject;
  CString     ObjectFileName;
  CStdioFile  ObjectFile;
  int         Success;

  ObjectFileName =  OBJECTS_DIR;
  ObjectFileName += ObjectId;
  ObjectFileName += ".txt";
  Success = ObjectFile.Open(ObjectFileName,
                 CFile::modeRead |
                 CFile::typeText);
  if(Success)
  {
    ObjectFile.Close();
    pObject = new Object(ObjectId);
    return pObject;
  }
  else
  {
    return NULL;
  }
}

/***********************************************************
* Remove an object from player's equipment                 *
************************************************************/

void Object::RemoveObjFromPlayerEqu(CString ObjectId)
{
  int        BytesInFile;
  bool       ObjectIdRemoved;
  CString    ObjectIdCheck;
  CString    PlayerEquFileName;
  CString    PlayerEquFileNameTmp;
  CStdioFile PlayerEquFile;
  CStdioFile PlayerEquFileTmp;
  CString    Stuff;
  int        Success;
  CString    TmpStr;

  ObjectId.MakeLower();
  // Open PlayerEqu file
  PlayerEquFileName =  PLAYER_EQU_DIR;
  PlayerEquFileName += pDnodeActor->PlayerName;
  PlayerEquFileName += ".txt";
  Success = PlayerEquFile.Open(PlayerEquFileName,
                    CFile::modeRead |
                    CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Object::RemoveObjFromPlayerEqu - Open PlayerEqu file failed", MB_ICONSTOP);
    _endthread();
  }
  // Open temp PlayerEqu file
  PlayerEquFileNameTmp =  PLAYER_EQU_DIR;
  PlayerEquFileNameTmp += pDnodeActor->PlayerName;
  PlayerEquFileNameTmp += ".tmp.txt";
  Success = PlayerEquFileTmp.Open(PlayerEquFileNameTmp,
                    CFile::modeCreate |
                    CFile::modeWrite  |
                    CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Object::RemoveObjFromPlayerEqu - Open PlayerEqu temp file failed", MB_ICONSTOP);
    _endthread();
  }
  // Write temp PlayerEqu file
  ObjectIdRemoved = false;
  PlayerEquFile.ReadString(Stuff);
  while (Stuff != "")
  {
    if (ObjectIdRemoved)
    { // Object has been removed, just write the rest of the objects
      Stuff += "\n";
      PlayerEquFileTmp.WriteString(Stuff);
      PlayerEquFile.ReadString(Stuff);
      continue;
    }
    ObjectIdCheck = Utility::GetWord(Stuff, 2);
    if (ObjectId == ObjectIdCheck)
    { // Found it, skipping it will remove it from the file
      ObjectIdRemoved = true;
      PlayerEquFile.ReadString(Stuff);
      continue;
    }
    // None of the above conditions satisfied, just write it
    Stuff += "\n";
    PlayerEquFileTmp.WriteString(Stuff);
    PlayerEquFile.ReadString(Stuff);
  }
  if (!ObjectIdRemoved)
  { // Object not removed, this is definitely BAD!
    AfxMessageBox("Object::RemoveObjFromPlayerEqu - Object not removed", MB_ICONSTOP);
    _endthread();
  }
  BytesInFile = PlayerEquFileNameTmp.GetLength();
  PlayerEquFile.Close();
  PlayerEquFileTmp.Close();
  CFile::Remove(PlayerEquFileName);
  if (BytesInFile > 0)
  { // If the file is not empty, rename it
    CFile::Rename(PlayerEquFileNameTmp, PlayerEquFileName);
  }
  else
  { // If the file is empty, delete it
    CFile::Remove(PlayerEquFileNameTmp);
  }
}

/***********************************************************
* Remove an object from player's inventory                 *
************************************************************/

void Object::RemoveObjFromPlayerInv(CString ObjectId, int     Count)
{
  int        BytesInFile;
  bool       ObjectIdRemoved;
  CString    ObjectIdCheck;
  int        ObjCount;
  CString    PlayerObjFileName;
  CString    PlayerObjFileNameTmp;
  CStdioFile PlayerObjFile;
  CStdioFile PlayerObjFileTmp;
  CString    Stuff;
  int        Success;
  CString    TmpStr;

  ObjectId.MakeLower();
  // Open PlayerObj file
  PlayerObjFileName =  PLAYER_OBJ_DIR;
  PlayerObjFileName += pDnodeActor->PlayerName;
  PlayerObjFileName += ".txt";
  Success = PlayerObjFile.Open(PlayerObjFileName,
                    CFile::modeRead |
                    CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Object::RemoveObjFromPlayerInv - Open PlayerObj file failed", MB_ICONSTOP);
    _endthread();
  }
  // Open temp PlayerObj file
  PlayerObjFileNameTmp =  PLAYER_OBJ_DIR;
  PlayerObjFileNameTmp += pDnodeActor->PlayerName;
  PlayerObjFileNameTmp += ".tmp.txt";
  Success = PlayerObjFileTmp.Open(PlayerObjFileNameTmp,
                    CFile::modeCreate |
                    CFile::modeWrite  |
                    CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Object::RemoveObjFromPlayerInv - Open PlayerObj temp file failed", MB_ICONSTOP);
    _endthread();
  }
  // Write temp PlayerObj file
  ObjectIdRemoved = false;
  PlayerObjFile.ReadString(Stuff);
  while (Stuff != "")
  {
    if (ObjectIdRemoved)
    { // Object has been removed, just write the rest of the objects
      Stuff += "\n";
      PlayerObjFileTmp.WriteString(Stuff);
      PlayerObjFile.ReadString(Stuff);
      continue;
    }
    ObjectIdCheck = Utility::GetWord(Stuff, 2);
    if (ObjectId == ObjectIdCheck)
    { // Found it, subtract 'count' from ObjCount
      ObjCount = atoi(Utility::GetWord(Stuff, 1));
      ObjCount -= Count;
      ObjectIdRemoved = true;
      if (ObjCount > 0)
      {
        TmpStr.Format("%d", ObjCount);
        ObjectId = TmpStr + " " + ObjectId;
        ObjectId += "\n";
        PlayerObjFileTmp.WriteString(ObjectId);
      }
      PlayerObjFile.ReadString(Stuff);
      continue;
    }
    // None of the above conditions satisfied, just write it
    Stuff += "\n";
    PlayerObjFileTmp.WriteString(Stuff);
    PlayerObjFile.ReadString(Stuff);
  }
  if (!ObjectIdRemoved)
  { // Object not removed, this is definitely BAD!
    AfxMessageBox("Object::RemoveObjFromPlayerInv - Object not removed", MB_ICONSTOP);
    _endthread();
  }
  BytesInFile = PlayerObjFileNameTmp.GetLength();
  PlayerObjFile.Close();
  PlayerObjFileTmp.Close();
  CFile::Remove(PlayerObjFileName);
  if (BytesInFile > 0)
  { // If the file is not empty, rename it
    CFile::Rename(PlayerObjFileNameTmp, PlayerObjFileName);
  }
  else
  { // If the file is empty, delete it
    CFile::Remove(PlayerObjFileNameTmp);
  }
}

/***********************************************************
* Remove an object from room                               *
************************************************************/

void Object::RemoveObjFromRoom(CString ObjectId)
{
  int        BytesInFile;
  bool       ObjectIdRemoved;
  CString    ObjectIdCheck;
  int        ObjCount;
  CString    RoomObjFileName;
  CString    RoomObjFileNameTmp;
  CStdioFile RoomObjFile;
  CStdioFile RoomObjFileTmp;
  CString    Stuff;
  int        Success;
  CString    TmpStr;

  ObjectId.MakeLower();
  // Open RoomObj file
  RoomObjFileName =  ROOM_OBJ_DIR;
  RoomObjFileName += pDnodeActor->pPlayer->RoomId;
  RoomObjFileName += ".txt";
  Success = RoomObjFile.Open(RoomObjFileName,
                  CFile::modeRead |
                  CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Object::RemoveObjFromRoom - Open RoomObj file failed", MB_ICONSTOP);
    _endthread();
  }
  // Open temp RoomObj file
  RoomObjFileNameTmp =  ROOM_OBJ_DIR;
  RoomObjFileNameTmp += pDnodeActor->pPlayer->RoomId;
  RoomObjFileNameTmp += ".tmp.txt";
  Success = RoomObjFileTmp.Open(RoomObjFileNameTmp,
                  CFile::modeCreate |
                  CFile::modeWrite  |
                  CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Object::RemoveObjFromRoom - Open RoomObj temp file failed", MB_ICONSTOP);
    _endthread();
  }
  // Write temp RoomObj file
  ObjectIdRemoved = false;
  RoomObjFile.ReadString(Stuff);
  while (Stuff != "")
  {
    if (ObjectIdRemoved)
    { // Object has been removed, just write the rest of the objects
      Stuff += "\n";
      RoomObjFileTmp.WriteString(Stuff);
      RoomObjFile.ReadString(Stuff);
      continue;
    }
    ObjectIdCheck = Utility::GetWord(Stuff, 2);
    if (ObjectId == ObjectIdCheck)
    { // Found it, subtract 1 from count
      ObjCount = atoi(Utility::GetWord(Stuff, 1));
      ObjCount--;
      ObjectIdRemoved = true;
      if (ObjCount > 0)
      {
        TmpStr.Format("%d", ObjCount);
        ObjectId = TmpStr + " " + ObjectId;
        ObjectId += "\n";
        RoomObjFileTmp.WriteString(ObjectId);
      }
      RoomObjFile.ReadString(Stuff);
      continue;
    }
    // None of the above conditions satisfied, just write it
    Stuff += "\n";
    RoomObjFileTmp.WriteString(Stuff);
    RoomObjFile.ReadString(Stuff);
  }
  if (!ObjectIdRemoved)
  { // Object not removed, this is definitely BAD!
    AfxMessageBox("Object::RemoveObjFromRoom - Object not removed", MB_ICONSTOP);
    _endthread();
  }
  BytesInFile = (int) RoomObjFileTmp.GetLength();
  RoomObjFile.Close();
  RoomObjFileTmp.Close();
  CFile::Remove(RoomObjFileName);
  if (BytesInFile > 0)
  { // If the file is not empty, rename it
    CFile::Rename(RoomObjFileNameTmp, RoomObjFileName);
  }
  else
  { // If the file is empty, delete it
    CFile::Remove(RoomObjFileNameTmp);
  }
}

/***********************************************************
* Show player equipment                                    *
************************************************************/

void Object::ShowPlayerEqu(Dnode *pDnodeTgt)
{
  Object     *pObject;
  CString     ObjectId;
  CStdioFile  PlayerEquFile;
  CString     PlayerEquFileName;
  CString     Stuff;
  int         Success;
  CString     WearPosition;

  // Open PlayerEqu file
  PlayerEquFileName =  PLAYER_EQU_DIR;
  PlayerEquFileName += pDnodeTgt->PlayerName;
  PlayerEquFileName += ".txt";
  Success = PlayerEquFile.Open(PlayerEquFileName,
                    CFile::modeRead |
                    CFile::typeText);
  if(!Success)
  { // No player equipment
    if (pDnodeActor == pDnodeTgt)
    { // Player is checking their own equipment
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->PlayerOut += "You have no equipment!\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeTgt->pPlayer->GetOutput();
      return;
    }
    else
    { // A player is looking at another player
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->PlayerOut += pDnodeTgt->PlayerName;
      pDnodeActor->PlayerOut += " has no equipment!";
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
  }
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "Equipment\r\n";
  pDnodeActor->PlayerOut += "---------\r\n";
  PlayerEquFile.ReadString(Stuff);
  while (Stuff != "")
  {
    WearPosition = Utility::GetWord(Stuff, 1);
    WearPosition = Utility::TranslateWord(WearPosition);
    ObjectId = Utility::GetWord(Stuff, 2);
    pObject = new Object(ObjectId);
    pDnodeActor->PlayerOut += WearPosition;
    pDnodeActor->PlayerOut += pObject->Desc1;
    pDnodeActor->PlayerOut += "\r\n";
    delete pObject;
    PlayerEquFile.ReadString(Stuff);
  }
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
  PlayerEquFile.Close();
}

/***********************************************************
* Show player inventory                                    *
************************************************************/

void Object::ShowPlayerInv()
{
  Object     *pObject;
  CString     ObjectCount;
  CString     ObjectId;
  CStdioFile  PlayerObjFile;
  CString     PlayerObjFileName;
  CString     Stuff;
  int         Success;

  // Open PlayerObj file
  PlayerObjFileName =  PLAYER_OBJ_DIR;
  PlayerObjFileName += pDnodeActor->PlayerName;
  PlayerObjFileName += ".txt";
  Success = PlayerObjFile.Open(PlayerObjFileName,
                    CFile::modeRead |
                    CFile::typeText);
  if(!Success)
  {
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "It is sad, but you have nothing in your inventory.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "Inventory\r\n";
  pDnodeActor->PlayerOut += "---------\r\n";
  PlayerObjFile.ReadString(Stuff);
  while (Stuff != "")
  {
    ObjectCount = Utility::GetWord(Stuff, 1);
    ObjectId = Utility::GetWord(Stuff, 2);
    pObject = new Object(ObjectId);
    pDnodeActor->PlayerOut += "(" + ObjectCount + ") ";
    pDnodeActor->PlayerOut += pObject->Desc1;
    pDnodeActor->PlayerOut += "\r\n";
    delete pObject;
    PlayerObjFile.ReadString(Stuff);
  }
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
  PlayerObjFile.Close();
}

/***********************************************************
* Show objects in room                                     *
************************************************************/

void Object::ShowObjsInRoom(Dnode *pDnode)
{
  Object     *pObject;
  CString     ObjectCount;
  CString     ObjectId;
  CStdioFile  RoomObjFile;
  CString     RoomObjFileName;
  CString     Stuff;
  int         Success;

  // Open RoomObj file
  RoomObjFileName =  ROOM_OBJ_DIR;
  RoomObjFileName += pDnode->pPlayer->RoomId;
  RoomObjFileName += ".txt";
  Success = RoomObjFile.Open(RoomObjFileName,
                  CFile::modeRead |
                  CFile::typeText);
  if(!Success)
  { // No objects in room to display
    return;
  }
  RoomObjFile.ReadString(Stuff);
  while (Stuff != "")
  { // For each object in the room
    ObjectCount = Utility::GetWord(Stuff, 1);
    ObjectId    = Utility::GetWord(Stuff, 2);
    pObject     = new Object(ObjectId);
    pObject->Type.MakeLower();
    pDnode->PlayerOut += "\r\n";
    if (pObject->Type != "notake")
    { // Should be only 1 NoTake type object in a room, like signs or statues
      pDnode->PlayerOut += "(" + ObjectCount + ") ";
    }
    pDnode->PlayerOut += pObject->Desc2;
    delete pObject;
    RoomObjFile.ReadString(Stuff);
  }
  RoomObjFile.Close();
}

/***********************************************************
* Find an object where ever it is                          *
************************************************************/

void Object::WhereObj(CString ObjectIdSearch)
{
  WhereObjPlayerEqu(ObjectIdSearch);
  WhereObjPlayerObj(ObjectIdSearch);
  WhereObjRoomObj(ObjectIdSearch);
}

/***********************************************************
* Where is object in PlayerEqu                               *
************************************************************/

void Object::WhereObjPlayerEqu(CString ObjectIdSearch)
{
  CFileFind   FileList;
  CString     FileName;
  CString     ObjectId;
  BOOL        MoreFiles;
  CString     PlayerEquFileName;
  CStdioFile  PlayerEquFile;
  CString     PlayerName;
  CString     Stuff;
  int         Success;

  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "Objects in player equipment";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "---------------------------";
  pDnodeActor->PlayerOut += "\r\n";
  if (ChgDir(PLAYER_EQU_DIR))
  { // Change directory failed
    AfxMessageBox("Object::WhereObjPlayerEqu - Change directory to PLAYER_EQU_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  MoreFiles = FileList.FindFile("*.*");
  while (MoreFiles)
  {
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    {
      continue;
    }
    FileName = FileList.GetFileName();
    // Open PlayerEqu file
    PlayerEquFileName = FileName;
    Success = PlayerEquFile.Open(PlayerEquFileName,
                    CFile::modeRead |
                    CFile::typeText);
    if(!Success)
    { // File does not exist - Very bad!
      AfxMessageBox("Object::WhereObjPlayerEqu - Open PlayerEqu file failed", MB_ICONSTOP);
      _endthread();
    }
    PlayerName = FileName.Left(FileName.GetLength()-4);
    PlayerEquFile.ReadString(Stuff);
    while (Stuff != "")
    {
      ObjectId      = Utility::GetWord(Stuff, 2);
      if (ObjectId == ObjectIdSearch)
      {
        pDnodeActor->PlayerOut += PlayerName;
        pDnodeActor->PlayerOut += " ";
        pDnodeActor->PlayerOut += Stuff;
        pDnodeActor->PlayerOut += "&N";
        pDnodeActor->PlayerOut += "\r\n";
      }
      PlayerEquFile.ReadString(Stuff);
    }
    PlayerEquFile.Close();
  }
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("Object::WhereObjPlayerEqu - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
}

/***********************************************************
* Where is object in PlayerObj                               *
************************************************************/

void Object::WhereObjPlayerObj(CString ObjectIdSearch)
{
  CFileFind  FileList;
  CString    FileName;
  CString    ObjectId;
  BOOL       MoreFiles;
  CString    PlayerObjFileName;
  CStdioFile PlayerObjFile;
  CString    PlayerName;
  CString    Stuff;
  int        Success;

  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "Objects in player inventory";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "---------------------------";
  pDnodeActor->PlayerOut += "\r\n";
  if (ChgDir(PLAYER_OBJ_DIR))
  { // Change directory failed
    AfxMessageBox("Object::WhereObjPlayerObj - Change directory to PLAYER_OBJ_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  MoreFiles = FileList.FindFile("*.*");
  while (MoreFiles)
  {
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    {
      continue;
    }
    FileName = FileList.GetFileName();
    // Open PlayerObj file
    PlayerObjFileName = FileName;
    Success = PlayerObjFile.Open(PlayerObjFileName,
                    CFile::modeRead |
                    CFile::typeText);
    if(!Success)
    { // File does not exist - Very bad!
      AfxMessageBox("Object::WhereObjPlayerObj - Open PlayerObj file failed", MB_ICONSTOP);
      _endthread();
    }
    PlayerName = FileName.Left(FileName.GetLength()-4);
    PlayerObjFile.ReadString(Stuff);
    while (Stuff != "")
    {
      ObjectId      = Utility::GetWord(Stuff, 2);
      if (ObjectId == ObjectIdSearch)
      {
        pDnodeActor->PlayerOut += PlayerName;
        pDnodeActor->PlayerOut += " ";
        pDnodeActor->PlayerOut += Stuff;
        pDnodeActor->PlayerOut += "&N";
        pDnodeActor->PlayerOut += "\r\n";
      }
      PlayerObjFile.ReadString(Stuff);
    }
    PlayerObjFile.Close();
  }
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("Object::WhereObjPlayerObj - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
}

/***********************************************************
* Where is object in RoomObj                               *
************************************************************/

void Object::WhereObjRoomObj(CString ObjectIdSearch)
{

  CFileFind  FileList;
  CString    FileName;
  CString    ObjectId;
  BOOL       MoreFiles;
  CString    RoomName;
  CString    RoomObjFileName;
  CStdioFile RoomObjFile;
  CString    Stuff;
  int        Success;

  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "Objects in rooms";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "----------------";
  pDnodeActor->PlayerOut += "\r\n";
  if (ChgDir(ROOM_OBJ_DIR))
  { // Change directory failed
    AfxMessageBox("Object::WhereObjRoomObj - Change directory to ROOM_OBJ_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  MoreFiles = FileList.FindFile("*.*");
  while (MoreFiles)
  {
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    {
      continue;
    }
    FileName = FileList.GetFileName();
    // Open RoomObj file
    RoomObjFileName = FileName;
    Success = RoomObjFile.Open(RoomObjFileName,
                    CFile::modeRead |
                    CFile::typeText);
    if(!Success)
    { // File does not exist - Very bad!
      AfxMessageBox("Object::WhereObj - Open RoomObj file failed", MB_ICONSTOP);
      _endthread();
    }
    RoomName = FileName.Left(FileName.GetLength()-4);
    RoomObjFile.ReadString(Stuff);
    while (Stuff != "")
    { // For each room object
      ObjectId = Utility::GetWord(Stuff, 2);
      if (ObjectId == ObjectIdSearch)
      { // Match
        pDnodeActor->PlayerOut += RoomName;
        pDnodeActor->PlayerOut += " ";
        pDnodeActor->PlayerOut += Stuff;
        pDnodeActor->PlayerOut += "&N";
        pDnodeActor->PlayerOut += "\r\n";
      }
      RoomObjFile.ReadString(Stuff);
    }
    RoomObjFile.Close();
  }
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("Object::WhereObj - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
}

////////////////////////////////////////////////////////////
// Public functions                                       //
////////////////////////////////////////////////////////////

/***********************************************************
* Examine object                                           *
************************************************************/

void Object::ExamineObj(CString ObjectId)
{
  OpenFile(ObjectId);
  while (Stuff != "Desc3:")
  {
    ObjectFile.ReadString(Stuff); // Do not use ReadLine() here
  }
  // Object Description 3
  ObjectFile.ReadString(Stuff); // Do not use ReadLine() here
  while (Stuff != "End Desc3")
  {
    pDnodeActor->PlayerOut += Stuff;
    pDnodeActor->PlayerOut += "\r\n";
    ObjectFile.ReadString(Stuff); // Do not use ReadLine() here
  }
  pDnodeActor->PlayerOut += "&N";
  CloseFile();
}

////////////////////////////////////////////////////////////
// Private functions                                      //
////////////////////////////////////////////////////////////

/***********************************************************
* Close Object file                                        *
************************************************************/

void Object::CloseFile()
{
  ObjectFile.Close();
}

/***********************************************************
* Open Object file                                         *
************************************************************/

void Object::OpenFile(CString ObjectId)
{
  CString ObjectFileName;
  int     Success;

  ObjectFileName =  OBJECTS_DIR;
  ObjectFileName += ObjectId;
  ObjectFileName += ".txt";
  Success = ObjectFile.Open(ObjectFileName,
                 CFile::modeRead |
                 CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Object::OpenFile - Object does not exist!", MB_ICONSTOP);
    _endthread();
  }
}

/***********************************************************
* Parse object stuff                                       *
************************************************************/

void Object::ParseStuff()
{
  ReadLine();
  while (Stuff != "")
  {
    if (Stuff.Left(9) == "ObjectId:")
    {
      ObjectId = Stuff.Right(Stuff.GetLength()-9);
      ObjectId.TrimLeft();
    }
    else
    if (Stuff.Left(6) == "Names:")
    {
      Names = Stuff.Right(Stuff.GetLength()-6);
      Names.TrimLeft();
    }
    else
    if (Stuff.Left(6) == "Desc1:")
    {
      Desc1 = Stuff.Right(Stuff.GetLength()-6);
      Desc1.TrimLeft();
    }
    else
    if (Stuff.Left(6) == "Desc2:")
    {
      Desc2 = Stuff.Right(Stuff.GetLength()-6);
      Desc2.TrimLeft();
    }
    else
    if (Stuff.Left(6) == "Desc3:")
    {
    // Desc3 can be multi-line and is dealt with in 'ExamineObj'
    }
    else
    if (Stuff.Left(7) == "Weight:")
    {
      Weight = atoi(Stuff.Right(Stuff.GetLength()-7));
    }
    else
    if (Stuff.Left(5) == "Cost:")
    {
      Cost = atoi(Stuff.Right(Stuff.GetLength()-5));
    }
    else
    if (Stuff.Left(5) == "Type:")
    {
      Type = Stuff.Right(Stuff.GetLength()-5);
      Type.TrimLeft();
    }
    else
    if (Stuff.Left(11) == "ArmorValue:")
    {
      ArmorValue = atoi(Stuff.Right(Stuff.GetLength()-11));
    }
    else
    if (Stuff.Left(10) == "ArmorWear:")
    {
      ArmorWear = Stuff.Right(Stuff.GetLength()-10);
      ArmorWear.TrimLeft();
      WearPosition = ArmorWear;
      WearPosition.MakeLower();
    }
    else
    if (Stuff.Left(11) == "WeaponType:")
    {
      WeaponType = Stuff.Right(Stuff.GetLength()-11);
      WeaponType.TrimLeft();
      ArmorWear = "wielded";
      WearPosition = "wielded";
    }
    else
    if (Stuff.Left(13) == "WeaponDamage:")
    {
      WeaponDamage = atoi(Stuff.Right(Stuff.GetLength()-13));
    }
    else
    if (Stuff.Left(8) == "FoodPct:")
    {
      FoodPct = atoi(Stuff.Right(Stuff.GetLength()-8));
    }
    else
    if (Stuff.Left(9) == "DrinkPct:")
    {
      DrinkPct = atoi(Stuff.Right(Stuff.GetLength()-9));
    }
    ReadLine();
  }
}

/***********************************************************
* Read a line from Object file                             *
************************************************************/

void Object::ReadLine()
{
  ObjectFile.ReadString(Stuff);
  Stuff.TrimLeft();
  Stuff.TrimRight();
}