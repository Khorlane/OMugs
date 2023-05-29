/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   GenerateRooms.cpp                                *
* Usage:  Generate room files from two text files that are *
*         created by exporting from a zMapper database.    *
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
#include "GenerateRooms.h"

/***********************************************************
* Globals                                                  *
************************************************************/

#pragma warning(disable : 4995)
CDaoDatabase  MapDatabase;
CDaoRecordset RoomsRecordset(&MapDatabase);
CDaoRecordset ExitsRecordset(&MapDatabase);

/***********************************************************
* GenerateRooms constructor                                *
************************************************************/

GenerateRooms::GenerateRooms()
{
  Init();
  OpenRecordsets();
  GetFields1();
  GetFields2();
  while (MoreRooms)
  {
    Parse1();
  }
  CloseRecordsets();
  Final();
}

/***********************************************************
* GenerateRooms destructor                                 *
************************************************************/

GenerateRooms::~GenerateRooms()
{
}

/***********************************************************
* Close recordsets                                         *
************************************************************/

void GenerateRooms::CloseRecordsets()
{
  //************************
  //* Close RoomsRecordset *
  //************************
  try
  {
    RoomsRecordset.Close();
  }
  catch(CDaoException* e)
  {
    AfxMessageBox(e->m_pErrorInfo->m_strDescription, MB_ICONSTOP);
    _endthread();
  }
  //************************
  //* Close ExitsRecordset *
  //************************
  try
  {
    ExitsRecordset.Close();
  }
  catch(CDaoException* e)
  {
    AfxMessageBox(e->m_pErrorInfo->m_strDescription, MB_ICONSTOP);
    _endthread();
  }
  //************************
  //* Close map database   *
  //************************
  try
  {
    MapDatabase.Close();
  }
  catch(CDaoException* e)
  {
    AfxMessageBox(e->m_pErrorInfo->m_strDescription, MB_ICONSTOP);
    _endthread();
  }
}

/***********************************************************
* Final                                                    *
************************************************************/

void GenerateRooms::Final()
{
  CString Message;

  // Room rows read
  sprintf(Buf, "%d", RowCount1);
  TmpStr = ConvertStringToCString(Buf);
  Message  = "Room rows read from Rooms recordset = ";
  Message += TmpStr;
  Message += "\n";
  // Exit rows read
  sprintf(Buf, "%d", RowCount2);
  TmpStr = ConvertStringToCString(Buf);
  Message += "Exit rows read from Exits recordset = ";
  Message += TmpStr;
  Message += "\n";
  // Rooms created
  sprintf(Buf, "%d", RoomCount);
  TmpStr = ConvertStringToCString(Buf);
  Message += "Rooms created = ";
  Message += TmpStr;
  AfxMessageBox(Message, MB_ICONINFORMATION);
}

/***********************************************************
* Init                                                     *
************************************************************/

void GenerateRooms::Init()
{
  MoreRooms  = true;
  LastStuff1 = false;
  LastStuff2 = false;
  RowCount1  = 0;
  RowCount2  = 0;
  RoomCount  = 0;
  //******************************
  //* Delete existing room files *
  //******************************
  // Change to home directory
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("GenerateRooms::Init - Change to home directory failed (1)");
    _endthread();
  }
  // Change to rooms directory
  if (ChgDir(ROOMS_DIR))
  { // Change directory failed
    AfxMessageBox("GenerateRooms::Init - Change to rooms directory failed");
    _endthread();
  }
  MoreFiles = FileList.FindFile("*.*");
  // FileList now contains a list of room files
  while (MoreFiles)
  { // For each room file
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    { // Skip directories
      continue;
    }
    RoomFileName = FileList.GetFileName();
    CFile::Remove(RoomFileName);
  }
  // Change back to home directory
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("GenerateRooms::Init - Change to home directory failed (2)");
    _endthread();
  }
}

/***********************************************************
* Open recordsets                                          *
************************************************************/

