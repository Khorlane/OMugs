/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Validate.cpp                                     *
* Usage:  Validate Rooms, Objects, Mobiles, etc            *
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
#include "Validate.h"

/***********************************************************
* Globals                                                  *
************************************************************/

/***********************************************************
* Validate constructor                                     *
************************************************************/

Validate::Validate()
{
}

/***********************************************************
* Validate destructor                                      *
************************************************************/

Validate::~Validate()
{
}

////////////////////////////////////////////////////////////
// Public functions static                                //
////////////////////////////////////////////////////////////

/***********************************************************
 * Log Validation Error                                    *
 ***********************************************************/

void Validate::LogValErr(CString Message, CString FileName)
{
  CString LogBuf;
  CString TmpStr;

  if (StrGetLength(Message) > 50)
  { // Message is too long, chop it off
    Message = Message.Left(50);
  }
  Message = " - " + Message + " ";
  while (StrGetLength(Message) < 55)
  {
    Message += "-";
  }
  Message += "> ";
  LogBuf  = "ValErr";
  LogBuf += Message;
  LogBuf += FileName;
  LogIt(LogBuf);
  ValErr = true;
}

/***********************************************************
 * ValidateIt                                              *
 ***********************************************************/

bool Validate::ValidateIt(CString ValidationType)
{
  CString LogBuf;

  ValErr = false;
  ValidationType = StrMakeLower(ValidationType);
  if (ValidationType == "all")
  {
    Validate::ValidateAll();
  }
  else
  if (ValidationType == "mobiles")
  {
    Validate::ValidateLibraryMobiles();
    Validate::ValidateLibraryWorldMobiles();
  }
  else
  if (ValidationType == "objects")
  {
    Validate::ValidateLibraryObjects();
    Validate::ValidateLibraryLoot();
    Validate::ValidateLibraryShops();
  }
  else
  if (ValidationType == "rooms")
  {
    Validate::ValidateLibraryRooms();
    Validate::ValidateLibraryShops();
    Validate::ValidateLibraryWorldMobiles();
  }
  if (ValErr)
  {
    LogBuf  = "ValErr - Validation failed!!";
    LogIt(LogBuf);
  }
  else
  {
    LogBuf  = "Validation successful!!";
    LogIt(LogBuf);
  }
  return ValErr;
}

/***********************************************************
 * Validate all                                            *
 ***********************************************************/

void Validate::ValidateAll()
{
  ValidateLibraryLoot();
  ValidateLibraryMobiles();
  ValidateLibraryObjects();
  ValidateLibraryRooms();
  ValidateLibraryShops();
  ValidateLibraryWorldMobiles();
  ValidateRunningPlayers();
  ValidateRunningPlayersPlayerEqu();
  ValidateRunningPlayersPlayerObj();
  ValidateRunningRoomMob();
  ValidateRunningRoomObj();
}

/***********************************************************
 * Validate LibraryLoot                                    *
 ***********************************************************/

void Validate::ValidateLibraryLoot()
{
  CFileFind  FileList;
  CString    FileName;
  int        LineCount;
  CString    LogBuf;
  CString    Message;
  CString    ObjectId;
  CStdioFile ObjectIdFile;
  CString    ObjectIdFileName;
  CStdioFile LootFile;
  CString    LootFileName;
  CString    PlayerName;
  BOOL       MoreFiles;
  CString    Stuff;
  int        Success;

  LogBuf = "Begin validation LibraryLoot";
  LogIt(LogBuf);
  if (ChgDir(LOOT_DIR))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateLibraryLoot - Change directory to LOOT_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  // Get list of all LibraryLoot files
  MoreFiles = FileList.FindFile("*.*");
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateLibraryLoot - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  while (MoreFiles)
  {
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    { // Skip directories
      continue;
    }
    // Open player file
    LootFileName = FileList.GetFileName();
    PlayerName = StrLeft(LootFileName, StrGetLength(LootFileName) - 4);
    LootFileName = LOOT_DIR + LootFileName;
    Success = LootFile.Open(LootFileName,
                 CFile::modeRead |
                 CFile::typeText);
    if(!Success)
    { // File does not exist - Very bad!
      AfxMessageBox("Validate::ValidateLibraryLoot - Open loot file failed", MB_ICONSTOP);
      _endthread();
    }
    LineCount = 0;
    LootFile.ReadString(Stuff);
    while (Stuff != "")
    { // For all lines
      LineCount++;
      ObjectId = StrGetWord(Stuff, 3);
      //************
      //* ObjectId *
      //************
      ObjectIdFileName = OBJECTS_DIR;
      ObjectIdFileName += ObjectId;
      ObjectIdFileName += ".txt";
      Success = ObjectIdFile.Open(ObjectIdFileName,
                       CFile::modeRead |
                       CFile::typeText);
      if(Success)
      { // RoomId file found, don't leave it open
        ObjectIdFile.Close();
      }
      else
      { // ObjectId file not found
        Message  = "Object file";
        Message += " '";
        Message += ObjectId;
        Message += "' ";
        Message += "not found";
        FileName = LootFileName;
        LogValErr(Message, FileName);
      }
      LootFile.ReadString(Stuff);
    }
    LootFile.Close();
  }
  LogBuf = "Done  validating LibraryLoot";
  LogIt(LogBuf);
}

/***********************************************************
 * Validate LibraryMobiles                                 *
 ***********************************************************/

