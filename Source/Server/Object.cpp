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

#include "Object.h"

/***********************************************************
* Globals                                                  *
************************************************************/

/***********************************************************
* Object constructor                                       *
************************************************************/

Object::Object(string ObjectId)
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

bool Object::AddObjToPlayerEqu(string WearPosition, string ObjectId)
{
  bool       NewPlayerEquFile;
  bool       ObjectIdAdded;
  string     PlayerEquFileName;
  string     PlayerEquFileNameTmp;
  ifstream   PlayerEquFile;
  ofstream   PlayerEquFileTmp;
  string     WearPositionCheck;
  bool       WearWieldFailed;

  WearWieldFailed = false;
  ObjectId = StrMakeLower(ObjectId);
  // Open PlayerEqu file
  PlayerEquFileName =  PLAYER_EQU_DIR;
  PlayerEquFileName += pDnodeActor->PlayerName;
  PlayerEquFileName += ".txt";
  NewPlayerEquFile = false;
  PlayerEquFile.open(PlayerEquFileName);
  if (!PlayerEquFile.is_open())
  {
    NewPlayerEquFile = true;
  }
  // Open temp PlayerEqu file
  PlayerEquFileNameTmp =  PLAYER_EQU_DIR;
  PlayerEquFileNameTmp += pDnodeActor->PlayerName;
  PlayerEquFileNameTmp += ".tmp.txt";
  PlayerEquFileTmp.open(PlayerEquFileNameTmp);
  if (!PlayerEquFileTmp.is_open())
  {
    LogBuf = "Object::AddObjToPlayerEqu - Open PlayerEqu temp file failed";
    LogIt(LogBuf);
    _endthread();
  }
  WearPosition = TranslateWord(WearPosition);
  if (NewPlayerEquFile)
  { // New player equipment file, write the object and return
    ObjectId = WearPosition + " " + ObjectId;
    PlayerEquFileTmp << ObjectId << endl;
    PlayerEquFileTmp.close();
    Rename(PlayerEquFileNameTmp, PlayerEquFileName);
    return WearWieldFailed;
  }
  // Write temp PlayerEqu file
  ObjectIdAdded = false;
  getline(PlayerEquFile, Stuff);
  while (Stuff != "")
  {
    if (ObjectIdAdded)
    { // New object has been written, just write the rest of the objects
      PlayerEquFileTmp << Stuff << endl;
      getline(PlayerEquFile, Stuff);
      continue;
    }
    WearPositionCheck = StrGetWord(Stuff, 1);
    if (WearPosition < WearPositionCheck)
    { // Add new object in alphabetical order by translated WearPosition
      ObjectId = WearPosition + " " + ObjectId;
      PlayerEquFileTmp << ObjectId << endl;
      ObjectIdAdded = true;
      PlayerEquFileTmp << Stuff << endl;
      getline(PlayerEquFile, Stuff);
      continue;
    }
    if (WearPosition == WearPositionCheck)
    { // Already wearing an object in that position
      WearWieldFailed = true;
      ObjectIdAdded = true; // Not really added
      PlayerEquFileTmp << Stuff << endl;
      getline(PlayerEquFile, Stuff);
      continue;
    }
    // None of the above conditions satisfied, just write it
    PlayerEquFileTmp << Stuff << endl;
    getline(PlayerEquFile, Stuff);
  }
  if (!ObjectIdAdded)
  { // New object is alphabetically last
    ObjectId = WearPosition + " " + ObjectId;
    PlayerEquFileTmp << ObjectId << endl;
    ObjectIdAdded = true;
  }
  PlayerEquFile.close();
  PlayerEquFileTmp.close();
  Remove(PlayerEquFileName);
  Rename(PlayerEquFileNameTmp, PlayerEquFileName);
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

  pDnodeTgt = pDnodeTgt1;
  ObjectId = StrMakeLower(ObjectId);
  // Open PlayerObj file
  PlayerObjFileName =  PLAYER_OBJ_DIR;
  PlayerObjFileName += pDnodeTgt->PlayerName;
  PlayerObjFileName += ".txt";
  NewPlayerObjFile = false;
  PlayerObjFile.open(PlayerObjFileName);
  if (!PlayerObjFile.is_open())
  {
    NewPlayerObjFile = true;
  }
  // Open temp PlayerObj file
  PlayerObjFileNameTmp =  PLAYER_OBJ_DIR;
  PlayerObjFileNameTmp += pDnodeTgt->PlayerName;
  PlayerObjFileNameTmp += ".tmp.txt";
  PlayerObjFileTmp.open(PlayerObjFileNameTmp);
  if (!PlayerObjFileTmp.is_open())
  {
    LogBuf = "Object::AddObjToPlayerInv - Open PlayerObj temp file failed";
    LogIt(LogBuf);
    _endthread();
  }
  if (NewPlayerObjFile)
  { // New player inventory file, write the object and return
    ObjectId = "1 " + ObjectId;
    PlayerObjFileTmp << ObjectId << endl;
    PlayerObjFileTmp.close();
    Rename(PlayerObjFileNameTmp, PlayerObjFileName);
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
  Remove(PlayerObjFileName);
  Rename(PlayerObjFileNameTmp, PlayerObjFileName);
}

/***********************************************************
* Add an object to room                                    *
************************************************************/

void Object::AddObjToRoom(string RoomId, string ObjectId)
{
  bool       NewRoomObjFile;
  bool       ObjectIdAdded;
  string     ObjectIdCheck;
  int        ObjCount;
  string     RoomObjFileName;
  string     RoomObjFileNameTmp;
  ifstream   RoomObjFile;
  ofstream   RoomObjFileTmp;

  ObjectId = StrMakeLower(ObjectId);
  // Open RoomObj file
  RoomObjFileName =  ROOM_OBJ_DIR;
  RoomObjFileName += RoomId;
  RoomObjFileName += ".txt";
  NewRoomObjFile = false;
  RoomObjFile.open(RoomObjFileName);
  if (!RoomObjFile.is_open())
  {
    NewRoomObjFile = true;
  }
  // Open temp RoomObj file
  RoomObjFileNameTmp =  ROOM_OBJ_DIR;
  RoomObjFileNameTmp += RoomId;
  RoomObjFileNameTmp += ".tmp.txt";
  RoomObjFileTmp.open(RoomObjFileNameTmp);
  if (!RoomObjFileTmp.is_open())
  {
    LogBuf = "Object::AddObjToRoom - Open RoomObj temp file failed";
    LogIt(LogBuf);
    _endthread();
  }
  if (NewRoomObjFile)
  { // New room object file, write the object and return
    ObjectId = "1 " + ObjectId;
    RoomObjFileTmp << ObjectId << endl;
    RoomObjFileTmp.close();
    ErrorCode = Rename(RoomObjFileNameTmp, RoomObjFileName);
    if (ErrorCode.value() != 0)
    {
      LogBuf = "Object::AddObjToRoom - Rename RoomObj temp file failed";
      LogIt(LogBuf);
      exit(1);
    }
    return;
  }
  // Write temp RoomObj file
  ObjectIdAdded = false;
  getline(RoomObjFile, Stuff);
  while (Stuff != "")
  {
    if (ObjectIdAdded)
    { // New object has been written, just write the rest of the objects
      Stuff += "\n";
      RoomObjFileTmp << Stuff << endl;
      getline(RoomObjFile, Stuff);
      continue;
    }
    ObjectIdCheck = StrGetWord(Stuff, 2);
    if (ObjectId < ObjectIdCheck)
    { // Add new object in alphabetical order
      ObjectId = "1 " + ObjectId;
      RoomObjFileTmp << ObjectId << endl;
      ObjectIdAdded = true;
      RoomObjFileTmp << Stuff << endl;
      getline(RoomObjFile, Stuff);
      continue;
    }
    if (ObjectId == ObjectIdCheck)
    { // Existing object same as new object, add 1 to count
      ObjCount = stoi(StrGetWord(Stuff, 1));
      ObjCount++;
      sprintf(Buf, "%d", ObjCount);
      TmpStr = Buf;
      ObjectId = TmpStr + " " + ObjectId;
      ObjectId += "\n";
      RoomObjFileTmp << ObjectId << endl;
      ObjectIdAdded = true;
      getline(RoomObjFile, Stuff);
      continue;
    }
    // None of the above conditions satisfied, just write it
    RoomObjFileTmp << Stuff << endl;
    getline(RoomObjFile, Stuff);
  }
  if (!ObjectIdAdded)
  { // New object is alphabetically last
    ObjectId = "1 " + ObjectId;
    RoomObjFileTmp << ObjectId << endl;
    ObjectIdAdded = true;
  }
  RoomObjFile.close();
  RoomObjFileTmp.close();
  Remove(RoomObjFileName);
  Rename(RoomObjFileNameTmp, RoomObjFileName);
}

/***********************************************************
* Calculate player armor class                             *
************************************************************/

int Object::CalcPlayerArmorClass()
{
  int         ArmorClass;
  string      ObjectId;
  ifstream    PlayerEquFile;
  string      PlayerEquFileName;
  string      WearPosition;

  ArmorClass = 0;
  // Open PlayerObj file
  PlayerEquFileName =  PLAYER_EQU_DIR;
  PlayerEquFileName += pDnodeActor->PlayerName;
  PlayerEquFileName += ".txt";
  PlayerEquFile.open(PlayerEquFileName);
  if (!PlayerEquFile.is_open())
  { // No player equipment
    return ArmorClass;
  }
  getline(PlayerEquFile, Stuff);
  while (Stuff != "")
  {
    ObjectId = StrGetWord(Stuff, 2);
    pObject = new Object(ObjectId);
    ArmorClass += pObject->ArmorValue;
    delete pObject;
    pObject = NULL;
    getline(PlayerEquFile, Stuff);
  }
  PlayerEquFile.close();
  return ArmorClass;
}

/***********************************************************
* Is object in player's equipment?                         *
************************************************************/

void Object::IsObjInPlayerEqu(string ObjectName)
{
  string      NamesCheck;
  string      ObjectId;
  string      ObjectIdCheck;
  string      ObjectNameCheck;
  string      PlayerEquFileName;
  ifstream    PlayerEquFile;

  // Open PlayerEqu file
  PlayerEquFileName =  PLAYER_EQU_DIR;
  PlayerEquFileName += pDnodeActor->PlayerName;
  PlayerEquFileName += ".txt";
  //*******************************
  //* Try matching using ObjectId *
  //*******************************
  PlayerEquFile.open(PlayerEquFileName);
  if (!PlayerEquFile.is_open())
  { // Player has no objects
    pObject = NULL;
    return;
  }
  getline(PlayerEquFile, Stuff);
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
    getline(PlayerEquFile, Stuff);
  }
  PlayerEquFile.close();
  //***************************************************
  //* No match found, try getting match using 'names' *
  //***************************************************
  PlayerEquFile.open(PlayerEquFileName);
  if (!PlayerEquFile.is_open())
  { // Player has no objects
    pObject = NULL;
    return;
  }
  getline(PlayerEquFile, Stuff);
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
    if (StrIsWord(ObjectName, NamesCheck))
    { // Match
      return;
    }
    else
    { // No match
      delete pObject;
      pObject = NULL;
    }
    getline(PlayerEquFile, Stuff);
  }
  PlayerEquFile.close();
  // Object not found in player's inventory
  pObject = NULL;
  return;
}

