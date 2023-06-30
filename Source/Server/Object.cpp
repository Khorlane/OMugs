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

#include "stdafx.h" // This is only here to speed up compiles
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
  ObjectId = StrMakeLower(ObjectId);
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
  WearPosition = TranslateWord(WearPosition);
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
    WearPositionCheck = StrGetWord(Stuff, 1);
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
  BytesInFile = StrGetLength(PlayerEquFileNameTmp); // TODO - steve - What is this doing?
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

void Object::AddObjToPlayerInv(Dnode *pDnodeTgt1, string ObjectId)
{
  bool       NewPlayerObjFile;
  bool       ObjectIdAdded;
  string     ObjectIdCheck;
  int        ObjCount;
  string     PlayerObjFileName;
  string     PlayerObjFileNameTmp;
  ifstream   PlayerObjFile;
  ofstream   PlayerObjFileTmp;
  string     Stuff;
  string     TmpStr;

  pDnodeTgt = pDnodeTgt1;
  ObjectId = StrMakeLower(ObjectId);
  // Open PlayerObj file
  PlayerObjFileName =  PLAYER_OBJ_DIR;
  PlayerObjFileName += ConvertCStringToString(pDnodeTgt->PlayerName);
  PlayerObjFileName += ".txt";
  NewPlayerObjFile = false;
  PlayerObjFile.open(PlayerObjFileName);
  if(!PlayerObjFile.is_open())
  {
    NewPlayerObjFile = true;
  }
  // Open temp PlayerObj file
  PlayerObjFileNameTmp =  PLAYER_OBJ_DIR;
  PlayerObjFileNameTmp += ConvertCStringToString(pDnodeTgt->PlayerName);
  PlayerObjFileNameTmp += ".tmp.txt";
  PlayerObjFileTmp.open(PlayerObjFileNameTmp);
  if(!PlayerObjFileTmp.is_open())
  {
    AfxMessageBox("Object::AddObjToPlayerInv - Open PlayerObj temp file failed", MB_ICONSTOP);
    _endthread();
  }
  if (NewPlayerObjFile)
  { // New player inventory file, write the object and return
    ObjectId = "1 " + ObjectId;
    PlayerObjFileTmp << ObjectId << endl;
    PlayerObjFileTmp.close();
    CFile::Rename(ConvertStringToCString(PlayerObjFileNameTmp), ConvertStringToCString(PlayerObjFileName));
    return;
  }
  // Write temp PlayerObj file
  ObjectIdAdded = false;
  getline(PlayerObjFile, Stuff);
  while (Stuff != "")
  {
    if (ObjectIdAdded)
    { // New object has been written, just write the rest of the objects
      PlayerObjFileTmp << Stuff << endl;
      getline(PlayerObjFile, Stuff);
      continue;
    }
    ObjectIdCheck = StrGetWord(Stuff, 2);
    if (ObjectId < ObjectIdCheck)
    { // Add new object in alphabetical order
      ObjectId = "1 " + ObjectId;
      PlayerObjFileTmp << ObjectId << endl;
      ObjectIdAdded = true;
      PlayerObjFileTmp << Stuff << endl;
      getline(PlayerObjFile, Stuff);
      continue;
    }
    if (ObjectId == ObjectIdCheck)
    { // Existing object same as new object, add 1 to count
      ObjCount = stoi(StrGetWord(Stuff, 1));
      ObjCount++;
      sprintf(Buf, "%d", ObjCount);
      TmpStr = (string)Buf;
      ObjectId = TmpStr + " " + ObjectId;
      PlayerObjFileTmp << ObjectId << endl;
      ObjectIdAdded = true;
      getline(PlayerObjFile, Stuff);
      continue;
    }
    // None of the above conditions satisfied, just write it
    PlayerObjFileTmp << Stuff << endl;
    getline(PlayerObjFile, Stuff);
  }
  if (!ObjectIdAdded)
  { // New object is alphabetically last
    ObjectId = "1 " + ObjectId;
    PlayerObjFileTmp << ObjectId << endl;
    ObjectIdAdded = true;
  }
  PlayerObjFile.close();
  PlayerObjFileTmp.close();
  CFile::Remove(ConvertStringToCString(PlayerObjFileName));
  CFile::Rename(ConvertStringToCString(PlayerObjFileNameTmp), ConvertStringToCString(PlayerObjFileName));
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

  ObjectId = StrMakeLower(ObjectId);
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
    ObjectIdCheck = StrGetWord(Stuff, 2);
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
      ObjCount = atoi(StrGetWord(Stuff, 1));
      ObjCount++;
      sprintf(Buf, "%d", ObjCount);
      TmpStr = ConvertStringToCString(Buf);
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
  BytesInFile = StrGetLength(RoomObjFileNameTmp);
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
    ObjectId = StrGetWord(Stuff, 2);
    pObject = new Object(ObjectId);
    ArmorClass += pObject->ArmorValue;
    delete pObject;
    pObject = NULL;
    PlayerEquFile.ReadString(Stuff);
  }
  PlayerEquFile.Close();
  return ArmorClass;
}