void Validate::ValidateLibraryMobiles()
{
  CString    FieldName;
  CString    FieldValue;
  CFileFind  FileList;
  CString    FileName;
  int        LineCount;
  CString    LogBuf;
  CStdioFile LootFile;
  CString    LootFileName;
  CString    Message;
  CStdioFile MobileFile;
  CString    MobileFileName;
  CString    MobileId;
  BOOL       MoreFiles;
  CString    Stuff;
  int        Success;

  LogBuf = "Begin validation LibraryMobiles";
  LogIt(LogBuf);
  if (ChgDir(MOBILES_DIR))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateLibraryMobiles - Change directory to MOBILES_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  // Get list of all LibraryMobiles files
  MoreFiles = FileList.FindFile("*.*");
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateLibraryMobiles - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  while (MoreFiles)
  {
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    { // Skip directories
      continue;
    }
    // Open mobile file
    MobileFileName = FileList.GetFileName();
    MobileId = StrLeft(MobileFileName, StrGetLength(MobileFileName) - 4);
    MobileFileName = MOBILES_DIR + MobileFileName;
    Success = MobileFile.Open(MobileFileName,
                   CFile::modeRead |
                   CFile::typeText);
    if(!Success)
    { // File does not exist - Very bad!
      AfxMessageBox("Validate::ValidateLibraryMobiles - Open mobile file failed", MB_ICONSTOP);
      _endthread();
    }
    LineCount = 0;
    MobileFile.ReadString(Stuff);
    while (Stuff != "")
    { // For all lines
      LineCount++;
      FieldName  = StrGetWord(Stuff, 1);
      FieldValue = StrGetWord(Stuff, 2);
      //********************************
      //* MobileId field must be first *
      //********************************
      if (LineCount == 1)
      { // First line
        if (FieldName != "MobileId:")
        { // Must be MobileId
          Message  = "MobileId field must be the first field";
          FileName = MobileFileName;
          LogValErr(Message, FileName);
        }
      }
      //*************
      //* MobileId: *
      //*************
      if (FieldName == "MobileId:")
      { // MobileId field validation
        if (MobileId != FieldValue)
        { // MobileId must match file name
          Message = "MobileId must match file name";
          FileName = MobileFileName;
          LogValErr(Message, FileName);
        }
      }
      //********
      //* Sex: *
      //********
      if (FieldName == "Sex:")
      { // Sex field validation
        if (IsNotWord(FieldValue, "F M N"))
        { // Invalid mobile sex
          Message = "Mobile sex is invalid";
          FileName = MobileFileName;
          LogValErr(Message, FileName);
        }
      }
      //***********
      //* Action: *
      //***********
      if (FieldName == "Action:")
      { // Sex field validation
        if (IsNotWord(FieldValue, "None Aggro Faction Destroy Help NoMove Wimpy"))
        { // Invalid mobile action
          Message = "Mobile action is invalid";
          FileName = MobileFileName;
          LogValErr(Message, FileName);
        }
      }
      //************
      //* Faction: *
      //************
      if (FieldName == "Faction:")
      { // Faction field validation
        if (IsNotWord(FieldValue, "Evil Lawless Neutral Lawful Good"))
        { // Invalid mobile faction
          Message = "Mobile faction is invalid";
          FileName = MobileFileName;
          LogValErr(Message, FileName);
        }
      }
      //***********
      //* Attack: *
      //***********
      if (FieldName == "Attack:")
      { // Faction field validation
        FieldValue = StrMakeLower(FieldValue);
        if (IsNotWord(FieldValue, "bites claws crushes hits mauls pierces punches slashes stabs stings thrashes"))
        { // Invalid mobile attack
          Message = "Mobile attack is invalid";
          FileName = MobileFileName;
          LogValErr(Message, FileName);
        }
      }
      //*********
      //* Loot: *
      //*********
      if (FieldName == "Loot:")
      { // Loot field validation
        LootFileName = LOOT_DIR;
        LootFileName += FieldValue;
        LootFileName += ".txt";
        Success = LootFile.Open(LootFileName,
                     CFile::modeRead |
                     CFile::typeText);
        if(Success)
        { // Loot file found, don't leave it open
          LootFile.Close();
        }
        else
        { // Loot file not found
          Message = "Loot file not found";
          FileName = MobileFileName;
          LogValErr(Message, FileName);
        }
      }
      MobileFile.ReadString(Stuff);
    }
    MobileFile.Close();
  }
  LogBuf = "Done  validating LibraryMobiles";
  LogIt(LogBuf);
}

/***********************************************************
 * Validate LibraryObjects                                 *
 ***********************************************************/