/***********************************************************
* Is object in player's inventory?                         *
************************************************************/

void Object::IsObjInPlayerInv(string ObjectName)
{
  string      NamesCheck;
  string      ObjectId;
  string      ObjectIdCheck;
  string      ObjectNameCheck;
  string      PlayerObjFileName;
  ifstream    PlayerObjFile;

  // Open PlayerObj file
  PlayerObjFileName =  PLAYER_OBJ_DIR;
  PlayerObjFileName += pDnodeActor->PlayerName;
  PlayerObjFileName += ".txt";
  //*******************************
  //* Try matching using ObjectId *
  //*******************************
  PlayerObjFile.open(PlayerObjFileName);
  if (!PlayerObjFile.is_open())
  { // Player has no objects
    pObject = NULL;
    return;
  }
  getline(PlayerObjFile, Stuff);
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
    getline(PlayerObjFile, Stuff);
  }
  PlayerObjFile.close();
  //***************************************************
  //* No match found, try getting match using 'names' *
  //***************************************************
  PlayerObjFile.open(PlayerObjFileName);
  if (!PlayerObjFile.is_open())
  { // Player has no objects
    pObject = NULL;
    return;
  }
  getline(PlayerObjFile, Stuff);
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
    if (StrIsWord(ObjectName, NamesCheck))
    { // Match
      return;
    }
    else
    { // No match
      delete pObject;
      pObject = NULL;
    }
    getline(PlayerObjFile, Stuff);
  }
  PlayerObjFile.close();
  // Object not found in player's inventory
  return;
}

