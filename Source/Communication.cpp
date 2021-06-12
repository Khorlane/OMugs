/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   Communication.cpp                                *
* Usage:  Winsock tcp/ip telnet player communications      *
* Author: Steve Bryant (khorlane@www.holyquest.org)        *
*                                                          *
* This program belongs to Stephen L. Bryant.               *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

/***********************************************************
* Includes                                                 *
************************************************************/

#include "stdafx.h"
#include "Color.h"
#include "Communication.h"
#include "Config.h"
#include "Descriptor.h"
#include "Help.h"
#include "Log.h"
#include "Player.h"
#include "Room.h"
#include "Social.h"
//#include "PlayerList.h"

/***********************************************************
* Globals                                                  *
************************************************************/

extern bool   StateRunning;
extern bool   StateConnections;
extern Help   *pHelp;
extern Log    *pLog;
extern Room   *pRoom;
extern Social *pSocial;

extern CString GetWord(CString CmdStr, int WordNbr);
extern CString GetWords(CString CmdStr, int WordNbr);

const CString Normal        = NORMAL;
const CString BrightBlack   = BBLACK;
const CString BrightRed     = BRED;
const CString BrightGreen   = BGREEN;
const CString BrightYellow  = BYELLOW;
const CString BrightBlue    = BBLUE;
const CString BrightMagenta = BMAGENTA;
const CString BrightCyan    = BCYAN;
const CString BrightWhite   = BWHITE;

/************************************************************
 * Communication class constructor - Establish port         *
 ************************************************************/

Communication::Communication(int port)
{
  unsigned long FionbioParm;
  struct        linger      ld;
  CString       LogBuf;
  int           OptionValue;
  int           Result;
  struct        sockaddr_in sa;

  FionbioParm       = 1;
  ld.l_onoff        = 0;
  ld.l_linger       = 0;
  OptionValue       = 1;
  StateConnections  = true;
  WORD wVersionRequested;
  WSADATA wsaData;
  wVersionRequested = MAKEWORD(1, 1);
  // Initialize WinSock API (pg 320)
  Result = WSAStartup(wVersionRequested, &wsaData);
  if (Result)
  {
    LogBuf.Format("%s", strerror(errno));
    LogBuf = "WinSock not available!: " + LogBuf;
    pLog->LogIt(LogBuf);
    exit(-1);
  }
  // Establish a streaming socket (pg 51)
  ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (ListenSocket == SOCKET_ERROR)
  {
    LogBuf.Format("%s", strerror(errno));
    LogBuf = "Communication: initializing socket: " + LogBuf;
    pLog->LogIt(LogBuf);
    exit(-1);
  }
  // Enable reuse of local socket name (pg 305)
  Result = setsockopt(ListenSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&OptionValue, sizeof(OptionValue));
  if (Result)
  {
    LogBuf.Format("%s", strerror(errno));
    LogBuf = "Communication: setsockopt: " + LogBuf;
    pLog->LogIt(LogBuf);
    exit(-1);
  }
  // Establish underlying TCP/IP buffer size (pg 305)
  Result = setsockopt(ListenSocket, SOL_SOCKET, SO_SNDBUF, (char *) &OptionValue, sizeof(OptionValue));
  if (Result)
  {
    LogBuf.Format("%s", strerror(errno));
    LogBuf = "Communication: setsockopt SNDBUF: " + LogBuf;
    pLog->LogIt(LogBuf);
    exit (-1); /* fixme - bad exit strategy */
  }
  // Disable linger and set timeout to zero (pg 301)
  Result = setsockopt(ListenSocket, SOL_SOCKET, SO_LINGER, (char *) &ld, sizeof(ld));
  if (Result)
  {
    LogBuf.Format("%s", strerror(errno));
    LogBuf = "Communication: setsockopt SO_LINGER: " + LogBuf;
    pLog->LogIt(LogBuf);
    exit (-1);
  }
  // Initialize sockaddr structure (pg 53)
  memset((char *)&sa, 0, sizeof(sa));
  sa.sin_family       = AF_INET;
  sa.sin_addr.s_addr  = INADDR_ANY;
  sa.sin_port         = htons(port);
  // Associate a local address with the socket (pg 56)
  Result = bind(ListenSocket, (struct sockaddr *)&sa, sizeof(sa));
  if (Result)
  {
    LogBuf.Format("%s", strerror(errno));
    LogBuf = "Communication: bind: " + LogBuf;
    pLog->LogIt(LogBuf);
    exit(-1);
  }
  // Make socket nonblocking (pg 286)
  Result = ioctlsocket(ListenSocket, FIONBIO, &FionbioParm);
  if (Result)
  {
    LogBuf.Format("%s", strerror(errno));
    LogBuf = "Communication: ioctlsocket: " + LogBuf;
    pLog->LogIt(LogBuf);
    exit(-1);
  }
  // Listen on port and limit pending connections (pg 60)
  Result = listen(ListenSocket, 20);
  if (Result)
  {
    LogBuf.Format("%s", strerror(errno));
    LogBuf = "Communication: listen: " + LogBuf;
    pLog->LogIt(LogBuf);
    ::closesocket(ListenSocket);
    exit(-1);
  }
  LogBuf.Format("%d", port);
  LogBuf = "Listening on port " + LogBuf;
  pLog->LogIt(LogBuf);
  FdList  = new Descriptor();
  pHelp   = new Help();
  pRoom   = new Room();
  pSocial = new Social();
}

/************************************************************
 * Communication class destructor                           *
 ************************************************************/

Communication::~Communication()
{
  ::closesocket(ListenSocket);
  exit(-1);
}

/************************************************************
 * Check for new connections                                *
 ************************************************************/

void Communication::CheckForNewConnections()
{
  CString LogBuf;
  Dnode   *pDnode1;
  int     SocketCount;
  static  struct timeval TimeOut;

  if (StateConnections && Dnode::GetCount() == 1)
  {
    pLog->LogIt("No Connections");
    StateConnections = false;
  }
  TimeOut.tv_sec = 0;
  TimeOut.tv_usec = 1;
  FD_ZERO(&InpSet);
  FD_ZERO(&OutSet);
  FD_ZERO(&ExcSet);
  FD_SET(ListenSocket, &InpSet);
  FdList->SetpDnodeCursorFirst();
  while (!FdList->EndOfDnodeList())
  {
    pDnode1 = FdList->GetDnode();
    FD_SET(pDnode1->DnodeFd, &InpSet);
    FD_SET(pDnode1->DnodeFd, &OutSet);
    FD_SET(pDnode1->DnodeFd, &ExcSet);
    FdList->SetpDnodeCursorNext();
  }
  SocketCount = select(-1, &InpSet, &OutSet, &ExcSet, &TimeOut);
  if (SocketCount == -1)
  {
    LogBuf.Format("%s", strerror(errno));
    LogBuf = "Communication::CheckForNewConnections: select: " + LogBuf;
    pLog->LogIt(LogBuf);
    exit(-1);
  }
  if (FD_ISSET(ListenSocket, &InpSet))
  {
    NewConnection(ListenSocket);
  }
  return;
}

/************************************************************
 * New connection                                           *
 ************************************************************/

void Communication::NewConnection(int ListenSocket)
{
  CString LogBuf;
  Dnode   *pDnode1;
  int     SocketHandle;
  int     Result;
  int     SockSize;
  struct  sockaddr_in Sock;
  unsigned long FionbioParm;
    
  FionbioParm = 1;
  SockSize    = sizeof(Sock);
  // Return a new socket for a newly created connection (pg 63)
  SocketHandle = accept(ListenSocket, (struct sockaddr *)&Sock, &SockSize);
  if (!SocketHandle)
  {
    LogBuf.Format("%s", strerror(errno));
    LogBuf = "Communication::CheckForNewConnections: accept: " + LogBuf;
    pLog->LogIt(LogBuf);
    exit (-1);
  }
  // Make socket nonblocking (pg 286)
  Result = ioctlsocket(ListenSocket, FIONBIO, &FionbioParm);
  if (Result)
  {
    LogBuf.Format("%s", strerror(errno));
    LogBuf = "NewConnection: ioctlsocket: " + LogBuf;
    pLog->LogIt(LogBuf);
    exit(-1);
  }
  LogBuf.Format("%d", SocketHandle);
  LogBuf = "New connection on descriptor " + LogBuf;
  pLog->LogIt(LogBuf);
  pDnode1 = new Dnode(SocketHandle);
  FdList->AppendIt(pDnode1);
  StateConnections = true;
  return;
}