void Validate::ValidateLibraryObjects()
{

  CString    FieldName;
  CString    FieldValue;
  CFileFind  FileList;
  CString    FileName;
  int        LineCount;
  CString    LogBuf;
  CStdioFile LootFile;
  CString    LootFileName;
  CString    Message;
  CStdioFile ObjectFile;
  CString    ObjectFileName;
  CString    ObjectId;
  BOOL       MoreFiles;
  CString    Stuff;
  int        Success;

  LogBuf = "Begin validation LibraryObjects";
  LogIt(LogBuf);
  if (ChgDir(OBJECTS_DIR))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateLibraryObjects - Change directory to OBJECTS_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  // Get list of all LibarryObjects files
  MoreFiles = FileList.FindFile("*.*");
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateLibraryObjects - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  while (MoreFiles)
  {
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    { // Skip directories
      continue;
    }
    // Open object file
    ObjectFileName = FileList.GetFileName();
    ObjectId = StrLeft(ObjectFileName, StrGetLength(ObjectFileName)-4);
    ObjectFileName = OBJECTS_DIR + ObjectFileName;
    Success = ObjectFile.Open(ObjectFileName,
                   CFile::modeRead |
                   CFile::typeText);
    if(!Success)
    { // File does not exist - Very bad!
      AfxMessageBox("Validate::ValidateLibraryObjects - Open object file failed", MB_ICONSTOP);
      _endthread();
    }
    LineCount = 0;
    ObjectFile.ReadString(Stuff);
    while (Stuff != "")
    { // For all lines
      LineCount++;
      FieldName  = StrGetWord(Stuff, 1);
      FieldValue = StrGetWord(Stuff, 2);
      //********************************
      //* ObjectId field must be first *
      //********************************
      if (LineCount == 1)
      { // First line
        if (FieldName != "ObjectId:")
        { // Must be ObjectId
          Message  = "ObjectId field must be the first field";
          FileName = ObjectFileName;
          LogValErr(Message, FileName);
        }
      }
      //*************
      //* ObjectId: *
      //*************
      if (FieldName == "ObjectId:")
      { // ObjectId field validation
        if (ObjectId != FieldValue)
        { // ObjectId must match file name
          Message = "ObjectId must match file name";
          FileName = ObjectFileName;
          LogValErr(Message, FileName);
        }
      }
      //*********
      //* Type: *
      //*********
      if (FieldName == "Type:")
      { // Type field validation
        if (IsNotWord(FieldValue, "Armor Container Drink Food Junk Key Light NoTake Treasure Weapon"))
        { // Invalid object type
          Message = "Object type is invalid";
          FileName = ObjectFileName;
          LogValErr(Message, FileName);
        }
        //***************
        //* Type: Armor *
        //***************
        if (FieldValue == "armor")
        { //***************
          //* ArmorValue: *
          //***************
          ObjectFile.ReadString(Stuff);
          LineCount++;
          FieldName  = StrGetWord(Stuff, 1);
          FieldValue = StrGetWord(Stuff, 2);
          if (FieldName != "ArmorValue:")
          { // ArmorValue must follow 'Type: Armor' specification
            Message = "ArmorValue must follow 'Type: Armor' specification";
            FileName = ObjectFileName;
            LogValErr(Message, FileName);
          }
          //**************
          //* ArmorWear: *
          //**************
          ObjectFile.ReadString(Stuff);
          LineCount++;
          FieldName  = StrGetWord(Stuff, 1);
          FieldValue = StrGetWord(Stuff, 2);
          if (FieldName != "ArmorWear:")
          { // ArmorWear must follow 'ArmorValue' specification
            Message = "ArmorWear must follow 'ArmorValue' specification";
            FileName = ObjectFileName;
            LogValErr(Message, FileName);
          }
          else
          { // Validate 'wear' positions
            if (IsNotWord(FieldValue, "Head Ear Neck Shoulders Chest Back Arms Wrist Hands Finger Shield Waist Legs Ankle Feet"))
            { // Invalid wear position
              Message = "Wear position is invalid";
              FileName = ObjectFileName;
              LogValErr(Message, FileName);
            }
          }
        }
        //****************
        //* Type: Weapon *
        //****************
        if (FieldValue == "weapon")
        { //***************
          //* WeaponType: *
          //***************
          ObjectFile.ReadString(Stuff);
          LineCount++;
          FieldName  = StrGetWord(Stuff, 1);
          FieldValue = StrGetWord(Stuff, 2);
          if (FieldName != "WeaponType:")
          { // WeaponType must follow 'Type: Weapon' specification
            Message = "WeaponType must follow 'Type: Weapon' specification";
            FileName = ObjectFileName;
            LogValErr(Message, FileName);
          }
          else
          { // Validate WeaponType
            if (IsNotWord(FieldValue, "Axe Club Dagger Hammer Spear Staff Sword"))
            { // Invalid weapon type
              Message = "Weapon type is invalid";
              FileName = ObjectFileName;
              LogValErr(Message, FileName);
            }
          }
          //*****************
          //* WeaponDamage: *
          //*****************
          ObjectFile.ReadString(Stuff);
          LineCount++;
          FieldName  = StrGetWord(Stuff, 1);
          FieldValue = StrGetWord(Stuff, 2);
          if (FieldName != "WeaponDamage:")
          { // WeaponDamage must follow WeaponType specification
            Message = "WeaponDamage must follow WeaponType specification";
            FileName = ObjectFileName;
            LogValErr(Message, FileName);
          }
        }
      }
      ObjectFile.ReadString(Stuff);
    }
    ObjectFile.Close();
  }
  LogBuf = "Done  validating LibraryObjects";
  LogIt(LogBuf);
}

/***********************************************************
 * Validate LibraryRooms                                          *
 ***********************************************************/