/***********************************************************
* Is object in room                                        *
************************************************************/

void Object::IsObjInRoom(string ObjectName)
{
  string      NamesCheck;
  string      ObjectId;
  string      ObjectIdCheck;
  string      ObjectNameCheck;
  string      RoomObjFileName;
  ifstream    RoomObjFile;

  // Open RoomObj file
  RoomObjFileName =  ROOM_OBJ_DIR;
  RoomObjFileName += pDnodeActor->pPlayer->RoomId;
  RoomObjFileName += ".txt";
  //*******************************
  //* Try matching using ObjectId *
  //*******************************
  RoomObjFile.open(RoomObjFileName);
  if (!RoomObjFile.is_open())
  { // Room has no objects
    pObject = NULL;
    return;
  }
  getline(RoomObjFile, Stuff);
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
    getline(RoomObjFile, Stuff);
  }
  RoomObjFile.close();
  //***************************************************
  //* No match found, try getting match using 'names' *
  //***************************************************
  RoomObjFile.open(RoomObjFileName);
  if (!RoomObjFile.is_open())
  { // Room has no objects
    pObject = NULL;
    return;
  }
  getline(RoomObjFile, Stuff);
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
    if (StrIsWord(ObjectName, NamesCheck))
    { // Match
      return;
    }
    else
    { // No match
      delete pObject;
      pObject = NULL;
    }
    getline(RoomObjFile, Stuff);
  }
  RoomObjFile.close();
  // Object not found in room
  return;
}