void GenerateRooms::OpenRecordsets()
{
  // Office 2000 uses DAO 3.6. MFC by default uses DAO 3.5.
  // To use DAO 3.6 from MFC you have to do the following
  // AfxGetModuleState()->m_dwVersion = 0x0601;
  //*********************
  //* Open map database *
  //*********************
  // Change to home directory
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("GenerateRooms::OpenRecordsets - Change to home directory failed");
    _endthread();;
  }
  DatabaseName  = WORLD_MAP_DIR;
  DatabaseName += "HolyQuest";
  try
  {
    MapDatabase.Open(DatabaseName);
  }
  catch(CDaoException* e)
  {
    AfxMessageBox(e->m_pErrorInfo->m_strDescription, MB_ICONSTOP);
    _endthread();
  }
  //***********************
  //* Open RoomsRecordset *
  //***********************
  SqlStmt1 = GetSqlStmt("GetRoomInfo");
  try
  {
    RoomsRecordset.Open(AFX_DAO_USE_DEFAULT_TYPE, SqlStmt1, 0);
  }
  catch(CDaoException* e)
  {
    AfxMessageBox(e->m_pErrorInfo->m_strDescription, MB_ICONSTOP);
    _endthread();
  }
  //***********************
  //* Open ExitsRecordset *
  //***********************
  SqlStmt2 = GetSqlStmt("GetExitInfo");
  try
  {
    ExitsRecordset.Open(AFX_DAO_USE_DEFAULT_TYPE, SqlStmt2, 0);
  }
  catch(CDaoException* e)
  {
    AfxMessageBox(e->m_pErrorInfo->m_strDescription, MB_ICONSTOP);
    _endthread();
  }
}

/***********************************************************
* Open room file                                           *
************************************************************/

void GenerateRooms::OpenRoomFile()
{
  //******************
  //* Open room file *
  //******************
  RoomFileName  = ROOMS_DIR;
  RoomFileName += oRoomId;
  RoomFileName += ".txt";
  Success = GenRoomFile.Open(RoomFileName,
                  CFile::modeCreate |
                  CFile::modeWrite  |
                  CFile::typeText);
  if(!Success)
  {
    Message  = "GenerateRooms::OpenRoomFile - file failed to open";
    Message += RoomFileName;
    AfxMessageBox(Message, MB_ICONSTOP);
    _endthread();
  }
}

/***********************************************************
* Parse RoomsRecordset - Room name, desc, type, terrain    *
************************************************************/