void Validate::ValidateLibraryRooms()
{
  CStdioFile ExitToRoomIdFile;
  CString    ExitToRoomIdFileName;
  CString    FieldName;
  CString    FieldValue;
  CFileFind  FileList;
  CString    FileName;
  int        i;
  int        j;
  int        LineCount;
  CString    LogBuf;
  CString    Message;
  BOOL       MoreFiles;
  CStdioFile RoomFile;
  CString    RoomFileName;
  CString    RoomId;
  bool       RoomTypeError;
  CString    Stuff;
  int        Success;
  CString    TmpStr;

  LogBuf = "Begin validation LibraryRooms";
  LogIt(LogBuf);
  if (ChgDir(ROOMS_DIR))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateLibraryRooms - Change directory to ROOMS_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  // Get list of all LibraryRooms files
  MoreFiles = FileList.FindFile("*.*");
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateRooms - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  while (MoreFiles)
  {
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    { // Skip directories
      continue;
    }
    // Open room file
    RoomFileName = FileList.GetFileName();
    RoomId = StrLeft(RoomFileName, StrGetLength(RoomFileName) - 4);
    RoomFileName = ROOMS_DIR + RoomFileName;
    Success = RoomFile.Open(RoomFileName,
                 CFile::modeRead |
                 CFile::typeText);
    if(!Success)
    { // File does not exist - Very bad!
      AfxMessageBox("Validate::ValidateLibraryRooms - Open room file failed", MB_ICONSTOP);
      _endthread();
    }
    LineCount = 0;
    RoomFile.ReadString(Stuff);
    while (Stuff != "End of Room")
    { // For all lines
      LineCount++;
      FieldName  = StrGetWord(Stuff, 1);
      FieldValue = GetWords(Stuff, 2);
      //********************************
      //* RoomId field must be first *
      //********************************
      if (LineCount == 1)
      { // First line
        if (FieldName != "RoomId:")
        { // Must be RoomId
          Message  = "RoomId field must be the first field";
          FileName = RoomFileName;
          LogValErr(Message, FileName);
        }
      }
      //***********
      //* RoomId: *
      //***********
      if (FieldName == "RoomId:")
      { // RoomId field validation
        if (RoomId != FieldValue)
        { // RoomId must match file name
          Message = "RoomId must match file name";
          FileName = RoomFileName;
          LogValErr(Message, FileName);
        }
      }
      //*************
      //* RoomType: *
      //*************
      RoomTypeError = false;
      if (FieldName == "RoomType:")
      { // RoomType validation
        j = StrCountWords(FieldValue);
        for (i = 1; i <= j; i++)
        { // Check each word in FieldValue
          TmpStr = StrGetWord(FieldValue, i);
          if (IsWord(TmpStr, "None Dark Drink NoFight NoNPC"))
          { // Valid RoomType
            if (TmpStr != "None")
            { // Exclude RoomType of 'None'
            }
          }
          else
          { // Invalid RoomType
            Message = "RoomType has an invalid entry";
            FileName = RoomFileName;
            LogValErr(Message, FileName);
            RoomTypeError = true;
            break;
          }
        }
      }
      //************
      //* Terrain: *
      //************
      if (FieldName == "Terrain:")
      { // Terrain validation
        if (IsWord(FieldValue, "Inside Street Road Field Forest Swamp Desert Hill Mountain"))
        { // Valid Terrain
        }
        else
        { // Invalid Terrain
          Message = "Terrain is invalid";
          FileName = RoomFileName;
          LogValErr(Message, FileName);
        }
      }
      //*****************
      //* ExitToRoomId: *
      //*****************
      if (FieldName == "ExitToRoomId:")
      { // ExitToRoomId field validation
        ExitToRoomIdFileName = ROOMS_DIR;
        ExitToRoomIdFileName += FieldValue;
        ExitToRoomIdFileName += ".txt";
        Success = ExitToRoomIdFile.Open(ExitToRoomIdFileName,
                             CFile::modeRead |
                             CFile::typeText);
        if(Success)
        { // ExitToRoomId file found, don't leave it open
          ExitToRoomIdFile.Close();
        }
        else
        { // ExitToRoom file not found
          Message  = "Room file";
          Message += " '";
          Message += FieldValue;
          Message += "' ";
          Message += "not found";
          FileName = RoomFileName;
          LogValErr(Message, FileName);
        }
      }
      RoomFile.ReadString(Stuff);
      if (RoomFile.GetPosition() == RoomFile.GetLength())
      { // End of file reached
        Stuff = "End of Room";
      }
    }
    RoomFile.Close();
  }
  LogBuf = "Done  validating LibraryRooms";
  LogIt(LogBuf);
}

/***********************************************************
 * Validate LibraryShops                                          *
 ***********************************************************/