/***********************************************************
* Is this a valid object?                                  *
************************************************************/

void Object::IsObject(string ObjectId)
{
  string      ObjectFileName;
  ifstream    ObjectFile;

  ObjectFileName =  OBJECTS_DIR;
  ObjectFileName += ObjectId;
  ObjectFileName += ".txt";
  if (FileExist(ObjectFileName))
  {
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

void Object::RemoveObjFromPlayerEqu(string ObjectId)
{
  int        BytesInFile;
  bool       ObjectIdRemoved;
  string     ObjectIdCheck;
  string     PlayerEquFileName;
  string     PlayerEquFileNameTmp;
  ifstream   PlayerEquFile;
  ofstream   PlayerEquFileTmp;

  ObjectId = StrMakeLower(ObjectId);
  // Open PlayerEqu file
  PlayerEquFileName =  PLAYER_EQU_DIR;
  PlayerEquFileName += pDnodeActor->PlayerName;
  PlayerEquFileName += ".txt";
  PlayerEquFile.open(PlayerEquFileName);
  if (!PlayerEquFile.is_open())
  {
    LogBuf = "Object::RemoveObjFromPlayerEqu - Open PlayerEqu file failed";
    LogIt(LogBuf);
    _endthread();
  }
  // Open temp PlayerEqu file
  PlayerEquFileNameTmp =  PLAYER_EQU_DIR;
  PlayerEquFileNameTmp += pDnodeActor->PlayerName;
  PlayerEquFileNameTmp += ".tmp.txt";
  PlayerEquFileTmp.open(PlayerEquFileNameTmp);
  if (!PlayerEquFileTmp.is_open())
  {
    LogBuf = "Object::RemoveObjFromPlayerEqu - Open PlayerEqu temp file failed";
    LogIt(LogBuf);
    _endthread();
  }
  // Write temp PlayerEqu file
  ObjectIdRemoved = false;
  getline(PlayerEquFile, Stuff);
  while (Stuff != "")
  {
    if (ObjectIdRemoved)
    { // Object has been removed, just write the rest of the objects
      PlayerEquFileTmp << Stuff << endl;
      getline(PlayerEquFile, Stuff);
      continue;
    }
    ObjectIdCheck = StrGetWord(Stuff, 2);
    if (ObjectId == ObjectIdCheck)
    { // Found it, skipping it will remove it from the file
      ObjectIdRemoved = true;
      getline(PlayerEquFile, Stuff);
      continue;
    }
    // None of the above conditions satisfied, just write it
    PlayerEquFileTmp << Stuff << endl;
    getline(PlayerEquFile, Stuff);
  }
  if (!ObjectIdRemoved)
  { // Object not removed, this is definitely BAD!
    LogBuf = "Object::RemoveObjFromPlayerEqu - Object not removed";
    LogIt(LogBuf);
    _endthread();
  }
  BytesInFile = (int)PlayerEquFileTmp.tellp();
  PlayerEquFile.close();
  PlayerEquFileTmp.close();
  Remove(PlayerEquFileName);
  if (BytesInFile > 0)
  { // If the file is not empty, rename it
    Rename(PlayerEquFileNameTmp, PlayerEquFileName);
  }
  else
  { // If the file is empty, delete it
    Remove(PlayerEquFileNameTmp);
  }
}

/***********************************************************
* Remove an object from player's inventory                 *
************************************************************/

void Object::RemoveObjFromPlayerInv(string ObjectId, int Count)
{
  int        BytesInFile;
  bool       ObjectIdRemoved;
  string     ObjectIdCheck;
  int        ObjCount;
  string     PlayerObjFileName;
  string     PlayerObjFileNameTmp;
  ifstream   PlayerObjFile;
  ofstream   PlayerObjFileTmp;

  ObjectId = StrMakeLower(ObjectId);
  // Open PlayerObj file
  PlayerObjFileName =  PLAYER_OBJ_DIR;
  PlayerObjFileName += pDnodeActor->PlayerName;
  PlayerObjFileName += ".txt";
  PlayerObjFile.open(PlayerObjFileName);
  if (!PlayerObjFile.is_open())
  {
    LogBuf = "Object::RemoveObjFromPlayerInv - Open PlayerObj file failed";
    LogIt(LogBuf);
    _endthread();
  }
  // Open temp PlayerObj file
  PlayerObjFileNameTmp =  PLAYER_OBJ_DIR;
  PlayerObjFileNameTmp += pDnodeActor->PlayerName;
  PlayerObjFileNameTmp += ".tmp.txt";
  PlayerObjFileTmp.open(PlayerObjFileNameTmp);
  if (!PlayerObjFileTmp.is_open())
  {
    LogBuf = "Object::RemoveObjFromPlayerInv - Open PlayerObj temp file failed";
    LogIt(LogBuf);
    _endthread();
  }
  // Write temp PlayerObj file
  ObjectIdRemoved = false;
  getline(PlayerObjFile, Stuff);
  while (Stuff != "")
  {
    if (ObjectIdRemoved)
    { // Object has been removed, just write the rest of the objects
      PlayerObjFileTmp << Stuff << endl;
      getline(PlayerObjFile, Stuff);
      continue;
    }
    ObjectIdCheck = StrGetWord(Stuff, 2);
    if (ObjectId == ObjectIdCheck)
    { // Found it, subtract 'count' from ObjCount
      ObjCount = stoi(StrGetWord(Stuff, 1));
      ObjCount -= Count;
      ObjectIdRemoved = true;
      if (ObjCount > 0)
      {
        sprintf(Buf, "%d", ObjCount);
        TmpStr = Buf;
        ObjectId = TmpStr + " " + ObjectId;
        PlayerObjFileTmp << ObjectId << endl;
      }
      getline(PlayerObjFile, Stuff);
      continue;
    }
    // None of the above conditions satisfied, just write it
    PlayerObjFileTmp << Stuff << endl;
    getline(PlayerObjFile, Stuff);
  }
  if (!ObjectIdRemoved)
  { // Object not removed, this is definitely BAD!
    LogBuf = "Object::RemoveObjFromPlayerInv - Object not removed";
    LogIt(LogBuf);
    _endthread();
  }
  BytesInFile = (int) PlayerObjFileTmp.tellp();
  PlayerObjFile.close();
  PlayerObjFileTmp.close();
  Remove(PlayerObjFileName);
  if (BytesInFile > 0)
  { // If the file is not empty, rename it
    Rename(PlayerObjFileNameTmp, PlayerObjFileName);
  }
  else
  { // If the file is empty, delete it
    Remove(PlayerObjFileNameTmp);
  }
}

/***********************************************************
* Remove an object from room                               *
************************************************************/

void Object::RemoveObjFromRoom(string ObjectId)
{
  int        BytesInFile;
  bool       ObjectIdRemoved;
  string     ObjectIdCheck;
  int        ObjCount;
  string     RoomObjFileName;
  string     RoomObjFileNameTmp;
  ifstream   RoomObjFile;
  ofstream   RoomObjFileTmp;

  ObjectId = StrMakeLower(ObjectId);
  // Open RoomObj file
  RoomObjFileName =  ROOM_OBJ_DIR;
  RoomObjFileName += pDnodeActor->pPlayer->RoomId;
  RoomObjFileName += ".txt";
  RoomObjFile.open(RoomObjFileName);
  if (!RoomObjFile.is_open())
  {
    LogBuf = "Object::RemoveObjFromRoom - Open RoomObj file failed";
    LogIt(LogBuf);
    _endthread();
  }
  // Open temp RoomObj file
  RoomObjFileNameTmp =  ROOM_OBJ_DIR;
  RoomObjFileNameTmp += pDnodeActor->pPlayer->RoomId;
  RoomObjFileNameTmp += ".tmp.txt";
  RoomObjFileTmp.open(RoomObjFileNameTmp);
  if (!RoomObjFileTmp.is_open())
  {
    LogBuf = "Object::RemoveObjFromRoom - Open RoomObj temp file failed";
    LogIt(LogBuf);
    _endthread();
  }
  // Write temp RoomObj file
  ObjectIdRemoved = false;
  getline(RoomObjFile, Stuff);
  while (Stuff != "")
  {
    if (ObjectIdRemoved)
    { // Object has been removed, just write the rest of the objects
      RoomObjFileTmp << Stuff << endl;
      getline(RoomObjFile, Stuff);
      continue;
    }
    ObjectIdCheck = StrGetWord(Stuff, 2);
    if (ObjectId == ObjectIdCheck)
    { // Found it, subtract 1 from count
      ObjCount = stoi(StrGetWord(Stuff, 1));
      ObjCount--;
      ObjectIdRemoved = true;
      if (ObjCount > 0)
      {
        sprintf(Buf, "%d", ObjCount);
        TmpStr = Buf;
        ObjectId = TmpStr + " " + ObjectId;
        RoomObjFileTmp << ObjectId << endl;
      }
      getline(RoomObjFile, Stuff);
      continue;
    }
    // None of the above conditions satisfied, just write it
    RoomObjFileTmp << Stuff << endl;
    getline(RoomObjFile, Stuff);
  }
  if (!ObjectIdRemoved)
  { // Object not removed, this is definitely BAD!
    LogBuf = "Object::RemoveObjFromRoom - Object not removed";
    LogIt(LogBuf);
    _endthread();
  }
  BytesInFile = (int) RoomObjFileTmp.tellp();
  RoomObjFile.close();
  RoomObjFileTmp.close();
  Remove(RoomObjFileName);
  if (BytesInFile > 0)
  { // If the file is not empty, rename it
    Rename(RoomObjFileNameTmp, RoomObjFileName);
  }
  else
  { // If the file is empty, delete it
    Remove(RoomObjFileNameTmp);
  }
}

/***********************************************************
* Show player equipment                                    *
************************************************************/

void Object::ShowPlayerEqu(Dnode *pDnodeTgt1)
{
  string     ObjectId;
  ifstream   PlayerEquFile;
  string     PlayerEquFileName;
  string     WearPosition;

  pDnodeTgt = pDnodeTgt1;
  // Open PlayerEqu file
  PlayerEquFileName =  PLAYER_EQU_DIR;
  PlayerEquFileName += pDnodeTgt->PlayerName;
  PlayerEquFileName += ".txt";
  PlayerEquFile.open(PlayerEquFileName);
  if (!PlayerEquFile.is_open())
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
  getline(PlayerEquFile, Stuff);
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
    getline(PlayerEquFile, Stuff);
  }
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
  PlayerEquFile.close();
}

/***********************************************************
* Show player inventory                                    *
************************************************************/

void Object::ShowPlayerInv()
{
  string      ObjectCount;
  string      ObjectId;
  ifstream    PlayerObjFile;
  string      PlayerObjFileName;

  // Open PlayerObj file
  PlayerObjFileName =  PLAYER_OBJ_DIR;
  PlayerObjFileName += pDnodeActor->PlayerName;
  PlayerObjFileName += ".txt";
  PlayerObjFile.open(PlayerObjFileName);
  if (!PlayerObjFile.is_open())
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
  getline(PlayerObjFile, Stuff);
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
    getline(PlayerObjFile, Stuff);
  }
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
  PlayerObjFile.close();
}