/************************************************************
 * Read message                                             *
 ************************************************************/

void Communication::RecvIt()
{
  int     nRead;
  int     DnodeFdSave;
  char    InpStr[MAX_INPUT_LENGTH];
  char    buffout[4096];
  CString CmdStr;
  CString LogBuf;
  LPTSTR  lpsz;
  Dnode   *pDnode1;

  FdList->SetpDnodeCursorFirst();
  while (!FdList->EndOfDnodeList())
  { // Loop thru all connections
    pDnode1 = FdList->GetDnode();
    if (FD_ISSET(pDnode1->DnodeFd, &OutSet))
    {
      int dummy = 0;
    }
    if (FD_ISSET(pDnode1->DnodeFd, &ExcSet))
    { // Kick out connections with exceptions
      pDnode1->PlayerStateBye = true;
    }
    else
    { // Good connection
      if (FD_ISSET(pDnode1->DnodeFd, &InpSet))
      { // Receive
        memset(InpStr, '\0', sizeof(InpStr));
        nRead = ::recv(pDnode1->DnodeFd, InpStr, MAX_INPUT_LENGTH-1, 0);
        if (nRead == 0)
        { // Should be input but there is none -- disconnected ??
          pDnode1->PlayerStateBye = true;
        }
        if (WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError() == WSAEINTR)
        {
         nRead = 0;
        }
        if (nRead > 0)
        { // Got something ... process it
          CmdStr = InpStr;
          pDnode1->PlayerInp += CmdStr;
          ProcessCommands(pDnode1);
        }
      }
    }
    if (pDnode1->PlayerStateSendBanner)
    { // New connection ... Send welcome
      pDnode1->PlayerStateSendBanner = false;
      pDnode1->PlayerStateLoggingOn = true;
      pDnode1->PlayerStateWaitNewCharacter = true;
      pDnode1->PlayerOut += "Welcome to HolyQuest!\r\n\r\n";
      pDnode1->PlayerOut += "Create a new character Y-N?\r\n";
    }
    if (pDnode1->PlayerOut.GetLength() > 0)
    { // Send output to player, Replace or strip out color codes 1st
      Color(pDnode1);
      strcpy(lpsz = buffout, pDnode1->PlayerOut);
      SendIt(pDnode1, buffout);
    }
    if (pDnode1->PlayerStateBye)
    { // Player leaving the game ... disconnect them
      CloseIt(pDnode1);
      DnodeFdSave = pDnode1->DnodeFd;
      if (FdList->DeleteNode())
      { // When connection is deleted from list, log it
        LogBuf.Format("%d", DnodeFdSave);
        LogBuf = "Closed connection on descriptor " + LogBuf;
        pLog->LogIt(LogBuf);
        if (Dnode::GetCount() == 0)
        { // Connection count is zero
          LogBuf = "No connections";
          pLog->LogIt(LogBuf);
        }
      }
    }
    FdList->SetpDnodeCursorNext();
  }
}

/************************************************************
 * Process commands                                         *
 ************************************************************/

void Communication::ProcessCommands(Dnode *pDnode1)
{
  int x;

  x = pDnode1->PlayerInp.FindOneOf("\r\n");
  while (x > 0)
  {
    ParseIt(pDnode1);
    if (StateRunning)
    {
      x = pDnode1->PlayerInp.FindOneOf("\r\n");
    }
    else
    {
      x = -1;
    }
  }
}

/************************************************************
 * Send message                                             *
 ************************************************************/

void Communication::SendIt(Dnode *pDnode1, char *arg)
{
  int Length;
  int Written;

  if (!arg || arg[0] == '\0')
  {
    return;
  }
  Length = strlen(arg);
  Written = ::send(pDnode1->DnodeFd, arg, Length, 0);
  if (Written == Length)
  {
    pDnode1->PlayerOut = "";
  }
  else
  {
    pDnode1->PlayerOut = pDnode1->PlayerOut.Right(Length-Written);
  }
}

/************************************************************
 * Close a socket                                           *
 ************************************************************/

void Communication::CloseIt(Dnode *pDnode1)
{
  ::closesocket(pDnode1->DnodeFd);
}

/************************************************************
 * Logon                                                    *
 ************************************************************/

void Communication::DoLogon(Dnode *pDnode1, CString CmdStr)
{
  CString LogBuf;

  if (pDnode1->PlayerStateWaitNewCharacter)
  {
    pDnode1->PlayerStateWaitNewCharacter = false;
    LogonWaitNewCharacter(pDnode1, CmdStr);
    return;
  }
  if (pDnode1->PlayerStateWaitName)
  {
    pDnode1->PlayerStateWaitName = false;
    LogonWaitName(pDnode1, CmdStr);
    return;
  }
  if (pDnode1->PlayerStateWaitNameConfirmation)
  {
    pDnode1->PlayerStateWaitNameConfirmation = false;
    LogonWaitNameConfirmation(pDnode1, CmdStr);
    return;
  }
  if (pDnode1->PlayerStateWaitPassword)
  {
    pDnode1->PlayerStateWaitPassword = false;
    LogonWaitPassword(pDnode1, CmdStr); 
    return;
  }
  if (pDnode1->PlayerStateWaitMaleFemale)
  {
    pDnode1->PlayerStateWaitMaleFemale = false;
    LogonWaitMaleFemale(pDnode1, CmdStr);
    return;
  }
}

/************************************************************
 * Logon wait new character                                 *
 ************************************************************/

void Communication::LogonWaitNewCharacter(Dnode *pDnode1, CString CmdStr)
{
  CmdStr.MakeUpper();
  if (!(CmdStr.FindOneOf("YN") == 0 && CmdStr.GetLength() == 1))
  { // Not Y or N ... try again
    pDnode1->PlayerStateWaitNewCharacter = true;
    pDnode1->PlayerOut += "\r\nYou must enter a Y or N\r\n";
    pDnode1->PlayerOut += "Create a new character Y-N?\r\n";
  }
  else
  { // Y or N entered ... store response for use later
    pDnode1->PlayerStateWaitName = true;
    pDnode1->PlayerNewCharacter = CmdStr;
    pDnode1->PlayerOut += "\r\nName:\r\n";
  }
}

/************************************************************
 * Logon wait name                                          *
 ************************************************************/

void Communication::LogonWaitName(Dnode *pDnode1, CString CmdStr)
{
  Dnode *pDnode2;

  pDnode1->PlayerName = CmdStr;
  if (pDnode1->PlayerNewCharacter == "Y")
  { // New player
    pDnode1->PlayerStateWaitNameConfirmation = true;
    pDnode1->PlayerOut += "\r\nYou wish to be known as ";
    pDnode1->PlayerOut += pDnode1->PlayerName;
    pDnode1->PlayerOut += "? Y-N\r\n";
  }
  else
  { // Returning player
    if (Player::IsNew(pDnode1->PlayerName))
    { // Name not found on file
      pDnode1->PlayerStateWaitName = true;
      pDnode1->PlayerOut += pDnode1->PlayerName;
      pDnode1->PlayerOut += " is not a citizen of this realm\r\n";
      pDnode1->PlayerOut += "\r\nName:\r\n";
    }
    else
    { // Name found on file
      pDnode1->PlayerStateWaitPassword = true;
      FdList->SetpDnodeCursorFirst();
      while (!FdList->EndOfDnodeList())
      { // Prevent same player logging on more than once
        pDnode2 = FdList->GetDnode();
        if (pDnode1 != pDnode2)
        {
          if (pDnode2->pPlayer->Name == pDnode1->PlayerName)
          { // Kick em back to name prompt
            pDnode1->PlayerStateWaitPassword = false;
            pDnode1->PlayerStateWaitName = true;
            pDnode1->PlayerOut +=  pDnode1->PlayerName;
            pDnode1->PlayerOut += " is already logged on.\r\n ";
            pDnode1->PlayerOut += "\r\nName:\r\n";
          }
        }
        FdList->SetpDnodeCursorNext();
      }
      if (pDnode1->PlayerStateWaitPassword)
      { // Prompt for password
        pDnode1->PlayerOut += "\r\nPassword:\r\n";
        pDnode1->pPlayer = new Player(); 
        pDnode1->pPlayer->Name = pDnode1->PlayerName;
        pDnode1->pPlayer->ParseStuff(pDnode1->PlayerName);
        pDnode1->PlayerPassword = pDnode1->pPlayer->Password;
      }
    }
  }
}