void Validate::ValidateLibraryShops()
{
  CString    FieldName;
  CString    FieldValue;
  CFileFind  FileList;
  CString    FileName;
  int        LineCount;
  CString    LogBuf;
  CString    Message;
  CString    ObjectId;
  CStdioFile ObjectIdFile;
  CString    ObjectIdFileName;
  CStdioFile ShopFile;
  CString    ShopFileName;
  CString    PlayerName;
  BOOL       MoreFiles;
  CString    Stuff;
  int        Success;

  LogBuf = "Begin validation LibraryShops";
  LogIt(LogBuf);
  if (ChgDir(SHOPS_DIR))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateLibraryShops - Change directory to SHOPS_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  // Get list of all LibraryShops files
  MoreFiles = FileList.FindFile("*.*");
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateLibraryShops - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  while (MoreFiles)
  {
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    { // Skip directories
      continue;
    }
    // Open player file
    ShopFileName = FileList.GetFileName();
    PlayerName = StrLeft(ShopFileName, StrGetLength(ShopFileName) - 4);
    ShopFileName = SHOPS_DIR + ShopFileName;
    Success = ShopFile.Open(ShopFileName,
                 CFile::modeRead |
                 CFile::typeText);
    if(!Success)
    { // File does not exist - Very bad!
      AfxMessageBox("Validate::ValidateLibraryShops - Open shop file failed", MB_ICONSTOP);
      _endthread();
    }
    LineCount = 0;
    ShopFile.ReadString(Stuff);
    while (Stuff != "End of Shop")
    { // For all lines
      LineCount++;
      FieldName  = StrGetWord(Stuff, 1);
      FieldValue = StrGetWord(Stuff, 2);
      if (FieldName != "Item:")
      { // Not an item line
        ShopFile.ReadString(Stuff);
        if (ShopFile.GetPosition() == ShopFile.GetLength())
        { // ObjectId file not found
          if (Stuff != "End of Shop")
          { // 'End of Shop' must be last line
            Message  = "'End of Shop' must be the last line";
            FileName = ShopFileName;
            LogValErr(Message, FileName);
            Stuff = "End of Shop";
          }
        }
        continue;
      }
      //********
      //* Item *
      //********
      ObjectId = FieldValue;
      ObjectIdFileName = OBJECTS_DIR;
      ObjectIdFileName += ObjectId;
      ObjectIdFileName += ".txt";
      Success = ObjectIdFile.Open(ObjectIdFileName,
                       CFile::modeRead |
                       CFile::typeText);
      if(Success)
      { // RoomId file found, don't leave it open
        ObjectIdFile.Close();
      }
      else
      { // ObjectId file not found
        Message  = "Object file";
        Message += " '";
        Message += ObjectId;
        Message += "' ";
        Message += "not found";
        FileName = ShopFileName;
        LogValErr(Message, FileName);
      }
      ShopFile.ReadString(Stuff);
    }
    ShopFile.Close();
  }
  LogBuf = "Done  validating LibraryShops";
  LogIt(LogBuf);
}

/***********************************************************
 * Validate LibraryWorldMobiles                            *
 ***********************************************************/

void Validate::ValidateLibraryWorldMobiles()
{
  CString    FieldName;
  CString    FieldValue;
  CFileFind  FileList;
  CString    FileName;
  int        LineCount;
  CString    LogBuf;
  CString    Message;
  CString    MobileId;
  CStdioFile MobileIdFile;
  CString    MobileIdFileName;
  BOOL       MoreFiles;
  CStdioFile RoomIdFile;
  CString    RoomIdFileName;
  CString    Stuff;
  int        Success;
  CStdioFile WorldMobileFile;
  CString    WorldMobileFileName;
  CString    WorldMobileName;

  LogBuf = "Begin validation LibraryWorldMobiles";
  LogIt(LogBuf);
  if (ChgDir(WORLD_MOBILES_DIR))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateLibraryWorldMobiles - Change directory to WORLD_MOBILES_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  // Get list of all LibraryWorldMobiles files
  MoreFiles = FileList.FindFile("*.*");
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateLibraryWorldMobiles - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  while (MoreFiles)
  {
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    { // Skip directories
      continue;
    }
    // Open world mobile file
    WorldMobileFileName = FileList.GetFileName();
    if (WorldMobileFileName == "ReadMe.txt")
    { // Skip ReadMe files
      continue;
    }
    WorldMobileName = StrLeft(WorldMobileFileName, StrGetLength(WorldMobileFileName) - 4);
    MobileId = WorldMobileName;
    WorldMobileFileName = WORLD_MOBILES_DIR + WorldMobileFileName;
    Success = WorldMobileFile.Open(WorldMobileFileName,
                        CFile::modeRead |
                        CFile::typeText);
    if(!Success)
    { // File does not exist - Very bad!
      AfxMessageBox("Validate::ValidateLibraryWorldMobiles - Open world mobile file failed", MB_ICONSTOP);
      _endthread();
    }
    //************
    //* MobileId *
    //************
    MobileIdFileName = MOBILES_DIR;
    MobileIdFileName += MobileId;
    MobileIdFileName += ".txt";
    Success = MobileIdFile.Open(MobileIdFileName,
                     CFile::modeRead |
                     CFile::typeText);
    if(Success)
    { // MobileId file found, don't leave it open
      MobileIdFile.Close();
    }
    else
    { // MobileId file not found
      Message  = "Mobile file";
      Message += " '";
      Message += MobileId;
      Message += "' ";
      Message += "not found";
      FileName = WorldMobileFileName;
      LogValErr(Message, FileName);
    }
    //***********************
    //* Check file contents *
    //***********************
    LineCount = 0;
    WorldMobileFile.ReadString(Stuff);
    while (Stuff != "")
    { // For all lines
      LineCount++;
      FieldName  = StrGetWord(Stuff, 1);
      FieldValue = StrGetWord(Stuff, 2);
      //**********
      //* RoomId *
      //**********
      if (FieldName == "RoomId:")
      { // RoomId field validation
        RoomIdFileName = ROOMS_DIR;
        RoomIdFileName += FieldValue;
        RoomIdFileName += ".txt";
        Success = RoomIdFile.Open(RoomIdFileName,
                       CFile::modeRead |
                       CFile::typeText);
        if(Success)
        { // RoomId file found, don't leave it open
          RoomIdFile.Close();
        }
        else
        { // RoomId file not found
          Message  = "Room file";
          Message += " '";
          Message += FieldValue;
          Message += "' ";
          Message += "not found";
          FileName = WorldMobileFileName;
          LogValErr(Message, FileName);
        }
      }
      WorldMobileFile.ReadString(Stuff);
    }
    WorldMobileFile.Close();
  }
  LogBuf = "Done  validating LibraryWorldMobiles";
  LogIt(LogBuf);
}

/***********************************************************
 * Validate RunningPlayers                                 *
 ***********************************************************/