/***********************************************************
* Show objects in room                                     *
************************************************************/

void Object::ShowObjsInRoom(Dnode *pDnode)
{
  string      ObjectCount;
  string      ObjectId;
  ifstream    RoomObjFile;
  string      RoomObjFileName;

  // Open RoomObj file
  RoomObjFileName =  ROOM_OBJ_DIR;
  RoomObjFileName += pDnode->pPlayer->RoomId;
  RoomObjFileName += ".txt";
  RoomObjFile.open(RoomObjFileName);
  if (!RoomObjFile.is_open())
  { // No objects in room to display
    return;
  }
  getline(RoomObjFile, Stuff);
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
    getline(RoomObjFile, Stuff);
  }
  RoomObjFile.close();
}

/***********************************************************
* Find an object where ever it is                          *
************************************************************/

void Object::WhereObj(string ObjectIdSearch)
{
  WhereObjPlayerEqu(ObjectIdSearch);
  WhereObjPlayerObj(ObjectIdSearch);
  WhereObjRoomObj(ObjectIdSearch);
}

/***********************************************************
* Where is object in PlayerEqu                               *
************************************************************/

void Object::WhereObjPlayerEqu(string ObjectIdSearch)
{
  string      FileName;
  string      ObjectId;
  string      PlayerEquFileName;
  ifstream    PlayerEquFile;
  string      PlayerName;

  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "Objects in player equipment";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "---------------------------";
  pDnodeActor->PlayerOut += "\r\n";
  if (ChgDir(PLAYER_EQU_DIR))
  { // Change directory failed
    LogBuf = "Object::WhereObjPlayerEqu - Change directory to PLAYER_EQU_DIR failed";
    LogIt(LogBuf);
    _endthread();
  }
  for (const auto &entry : fs::directory_iterator("./"))
  {
    if (entry.is_directory())
    {
      continue;
    }
    FileName = entry.path().filename().string();
    // Open PlayerEqu file
    PlayerEquFileName = FileName;
    PlayerEquFile.open(PlayerEquFileName);
    if (!PlayerEquFile.is_open())
    { // File does not exist - Very bad!
      LogBuf = "Object::WhereObjPlayerEqu - Open PlayerEqu file failed";
      LogIt(LogBuf);
      _endthread();
    }
    PlayerName = StrLeft(FileName, StrGetLength(FileName) - 4);
    getline(PlayerEquFile, Stuff);
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
      getline(PlayerEquFile, Stuff);
    }
    PlayerEquFile.close();
  }
  if (ChgDir(HomeDir))
  { // Change directory failed
    LogBuf = "Object::WhereObjPlayerEqu - Change directory to HomeDir failed";
    LogIt(LogBuf);
    _endthread();
  }
}