/***********************************************************
* Is object in player's equipment?                         *
************************************************************/

void Object::IsObjInPlayerEqu(CString ObjectName)
{
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
    pObject = NULL;
    return;
  }
  PlayerEquFile.ReadString(Stuff);
  while (Stuff != "")
  { // For each player equipment object 
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
        LogBuf  = ObjectId;
        LogBuf += " is an invalid item found in player equipment - ";
        LogBuf += "Object::IsObjInPlayerEqu";
        LogIt(LogBuf);
        delete pObject;
        pObject = NULL;
        return;
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
    pObject = NULL;
    return;
  }
  PlayerEquFile.ReadString(Stuff);
  while (Stuff != "")
  { // For each player equipment object 
    ObjectId = StrGetWord(Stuff, 2);
    pObject = new Object(ObjectId);
    if (!pObject)
    { // Object does not exist, Log it
      LogBuf  = ObjectId;
      LogBuf += " is an invalid item found in player equipment - ";
      LogBuf += "Object::IsObjInPlayerEqu";
      LogIt(LogBuf);
      delete pObject;
      pObject = NULL;
      return;
    }
    NamesCheck = pObject->Names;
    NamesCheck = StrMakeLower(NamesCheck);
    if (IsWord(ObjectName, NamesCheck))
    { // Match
      return;
    }
    else
    { // No match
      delete pObject;
      pObject = NULL;
    }
    PlayerEquFile.ReadString(Stuff);
  }
  PlayerEquFile.Close();
  // Object not found in player's inventory
  pObject = NULL;
  return;
}

/***********************************************************
* Is object in player's inventory?                         *
************************************************************/

void Object::IsObjInPlayerInv(CString ObjectName)
{
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
    pObject = NULL;
    return;
  }
  PlayerObjFile.ReadString(Stuff);
  while (Stuff != "")
  { // For all items in player inventory
    ObjectId = StrGetWord(Stuff, 2);
    ObjectName = StrMakeLower(ObjectName);
    if (ObjectName == ObjectId)
    { // Found a match
      pObject = new Object(ObjectId);
      if (pObject)
      { // Object exists
        pObject->Count = StrGetWord(Stuff, 1);
        return;
      }
      else
      { // Object does not exist, Log it
        LogBuf  = ObjectId;
        LogBuf += " is an invalid item found in player inventory - ";
        LogBuf += "Object::IsObjInPlayerInv";
        LogIt(LogBuf);
        delete pObject;
        pObject = NULL;
        return;
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
    pObject = NULL;
    return;
  }
  PlayerObjFile.ReadString(Stuff);
  while (Stuff != "")
  { // For all items in player inventory
    ObjectId = StrGetWord(Stuff, 2);
    pObject  = new Object(ObjectId);
    if (!pObject)
    { // Object does not exist, Log it
      LogBuf  = ObjectId;
      LogBuf += " is an invalid item found in player inventory - ";
      LogBuf += "Object::IsObjInPlayerInv";
      LogIt(LogBuf);
      delete pObject;
      pObject = NULL;
      return;
    }
    pObject->Count = StrGetWord(Stuff, 1);
    NamesCheck     = pObject->Names;
    NamesCheck = StrMakeLower(NamesCheck);
    if (IsWord(ObjectName, NamesCheck))
    { // Match
      return;
    }
    else
    { // No match
      delete pObject;
      pObject = NULL;
    }
    PlayerObjFile.ReadString(Stuff);
  }
  PlayerObjFile.Close();
  // Object not found in player's inventory
  return;
}

/***********************************************************
* Is object in room                                        *
************************************************************/

void Object::IsObjInRoom(CString ObjectName)
{
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
    pObject = NULL;
    return;
  }
  RoomObjFile.ReadString(Stuff);
  while (Stuff != "")
  { // For each item in room
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
        LogBuf  = ObjectId;
        LogBuf += " is an invalid item found in room - ";
        LogBuf += "Object::IsObjInRoom";
        LogIt(LogBuf);
        delete pObject;
        pObject = NULL;
        return;
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
    pObject = NULL;
    return;
  }
  RoomObjFile.ReadString(Stuff);
  while (Stuff != "")
  { // For each item in room
    ObjectId = StrGetWord(Stuff, 2);
    pObject = new Object(ObjectId);
    if (!pObject)
    { // Object does not exist, Log it
      LogBuf  = ObjectId;
      LogBuf += " is an invalid item found in room - ";
      LogBuf += "Object::IsObjInRoom";
      LogIt(LogBuf);
      delete pObject;
      pObject = NULL;
      return;
    }
    NamesCheck = pObject->Names;
    NamesCheck = StrMakeLower(NamesCheck);
    if (IsWord(ObjectName, NamesCheck))
    { // Match
      return;
    }
    else
    { // No match
      delete pObject;
      pObject = NULL;
    }
    RoomObjFile.ReadString(Stuff);
  }
  RoomObjFile.Close();
  // Object not found in room
  return;
}