void Validate::ValidateRunningPlayers()
{
  CString    FieldName;
  CString    FieldValue;
  CFileFind  FileList;
  CString    FileName;
  int        LineCount;
  CString    LogBuf;
  CString    Message;
  CStdioFile PlayerFile;
  CString    PlayerFileName;
  CString    PlayerName;
  BOOL       MoreFiles;
  CStdioFile RoomIdFile;
  CString    RoomIdFileName;
  CString    Stuff;
  int        Success;

  LogBuf = "Begin validation RunningPlayers";
  LogIt(LogBuf);
  if (ChgDir(PLAYER_DIR))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateRunningPlayers - Change directory to PLAYER_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  // Get list of all RunningPlayers files
  MoreFiles = FileList.FindFile("*.*");
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateRunningPlayers - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  while (MoreFiles)
  {
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    { // Skip directories
      continue;
    }
    // Open player file
    PlayerFileName = FileList.GetFileName();
    PlayerName = StrLeft(PlayerFileName, StrGetLength(PlayerFileName) - 4);
    PlayerFileName = PLAYER_DIR + PlayerFileName;
    Success = PlayerFile.Open(PlayerFileName,
                   CFile::modeRead |
                   CFile::typeText);
    if(!Success)
    { // File does not exist - Very bad!
      AfxMessageBox("Validate::ValidateRunningPlayers - Open player file failed", MB_ICONSTOP);
      _endthread();
    }
    LineCount = 0;
    PlayerFile.ReadString(Stuff);
    while (Stuff != "")
    { // For all lines
      LineCount++;
      StrReplace(Stuff, ":", " ");
      FieldName  = StrGetWord(Stuff, 1);
      FieldValue = StrGetWord(Stuff, 2);
      //****************************
      //* Name field must be first *
      //****************************
      if (LineCount == 1)
      { // First line
        if (FieldName != "Name")
        { // Must be Name
          Message  = "Name field must be the first field";
          FileName = PlayerFileName;
          LogValErr(Message, FileName);
        }
      }
      //********
      //* Name *
      //********
      if (FieldName == "Name")
      { // Name field validation
        if (PlayerName != FieldValue)
        { // Name must match file name
          Message = "Name must match file name";
          FileName = PlayerFileName;
          LogValErr(Message, FileName);
        }
      }
      //**********
      //* RoomId *
      //**********
      if (FieldName == "RoomId")
      { // RoomId field validation
        RoomIdFileName = ROOMS_DIR;
        RoomIdFileName += FieldValue;
        RoomIdFileName += ".txt";
        Success = RoomIdFile.Open(RoomIdFileName,
                       CFile::modeRead |
                       CFile::typeText);
        if(Success)
        { // RoomId file found, don't leave it open
          RoomIdFile.Close();
        }
        else
        { // RoomId file not found
          Message  = "Room file";
          Message += " '";
          Message += FieldValue;
          Message += "' ";
          Message += "not found";
          FileName = PlayerFileName;
          LogValErr(Message, FileName);
        }
      }
      PlayerFile.ReadString(Stuff);
    }
    PlayerFile.Close();
  }
  LogBuf = "Done  validating RunningPlayers";
  LogIt(LogBuf);
}

/***********************************************************
 * Validate RunningPlayersPlayerEqu                        *
 ***********************************************************/

void Validate::ValidateRunningPlayersPlayerEqu()
{
  CFileFind  FileList;
  CString    FileName;
  int        LineCount;
  CString    LogBuf;
  CString    Message;
  CString    ObjectId;
  CStdioFile ObjectIdFile;
  CString    ObjectIdFileName;
  CStdioFile PlayerEquFile;
  CString    PlayerEquFileName;
  CString    PlayerName;
  BOOL       MoreFiles;
  CString    Stuff;
  int        Success;
  CString    WearPosition;

  LogBuf = "Begin validation RunningPlayersPlayerEqu";
  LogIt(LogBuf);
  if (ChgDir(PLAYER_EQU_DIR))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateRunningPlayersPlayerEqu - Change directory to PLAYER_EQU_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  // Get list of all RunningPlayersPlayerEqu files
  MoreFiles = FileList.FindFile("*.*");
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateRunningPlayersPlayerEqu - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  while (MoreFiles)
  {
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    { // Skip directories
      continue;
    }
    // Open player file
    PlayerEquFileName = FileList.GetFileName();
    PlayerName = StrLeft(PlayerEquFileName, StrGetLength(PlayerEquFileName) - 4);
    PlayerEquFileName = PLAYER_EQU_DIR + PlayerEquFileName;
    Success = PlayerEquFile.Open(PlayerEquFileName,
                      CFile::modeRead |
                      CFile::typeText);
    if(!Success)
    { // File does not exist - Very bad!
      AfxMessageBox("Validate::ValidateRunningPlayersPlayerEqu - Open player file failed", MB_ICONSTOP);
      _endthread();
    }
    LineCount = 0;
    PlayerEquFile.ReadString(Stuff);
    while (Stuff != "")
    { // For all lines
      LineCount++;
      WearPosition = StrGetWord(Stuff, 1);
      ObjectId     = StrGetWord(Stuff, 2);
      //*****************
      //* Wear position *
      //*****************
      if (WearPosition < "01")
      { // Wear position must be 01 - 20
        Message  = "Wear position < 01";
        FileName = PlayerEquFileName;
        LogValErr(Message, FileName);
      }
      if (WearPosition > "20")
      { // Wear position must be 01 - 20
        Message  = "Wear position > 20";
        FileName = PlayerEquFileName;
        LogValErr(Message, FileName);
      }
      //************
      //* ObjectId *
      //************
      ObjectIdFileName = OBJECTS_DIR;
      ObjectIdFileName += ObjectId;
      ObjectIdFileName += ".txt";
      Success = ObjectIdFile.Open(ObjectIdFileName,
                       CFile::modeRead |
                       CFile::typeText);
      if(Success)
      { // RoomId file found, don't leave it open
        ObjectIdFile.Close();
      }
      else
      { // ObjectId file not found
        Message  = "Object file";
        Message += " '";
        Message += ObjectId;
        Message += "' ";
        Message += "not found";
        FileName = PlayerEquFileName;
        LogValErr(Message, FileName);
      }
      PlayerEquFile.ReadString(Stuff);
    }
    PlayerEquFile.Close();
  }
  LogBuf = "Done  validating RunningPlayersPlayerEqu";
  LogIt(LogBuf);
}