/***********************************************************
* Where is object in PlayerObj                               *
************************************************************/

void Object::WhereObjPlayerObj(string ObjectIdSearch)
{
  string     FileName;
  string     ObjectId;
  string     PlayerObjFileName;
  ifstream   PlayerObjFile;
  string     PlayerName;

  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "Objects in player inventory";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "---------------------------";
  pDnodeActor->PlayerOut += "\r\n";
  if (ChgDir(PLAYER_OBJ_DIR))
  { // Change directory failed
    LogBuf = "Object::WhereObjPlayerObj - Change directory to PLAYER_OBJ_DIR failed";
    LogIt(LogBuf);
    _endthread();
  }
  for (const auto &entry : fs::directory_iterator("./"))
  {
    if (entry.is_directory())
    {
      continue;
    }
    FileName = entry.path().filename().string();
    // Open PlayerObj file
    PlayerObjFileName = FileName;
    PlayerObjFile.open(PlayerObjFileName);
    if (!PlayerObjFile.is_open())
    { // File does not exist - Very bad!
      LogBuf = "Object::WhereObjPlayerObj - Open PlayerObj file failed";
      LogIt(LogBuf);
      _endthread();
    }
    PlayerName = StrLeft(FileName, StrGetLength(FileName) - 4);
    getline(PlayerObjFile, Stuff);
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
      getline(PlayerObjFile, Stuff);
    }
    PlayerObjFile.close();
  }
  if (ChgDir(HomeDir))
  { // Change directory failed
    LogBuf = "Object::WhereObjPlayerObj - Change directory to HomeDir failed";
    LogIt(LogBuf);
    _endthread();
  }
}