/************************************************************
 * Logon wait name confirmation                             *
 ************************************************************/

void Communication::LogonWaitNameConfirmation(Dnode *pDnode1, CString CmdStr)
{
  CmdStr.MakeUpper();
  if (!(CmdStr.FindOneOf("YN") == 0 && CmdStr.GetLength() == 1))
  { // Not Y or N ... try again
    pDnode1->PlayerStateWaitNameConfirmation = true;
    pDnode1->PlayerOut += "\r\nYou must enter a Y or N\r\n";
    pDnode1->PlayerOut += "\r\nYou wish to be known as ";
    pDnode1->PlayerOut += pDnode1->PlayerName;
    pDnode1->PlayerOut += "? Y-N\r\n";
  }
  else
  { // Y or N entered
    if (CmdStr == "N")
    { // N ... changed their mind ... try again
      pDnode1->PlayerStateWaitName = true;
      pDnode1->PlayerOut += "\r\nOk then, Try again.\r\n";
      pDnode1->PlayerOut += "\r\nName:\r\n";
    }
    else
    { // Y entered ... they like the name
      if (!Player::ValidName(pDnode1->PlayerName))
      { // Name is invalid ... try again
        pDnode1->PlayerStateWaitName = true;
        pDnode1->PlayerOut += pDnode1->PlayerName;
        pDnode1->PlayerOut += " is not an acceptable name in this realm\r\n";
        pDnode1->PlayerOut += "\r\nName:\r\n";
      }
      else
      { // Name is valid
        if (!Player::IsNew(pDnode1->PlayerName))
        { // Name aleady used ... try again
          pDnode1->PlayerStateWaitName = true;
          pDnode1->PlayerOut += pDnode1->PlayerName;
          pDnode1->PlayerOut += " belongs to an exiting character\r\n";
          pDnode1->PlayerOut += "Try a different name\r\n";
          pDnode1->PlayerOut += "\r\nName:\r\n";
        }
        else
        { // New player with valid name that has not been used
          pDnode1->PlayerStateWaitPassword = true;
          pDnode1->PlayerOut += "\r\nPassword:\r\n";
        }
      }
    }
  }
}

/************************************************************
 * Logon wait password                                      *
 ************************************************************/

void Communication::LogonWaitPassword(Dnode *pDnode1, CString CmdStr)
{
  Dnode   *pDnode2;
  CString LogBuf;

  if (pDnode1->PlayerPassword == CmdStr)
  { // Password matches
    FdList->SetpDnodeCursorFirst();
    while (!FdList->EndOfDnodeList())
    {
      pDnode2 = FdList->GetDnode();
      if (pDnode1 != pDnode2)
      { // This connection is not the one logging on
        if (pDnode2->PlayerStatePlaying)
        { // Ok to send stuff to this connection 
          pDnode2->PlayerOut += "\r\n";
          pDnode2->PlayerOut +=  pDnode1->PlayerName;
          pDnode2->PlayerOut += " has entered the game.\r\n";
          pDnode2->pPlayer->CreatePrompt();
          pDnode2->PlayerOut += pDnode2->pPlayer->GetOutput();
        }
      }
      FdList->SetpDnodeCursorNext();
    }
    if (pDnode1->PlayerNewCharacter == "Y")
    { // Password matches and a new player, save them
      pDnode1->PlayerStateWaitMaleFemale = true;
      pDnode1->pPlayer = new Player();
      pDnode1->pPlayer->Name = pDnode1->PlayerName;
      pDnode1->pPlayer->Password = pDnode1->PlayerPassword;
      pDnode1->pPlayer->Save();
      pDnode1->PlayerOut += "\r\n";
      pDnode1->PlayerOut +=  pDnode1->PlayerName;
      pDnode1->PlayerOut += ", remember your password.\r\n";
      pDnode1->PlayerOut += "You must know it log in again.\r\n";
      pDnode1->PlayerOut += "\r\nSex of this character M-F?\r\n";
    }
    else
    { // Password matches and returning player, let them play
      pDnode1->PlayerStateLoggingOn = false;
      pDnode1->PlayerStatePlaying = true;
      pDnode1->PlayerOut += "\r\nMay your travels be safe.\r\n\r\n";
      pRoom->ShowRoom(pDnode1);
    }
  }
  else
  { // Password does not match
    if (pDnode1->PlayerNewCharacter == "Y")
    { // New player
      pDnode1->PlayerWrongPasswordCount++;
      pDnode1->PlayerStateWaitPassword = true;
      if (pDnode1->PlayerWrongPasswordCount == 1)
      { // First password entered
        pDnode1->PlayerPassword = CmdStr;
        pDnode1->PlayerOut += "\r\nRetype Password:\r\n";
      }
      else
      {
        if (pDnode1->PlayerWrongPasswordCount < 4)
        { // Can't seem to type the same password ... doh!
          pDnode1->PlayerOut += "\r\nRetyped Password does not match\r\n";
          pDnode1->PlayerOut += "Try again\r\n";
          pDnode1->PlayerOut += "\r\nPassword:\r\n";
        }
        else
        { // Mis-matched password entered 3 times, boot them off!
          pDnode1->PlayerStateBye = true;
          pDnode1->PlayerOut += "\r\nWrong password entered 3 times\r\n";
          pDnode1->PlayerOut += "\r\nYou have been disconnected\r\n";
        }
      }
    }
    else
    { // Returning player
      pDnode1->PlayerWrongPasswordCount++;
      if (pDnode1->PlayerWrongPasswordCount < 3)
      { // Tries < 3
        pDnode1->PlayerStateWaitPassword = true;
        pDnode1->PlayerOut += "\r\nWrong password\r\n";
        pDnode1->PlayerOut += "\r\nPassword:\r\n";
      }
      else
      { // Wrong password entered 3 times, log it, boot them off!
        pDnode1->PlayerStateBye = true;
        pDnode1->PlayerOut += "\r\nWrong password entered 3 times\r\n";
        pDnode1->PlayerOut += "\r\nYou have been disconnected\r\n";
        LogBuf = "Password failure for ";
        LogBuf += pDnode1->PlayerName;
        pLog->LogIt(LogBuf);
      }
    }
  }
}

/************************************************************
 * Logon wait male female                                   *
 ************************************************************/

void Communication::LogonWaitMaleFemale(Dnode *pDnode1, CString CmdStr)
{
  CmdStr.MakeUpper();
  if (!(CmdStr.FindOneOf("MF") == 0 && CmdStr.GetLength() == 1))
  { // Not M or F
    pDnode1->PlayerStateWaitMaleFemale = true;
    pDnode1->PlayerOut += "You must enter a M or F\r\n";
    pDnode1->PlayerOut += "\r\nSex of this character M-F?\r\n";
  }
  else
  { // M or F entered, save them, let them play
    pDnode1->PlayerStateLoggingOn = false;
    pDnode1->PlayerStatePlaying = true;
    pDnode1->pPlayer->Sex = CmdStr;
    pDnode1->pPlayer->Save();
    pDnode1->PlayerOut += "\r\nMay your travels be safe.\r\n\r\n";
    pRoom->ShowRoom(pDnode1);
  }
}

/************************************************************
 * Parse command                                            *
 ************************************************************/