/***********************************************************
 * Validate RunningPlayersPlayerObj                        *
 ***********************************************************/

void Validate::ValidateRunningPlayersPlayerObj()
{
  CFileFind  FileList;
  CString    FileName;
  int        LineCount;
  CString    LogBuf;
  CString    Message;
  CString    ObjectId;
  CStdioFile ObjectIdFile;
  CString    ObjectIdFileName;
  CStdioFile PlayerObjFile;
  CString    PlayerObjFileName;
  CString    PlayerName;
  BOOL       MoreFiles;
  CString    Stuff;
  int        Success;

  LogBuf = "Begin validation RunningPlayersPlayerObj";
  LogIt(LogBuf);
  if (ChgDir(PLAYER_OBJ_DIR))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateRunningPlayersPlayerObj - Change directory to PLAYER_OBJ_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  // Get list of all RunningPlayersPlayerObj files
  MoreFiles = FileList.FindFile("*.*");
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateRunningPlayersPlayerObj - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  while (MoreFiles)
  {
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    { // Skip directories
      continue;
    }
    // Open player file
    PlayerObjFileName = FileList.GetFileName();
    PlayerName = StrLeft(PlayerObjFileName, StrGetLength(PlayerObjFileName) - 4);
    PlayerObjFileName = PLAYER_OBJ_DIR + PlayerObjFileName;
    Success = PlayerObjFile.Open(PlayerObjFileName,
                      CFile::modeRead |
                      CFile::typeText);
    if(!Success)
    { // File does not exist - Very bad!
      AfxMessageBox("Validate::ValidateRunningPlayersPlayerObj - Open player file failed", MB_ICONSTOP);
      _endthread();
    }
    LineCount = 0;
    PlayerObjFile.ReadString(Stuff);
    while (Stuff != "")
    { // For all lines
      LineCount++;
      ObjectId = StrGetWord(Stuff, 2);
      //************
      //* ObjectId *
      //************
      ObjectIdFileName = OBJECTS_DIR;
      ObjectIdFileName += ObjectId;
      ObjectIdFileName += ".txt";
      Success = ObjectIdFile.Open(ObjectIdFileName,
                       CFile::modeRead |
                       CFile::typeText);
      if(Success)
      { // RoomId file found, don't leave it open
        ObjectIdFile.Close();
      }
      else
      { // ObjectId file not found
        Message  = "Object file";
        Message += " '";
        Message += ObjectId;
        Message += "' ";
        Message += "not found";
        FileName = PlayerObjFileName;
        LogValErr(Message, FileName);
      }
      PlayerObjFile.ReadString(Stuff);
    }
    PlayerObjFile.Close();
  }
  LogBuf = "Done  validating RunningPlayersPlayerObj";
  LogIt(LogBuf);
}

/***********************************************************
 * Validate ValidateRunningRoomMob                         *
 ***********************************************************/