/***********************************************************
* Is this a valid object?                                  *
************************************************************/

void Object::IsObject(CString ObjectId)
{
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
    return;
  }
  else
  {
    pObject = NULL;
    return;
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

  ObjectId = StrMakeLower(ObjectId);
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
    ObjectIdCheck = StrGetWord(Stuff, 2);
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
  BytesInFile = StrGetLength(PlayerEquFileNameTmp);
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

void Object::RemoveObjFromPlayerInv(CString ObjectId, int Count)
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

  ObjectId = StrMakeLower(ObjectId);
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
    ObjectIdCheck = StrGetWord(Stuff, 2);
    if (ObjectId == ObjectIdCheck)
    { // Found it, subtract 'count' from ObjCount
      ObjCount = atoi(StrGetWord(Stuff, 1));
      ObjCount -= Count;
      ObjectIdRemoved = true;
      if (ObjCount > 0)
      {
        sprintf(Buf, "%d", ObjCount);
        TmpStr = ConvertStringToCString(Buf);
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
  BytesInFile = StrGetLength(PlayerObjFileNameTmp); // TODO - steve - What is this doing?
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

  ObjectId = StrMakeLower(ObjectId);
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
    ObjectIdCheck = StrGetWord(Stuff, 2);
    if (ObjectId == ObjectIdCheck)
    { // Found it, subtract 1 from count
      ObjCount = atoi(StrGetWord(Stuff, 1));
      ObjCount--;
      ObjectIdRemoved = true;
      if (ObjCount > 0)
      {
        sprintf(Buf, "%d", ObjCount);
        TmpStr = ConvertStringToCString(Buf);
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

void Object::ShowPlayerEqu(Dnode *pDnodeTgt1)
{
  CString     ObjectId;
  CStdioFile  PlayerEquFile;
  CString     PlayerEquFileName;
  CString     Stuff;
  int         Success;
  CString     WearPosition;

  pDnodeTgt = pDnodeTgt1;
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
    WearPosition = StrGetWord(Stuff, 1);
    WearPosition = TranslateWord(WearPosition);
    ObjectId = StrGetWord(Stuff, 2);
    pObject = new Object(ObjectId);
    pDnodeActor->PlayerOut += WearPosition;
    pDnodeActor->PlayerOut += pObject->Desc1;
    pDnodeActor->PlayerOut += "\r\n";
    delete pObject;
    pObject = NULL;
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
    ObjectCount = StrGetWord(Stuff, 1);
    ObjectId = StrGetWord(Stuff, 2);
    pObject = new Object(ObjectId);
    pDnodeActor->PlayerOut += "(" + ObjectCount + ") ";
    pDnodeActor->PlayerOut += pObject->Desc1;
    pDnodeActor->PlayerOut += "\r\n";
    delete pObject;
    pObject = NULL;
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
    ObjectCount = StrGetWord(Stuff, 1);
    ObjectId    = StrGetWord(Stuff, 2);
    pObject     = new Object(ObjectId);
    pObject->Type = StrMakeLower(pObject->Type);
    pDnode->PlayerOut += "\r\n";
    if (pObject->Type != "notake")
    { // Should be only 1 NoTake type object in a room, like signs or statues
      pDnode->PlayerOut += "(" + ObjectCount + ") ";
    }
    pDnode->PlayerOut += pObject->Desc2;
    delete pObject;
    pObject = NULL;
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
    PlayerName = StrLeft(FileName, StrGetLength(FileName) - 4);
    PlayerEquFile.ReadString(Stuff);
    while (Stuff != "")
    {
      ObjectId      = StrGetWord(Stuff, 2);
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
    PlayerName = StrLeft(FileName, StrGetLength(FileName) - 4);
    PlayerObjFile.ReadString(Stuff);
    while (Stuff != "")
    {
      ObjectId      = StrGetWord(Stuff, 2);
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
    RoomName = StrLeft(FileName, StrGetLength(FileName) - 4);
    RoomObjFile.ReadString(Stuff);
    while (Stuff != "")
    { // For each room object
      ObjectId = StrGetWord(Stuff, 2);
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
    if (StrLeft(Stuff, 9) == "ObjectId:")
    {
      ObjectId = StrRight(Stuff, StrGetLength(Stuff) - 9);
      ObjectId = StrTrimLeft(ObjectId);
    }
    else
    if (StrLeft(Stuff, 6) == "Names:")
    {
      Names = StrRight(Stuff, StrGetLength(Stuff) - 6);
      Names = StrTrimLeft(Names);
    }
    else
    if (StrLeft(Stuff, 6) == "Desc1:")
    {
      Desc1 = StrRight(Stuff, StrGetLength(Stuff) - 6);
      Desc1 = StrTrimLeft(Desc1);
    }
    else
    if (StrLeft(Stuff, 6) == "Desc2:")
    {
      Desc2 = StrRight(Stuff, StrGetLength(Stuff) - 6);
      Desc2 = StrTrimLeft(Desc2);
    }
    else
    if (StrLeft(Stuff, 6) == "Desc3:")
    {
    // Desc3 can be multi-line and is dealt with in 'ExamineObj'
    }
    else
    if (StrLeft(Stuff, 7) == "Weight:")
    {
      Weight = atoi(StrRight(Stuff, StrGetLength(Stuff) - 7));
    }
    else
    if (StrLeft(Stuff, 5) == "Cost:")
    {
      Cost = atoi(StrRight(Stuff, StrGetLength(Stuff) - 5));
    }
    else
    if (StrLeft(Stuff, 5) == "Type:")
    {
      Type = StrRight(Stuff, StrGetLength(Stuff) - 5);
      Type = StrTrimLeft(Type);
    }
    else
    if (StrLeft(Stuff, 11) == "ArmorValue:")
    {
      ArmorValue = atoi(StrRight(Stuff, StrGetLength(Stuff) - 11));
    }
    else
    if (StrLeft(Stuff, 10) == "ArmorWear:")
    {
      ArmorWear = StrRight(Stuff, StrGetLength(Stuff) - 10);
      ArmorWear = StrTrimLeft(ArmorWear);
      WearPosition = ArmorWear;
      WearPosition = StrMakeLower(WearPosition);
    }
    else
    if (StrLeft(Stuff, 11) == "WeaponType:")
    {
      WeaponType = StrRight(Stuff, StrGetLength(Stuff) - 11);
      WeaponType = StrTrimLeft(WeaponType);
      ArmorWear = "wielded";
      WearPosition = "wielded";
    }
    else
    if (StrLeft(Stuff, 13) == "WeaponDamage:")
    {
      WeaponDamage = atoi(StrRight(Stuff, StrGetLength(Stuff) - 13));
    }
    else
    if (StrLeft(Stuff, 8) == "FoodPct:")
    {
      FoodPct = atoi(StrRight(Stuff, StrGetLength(Stuff) - 8));
    }
    else
    if (StrLeft(Stuff, 9) == "DrinkPct:")
    {
      DrinkPct = atoi(StrRight(Stuff, StrGetLength(Stuff) - 9));
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
  Stuff = StrTrimLeft(Stuff);
  Stuff = StrTrimRight(Stuff);
}