/***********************************************************
* Where is object in RoomObj                               *
************************************************************/

void Object::WhereObjRoomObj(string ObjectIdSearch)
{
  string     FileName;
  string     ObjectId;
  string     RoomName;
  string     RoomObjFileName;
  ifstream   RoomObjFile;

  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "Objects in rooms";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "----------------";
  pDnodeActor->PlayerOut += "\r\n";
  if (ChgDir(ROOM_OBJ_DIR))
  { // Change directory failed
    LogBuf = "Object::WhereObjRoomObj - Change directory to ROOM_OBJ_DIR failed";
    LogIt(LogBuf);
    _endthread();
  }
  for (const auto &entry : fs::directory_iterator("./"))
  {
    if (entry.is_directory())
    {
      continue;
    }
    FileName = entry.path().filename().string();
    // Open RoomObj file
    RoomObjFileName = FileName;
    RoomObjFile.open(RoomObjFileName);
    if (!RoomObjFile.is_open())
    { // File does not exist - Very bad!
      LogBuf = "Object::WhereObj - Open RoomObj file failed";
      LogIt(LogBuf);
      _endthread();
    }
    RoomName = StrLeft(FileName, StrGetLength(FileName) - 4);
    getline(RoomObjFile, Stuff);
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
      getline(RoomObjFile, Stuff);
    }
    RoomObjFile.close();
  }
  if (ChgDir(HomeDir))
  { // Change directory failed
    LogBuf = "Object::WhereObj - Change directory to HomeDir failed";
    LogIt(LogBuf);
    _endthread();
  }
}