void GenerateRooms::Parse1()
{
  //*********************
  //* CurrentRoomNumber *
  //*********************
  CurrentRoomNbr = iRoomNbr;
  //************
  //* RoomName *
  //************
  oRoomName = iRoomName;
  //**********
  //* RoomId *
  //**********
  sprintf(Buf, "%d", CurrentRoomNbr);
  TmpStr = ConvertStringToCString(Buf);
  oRoomId = oRoomName + TmpStr;
  oRoomId = StrRemove(oRoomId, ' ');
  //************
  //* RoomType *
  //************
  Stuff = iRoomType;
  n = StrFind(Stuff, "RoomType:");
  if (n != 0)
  { // Sanity check
    AfxMessageBox("GenerateRooms::Parse1 - RoomType: not where it should be", MB_ICONSTOP);
    _endthread();
  }
  TmpStr = "RoomType:";
  Stuff = StrDelete(Stuff, 0, StrGetLength(TmpStr) + 1);
  Stuff = StrTrimLeft(Stuff);
  n = StrFindFirstChar(Stuff, '\r');
  Stuff = StrDelete(Stuff, n, StrGetLength(Stuff) - n);
  oRoomType = Stuff;
  //***********
  //* Terrain *
  //***********
  Stuff = iRoomType;
  n = StrFind(Stuff, "Terrain:");
  if (n == 0)
  { // Sanity check
    AfxMessageBox("GenerateRooms::Parse1 - Terrain: not where it should be", MB_ICONSTOP);
    _endthread();
  }
  TmpStr = "Terrain:";
  oTerrain = StrRight(Stuff, StrGetLength(Stuff) - n - StrGetLength(TmpStr));
  oTerrain = StrTrimLeft(oTerrain);
  oTerrain = StrTrimRight(oTerrain);
  //************
  //* RoomDesc *
  //************
  oRoomDesc = iRoomDesc;
  //***************************
  //* Start writing room info *
  //***************************
  OpenRoomFile();
  // RoomId
  GenRoomFile.WriteString("RoomId:       ");
  GenRoomFile.WriteString(oRoomId);
  GenRoomFile.WriteString("\n");
  // RoomType
  GenRoomFile.WriteString("RoomType:     ");
  GenRoomFile.WriteString(oRoomType);
  GenRoomFile.WriteString("\n");
  // Terrain
  GenRoomFile.WriteString("Terrain:      ");
  GenRoomFile.WriteString(oTerrain);
  GenRoomFile.WriteString("\n");
  // RoomName
  GenRoomFile.WriteString("RoomName:     ");
  GenRoomFile.WriteString(oRoomName);
  GenRoomFile.WriteString("\n");
  // RoomDesc
  GenRoomFile.WriteString("RoomDesc:");
  GenRoomFile.WriteString("\n");
  GenRoomFile.WriteString(oRoomDesc);
  GenRoomFile.WriteString("\n");
  // End of RoomDesc
  GenRoomFile.WriteString("End of RoomDesc");
  GenRoomFile.WriteString("\n");
  // Blank line
  GenRoomFile.WriteString("\n");
  //*****************************
  //* Parse and write exit info *
  //*****************************
  while (CurrentRoomNbr == iFromRoomNbr)
  { 
    Parse2();
    Read2();
    if (LastStuff2)
    { // End of exits recordset
      iFromRoomNbr = 0;
    }
    else
    { // Got a row
      GetFields2();
    }
  }
  // End of Exits
  GenRoomFile.WriteString("End of Exits");
  GenRoomFile.WriteString("\n");
  // Blank line
  GenRoomFile.WriteString("\n");
  // End of Room
  GenRoomFile.WriteString("End of Room");
  //*************
  //* Room done *
  //*************
  GenRoomFile.Close();
  RoomCount++;
  Read1();
  if (!LastStuff1)
  {
    GetFields1();
  }
  else
  {
    MoreRooms = false;
  }
}

/***********************************************************
* Parse ExitsRecordset - Exit direction, to room, to desc  *
************************************************************/