void Communication::ParseIt(Dnode *pDnode1)
{
  int     x;
  int     y;
  CString CmdStr;
  CString MudCmd;

  CmdStr = pDnode1->PlayerInp;
  y = CmdStr.GetLength();
  x = CmdStr.FindOneOf("\r\n");
  if (x <0)
  {
    return;
  }
  CmdStr = CmdStr.Left(x);
  pDnode1->PlayerInp = pDnode1->PlayerInp.Right(y-x);
  pDnode1->PlayerInp.TrimLeft();

/* Player logon */
  if (pDnode1->PlayerStateLoggingOn)
  {
    DoLogon(pDnode1, CmdStr);
    return;
  }

  MudCmd = GetWord(CmdStr, 1);
  MudCmd.MakeLower();

/* COLOR command */
  if (MudCmd == "color")
  {
    DoColor(pDnode1, CmdStr);
    return;
  }

/* FOLLOW command */
  if (MudCmd == "follow")
  {
    DoFollow(pDnode1, CmdStr);
    return;
  }

/* GO command */
  if (MudCmd == "go")
  {
    DoGo(pDnode1, CmdStr);
    return;
  }

/* GROUP command */
  if (MudCmd == "group")
  {
    DoGroup(pDnode1, CmdStr);
    return;
  }

/* GSAY command */
  if (MudCmd == "gsay")
  {
    DoGsay(pDnode1, CmdStr);
    return;
  }

/* HELP command */
  if (MudCmd == "help")
  {
    DoHelp(pDnode1, CmdStr);
    return;
  }

/* LOOK command */
  if (MudCmd == "look")
  {
    DoLook(pDnode1, CmdStr);
    return;
  }

/* MONEY command */
  if (MudCmd == "money")
  {
    DoMoney(pDnode1);
    return;
  }

/* QUIT command */
  if (MudCmd == "quit")
  {
    DoQuit(pDnode1);
    return;
  }

/* SAY command */
  if (MudCmd == "say")
  {
    DoSay(pDnode1, CmdStr);
    return;
  }

/* SIT command */
  if (MudCmd == "sit")
  {
    DoSit(pDnode1);
    return;
  }

/* STAND command */
  if (MudCmd == "stand")
  {
    DoStand(pDnode1);
    return;
  }

/* STATUS command */
  if (MudCmd == "status")
  {
    DoStatus(pDnode1);
    return;
  }

  /* STOP command */
  if (MudCmd == "stop")
  {
    DoStop();
    return;
  }

/* TELL command */
  if (MudCmd == "tell")
  {
    DoTell(pDnode1, CmdStr);
    return;
  }

/* TITLE command */
  if (MudCmd == "title")
  {
    DoTitle(pDnode1, CmdStr);
    return;
  }

/* WHO command */
  if (MudCmd == "who")
  {
    DoWho(pDnode1);
    return;
  }

/* GO command, implicit */
  if (pRoom->IsExit(pDnode1, CmdStr, "gogo"))
  {
    return;
  }

/* SOCIAL command */
  if (pSocial->IsSocial(pDnode1, CmdStr, MudCmd))
  {
    return;
  }

/* Bad command */
  pDnode1->PlayerOut += "Your command is not clear.\r\n";
  pDnode1->pPlayer->CreatePrompt();
  pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
}


/***********************************************************
* Color command                                            *
************************************************************/