////////////////////////////////////////////////////////////
// Public functions                                       //
////////////////////////////////////////////////////////////

/***********************************************************
* Examine object                                           *
************************************************************/

void Object::ExamineObj(string ObjectId)
{
  OpenFile(ObjectId);
  while (Stuff != "Desc3:")
  {
    getline(ObjectFile, Stuff); // Do not use ReadLine() here
  }
  // Object Description 3
  getline(ObjectFile, Stuff); // Do not use ReadLine() here
  while (Stuff != "End Desc3")
  {
    pDnodeActor->PlayerOut += Stuff;
    pDnodeActor->PlayerOut += "\r\n";
    getline(ObjectFile, Stuff); // Do not use ReadLine() here
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
  ObjectFile.close();
}

/***********************************************************
* Open Object file                                         *
************************************************************/

void Object::OpenFile(string ObjectId)
{
  string  ObjectFileName;

  ObjectFileName =  OBJECTS_DIR;
  ObjectFileName += ObjectId;
  ObjectFileName += ".txt";
  ObjectFile.open(ObjectFileName);
  if (ObjectFile.is_open())
  {
    LogBuf = "Object::OpenFile - Object does not exist!";
    LogIt(LogBuf);
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
      Weight = stoi(StrRight(Stuff, StrGetLength(Stuff) - 7));
    }
    else
    if (StrLeft(Stuff, 5) == "Cost:")
    {
      Cost = stoi(StrRight(Stuff, StrGetLength(Stuff) - 5));
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
      ArmorValue = stoi(StrRight(Stuff, StrGetLength(Stuff) - 11));
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
      WeaponDamage = stoi(StrRight(Stuff, StrGetLength(Stuff) - 13));
    }
    else
    if (StrLeft(Stuff, 8) == "FoodPct:")
    {
      FoodPct = stoi(StrRight(Stuff, StrGetLength(Stuff) - 8));
    }
    else
    if (StrLeft(Stuff, 9) == "DrinkPct:")
    {
      DrinkPct = stoi(StrRight(Stuff, StrGetLength(Stuff) - 9));
    }
    ReadLine();
  }
}

/***********************************************************
* Read a line from Object file                             *
************************************************************/

void Object::ReadLine()
{
  getline(ObjectFile, Stuff);
  Stuff = StrTrimLeft(Stuff);
  Stuff = StrTrimRight(Stuff);
}