void Validate::ValidateRunningRoomMob()
{
  CFileFind  FileList;
  CString    FileName;
  int        LineCount;
  CString    LogBuf;
  CString    Message;
  CString    MobileId;
  CStdioFile MobileIdFile;
  CString    MobileIdFileName;
  BOOL       MoreFiles;
  int        PositionOfDot;
  CString    RoomId;
  CStdioFile RoomIdFile;
  CString    RoomIdFileName;
  CString    Stuff;
  int        Success;
  CStdioFile RoomMobFile;
  CString    RoomMobFileName;

  LogBuf = "Begin validation RunningRoomMob";
  LogIt(LogBuf);
  if (ChgDir(ROOM_MOB_DIR))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateRunningRoomMob - Change directory to ROOM_MOB_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  // Get list of all RunningRoomMob files
  MoreFiles = FileList.FindFile("*.*");
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateRunningRoomMob - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  while (MoreFiles)
  {
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    { // Skip directories
      continue;
    }
    // Open world mobile file
    RoomMobFileName = FileList.GetFileName();
    if (RoomMobFileName == "ReadMe.txt")
    { // Skip ReadMe files
      continue;
    }
    RoomId = StrLeft(RoomMobFileName, StrGetLength(RoomMobFileName) - 4);
    RoomMobFileName = ROOM_MOB_DIR + RoomMobFileName;
    Success = RoomMobFile.Open(RoomMobFileName,
                    CFile::modeRead |
                    CFile::typeText);
    if(!Success)
    { // File does not exist - Very bad!
      AfxMessageBox("Validate::ValidateRunningRoomMob - Open world mobile file failed", MB_ICONSTOP);
      _endthread();
    }
    //**********
    //* RoomId *
    //**********
    RoomIdFileName = ROOMS_DIR;
    RoomIdFileName += RoomId;
    RoomIdFileName += ".txt";
    Success = RoomIdFile.Open(RoomIdFileName,
                   CFile::modeRead |
                   CFile::typeText);
    if(Success)
    { // RoomId file found, don't leave it open
      RoomIdFile.Close();
    }
    else
    { // RoomId file not found
      Message  = "Room file";
      Message += " '";
      Message += RoomId;
      Message += "' ";
      Message += "not found";
      FileName = RoomMobFileName;
      LogValErr(Message, FileName);
    }
    //***********************
    //* Check file contents *
    //***********************
    LineCount = 0;
    RoomMobFile.ReadString(Stuff);
    while (Stuff != "")
    { // For all lines
      LineCount++;
      MobileId = StrGetWord(Stuff, 2);
      PositionOfDot = MobileId.Find('.');
      if (PositionOfDot > 1)
      { // Mobile is hurt
        MobileId = MobileId.Left(PositionOfDot);
      }

      //************
      //* MobileId *
      //************
      MobileIdFileName = MOBILES_DIR;
      MobileIdFileName += MobileId;
      MobileIdFileName += ".txt";
      Success = MobileIdFile.Open(MobileIdFileName,
                       CFile::modeRead |
                       CFile::typeText);
      if(Success)
      { // MobileId file found, don't leave it open
        MobileIdFile.Close();
      }
      else
      { // MobileId file not found
        Message  = "Mobile file";
        Message += " '";
        Message += MobileId;
        Message += "' ";
        Message += "not found";
        FileName = RoomMobFileName;
        LogValErr(Message, FileName);
      }
      RoomMobFile.ReadString(Stuff);
    }
    RoomMobFile.Close();
  }
  LogBuf = "Done  validating RunningRoomMob";
  LogIt(LogBuf);
}

/***********************************************************
 * Validate ValidateRunningRoomObj                         *
 ***********************************************************/

void Validate::ValidateRunningRoomObj()
{
  CFileFind  FileList;
  CString    FileName;
  int        LineCount;
  CString    LogBuf;
  CString    Message;
  CString    ObjectId;
  CStdioFile ObjectIdFile;
  CString    ObjectIdFileName;
  BOOL       MoreFiles;
  CString    RoomId;
  CStdioFile RoomIdFile;
  CString    RoomIdFileName;
  CString    Stuff;
  int        Success;
  CStdioFile RoomObjFile;
  CString    RoomObjFileName;

  LogBuf = "Begin validation RunningRoomObj";
  LogIt(LogBuf);
  if (ChgDir(ROOM_OBJ_DIR))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateRunningRoomObj - Change directory to ROOM_OBJ_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  // Get list of all RunningRoomObj files
  MoreFiles = FileList.FindFile("*.*");
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("Validate::ValidateRunningRoomObj - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  while (MoreFiles)
  {
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    { // Skip directories
      continue;
    }
    // Open RunningRoomObj file
    RoomObjFileName = FileList.GetFileName();
    if (RoomObjFileName == "ReadMe.txt")
    { // Skip ReadMe files
      continue;
    }
    RoomId = StrLeft(RoomObjFileName, StrGetLength(RoomObjFileName) - 4);
    RoomObjFileName = ROOM_OBJ_DIR + RoomObjFileName;
    Success = RoomObjFile.Open(RoomObjFileName,
                    CFile::modeRead |
                    CFile::typeText);
    if(!Success)
    { // File does not exist - Very bad!
      AfxMessageBox("Validate::ValidateRunningRoomObj - Open world mobile file failed", MB_ICONSTOP);
      _endthread();
    }
    //**********
    //* RoomId *
    //**********
    RoomIdFileName = ROOMS_DIR;
    RoomIdFileName += RoomId;
    RoomIdFileName += ".txt";
    Success = RoomIdFile.Open(RoomIdFileName,
                   CFile::modeRead |
                   CFile::typeText);
    if(Success)
    { // RoomId file found, don't leave it open
      RoomIdFile.Close();
    }
    else
    { // RoomId file not found
      Message  = "Room file";
      Message += " '";
      Message += RoomId;
      Message += "' ";
      Message += "not found";
      FileName = RoomObjFileName;
      LogValErr(Message, FileName);
    }
    //***********************
    //* Check file contents *
    //***********************
    LineCount = 0;
    RoomObjFile.ReadString(Stuff);
    while (Stuff != "")
    { // For all lines
      LineCount++;
      ObjectId = StrGetWord(Stuff, 2);
      //************
      //* ObjectId *
      //************
      ObjectIdFileName = OBJECTS_DIR;
      ObjectIdFileName += ObjectId;
      ObjectIdFileName += ".txt";
      Success = ObjectIdFile.Open(ObjectIdFileName,
                       CFile::modeRead |
                       CFile::typeText);
      if(Success)
      { // ObjectId file found, don't leave it open
        ObjectIdFile.Close();
      }
      else
      { // ObjectId file not found
        Message  = "Object file";
        Message += " '";
        Message += ObjectId;
        Message += "' ";
        Message += "not found";
        FileName = RoomObjFileName;
        LogValErr(Message, FileName);
      }
      RoomObjFile.ReadString(Stuff);
    }
    RoomObjFile.Close();
  }
  LogBuf = "Done  validating RunningRoomObj";
  LogIt(LogBuf);
}