void Communication::DoColor(Dnode *pDnode1, CString CmdStr)
{
  CString TmpStr;

  TmpStr = GetWord(CmdStr, 2);
  TmpStr.MakeLower();
  if (TmpStr == "on")
  {
    pDnode1->pPlayer->Color = true;
    pDnode1->PlayerOut += "You will now see &RP&Gr&Ye&Bt&Mt&Cy&N &RC&Go&Yl&Bo&Mr&Cs&N.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  if (TmpStr == "off")
  {
    pDnode1->pPlayer->Color = false;
    pDnode1->PlayerOut += "Color is &Moff&N.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  if (pDnode1->pPlayer->Color)
  {
    pDnode1->PlayerOut += "&CColor&N is &Mon&N.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
  }
  else
  {
    pDnode1->PlayerOut += "Color is off.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
  }
}

/************************************************************
 * Follow command                                           *
 ************************************************************/

void Communication::DoFollow(Dnode *pDnode1, CString CmdStr)
{
  Dnode   *pDnode2; // Group leader
  Dnode   *pDnode3; // Other group members
  Dnode   *pDnode4; // Target of follow command
  int     i;
  CString Target;
  bool    TargetInGroup;
  CString TmpStr;

  Target = GetWord(CmdStr, 2);
  TmpStr = Target;
  TmpStr.MakeLower();
  //*************
  //* No target *
  //*************
  if (Target == "")
  { // Follow with no target
    pDnode1->PlayerOut += "Who would you like to follow?\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  //******************
  //* Stop following *
  //******************
  if (TmpStr == "none")
  { // Player wants to stop following
    if (pDnode1->pPlayer->pPlayerFollowers[0] == NULL)
    { // Player is not following anyone
      pDnode1->PlayerOut += "Ok. But alas, you were not following anyone";
      pDnode1->PlayerOut += "\r\n";
      pDnode1->pPlayer->CreatePrompt();
      pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
      return;
    }
    for (i=1;i<GRP_LIMIT;i++)
    {
      if (pDnode1->pPlayer == pDnode1->pPlayer->pPlayerFollowers[0]->pPlayerFollowers[i])
      {
        pDnode1->pPlayer->pPlayerFollowers[0]->pPlayerFollowers[i] = NULL;
      }
    }
    pDnode1->pPlayer->pPlayerFollowers[0] = NULL;
    pDnode1->PlayerOut += "You must now find your own way.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  //******************
  //* List followers *
  //******************
  if (TmpStr == "list")
  {
    if (pDnode1->pPlayer->pPlayerFollowers[0] == NULL)
    {
      pDnode1->PlayerOut += "You are not following anyone.\r\n";
    }
    else
    {
      pDnode1->PlayerOut += "You are following ";
      pDnode1->PlayerOut += pDnode1->pPlayer->pPlayerFollowers[0]->Name;
      pDnode1->PlayerOut += ".\r\n";
    }
    for (i=1;i<GRP_LIMIT;i++)
    {
      if (pDnode1->pPlayer->pPlayerFollowers[i] != NULL)
      {
        if (i == 1)
        {
          pDnode1->PlayerOut += "Followers\r\n";
          pDnode1->PlayerOut += "---------\r\n";
        }
        pDnode1->PlayerOut += pDnode1->pPlayer->pPlayerFollowers[i]->Name;
        pDnode1->PlayerOut += "\r\n";
      }
    }
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  //*******************************
  //* Player is already following *
  //*******************************
  if (pDnode1->pPlayer->pPlayerFollowers[0] != NULL)
  {
    pDnode1->PlayerOut += "You are already following ";
    pDnode1->PlayerOut += pDnode1->pPlayer->pPlayerFollowers[0]->Name;
    pDnode1->PlayerOut += ".\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  //******************
  //* Target online? *
  //******************
  pDnode4 = GetTargetDnode(Target);
  if (!pDnode4)
  { // Target not online
    pDnode1->PlayerOut += Target;
    pDnode1->PlayerOut += " is not online.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  //***************************
  //* Can not follow yourself *
  //***************************
  if (pDnode1 == pDnode4)
  { // Player trying to follow themself
    pDnode1->PlayerOut += "You can not follow yourself.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  //*************************************
  //* Player and target must be grouped *
  //*************************************
  if (pDnode1->pPlayer->pPlayerGrpMember[0] == NULL)
  { // Player is not in a group
    pDnode1->PlayerOut += "You must be in a group before you can follow.";
    pDnode1->PlayerOut += "\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  pDnode2 = GetTargetDnode(pDnode1->pPlayer->pPlayerGrpMember[0]->Name);
  TargetInGroup = false;
  for (i=0;i<GRP_LIMIT;i++)
  { // For each member of leader's group including the leader
    if (pDnode2->pPlayer->pPlayerGrpMember[i] == NULL)
    { // Done looping through group members
      break;
    }
    // Get group member's dnode
    pDnode3 = GetTargetDnode(pDnode2->pPlayer->pPlayerGrpMember[i]->Name);
    if (pDnode3 == pDnode4)
    { // Target member found
      TargetInGroup = true;
    }
  }
  if (!TargetInGroup)
  { // Target is not grouped with player
    pDnode1->PlayerOut += Target;
    pDnode1->PlayerOut += " is not in the group.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  //************************************
  //* Player can not follow a follower *
  //************************************
  for (i=1;i<GRP_LIMIT;i++)
  {
    if (pDnode4->pPlayer == pDnode1->pPlayer->pPlayerFollowers[i])
    {
      pDnode1->PlayerOut += "Can not! ";
      pDnode1->PlayerOut += pDnode4->pPlayer->Name;
      pDnode1->PlayerOut += " is following YOU.\r\n";
      pDnode1->pPlayer->CreatePrompt();
      pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
      return;
    }
  }
  //***********************
  //* Follow - make it so *
  //***********************
  // Message to player
  pDnode1->PlayerOut += "You begin to follow ";
  pDnode1->PlayerOut +=  pDnode4->PlayerName;
  pDnode1->PlayerOut += ".\r\n";
  pDnode1->pPlayer->CreatePrompt();
  pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
  // Message to player being followed
  pDnode4->PlayerOut += "\r\n";
  pDnode4->PlayerOut +=  pDnode1->PlayerName;
  pDnode4->PlayerOut += " begins to follow you.";
  pDnode4->PlayerOut += "\r\n";
  pDnode4->pPlayer->CreatePrompt();
  pDnode4->PlayerOut += pDnode1->pPlayer->GetOutput();
  // Make 'em follow
  pDnode1->pPlayer->pPlayerFollowers[0] = pDnode4->pPlayer;
  for (i=1;i<GRP_LIMIT;i++)
  { // Loop through target's list of followers to find an empty slot
    if (pDnode4->pPlayer->pPlayerFollowers[i] == NULL)
    { // Found a slot for the new follower
      break;
    }
  }
  pDnode4->pPlayer->pPlayerFollowers[i] = pDnode1->pPlayer;
}

/************************************************************
 * Go command                                              *
 ************************************************************/

void Communication::DoGo(Dnode *pDnode1, CString CmdStr)
{
  CString TmpStr;

  TmpStr = GetWord(CmdStr, 2);
  if (TmpStr == "")
  {
    pDnode1->PlayerOut += "Aimless wandering is not allowed.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  if (pRoom->IsExit(pDnode1, CmdStr, "go"))
  {
    return;
  }
  pDnode1->PlayerOut += "Go where?\r\n";
  pDnode1->pPlayer->CreatePrompt();
  pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
}

/************************************************************
 * Group command                                            *
 ************************************************************/

void Communication::DoGroup(Dnode *pDnode1, CString CmdStr)
{
  Dnode   *pDnode2; // Group leader
  int     i;
  int     j;
  bool    GrpFull;
  CString PlayerNameChk;
  CString TargetNameChk;
  CString TargetNameSav;
  CString TmpStr;

  PlayerNameChk     = pDnode1->PlayerName;
  TargetNameChk     = GetWord(CmdStr, 2);
  TargetNameSav     = TargetNameChk;
  PlayerNameChk.MakeLower();
  TargetNameChk.MakeLower();
  //************************
  //* Group with no target *
  //************************
  if (TargetNameChk.GetLength() < 1)
  { // Group command by itself, show group status
    if (pDnode1->pPlayer->pPlayerGrpMember[0] == NULL)
    { // Player is not in a group
      pDnode1->PlayerOut += "You are not in a group.\r\n";
      pDnode1->pPlayer->CreatePrompt();
      pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
      return;
    }
    else
    { // Player is in a group, show members
      pDnode1->PlayerOut += pDnode1->pPlayer->pPlayerGrpMember[0]->Name;
      pDnode1->PlayerOut += " \r\n";
      j = pDnode1->pPlayer->pPlayerGrpMember[0]->Name.GetLength();
      for (i=1;i<j+1;i++)
      {
        pDnode1->PlayerOut += "-";
      }
      for (i=1;i<GRP_LIMIT;i++)
      { // List group members
        if (pDnode1->pPlayer->pPlayerGrpMember[0]->pPlayerGrpMember[i] != NULL)
        {
          pDnode1->PlayerOut += " \r\n";
          pDnode1->PlayerOut += pDnode1->pPlayer->pPlayerGrpMember[0]->pPlayerGrpMember[i]->Name;
        }
      }
      pDnode1->PlayerOut += " \r\n";
      pDnode1->pPlayer->CreatePrompt();
      pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
      return;
    }
  }
  //***********************
  //* Turning grouping on *
  //***********************
  if (TargetNameChk == "on")
  {
    pDnode1->pPlayer->Group = true;
    pDnode1->PlayerOut += "You are now accepting requests to group.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  //************************
  //* Turning grouping off *
  //************************
  if (TargetNameChk == "off")
  {
    pDnode1->pPlayer->Group = false;
    pDnode1->PlayerOut += "You are now rejecting requests to group.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  //****************************************************
  //* Target is 'none' ... Player is leaving the group *
  //****************************************************
  if (TargetNameChk == "none")
  { // Player is disbanding
    if (pDnode1->pPlayer->pPlayerGrpMember[0] == NULL)
    { // Player is not in a group
      pDnode1->PlayerOut += "You are not in a group.\r\n";
      pDnode1->pPlayer->CreatePrompt();
      pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
      return;
    }
    GrpLeave(pDnode1);
    return;
  }
  //*********************************************************
  //* Trying to create a new group or trying to add members *
  //*********************************************************
  if (TargetNameChk == PlayerNameChk)
  { // Trying to group with self
    if (pDnode1->pPlayer->pPlayerGrpMember[0] == NULL)
    { // Player is not in a group
      pDnode1->PlayerOut += "One is a lonely number, try grouping with another player.\r\n";
      pDnode1->pPlayer->CreatePrompt();
      pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
      return;
    }
    else
    { // Player is in a group
      pDnode1->PlayerOut += "One is a lonely number, but wait, you are already in group.\r\n";
      pDnode1->pPlayer->CreatePrompt();
      pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
      return;
    }
  }
  pDnode2 = GetTargetDnode(TargetNameChk);
  if (!pDnode2)
  { // New group member ... not online
    pDnode1->PlayerOut += TargetNameSav;
    pDnode1->PlayerOut += " is not online.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  if (pDnode1->pPlayer->pPlayerGrpMember[0] == pDnode2->pPlayer)
  { // Player is trying to group with their group's leader
    pDnode1->PlayerOut += "You are already grouped with ";
    pDnode1->PlayerOut += pDnode2->PlayerName;
    pDnode1->PlayerOut += ", who is the leader.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  if (pDnode1->pPlayer->RoomId != pDnode2->pPlayer->RoomId)
  { // New group member is not in same room as leader
    pDnode1->PlayerOut += pDnode2->PlayerName;
    pDnode1->PlayerOut += " is not here.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  if (pDnode1->pPlayer->pPlayerGrpMember[0] != NULL)
  { // Player is in a group
    if (pDnode1->pPlayer != pDnode1->pPlayer->pPlayerGrpMember[0])
    { // But is not the leader, only leader may add members
      pDnode1->PlayerOut += "You are not the leader. Leader must add members.\r\n";
      pDnode1->pPlayer->CreatePrompt();
      pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
      return;
    }
  }
  if (pDnode2->pPlayer->pPlayerGrpMember[0] != NULL)
  { // New group member already in a group
    pDnode1->PlayerOut += pDnode2->PlayerName;
    pDnode1->PlayerOut += " is already in a group.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  GrpFull = true;
  for (i=0;i<GRP_LIMIT;i++)
  { // Is group full 
    if (pDnode1->pPlayer->pPlayerGrpMember[i] == NULL)
    { // Found an empty member slot
      j = i;
      GrpFull = false;
      break;
    }
  }
  if (GrpFull)
  { // Group is full
    pDnode1->PlayerOut += "Your group is full, maximum of ";
    TmpStr.Format("%d", GRP_LIMIT);
    TmpStr.TrimLeft();
    TmpStr.TrimRight();
    pDnode1->PlayerOut+= TmpStr;
    pDnode1->PlayerOut += " members allowed.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  if (!pDnode2->pPlayer->Group)
  { // New group member is not accepting group requests
    pDnode1->PlayerOut += pDnode2->PlayerName;
    pDnode1->PlayerOut += " is not accepting requests to group.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  //***********************************
  //* Ok, done checking ... group 'em *
  //***********************************
  if (pDnode1->pPlayer->pPlayerGrpMember[0] == NULL)
  { // Forming new group
    pDnode1->pPlayer->pPlayerGrpMember[0] = pDnode1->pPlayer;
    pDnode2->pPlayer->pPlayerGrpMember[0] = pDnode1->pPlayer;
    pDnode1->pPlayer->pPlayerGrpMember[1] = pDnode2->pPlayer;
    pDnode1->PlayerOut += "You have formed a new group with ";
    pDnode1->PlayerOut += pDnode2->PlayerName;
    pDnode1->PlayerOut += " as your first member.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
  }
  else
  { // Adding a member to an existing group
    pDnode1->pPlayer->pPlayerGrpMember[i] = pDnode2->pPlayer;
    pDnode2->pPlayer->pPlayerGrpMember[0] = pDnode1->pPlayer;
    pDnode1->PlayerOut += pDnode2->PlayerName;
    pDnode1->PlayerOut += " has been added to your group.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
  }
  pDnode2->PlayerOut += "\r\n";
  pDnode2->PlayerOut += "You have joined a group, ";
  pDnode2->PlayerOut += pDnode1->PlayerName;
  pDnode2->PlayerOut += " is the leader.\r\n";
  pDnode2->pPlayer->CreatePrompt();
  pDnode2->PlayerOut += pDnode1->pPlayer->GetOutput();
}

/************************************************************
 * Gsay command                                             *
 ************************************************************/

void Communication::DoGsay(Dnode *pDnode1, CString CmdStr)
{
  Dnode   *pDnode2; // Group leader
  Dnode   *pDnode3; // Other group members
  CString GsayMsg;
  int     i;

  if (pDnode1->pPlayer->pPlayerGrpMember[0] == NULL)
  { // Player is not in a group
    pDnode1->PlayerOut += "You are not in a group.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  GsayMsg = GetWords(CmdStr, 2);
  if (GsayMsg.GetLength() < 1)
  { // Player typed gsay but did not type a message
    pDnode1->PlayerOut += "Are you try to say something to the group?\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  // Send the gsay
  pDnode1->PlayerOut += "You say to the group: ";
  pDnode1->PlayerOut += "&C";
  pDnode1->PlayerOut += GsayMsg;
  pDnode1->PlayerOut += "&N";
  pDnode1->PlayerOut += "\r\n";
  pDnode1->pPlayer->CreatePrompt();
  pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
  // Get group leader
  pDnode2 = GetTargetDnode(pDnode1->pPlayer->pPlayerGrpMember[0]->Name);
  for (i=0;i<GRP_LIMIT;i++)
  { // For each member of leader's group including the leader
    if (pDnode2->pPlayer->pPlayerGrpMember[i] == NULL)
    { // Done looping through group members
      return;
    }
    // Send gsay to other group members
    pDnode3 = GetTargetDnode(pDnode2->pPlayer->pPlayerGrpMember[i]->Name);
    if (pDnode1 == pDnode3)
    { // Do not send gsay to originating player
      continue;
    }
    pDnode3->PlayerOut += "\r\n";
    pDnode3->PlayerOut += pDnode1->PlayerName;
    pDnode3->PlayerOut += " says to the group: ";
    pDnode3->PlayerOut += "&C";
    pDnode3->PlayerOut += GsayMsg;
    pDnode3->PlayerOut += "&N";
    pDnode3->PlayerOut += "\r\n";
    pDnode3->pPlayer->CreatePrompt();
    pDnode3->PlayerOut += pDnode3->pPlayer->GetOutput();
  }
}

/************************************************************
 * Help command                                             *
 ************************************************************/

void Communication::DoHelp(Dnode *pDnode1, CString CmdStr)
{
  if (pHelp->IsHelp(pDnode1, CmdStr))
  { // Help was found and sent to player
    return;
  }
  pDnode1->PlayerOut += "No help topic found.\r\n";
  pDnode1->pPlayer->CreatePrompt();
  pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
}

/************************************************************
 * Look command                                             *
 ************************************************************/

void Communication::DoLook(Dnode *pDnode1, CString CmdStr)
{
  CString TmpStr;

  TmpStr = GetWord(CmdStr, 2);
  if (TmpStr == "")
  {
    pRoom->ShowRoom(pDnode1);
    return;
  }
  if (pRoom->IsExit(pDnode1, CmdStr, "look"))
  {
    return;
  }
  pDnode1->PlayerOut += "What you are looking for is not here.\r\n";
  pDnode1->pPlayer->CreatePrompt();
  pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
}

/************************************************************
 * Money command                                            *
 ************************************************************/

void Communication::DoMoney(Dnode *pDnode1)
{
  pDnode1->pPlayer->ShowMoney();
  pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
  pDnode1->pPlayer->CreatePrompt();
  pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
}

/************************************************************
 * Quit command                                             *
 ************************************************************/

void Communication::DoQuit(Dnode *pDnode1)
{
  Dnode *pDnode2;

  pDnode1->pPlayer->Save();
  GrpLeave(pDnode1);
  FdList->SetpDnodeCursorFirst();
  while (!FdList->EndOfDnodeList())
  {
    pDnode2 = FdList->GetDnode();
    if (pDnode1 == pDnode2)
    { // Tell player bye
      pDnode1->PlayerOut += "\r\nBye Bye!\r\n";
      pDnode1->PlayerStateBye = true;
    }
    else
    {
      if (pDnode2->PlayerStatePlaying)
      { // Tell everyone else that the player has left
        pDnode2->PlayerOut += "\r\n";
        pDnode2->PlayerOut += pDnode1->PlayerName;
        pDnode2->PlayerOut += " has left the game.\r\n";
        pDnode2->pPlayer->CreatePrompt();
        pDnode2->PlayerOut += pDnode2->pPlayer->GetOutput();
      }
    }
    FdList->SetpDnodeCursorNext();
  }
}

/************************************************************
 * Say command                                              *
 ************************************************************/

void Communication::DoSay(Dnode *pDnode1, CString CmdStr)
{
  CString SayMsg;

  SayMsg = GetWords(CmdStr, 2);
  if (SayMsg.GetLength() < 1)
  {
    pDnode1->PlayerOut += "You try to speak, but no words come out of your mouth";
    pDnode1->PlayerOut += "\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  pDnode1->PlayerOut += "You say: ";
  pDnode1->PlayerOut += SayMsg;
  pDnode1->PlayerOut += "&N";
  pDnode1->PlayerOut += "\r\n";
  pDnode1->pPlayer->CreatePrompt();
  pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
  SayMsg = pDnode1->PlayerName + " says: " + SayMsg;
  SendToRoom(pDnode1, pDnode1, SayMsg);
}

/************************************************************
 * Sit command                                              *
 ************************************************************/

void Communication::DoSit(Dnode *pDnode1)
{
  CString SitMsg;

  if (pDnode1->pPlayer->Position == "sit")
  {
    pDnode1->PlayerOut += "You are already sitting down.";
    pDnode1->PlayerOut += "\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  pDnode1->pPlayer->Position = "sit";
  pDnode1->PlayerOut += "You sit down.";
  pDnode1->PlayerOut += "\r\n";
  pDnode1->pPlayer->CreatePrompt();
  pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
  SitMsg = pDnode1->PlayerName + " sits down.";
  SendToRoom(pDnode1, pDnode1, SitMsg);
}

/************************************************************
 * Stand command                                            *
 ************************************************************/

void Communication::DoStand(Dnode *pDnode1)
{
  CString StandMsg;

  if (pDnode1->pPlayer->Position == "stand")
  {
    pDnode1->PlayerOut += "You are already standing.";
    pDnode1->PlayerOut += "\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  pDnode1->pPlayer->Position = "stand";
  pDnode1->PlayerOut += "You stand up.";
  pDnode1->PlayerOut += "\r\n";
  pDnode1->pPlayer->CreatePrompt();
  pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
  StandMsg = pDnode1->PlayerName + " stands up.";
  SendToRoom(pDnode1, pDnode1, StandMsg);
}

/************************************************************
 * Status command                                           *
 ************************************************************/

void Communication::DoStatus(Dnode *pDnode1)
{
  pDnode1->pPlayer->ShowStatus();
  pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
  pDnode1->pPlayer->CreatePrompt();
  pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
}

/************************************************************
 * Stop command                                             *
 ************************************************************/

void Communication::DoStop()
{
  CString LogBuf;
  Dnode   *pDnode2;

  StateRunning = false;
  FdList->SetpDnodeCursorFirst();
  while (!FdList->EndOfDnodeList())
  {
    pDnode2 = FdList->GetDnode();
    pDnode2->pPlayer->Save();
    pDnode2->PlayerStateBye = true;
    pDnode2->PlayerOut += "\r\nGame is stopping ... Bye Bye!\r\n";
    LogBuf = pDnode2->PlayerName;
    LogBuf += " force disconnect";
    pLog->LogIt(LogBuf);
    FdList->SetpDnodeCursorNext();
  }
  pLog->LogIt("All players will be force disconnected");
}

/************************************************************
 * Tell command                                             *
 ************************************************************/

void Communication::DoTell(Dnode *pDnode1, CString CmdStr)
{
  Dnode   *pDnode2;
  CString LookupName;
  CString PlayerName;
  bool    TargetFound;
  CString TargetName;
  CString TargetNameSave;
  CString TellMsg;

  TargetFound     = false;
  PlayerName      = pDnode1->PlayerName;
  TargetName      = GetWord(CmdStr, 2);
  TargetNameSave  = TargetName;
  PlayerName.MakeLower();
  TargetName.MakeLower();
  if (TargetName == PlayerName)
  {
    pDnode1->PlayerOut += "Seems silly to tell yourself something!\r\n\r\n> ";
    return;
  }
  if (TargetName.GetLength() < 1)
  {
    pDnode1->PlayerOut += "Tell who?";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  TellMsg = GetWords(CmdStr, 3);
  if (TellMsg.GetLength() < 1)
  {
    pDnode1->PlayerOut += "Um, tell ";
    pDnode1->PlayerOut += TargetNameSave;
    pDnode1->PlayerOut += " what?";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  pDnode2 = GetTargetDnode(TargetName);
  if (pDnode2)
  { // Send the tell
    PlayerName = pDnode1->PlayerName;
    TargetName = pDnode2->PlayerName;

    pDnode1->PlayerOut += "&M";
    pDnode1->PlayerOut += "You told ";
    pDnode1->PlayerOut += TargetName;
    pDnode1->PlayerOut += ", ";
    pDnode1->PlayerOut += TellMsg;
    pDnode1->PlayerOut += "&N";
    pDnode1->PlayerOut += "\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();

    pDnode2->PlayerOut += "&M";
    pDnode2->PlayerOut += "\r\n";  
    pDnode2->PlayerOut += PlayerName;
    pDnode2->PlayerOut += " tells you, ";
    pDnode2->PlayerOut += TellMsg;
    pDnode2->PlayerOut += "&N";
    pDnode2->PlayerOut += "\r\n";
    pDnode2->pPlayer->CreatePrompt();
    pDnode2->PlayerOut += pDnode1->pPlayer->GetOutput();
  }
  else
  { // Tell player ... not found
    pDnode1->PlayerOut += TargetNameSave;
    pDnode1->PlayerOut += " is not online.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
  }
}

/***********************************************************
* Title command                                            *
************************************************************/

void Communication::DoTitle(Dnode *pDnode1, CString CmdStr)
{
  CString Title;
  CString TmpStr;

  TmpStr = GetWord(CmdStr, 2);
  TmpStr.MakeLower();
  if (TmpStr == "")
  { // Player entered 'title' by itself
    if (pDnode1->pPlayer->Title == "")
    { // Player has no title
      pDnode1->PlayerOut += "You do not have a title";
      pDnode1->PlayerOut += "\r\n";
      pDnode1->pPlayer->CreatePrompt();
      pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
      return;
    }
    else
    { // Show player's title
      pDnode1->PlayerOut += "Your title is: ";
      pDnode1->PlayerOut +=  pDnode1->pPlayer->Title;
      pDnode1->PlayerOut += "&N"; // In case title is messed up
      pDnode1->PlayerOut += "\r\n";
      pDnode1->pPlayer->CreatePrompt();
      pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
      return;
    }
  }
  if (TmpStr == "none")
  { // Player entered 'title none'
    if (pDnode1->pPlayer->Title == "")
    { // Player has no title
      pDnode1->PlayerOut += "You did not have a title and you still do not have a title";
      pDnode1->PlayerOut += "\r\n";
      pDnode1->pPlayer->CreatePrompt();
      pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
      return;
    }
    else
    {
      pDnode1->pPlayer->Title = "";
      pDnode1->PlayerOut += "Your title has been removed.\r\n";
      pDnode1->pPlayer->CreatePrompt();
      pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
      return;
    }
  }
  TmpStr = GetWords(CmdStr, 2);
  Title = TmpStr;
  // Strip out color codes so Title length can be checked
  TmpStr.Replace("&N", "");
  TmpStr.Replace("&K", "");
  TmpStr.Replace("&R", "");
  TmpStr.Replace("&G", "");
  TmpStr.Replace("&Y", "");
  TmpStr.Replace("&B", "");
  TmpStr.Replace("&M", "");
  TmpStr.Replace("&C", "");
  TmpStr.Replace("&W", "");
  if (TmpStr.GetLength() > 25)
  {
    pDnode1->PlayerOut += "Title must be less than 26 characters, color codes do not count.\r\n";
    pDnode1->pPlayer->CreatePrompt();
    pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
    return;
  }
  pDnode1->pPlayer->Title = Title;
  pDnode1->PlayerOut += "Your title has been set.\r\n";
  pDnode1->pPlayer->CreatePrompt();
  pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
}

/************************************************************
 * Who command                                              *
 ************************************************************/

void Communication::DoWho(Dnode *pDnode1)
{
  Dnode   *pDnode2;
  CString DisplayName;

  pDnode1->PlayerOut += "\n\r&CPlayers online&N\r\n";
  pDnode1->PlayerOut += "--------------\r\n";
  FdList->SetpDnodeCursorFirst();
  while (!FdList->EndOfDnodeList())
  {
    pDnode2 = FdList->GetDnode();
    if (pDnode2->PlayerStatePlaying)
    {
      DisplayName.Format("%-15s", pDnode2->PlayerName);
      pDnode1->PlayerOut += DisplayName;
      pDnode1->PlayerOut += " ";
      pDnode1->PlayerOut += pDnode2->pPlayer->Title;
      pDnode1->PlayerOut += "&N"; // Reset color
      pDnode1->PlayerOut += "\n\r";
    }
    FdList->SetpDnodeCursorNext();
  }
  pDnode1->pPlayer->CreatePrompt();
  pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
}

/***********************************************************
*  Return pointer of target, if target in 'playing' state  *
************************************************************/

Dnode *Communication::GetTargetDnode(CString TargetName)
{
  bool        TargetFound;
  CString     LookupName;
  Dnode       *pDnode2;

  TargetFound = false;
  TargetName.MakeLower();
  FdList->SetpDnodeCursorFirst();
  while (!FdList->EndOfDnodeList())
  {
    pDnode2 = FdList->GetDnode();
    LookupName = pDnode2->PlayerName;
    LookupName.MakeLower();
    if (TargetName == LookupName)
    { // Target found
      if (pDnode2->PlayerStatePlaying)
      { // Target is in 'playing' state
        TargetFound = true;
        break;
      }
    }
    FdList->SetpDnodeCursorNext();
  }  
  if (TargetFound)
  {
    return pDnode2;
  }
  else
  {
    return NULL;
  }
}

/***********************************************************
* Send output to other players in the same room as player  *
************************************************************/

void Communication::SendToRoom(Dnode *pDnode1, Dnode *pDnode2, CString MsgText)
{
  CString TargetRoomId;
  CString LookupRoomId;
  Dnode   *pDnode3;

  TargetRoomId = pDnode1->pPlayer->RoomId;
  FdList->SetpDnodeCursorFirst();
  while (!FdList->EndOfDnodeList())
  {
    pDnode3 = FdList->GetDnode();
    LookupRoomId = pDnode3->pPlayer->RoomId;
    if (pDnode1 != pDnode3)
    { // It's not player
      if (pDnode2 != pDnode3)
      { // It's not target
        if (TargetRoomId == LookupRoomId)
        { // Others who are in the same room
          if (pDnode3->PlayerStatePlaying)
          { // and are playing
            pDnode3->PlayerOut += "\r\n";
            pDnode3->PlayerOut += MsgText;
            pDnode3->PlayerOut += "&N"; // Reset color
            pDnode3->PlayerOut += "\r\n";
            pDnode3->pPlayer->CreatePrompt();
            pDnode3->PlayerOut += pDnode3->pPlayer->GetOutput();
          }
        }
      }
    }
    FdList->SetpDnodeCursorNext();
  }  
}

/***********************************************************
* Replace or strip out color codes                         *
************************************************************/

void Communication::Color(Dnode *pDnode1)
{
  if (!pDnode1->PlayerStatePlaying)
  { // Player's color variable is not available
    return;
  }
  if (pDnode1->pPlayer->Color)
  {
    pDnode1->PlayerOut.Replace("&N", Normal);
    pDnode1->PlayerOut.Replace("&K", BrightBlack);
    pDnode1->PlayerOut.Replace("&R", BrightRed);
    pDnode1->PlayerOut.Replace("&G", BrightGreen);
    pDnode1->PlayerOut.Replace("&Y", BrightYellow);
    pDnode1->PlayerOut.Replace("&B", BrightBlue);
    pDnode1->PlayerOut.Replace("&M", BrightMagenta);
    pDnode1->PlayerOut.Replace("&C", BrightCyan);
    pDnode1->PlayerOut.Replace("&W", BrightWhite);
  }
  else
  {
    pDnode1->PlayerOut.Replace("&N", "");
    pDnode1->PlayerOut.Replace("&K", "");
    pDnode1->PlayerOut.Replace("&R", "");
    pDnode1->PlayerOut.Replace("&G", "");
    pDnode1->PlayerOut.Replace("&Y", "");
    pDnode1->PlayerOut.Replace("&B", "");
    pDnode1->PlayerOut.Replace("&M", "");
    pDnode1->PlayerOut.Replace("&C", "");
    pDnode1->PlayerOut.Replace("&W", "");
  }
}

/***********************************************************
* Groups - Player is leaving the group                     *
************************************************************/

void Communication::GrpLeave(Dnode *pDnode1)
{
  if (pDnode1->pPlayer == pDnode1->pPlayer->pPlayerGrpMember[0])
  { // Player is group leader, disband the whole group
    GrpLeaveLeader(pDnode1);
  }
  else
  { // Player is a group member
    GrpLeaveMember(pDnode1);
  }
}

/***********************************************************
* Groups - Leader is leaving - Disband the whole group     *
************************************************************/

void Communication::GrpLeaveLeader(Dnode *pDnode1)
{
  Dnode *pDnode3; // Other group members
  int   i;
  int   j;

  // Player is group leader, disband the whole group
  for (i=1;i<GRP_LIMIT;i++)
  { // For each group member
    if (pDnode1->pPlayer->pPlayerGrpMember[i] != NULL)
    {// Get member's dnode before member's pointer is nulled
      pDnode3 = GetTargetDnode(pDnode1->pPlayer->pPlayerGrpMember[i]->Name);
      // Null member's leader pointer
      pDnode1->pPlayer->pPlayerGrpMember[i]->pPlayerGrpMember[0] = NULL;
      // Null member pointer
      pDnode1->pPlayer->pPlayerGrpMember[i] = NULL;
      // Let the group members know that group is disbanded
      pDnode3->PlayerOut += "\r\n";
      pDnode3->PlayerOut += "The group has been disbanded.\r\n";
      pDnode3->pPlayer->CreatePrompt();
      pDnode3->PlayerOut += pDnode1->pPlayer->GetOutput();
      // Member now has no group, remove any followers of this member
      for (j=0;j<GRP_LIMIT;j++)
      {
        pDnode1->pPlayer->pPlayerFollowers[i] = NULL;
      }
    }
  }
  // Complete the disbanding of the whole group
  pDnode1->pPlayer->pPlayerGrpMember[0] = NULL;
  pDnode1->PlayerOut += "Your group has been disbanded.\r\n";
  pDnode1->pPlayer->CreatePrompt();
  pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
}

/***********************************************************
* Groups - Member is leaving - Remove them from the group  *
************************************************************/

void Communication::GrpLeaveMember(Dnode *pDnode1)
{
  Dnode *pDnode2; // Group leader
  Dnode *pDnode3; // Other group members
  int   i;
  int   j;
  bool  GrpEmpty;

  for (i=1;i<GRP_LIMIT;i++)
  { // Loop thru leader's member list to find player
    if (pDnode1->pPlayer->pPlayerGrpMember[0]->pPlayerGrpMember[i] == pDnode1->pPlayer)
    { // Remove player from leader's group
      pDnode1->pPlayer->pPlayerGrpMember[0]->pPlayerGrpMember[i] = NULL;
      j = i; // Save player's subscript
    }
  }
  for (i=1;i<GRP_LIMIT;i++)
  {
    if (pDnode1->pPlayer == pDnode1->pPlayer->pPlayerFollowers[0]->pPlayerFollowers[i])
    {
      pDnode1->pPlayer->pPlayerFollowers[0]->pPlayerFollowers[i] = NULL;
    }
  }
  // Complete the disbanding and let the player know
  pDnode1->PlayerOut += "You have left the group.\r\n";
  pDnode1->pPlayer->CreatePrompt();
  pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
  // Let group leader know when a member leaves the group
  pDnode2 = GetTargetDnode(pDnode1->pPlayer->pPlayerGrpMember[0]->Name);
  pDnode2->PlayerOut += "\r\n";
  pDnode2->PlayerOut += pDnode1->PlayerName;
  pDnode2->PlayerOut += " has left your group.\r\n";
  GrpEmpty = true;
  for (i=1;i<GRP_LIMIT;i++)
  { // For each member of leader's group
    if (pDnode2->pPlayer->pPlayerGrpMember[i] != NULL)
    { // Let other group members know that player has left
      GrpEmpty = false;
      pDnode3 = GetTargetDnode(pDnode2->pPlayer->pPlayerGrpMember[i]->Name);
      pDnode3->PlayerOut += "\r\n";
      pDnode3->PlayerOut += pDnode1->PlayerName;
      pDnode3->PlayerOut += " has left the group.\r\n";
      if (pDnode1->pPlayer = pDnode3->pPlayer->pPlayerFollowers[0])
      {
        pDnode3->PlayerOut += "You stop following ";
        pDnode3->PlayerOut += pDnode1->PlayerName;
        pDnode3->PlayerOut += "\r\n";
        pDnode3->pPlayer->pPlayerFollowers[0] = NULL;
      }
      pDnode3->pPlayer->CreatePrompt();
      pDnode3->PlayerOut += pDnode3->pPlayer->GetOutput();
    }
  }
  pDnode1->pPlayer->pPlayerGrpMember[0] = NULL;
  if (GrpEmpty)
  { // Player was the last in the group, let the leader know
    pDnode2->pPlayer->pPlayerGrpMember[0] = NULL;
    pDnode2->PlayerOut += "Your group has disbanded.\r\n";
    // Leader has no group, remove any followers
    for (i=0;i<GRP_LIMIT;i++)
    {
      pDnode2->pPlayer->pPlayerFollowers[i] = NULL;
    }
  }
  pDnode2->pPlayer->CreatePrompt();
  pDnode2->PlayerOut += pDnode2->pPlayer->GetOutput();
  // Compact the list of members, so new members are at the end
  for (i=j;i<GRP_LIMIT-1;i++) // j is subscript of member who is leaving
  {
    pDnode2->pPlayer->pPlayerGrpMember[i] = pDnode2->pPlayer->pPlayerGrpMember[i+1];
    pDnode2->pPlayer->pPlayerGrpMember[i+1] = NULL;
  }
  // When a member leaves a group, remove any followers
  for (i=0;i<GRP_LIMIT;i++)
  {
    pDnode1->pPlayer->pPlayerFollowers[i] = NULL;
  }
}