void GenerateRooms::Parse2()
{
  //******************
  //* Alternate exit *
  //******************
  AlternateExit =iAlternateExit;
  if (StrLeft(AlternateExit, 3) == "go ")
  {
    AlternateExit= StrDelete(AlternateExit, 0, 3);
  }
  AlternateExit = StrTrimRight(AlternateExit);
  //************
  //* ExitName *
  //************
  if (iDirection == 0)
  {
    oExitName = "North";
  }
  else
  if (iDirection == 1)
  {
    oExitName = "NorthEast";
  }
  else
  if (iDirection == 2)
  {
    oExitName = "East";
  }
  else
  if (iDirection == 3)
  {
    oExitName = "SouthEast";
  }
  else
  if (iDirection == 4)
  {
    oExitName = "South";
  }
  else
  if (iDirection == 5)
  {
    oExitName = "SouthWest";
  }
  else
  if (iDirection == 6)
  {
    oExitName = "West";
  }
  else
  if (iDirection == 7)
  {
    oExitName = "NorthWest";
  }
  else
  if (iDirection == 8)
  {
    oExitName = "Up";
  }
  else
  if (iDirection == 9)
  {
    oExitName = "Down";
  }
  else
  if (iDirection == 10)
  {
    oExitName = AlternateExit;
  }
  //************
  //* ExitDesc *
  //************
  oExitDesc = iExitDesc;
  if (oExitDesc == "")
  {
    oExitDesc = "You see nothing special.";
  }
  //****************
  //* ExitToRoomId *
  //****************
  sprintf(Buf, "%d", iToRoomNbr);
  ToRoomNbr = ConvertStringToCString(Buf);
  ToRoomName = iToRoomName;
  oExitToRoomId = ToRoomName + ToRoomNbr;
  oExitToRoomId = StrRemove(oExitToRoomId, ' ');
  //***************************
  //* Start writing exit info *
  //***************************
  // ExitName
  GenRoomFile.WriteString("ExitName:     ");
  GenRoomFile.WriteString(oExitName);
  GenRoomFile.WriteString("\n");
  // ExitDesc
  GenRoomFile.WriteString("ExitDesc:     ");
  GenRoomFile.WriteString("\n");
  GenRoomFile.WriteString(oExitDesc);
  GenRoomFile.WriteString("\n");
  // ExitToRoomId
  GenRoomFile.WriteString("ExitToRoomId: ");
  GenRoomFile.WriteString(oExitToRoomId);
  GenRoomFile.WriteString("\n");
  // ExitDoor
  GenRoomFile.WriteString("ExitDoor:     ");
  GenRoomFile.WriteString("No");
  GenRoomFile.WriteString("\n");
  // ExitLocked
  GenRoomFile.WriteString("ExitLocked    ");
  GenRoomFile.WriteString("No");
  GenRoomFile.WriteString("\n");
  // ExitKeyObjId
  GenRoomFile.WriteString("ExitKeyObjId: ");
  GenRoomFile.WriteString("None");
  GenRoomFile.WriteString("\n");
  // Blank line
  GenRoomFile.WriteString("\n");
}

/***********************************************************
* Read RoomsRecordset - Room name, desc, type, terrain     *
************************************************************/

void GenerateRooms::Read1()
{
  RoomsRecordset.MoveNext();
  if (RoomsRecordset.IsEOF())
  { // End of file
    LastStuff1 = true;
  }
  else
  { // Got a row
    RowCount1++;
  }
}

/***********************************************************
* Get fields Room name, desc, type, terrain     *
************************************************************/

void GenerateRooms::GetFields1()
{
  RoomsRecordset.GetFieldValue("iRoomNbr", OleVar);
  iRoomNbr = OleVar.lVal;
  RoomsRecordset.GetFieldValue("iRoomName", OleVar);
  iRoomName = (LPCSTR) OleVar.pbstrVal;
  RoomsRecordset.GetFieldValue("iRoomType", OleVar);
  iRoomType = (LPCSTR) OleVar.pbstrVal;
  RoomsRecordset.GetFieldValue("iRoomDesc", OleVar);
  iRoomDesc = (LPCSTR) OleVar.pbstrVal;
}

/***********************************************************
* Read ExitsRecordset - Exit direction, to room, to desc   *
************************************************************/

void GenerateRooms::Read2()
{     
  ExitsRecordset.MoveNext();
  if (ExitsRecordset.IsEOF())
  { // End of file
    LastStuff2 = true;
  }
  else
  { // Got a row
    RowCount2++;
  }
}

/***********************************************************
* Get fields - direction, to room, to desc                 *
************************************************************/

void GenerateRooms::GetFields2()
{     
  ExitsRecordset.GetFieldValue("iFromRoomNbr",   OleVar);
  iFromRoomNbr = OleVar.lVal;
  ExitsRecordset.GetFieldValue("iDirection",     OleVar);
  iDirection = OleVar.lVal;
  ExitsRecordset.GetFieldValue("iAlternateExit", OleVar);
  iAlternateExit = (LPCSTR) OleVar.pbstrVal;
  ExitsRecordset.GetFieldValue("iExitDesc",      OleVar);
  iExitDesc = (LPCSTR) OleVar.pbstrVal;
  ExitsRecordset.GetFieldValue("iToRoomNbr",     OleVar);
  iToRoomNbr = OleVar.lVal;
  ExitsRecordset.GetFieldValue("iToRoomName",    OleVar);
  iToRoomName = (LPCSTR) OleVar.pbstrVal;
}