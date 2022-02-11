/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Communication.cpp                                *
* Usage:  Winsock tcp/ip telnet player communications      *
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
#include "Communication.h"

/***********************************************************
* Globals                                                  *
************************************************************/

Dnode           *pDnodeOthers;
fd_set           ExcSet;
fd_set           InpSet;
int              ListenSocket;
fd_set           OutSet;
vector<string>   ValidCmds;

/***********************************************************
 * Communication class constructor                         *
 ***********************************************************/

Communication::Communication()
{
}

/***********************************************************
 * Communication class destructor                          *
 ***********************************************************/

Communication::~Communication()
{
}

////////////////////////////////////////////////////////////
// Public functions static                                //
////////////////////////////////////////////////////////////

/***********************************************************
*  Return pointer of target, if target in 'playing' state  *
************************************************************/

Dnode *Communication::GetTargetDnode(CString TargetName)
{
  Dnode   *pDnodeLookup;
  bool     TargetFound;
  CString  LookupName;

  TargetFound = false;
  TargetName.MakeLower();
  // Get Dnode pointer for target player
  SetpDnodeCursorFirst();
  while (!EndOfDnodeList())
  { // Loop thru all connections
    pDnodeLookup = pDnodeCursor;
// TODO Can the line above replace the line below ???    
    pDnodeLookup = Descriptor::GetDnode();
    LookupName = pDnodeLookup->PlayerName;
    LookupName.MakeLower();
    if (TargetName == LookupName)
    { // Target found
      if (pDnodeLookup->PlayerStatePlaying)
      { // Target is valid
        TargetFound = true;
        break;
      }
    }
    SetpDnodeCursorNext();
  }
  // Re-position pDnodeCursor
  RepositionDnodeCursor();
  if (TargetFound)
  {
    return pDnodeLookup;
  }
  else
  {
    return NULL;
  }
}

/***********************************************************
* Check to see if player is fighting                       *
************************************************************/

bool Communication::IsFighting()
{
  int     RandomNumber;
  CString FightingMsg;

  if (!pDnodeActor->PlayerStateFighting)
  {
    return false;
  }
  // Player is fighting, generate random message
  RandomNumber = Utility::GetRandomNumber(5);
  switch(RandomNumber) 
  {
    case 1:
      FightingMsg = "You are fighting for your life!";
      break;
    case 2:
      FightingMsg = "Not now, your are fighting.";
      break;
    case 3:
      FightingMsg = "No can do, you are fighting";
      break;
    case 4:
      FightingMsg = "You are busy swinging a weapon.";
      break;
    case 5:
      FightingMsg = "NO!, now get back in the fight!.";
      break;
    default :
      FightingMsg = "You are fighting.";
  }
  pDnodeActor->PlayerOut += FightingMsg;
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  return true;
}

/***********************************************************
* Check to see if player is sleeping                       *
************************************************************/

bool Communication::IsSleeping()
{
  int     RandomNumber;
  CString SleepingMsg;

  if (pDnodeActor->pPlayer->Position != "sleep")
  {
    return false;
  }
  // Player is sleeping, generate random message
  RandomNumber = Utility::GetRandomNumber(5);
  switch(RandomNumber) 
  {
    case 1:
      SleepingMsg = "You must be dreaming.";
      break;
    case 2:
      SleepingMsg = "You dream about doing something.";
      break;
    case 3:
      SleepingMsg = "It's such a nice dream, please don't wake me.";
      break;
    case 4:
      SleepingMsg = "Your snoring almost wakes you up.";
      break;
    case 5:
      SleepingMsg = "Dream, dream, dreeeeaaaammmm, all I do is dream.";
      break;
    default :
      SleepingMsg = "You must be dreaming.";
  }
  pDnodeActor->PlayerOut += SleepingMsg;
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  return true;
}

/***********************************************************
* Send output to all players                               *
************************************************************/

void Communication::SendToAll(CString PlayerMsg, CString AllMsg)
{
  Descriptor::SetpDnodeCursorFirst();
  while (!Descriptor::EndOfDnodeList())
  { // Loop thru all connections
    pDnodeOthers = Descriptor::GetDnode();
    if (pDnodeActor == pDnodeOthers)
    { // Send to player
      pDnodeActor->PlayerOut += PlayerMsg;
    }
    else
    { // Send to everyone else
      if (pDnodeOthers->PlayerStatePlaying)
      { // Target player is in playing state
        if (pDnodeActor->PlayerStateInvisible)
        { // Sending player is invisible, no message is sent
          Descriptor::SetpDnodeCursorNext();
          continue;
        }
        pDnodeOthers->PlayerOut += AllMsg;
        pDnodeOthers->pPlayer->CreatePrompt();
        pDnodeOthers->PlayerOut += pDnodeOthers->pPlayer->GetOutput();
      }
    }
    Descriptor::SetpDnodeCursorNext();
  }
  // Re-position pDnodeCursor
  RepositionDnodeCursor();
}

/***********************************************************
* Send output to other players in the same room as player  *
************************************************************/

void Communication::SendToRoom(CString TargetRoomId, CString MsgText)
{
  CString  LookupRoomId;

  // Send output to all players in the room
  Descriptor::SetpDnodeCursorFirst();
  while (!Descriptor::EndOfDnodeList())
  { // Loop thru all connections
    pDnodeOthers = Descriptor::GetDnode();
    if (pDnodeOthers->PlayerStatePlaying)
    { // Player is logged in and playing
      LookupRoomId = pDnodeOthers->pPlayer->RoomId;
      if (pDnodeSrc != pDnodeOthers)
      { // It's not the originating player
        if (pDnodeTgt != pDnodeOthers)
        { // It's not the target player
          if (TargetRoomId == LookupRoomId)
          { // Others who are in the same room
            if (pDnodeOthers->pPlayer->Position != "sleep")
            { // and are not sleeping
              if (pDnodeSrc)
              { // Player sending a message, not an NPC
                if (pDnodeSrc->PlayerStateInvisible)
                { // Player is invisible, no message is sent
                  Descriptor::SetpDnodeCursorNext();
                  continue;
                }
              }
              if (pDnodeOthers->PlayerStateInvisible)
              { // Player is invisible, no message is sent
                Descriptor::SetpDnodeCursorNext();
                continue;
              }
              pDnodeOthers->PlayerOut += "\r\n";
              pDnodeOthers->PlayerOut += MsgText;
              pDnodeOthers->PlayerOut += "&N"; // Reset color
              pDnodeOthers->PlayerOut += "\r\n";
              pDnodeOthers->pPlayer->CreatePrompt();
              pDnodeOthers->PlayerOut += pDnodeOthers->pPlayer->GetOutput();
            }
          }
        }
      }
    }
    Descriptor::SetpDnodeCursorNext();
  }
  // Re-position pDnodeCursor
  RepositionDnodeCursor();
}

/***********************************************************
* Show players in a given room                             *
************************************************************/

void Communication::ShowPlayersInRoom(Dnode *pDnode)
{
  CString  LookupRoomId;
  CString  TargetRoomId;

  TargetRoomId = pDnode->pPlayer->RoomId;
  // Show players in same room as player
  Descriptor::SetpDnodeCursorFirst();
  while (!Descriptor::EndOfDnodeList())
  { // Loop thru all connections
    pDnodeOthers = Descriptor::GetDnode();
    if (pDnode != pDnodeOthers)
    { // It's another player
      if (pDnodeOthers->PlayerStatePlaying)
      { // who is playing
        LookupRoomId = pDnodeOthers->pPlayer->RoomId;
        if (TargetRoomId == LookupRoomId)
        { // and is in the same room
          if (pDnodeOthers->PlayerStateInvisible)
          { // Player is invisible, no show
            Descriptor::SetpDnodeCursorNext();
            continue;
          }
          pDnode->PlayerOut += "\r\n";
          pDnode->PlayerOut += "&W";
          pDnode->PlayerOut += pDnodeOthers->PlayerName;
          pDnode->PlayerOut += " is ";
          if (pDnodeOthers->PlayerStateFighting)
          { // and is fighting
            pDnode->PlayerOut += "engaged in a fight!";
          }
          else
          if (pDnodeOthers->pPlayer->Position == "sleep")
          { // or is asleep
            pDnode->PlayerOut += "here, sound asleep.";
          }
          else
          {  // or is sitting or standing
            pDnode->PlayerOut += pDnodeOthers->pPlayer->Position;
            if (pDnodeOthers->pPlayer->Position == "sit")
            { // Add the extra 't' so it comes out sitting vs siting
              pDnode->PlayerOut += "t";
            }
            pDnode->PlayerOut += "ing here.";
          }
          pDnode->PlayerOut += "&N";
        }
      }
    }
    Descriptor::SetpDnodeCursorNext();
  }
  // Re-position pDnodeCursor
  RepositionDnodeCursor();
}

/***********************************************************
 * Check for new connections                               *
 ***********************************************************/

void Communication::SockCheckForNewConnections()
{
  CString               LogBuf;
  int                   SocketCount;
  static struct timeval TimeOut{};

  TimeOut.tv_sec = 0;
  TimeOut.tv_usec = 1;
  FD_ZERO(&InpSet);
  FD_ZERO(&OutSet);
  FD_ZERO(&ExcSet);
  FD_SET(ListenSocket, &InpSet);
  // Check status of connections
  Descriptor::SetpDnodeCursorFirst();
  while (!Descriptor::EndOfDnodeList())
  { // Loop thru all connections
    pDnodeActor = Descriptor::GetDnode();
    FD_SET(pDnodeActor->DnodeFd, &InpSet);
    FD_SET(pDnodeActor->DnodeFd, &OutSet);
    FD_SET(pDnodeActor->DnodeFd, &ExcSet);
    Descriptor::SetpDnodeCursorNext();
  }
  // Detect socket state (pg 159)
  SocketCount = select(-1, &InpSet, &OutSet, &ExcSet, &TimeOut);
  if (SocketCount == -1)
  { // Something is wrong
    sprintf(Buf,"%s", strerror(errno));
    LogBuf = "Communication::SockCheckForNewConnections: select: " + ConvertStringToCString(Buf);
    Log::LogIt(LogBuf);
    AfxMessageBox("Communication::SockCheckForNewConnections - Error: select", MB_ICONSTOP);
    _endthread();
  }
  if (FD_ISSET(ListenSocket, &InpSet))
  { // Process new connection
    SockNewConnection();
  }
}

/***********************************************************
 * Close port                                              *
 ***********************************************************/

void Communication::SockClosePort(int Port)
{ 
  CString LogBuf;
  int     Result;

  // Close the socket (pg 70)
  Result = ::closesocket(ListenSocket);
  if (Result!= 0)
  {
    AfxMessageBox("Communication::~Communication - Error: closesocket", MB_ICONSTOP);
    _endthread();
  }
  sprintf(Buf, "%d", Port);
  LogBuf = "Closed port " + ConvertStringToCString(Buf);;
  Log::LogIt(LogBuf);
}

/***********************************************************
 * Open port                                               *
 ***********************************************************/

void Communication::SockOpenPort(int Port)
{
  unsigned long FionbioParm;
  struct        linger      ld{};
  CString       LogBuf;
  int           OptionValue;
  int           Result;
  struct        sockaddr_in sa{};
  WORD          VersionRequested;
  WSADATA       WsaData;

  FionbioParm       = 1;
  ld.l_onoff        = 0;
  ld.l_linger       = 0;
  OptionValue       = 1;
  VersionRequested  = MAKEWORD(1, 1);
  // Initialize WinSock API (pg 320)
  Result = WSAStartup(VersionRequested, &WsaData);
  if (Result != 0)
  {
    sprintf(Buf, "%s", strerror(errno));
    LogBuf = "WinSock not available!: " + ConvertStringToCString(Buf);
    Log::LogIt(LogBuf);
    AfxMessageBox("Communication::SockOpenPort - WinSock not available", MB_ICONSTOP);
    _endthread();
  }
  // Establish a streaming socket (pg 51)
  ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (ListenSocket == SOCKET_ERROR)
  {
    sprintf(Buf, "%s", strerror(errno));
    LogBuf = "Communication:SockOpenPort - Error: initializing socket: " + ConvertStringToCString(Buf);
    Log::LogIt(LogBuf);
    AfxMessageBox("Communication::SockOpenPort - Error: initializing socket", MB_ICONSTOP);
    _endthread();
  }
  // Enable reuse of local socket name (pg 305)
  Result = setsockopt(ListenSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&OptionValue, sizeof(OptionValue));
  if (Result != 0)
  {
    sprintf(Buf, "%s", strerror(errno));
    LogBuf = "Communication:SockOpenPort - Error: setsockopt: SOL_SOCKET SO_REUSEADDR" + ConvertStringToCString(Buf);
    Log::LogIt(LogBuf);
    AfxMessageBox("Communication::SockOpenPort - Error: setsockopt", MB_ICONSTOP);
    _endthread();
  }
  // Establish underlying TCP/IP buffer size (pg 305)
  Result = setsockopt(ListenSocket, SOL_SOCKET, SO_SNDBUF, (char *) &OptionValue, sizeof(OptionValue));
  if (Result != 0)
  {
    sprintf(Buf, "%s", strerror(errno));
    LogBuf = "Communication:SockOpenPort - Error: setsockopt SNDBUF: " + ConvertStringToCString(Buf);
    Log::LogIt(LogBuf);
    AfxMessageBox("Communication::SockOpenPort - Error: setsockopt SNDBUF", MB_ICONSTOP);
    _endthread();
  }
  // Disable linger and set timeout to zero (pg 301)
  Result = setsockopt(ListenSocket, SOL_SOCKET, SO_LINGER, (char *) &ld, sizeof(ld));
  if (Result != 0)
  {
    sprintf(Buf, "%s", strerror(errno));
    LogBuf = "Communication:SockOpenPort - Error: setsockopt SO_LINGER: " + ConvertStringToCString(Buf);
    Log::LogIt(LogBuf);
    AfxMessageBox("Communication::SockOpenPort - Error: setsockopt SO_LINGER", MB_ICONSTOP);
    _endthread();
  }
  // Initialize sockaddr structure (pg 53)
  memset((char *)&sa, 0, sizeof(sa));
  sa.sin_family       = AF_INET;
  sa.sin_addr.s_addr  = INADDR_ANY;
  sa.sin_port         = htons(Port);
  // Associate a local address with the socket (pg 56)
  Result = bind(ListenSocket, (struct sockaddr *)&sa, sizeof(sa));
  if (Result != 0)
  {
    sprintf(Buf, "%s", strerror(errno));
    LogBuf = "Communication:SockOpenPort - Error: bind " + ConvertStringToCString(Buf);
    Log::LogIt(LogBuf);
    AfxMessageBox("Communication::SockOpenPort - Error: bind", MB_ICONSTOP);
    _endthread();
  }
  // Make socket nonblocking (pg 286)
  Result = ioctlsocket(ListenSocket, FIONBIO, &FionbioParm);
  if (Result != 0)
  {
    sprintf(Buf, "%s", strerror(errno));
    LogBuf = "Communication:SockOpenPort ioctlsocket: " + ConvertStringToCString(Buf);
    Log::LogIt(LogBuf);
    AfxMessageBox("Communication::SockOpenPort - Error: ioctlsocket", MB_ICONSTOP);
    _endthread();
  }
  // Listen on port and limit pending connections (pg 60)
  Result = listen(ListenSocket, 20);
  if (Result != 0)
  {
    sprintf(Buf, "%s", strerror(errno));
    LogBuf = "Communication:SockOpenPort - Error: listen " + ConvertStringToCString(Buf);
    Log::LogIt(LogBuf);
    ::closesocket(ListenSocket);
    AfxMessageBox("Communication::SockOpenPort - Error: listen", MB_ICONSTOP);
    _endthread();
  }
  sprintf(Buf, "%d", Port);
  LogBuf = "Listening on port " + ConvertStringToCString(Buf);
  Log::LogIt(LogBuf);
  // Load command array
  CommandArrayLoad();
}

/***********************************************************
 * Receive player input, check player status, send output  *
 ***********************************************************/

void Communication::SockRecv()
{
  int      ConnectionCount;
  int      DnodeFdSave;
  char     InpStr[MAX_INPUT_LENGTH];
  int      LineFeedPosition;
  CString  LogBuf;
  int      RecvByteCount;

  //***********************
  //* Service connections *
  //***********************
  Descriptor::SetpDnodeCursorFirst();
  while (!Descriptor::EndOfDnodeList())
  { // Loop thru all connections
    pDnodeActor = Descriptor::GetDnode();
    //***************************
    //* Check connection status *
    //***************************
    if (!pDnodeActor->PlayerStatePlaying)
    { // Player is logging on
      pDnodeActor->InputTick++;
      if (pDnodeActor->InputTick >= INPUT_TICK)
      { // No input, kick 'em out
        pDnodeActor->PlayerStateBye = true;
        sprintf(Buf, "%d", pDnodeActor->DnodeFd);
        LogBuf = "Time out during logon on descriptor " + ConvertStringToCString(Buf);;
        Log::LogIt(LogBuf);
        pDnodeActor->PlayerOut += "\r\n";
        pDnodeActor->PlayerOut += "No input ... closing connection";
      }
    }
    if (FD_ISSET(pDnodeActor->DnodeFd, &OutSet))
    { // This code don't do nothin', sample OutSet check
      int dummy = 0;
    }
    if (FD_ISSET(pDnodeActor->DnodeFd, &ExcSet))
    { // Kick out connections with exceptions
      pDnodeActor->PlayerStateBye = true;
      if (pDnodeActor->PlayerStatePlaying)
      { // Player is playing, so save them
        pDnodeActor->PlayerStatePlaying = false;
        pDnodeActor->pPlayer->Save();
      }
    }
    else
    { // Good connection
      if (FD_ISSET(pDnodeActor->DnodeFd, &InpSet))
      { // Receive
        memset(InpStr, '\0', sizeof(InpStr));
        RecvByteCount = ::recv(pDnodeActor->DnodeFd, InpStr, MAX_INPUT_LENGTH-1, 0);
        if (RecvByteCount == 0)
        { // Should be input but there is none -- disconnected ??
          pDnodeActor->PlayerStateBye = true;
          if (pDnodeActor->PlayerStatePlaying)
          { // Player is playing, so save them
            pDnodeActor->PlayerStatePlaying = false;
            pDnodeActor->pPlayer->Save();
          }
        }
        if (WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError() == WSAEINTR)
        { // Nothing worth processing
         RecvByteCount = 0;
        }
        if (RecvByteCount > 0)
        { // Got something ... append it to player input
          pDnodeActor->PlayerInp += InpStr;
          pDnodeActor->InputTick = 0;
        }
      }
    }
    //*****************************
    //* Banner for new connection * 
    //*****************************
    if (pDnodeActor->PlayerStateSendBanner)
    { // New connection
      pDnodeActor->PlayerStateSendBanner = false;
      pDnodeActor->PlayerStateLoggingOn = true;
      pDnodeActor->PlayerStateWaitNewCharacter = true;
      // Send greeting  
      LogonGreeting();
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->PlayerOut += "Create a new character Y-N?";
      pDnodeActor->PlayerOut += "\r\n";
    }
    //***********************
    //* Update player stats *
    //***********************
    if (pDnodeActor->PlayerStatePlaying)
    { // Player is playing
      pDnodeActor->StatsTick++;
      if (pDnodeActor->StatsTick >= STATS_TICK)
      { // Update player stats
        pDnodeActor->StatsTick = 0;
        UpdatePlayerStats();
      }
      // Hunger & thirst
      pDnodeActor->HungerThirstTick++;
      if (pDnodeActor->HungerThirstTick >= HUNGER_THIRST_TICK)
      { // Decrement hunger & thirst
        pDnodeActor->HungerThirstTick = 0;
        pDnodeActor->pPlayer->Hunger++;
        pDnodeActor->pPlayer->Thirst++;
        if (pDnodeActor->pPlayer->Level < HUNGER_THIRST_LEVEL)
        { // No hunger or thirst .. yet
          pDnodeActor->pPlayer->Hunger = 0;
          pDnodeActor->pPlayer->Thirst = 0;
        }
        if (pDnodeActor->pPlayer->Admin)
        { // Admin immune from hunger and thirst
          pDnodeActor->pPlayer->Hunger = 0;
          pDnodeActor->pPlayer->Thirst = 0;
        }
        if (pDnodeActor->pPlayer->Hunger > 99)
        { // Player is hungry
          pDnodeActor->pPlayer->Hunger = 100;
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->PlayerOut += "You are extremely hungry!!!";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->pPlayer->CreatePrompt();
          pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
        }
        if (pDnodeActor->pPlayer->Thirst > 99)
        { // Player is thirsty
          pDnodeActor->pPlayer->Thirst = 100;
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->PlayerOut += "You are extremely thirsty!!!";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->pPlayer->CreatePrompt();
          pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
        }
      }
    }
    //*****************
    //* Handle fights *
    //*****************
    if (pDnodeActor->PlayerStateFighting)
    { // Fighting
      pDnodeActor->FightTick++;
      if (pDnodeActor->FightTick >= FIGHT_TICK)
      { // WHACK 'em
        pDnodeActor->FightTick = 0;
        Violence();
      }
    }
    //*********************
    //* Is Game stopping? *
    //*********************
    if (StateStopping)
    { // Game is stopping
      Descriptor::SetpDnodeCursorFirst();
      while (!Descriptor::EndOfDnodeList())
      { // Loop thru all connections
        pDnodeOthers = Descriptor::GetDnode();
        if (!pDnodeOthers->PlayerStateBye)
        { // If player is not already going Bye Bye
          pDnodeOthers->PlayerStateBye = true;
          pDnodeOthers->PlayerStatePlaying = false;
          if (pDnodeOthers->PlayerStatePlaying)
          { // Save them
            pDnodeOthers->pPlayer->Save();
          }
          pDnodeOthers->PlayerOut += "\r\n";
          pDnodeOthers->PlayerOut += "Game is stopping ... Bye Bye!";
          pDnodeOthers->PlayerOut += "\r\n";
          LogBuf  = pDnodeOthers->PlayerName;
          LogBuf += " will be force disconnected";
          Log::LogIt(LogBuf);
        }
        Descriptor::SetpDnodeCursorNext();
      }
      // Re-position pDnodeCursor
      RepositionDnodeCursor();
    }
    //**********************
    //* Send player output *
    //**********************
    if (pDnodeActor->PlayerOut.GetLength() > 0)
    { // Player has output, handle color codes
      Color();
      // 'send' resquires a standard c string
      SockSend((LPCTSTR) pDnodeActor->PlayerOut);
    }
    //**********************
    //* Is player quiting? *
    //**********************
    if (pDnodeActor->PlayerStateBye)
    { // Player leaving the game ... disconnect them
      if (!pDnodeActor->PlayerStateReconnecting)
      { // Player is just quiting, not reconnecting
        delete pDnodeActor->pPlayer;
      }
      DnodeFdSave = pDnodeActor->DnodeFd;
      if (Descriptor::DeleteNode())
      { // When connection is deleted from list, log it
        sprintf(Buf, "%d", DnodeFdSave);
        LogBuf = "Closed connection on descriptor " + ConvertStringToCString(Buf);;
        Log::LogIt(LogBuf);
        ConnectionCount = Dnode::GetCount();
        if (ConnectionCount == 1)
        { // Connection count is one means no players are connected
          if (StateStopping)
          { // OMugs is stopping
            StateRunning = false;
          }
        }
      }
      // Skip to next dnode, this player's dnode has been deleted
      Descriptor::SetpDnodeCursorNext();
      continue;
    }
    //************************
    //* Process player input *
    //************************
    if (!StateStopping)
    { // Game is not shutting down
      LineFeedPosition = pDnodeActor->PlayerInp.FindOneOf("\r\n");
      if (LineFeedPosition > -1)
      { // Found a newline, parse the command
        if (pDnodeActor->PlayerName != "Ixaka" && pDnodeActor->PlayerName != "Kwam")
        { // Log what player input
          LogBuf  = pDnodeActor->PlayerIpAddress;
          LogBuf += " ";
          if (pDnodeActor->pPlayer)
          {
            LogBuf += pDnodeActor->pPlayer->RoomId;
            LogBuf += " ";
          }
          LogBuf += pDnodeActor->PlayerInp;
          LogBuf.Replace("\r", " ");
          LogBuf.Replace("\n", " ");
          Log::LogIt(LogBuf);
        }
        CommandParse();
      }
    }
    // Get the next Dnode to process
    Descriptor::SetpDnodeCursorNext();
  }
}

////////////////////////////////////////////////////////////
// Private functions static                               //
////////////////////////////////////////////////////////////

/***********************************************************
* Replace or strip out color codes                         *
************************************************************/

void Communication::Color()
{
  string sPlayerOut;

  if (!pDnodeActor->PlayerStatePlaying)
  { // Player's color variable is not available
    return;
  }
  sPlayerOut = ConvertCStringToString(pDnodeActor->PlayerOut);
  if (pDnodeActor->pPlayer->Color)
  { // Player has turned color on
    StrReplace(sPlayerOut, "&N", Normal);
    StrReplace(sPlayerOut, "&K", BrightBlack);
    StrReplace(sPlayerOut, "&R", BrightRed);
    StrReplace(sPlayerOut, "&G", BrightGreen);
    StrReplace(sPlayerOut, "&Y", BrightYellow);
    StrReplace(sPlayerOut, "&B", BrightBlue);
    StrReplace(sPlayerOut, "&M", BrightMagenta);
    StrReplace(sPlayerOut, "&C", BrightCyan);
    StrReplace(sPlayerOut, "&W", BrightWhite);
  }
  else
  { // Strip out color codes
    StrReplace(sPlayerOut, "&N", "");
    StrReplace(sPlayerOut, "&K", "");
    StrReplace(sPlayerOut, "&R", "");
    StrReplace(sPlayerOut, "&G", "");
    StrReplace(sPlayerOut, "&Y", "");
    StrReplace(sPlayerOut, "&B", "");
    StrReplace(sPlayerOut, "&M", "");
    StrReplace(sPlayerOut, "&C", "");
    StrReplace(sPlayerOut, "&W", "");
  }
  pDnodeActor->PlayerOut = ConvertStringToCString(sPlayerOut);
}

/***********************************************************
 * Load command array                                      *
 ***********************************************************/

void Communication::CommandArrayLoad()
{
  CString    LogBuf;
  string     Stuff;
  ifstream   ValidCmdsFile;
  string     ValidCmdsFileName;

  ValidCmdsFileName  = VALID_CMDS_DIR;
  ValidCmdsFileName += "ValidCommands.txt";
  ValidCmdsFile.open(ValidCmdsFileName);
  if(!ValidCmdsFile.is_open())
  { // Open failed
    AfxMessageBox("Communication::CommandArrayLoad - Open Valid Commands file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  ValidCmds.clear();
  while (ValidCmdsFile.peek() != EOF)
  {
    getline(ValidCmdsFile, Stuff);
    ValidCmds.push_back(Stuff);
  }
  ValidCmdsFile.close();
  LogBuf = "Command array loaded";
  Log::LogIt(LogBuf);
}

/***********************************************************
 * Check command authorization, level, and validity        *
 ***********************************************************/

CString Communication::CommandCheck(CString MudCmdChk)
{
  CString CommandCheckResult;
  CString ValCmd;
  CString ValCmdInfo;
  CString WhoCanDo;

  CommandCheckResult = "Not Found";
  for (auto ValidCmd : ValidCmds)
  { // For each string in the ValidCmds vector
    ValCmdInfo = ConvertStringToCString(ValidCmd);
    ValCmd     = Utility::GetWord(ValCmdInfo, 1);
    WhoCanDo   = Utility::GetWord(ValCmdInfo, 2);
    if (MudCmdChk == ValCmd)
    { // Found the command
      if (WhoCanDo == "all")
      { // Anyone can do this command
        CommandCheckResult = "Ok";
        break;
      }
      else
      if (WhoCanDo == "admin")
      { // It's an Admin command
        if (pDnodeActor->pPlayer->Admin)
        { // Player is an Admin
          CommandCheckResult = "Ok";
          break;
        }
        else
        { // Player is not an Admin
          CommandCheckResult = "NotOk";
        }
      }
      else
      if (atoi(WhoCanDo) > pDnodeActor->pPlayer->Level)
      { // Failed level check
        CommandCheckResult = "Level " + WhoCanDo;
        break;
      }
      else
      { // Passed level check
        CommandCheckResult = "Ok";
        break;
      }
    }
  }
  if (CommandCheckResult == "")
  { // This should never be true
    AfxMessageBox("Communication::CommandCheck - Broke!", MB_ICONSTOP);
    _endthread();
  }
  return CommandCheckResult;
}

/***********************************************************
 * Command parsing                                         *
 ***********************************************************/

void Communication::CommandParse()
{
  CString  BadCommandMsg;
  int      CmdStrLength;
  CString  CommandCheckResult;
  CString  LogBuf;
  CString  MudCmdChk;
  bool     MudCmdOk;
  int      PositionOfNewline;
  int      RandomNumber;

  //**************************
  // Get next command string *
  //**************************
  CmdStr = pDnodeActor->PlayerInp;
  CmdStrLength = CmdStr.GetLength();
  PositionOfNewline = CmdStr.FindOneOf("\r\n");
  if (PositionOfNewline < 0)
  { // No newline found, skip out
    return;
  }
  CmdStr = CmdStr.Left(PositionOfNewline);
  pDnodeActor->PlayerInp = pDnodeActor->PlayerInp.Right(CmdStrLength - PositionOfNewline);
  pDnodeActor->PlayerInp.TrimLeft();
  if (CmdStr == "")
  { // Player hit enter without typing anything
    if (!pDnodeActor->PlayerStateLoggingOn)
    { // Player is not logging on
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput(); 
      return;
    }
  }
  //***************
  // Player logon *
  //***************
  if (pDnodeActor->PlayerStateLoggingOn)
  { // Player just connected and needs to logon
    DoLogon();
    return;
  }
  //*************
  // Get MudCmd *
  //*************
  MudCmd = Utility::GetWord(CmdStr, 1);
  MudCmd.MakeLower();
  // Translate 'n' into 'go north'
  MudCmd = Utility::TranslateWord(MudCmd);
  if (Utility::WordCount(MudCmd) == 2)
  { // Re-get MudCmd. In the case of 'go north', MudCmd is 'go'
    CmdStr = MudCmd;
    MudCmd = Utility::GetWord(CmdStr, 1);
    MudCmd.MakeLower();
  }
  // Check for spamming
  if (MudCmd != "go")
  { // 'go' command is ok
    pDnodeActor->CmdName3 = pDnodeActor->CmdName2;
    pDnodeActor->CmdName2 = pDnodeActor->CmdName1;
    pDnodeActor->CmdName1 = ConvertCStringToString(MudCmd);
    pDnodeActor->CmdTime3 = pDnodeActor->CmdTime2;
    pDnodeActor->CmdTime2 = pDnodeActor->CmdTime1;
    pDnodeActor->CmdTime1 = clock();
    if (pDnodeActor->CmdName1 == pDnodeActor->CmdName2)
    { // Command same as last command
      if (pDnodeActor->CmdName1 == pDnodeActor->CmdName3)
      { // Command same as last two commands
        if (pDnodeActor->CmdTime1 - pDnodeActor->CmdTime3 < 1000)
        { // Stop spamming
          pDnodeActor->PlayerOut += "&R";
          pDnodeActor->PlayerOut += "NO SPAMMING!!";
          pDnodeActor->PlayerOut += "&N";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->pPlayer->CreatePrompt();
          pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
          pDnodeActor->PlayerInp = "";
          return; 
        }
      }
    }
  }
  //****************
  // Check command *
  //****************
  MudCmdOk = false;
  MudCmdChk = MudCmd;
  CommandCheckResult = CommandCheck(MudCmdChk);
  if (CommandCheckResult == "Ok")
  { // Mud command is Ok for this player
    MudCmdOk = true;
  }
  else
  if (Utility::GetWord(CommandCheckResult, 1) == "Level")
  { // Level restriction on command
    pDnodeActor->PlayerOut += "You must attain level ";
    pDnodeActor->PlayerOut += Utility::GetWord(CommandCheckResult, 2);
    pDnodeActor->PlayerOut += " before you can use that command.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return; 
  }
  //******************
  //* SOCIAL command *
  //******************
  if (!MudCmdOk)
  { // Not validated yet, maybe cmd is a social
    if (Social::IsSocial())
    { // Yep, it was a social
      return;
    }
  }
  //**************
  // Bad command *
  //**************
  if (!MudCmdOk)
  { // Not a valid cmd and it is not a social
    RandomNumber = Utility::GetRandomNumber(5);
    switch(RandomNumber) 
    {
      case 1:
        BadCommandMsg = "How's that?";
        break;
      case 2:
        BadCommandMsg = "You try to give a command, but fail.";
        break;
      case 3:
        BadCommandMsg = "Hmmm, making up commands?";
        break;
      case 4:
        BadCommandMsg = "Ehh, what's that again?";
        break;
      case 5:
        BadCommandMsg = "Feeling creative?";
        break;
      default :
        BadCommandMsg = "Your command is not clear.";
    }
    pDnodeActor->PlayerOut += BadCommandMsg;
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }

  //**********************
  //* Process the MudCmd *
  //**********************

  /* ADVANCE command */
  if (MudCmd == "advance")
  {
    DoAdvance();
    return;
  }

  /* AFK command */
  if (MudCmd == "afk")
  {
    DoAfk();
    return;
  }

  /* ASSIST command */
  if (MudCmd == "assist")
  {
    DoAssist();
    return;
  }

  /* BUY command */
  if (MudCmd == "buy")
  {
    DoBuy();
    return;
  }

  /* CHAT command */
  if (MudCmd == "chat")
  {
    DoChat();
    return;
  }

  /* COLOR command */
  if (MudCmd == "color")
  {
    DoColor();
    return;
  }

  /* CONSIDER command */
  if (MudCmd == "consider")
  {
    DoConsider();
    return;
  }

  /* DELETE command */
  if (MudCmd == "delete")
  {
    DoDelete();
    return;
  }

  /* DESTROY command */
  if (MudCmd == "destroy")
  {
    DoDestroy();
    return;
  }

  /* DRINK command */
  if (MudCmd == "drink")
  {
    DoDrink();
    return;
  }

  /* DROP command */
  if (MudCmd == "drop")
  {
    DoDrop();
    return;
  }

  /* EAT command */
  if (MudCmd == "eat")
  {
    DoEat();
    return;
  }

  /* EMOTE command */
  if (MudCmd == "emote")
  {
    DoEmote();
    return;
  }

  /* EQUIPMENT command */
  if (MudCmd == "equipment")
  {
    DoEquipment();
    return;
  }

  /* EXAMINE command */
  if (MudCmd == "examine")
  {
    DoExamine();
    return;
  }

  /* FLEE command */
  if (MudCmd == "flee")
  {
    DoFlee();
    return;
  }

  /* FOLLOW command */
  if (MudCmd == "follow")
  {
    DoFollow(pDnodeActor, CmdStr);
    return;
  }

  /* GET command */
  if (MudCmd == "get")
  {
    DoGet();
    return;
  }

  /* GIVE command */
  if (MudCmd == "give")
  {
    DoGive();
    return;
  }

  /* GO command */
  if (MudCmd == "go")
  {
    DoGo();
    return;
  }

  /* GOTOARRIVE command */
  if (MudCmd == "gotoarrive")
  {
    DoGoToArrive();
    return;
  }

  /* GOTODEPART command */
  if (MudCmd == "gotodepart")
  {
    DoGoToDepart();
    return;
  }

  /* GOTO command */
  if (MudCmd == "goto")
  {
    DoGoTo();
    return;
  }

  /* GROUP command */
  if (MudCmd == "group")
  {
    DoGroup();
    return;
  }

  /* GSAY command */
  if (MudCmd == "gsay")
  {
    DoGsay();
    return;
  }

  /* HAIL command */
  if (MudCmd == "hail")
  {
    DoHail();
    return;
  }

  /* HELP command */
  if (MudCmd == "help")
  {
    DoHelp();
    return;
  }

  /* INVENTORY command */
  if (MudCmd == "inventory")
  {
    DoInventory();
    return;
  }

  /* INVISIBLE command */
  if (MudCmd == "invisible")
  {
    DoInvisible();
    return;
  }

  /* KILL command */
  if (MudCmd == "kill")
  {
    DoKill();
    return;
  }

  /* LIST command */
  if (MudCmd == "list")
  {
    DoList();
    return;
  }

  /* LOAD command */
  if (MudCmd == "load")
  {
    DoLoad();
    return;
  }

  /* LOOK command */
  if (MudCmd == "look")
  {
    DoLook(CmdStr);
    return;
  }

  /* MONEY command */
  if (MudCmd == "money")
  {
    DoMoney();
    return;
  }

  /* MOTD command */
  if (MudCmd == "motd")
  {
    DoMotd();
    return;
  }

  /* OneWhack command */
  if (MudCmd == "onewhack")
  {
    DoOneWhack();
    return;
  }

  /* PASSWORD command */
  if (MudCmd == "password")
  {
    DoPassword();
    return;
  }

  /* PLAYED command */
  if (MudCmd == "played")
  {
    DoPlayed();
    return;
  }

  /* QUIT command */
  if (MudCmd == "quit")
  {
    DoQuit();
    return;
  }

  /* REFRESH command */
  if (MudCmd == "refresh")
  {
    DoRefresh();
    return;
  }

  /* REMOVE command */
  if (MudCmd == "remove")
  {
    DoRemove();
    return;
  }

  /* RESTORE command */
  if (MudCmd == "restore")
  {
    DoRestore(CmdStr);
    return;
  }

  /* ROOMINFO command */
  if (MudCmd == "roominfo")
  {
    DoRoomInfo();
    return;
  }

  /* SAVE command */
  if (MudCmd == "save")
  {
    DoSave();
    return;
  }

  /* SAY command */
  if (MudCmd == "say")
  {
    DoSay();
    return;
  }

  /* SELL command */
  if (MudCmd == "sell")
  {
    DoSell();
    return;
  }

  /* SHOW command */
  if (MudCmd == "show")
  {
    DoShow();
    return;
  }

  /* SIT command */
  if (MudCmd == "sit")
  {
    DoSit();
    return;
  }

  /* SLEEP command */
  if (MudCmd == "sleep")
  {
    DoSleep();
    return;
  }

  /* STAND command */
  if (MudCmd == "stand")
  {
    DoStand();
    return;
  }

  /* STATUS command */
  if (MudCmd == "status")
  {
    DoStatus();
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
    DoTell();
    return;
  }

  /* TIME command */
  if (MudCmd == "time")
  {
    DoTime();
    return;
  }

  /* TITLE command */
  if (MudCmd == "title")
  {
    DoTitle();
    return;
  }

  /* TRAIN command */
  if (MudCmd == "train")
  {
    DoTrain();
    return;
  }

  /* WAKE command */
  if (MudCmd == "wake")
  {
    DoWake();
    return;
  }

  /* WEAR command */
  if (MudCmd == "wear")
  {
    DoWear();
    return;
  }

  /* WHERE command */
  if (MudCmd == "where")
  {
    DoWhere();
    return;
  }

  /* WHO command */
  if (MudCmd == "who")
  {
    DoWho();
    return;
  }

  /* WIELD command */
  if (MudCmd == "wield")
  {
    DoWield();
    return;
  }
  pDnodeActor->PlayerOut += "Command is valid, but not implemented at this time.";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  // Log it
  MudCmd  = Utility::MakeFirstUpper(MudCmd);
  LogBuf  = MudCmd;
  LogBuf += " is in command array, but Do";
  LogBuf += MudCmd;
  LogBuf += " is not coded.";
  Log::LogIt(LogBuf);
}

/***********************************************************
 * Advance command                                         *
 ***********************************************************/

void Communication::DoAdvance()
{
  int      Level;
  CString  LevelString;
  CString  LogBuf;
  CString  PlayerName;
  CString  PlayerNameSave;
  CString  TargetName;
  CString  TargetNameSave;
  CString  TmpStr;

  PlayerName      = pDnodeActor->PlayerName;
  TargetName      = Utility::GetWord(CmdStr, 2);
  PlayerNameSave  = PlayerName;
  TargetNameSave  = TargetName;
  PlayerName.MakeLower();
  TargetName.MakeLower();
  Level = atoi(Utility::GetWord(CmdStr, 3));
  sprintf(Buf, "%d", Level);
  LevelString = ConvertStringToCString(Buf);
  if (TargetName == "")
  { // No name given
    pDnodeActor->PlayerOut += "Advance who?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  // Get target Dnode pointer
  pDnodeTgt = GetTargetDnode(TargetName);
  if (!pDnodeTgt)
  { // Target player not found
    pDnodeActor->PlayerOut += TargetNameSave;
    pDnodeActor->PlayerOut += " is not online.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (Level == pDnodeTgt->pPlayer->Level)
  { // Advance to same level ... that's just plain silly
    pDnodeActor->PlayerOut += TargetNameSave;
    pDnodeActor->PlayerOut += " is already at level ";
    pDnodeActor->PlayerOut += LevelString;
    pDnodeActor->PlayerOut += ".";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (Level == 0)
  { // Advance to level 0 ... not valid
    pDnodeActor->PlayerOut += TargetNameSave;
    pDnodeActor->PlayerOut += " cannot be advanced to level ";
    pDnodeActor->PlayerOut += LevelString;
    pDnodeActor->PlayerOut += ".";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  TargetNameSave = pDnodeTgt->PlayerName;
  // Level up! or Down :(
  LogBuf = pDnodeTgt->PlayerName;
  if (Level > pDnodeTgt->pPlayer->Level)
  { // Level up!
    LogBuf += " has been advanced to level ";
  }
  else
  { // Level down :(
    LogBuf += " has been demoted to level ";
  }
  LogBuf += LevelString;
  LogBuf += " by ";
  LogBuf += pDnodeActor->PlayerName;
  Log::LogIt(LogBuf);
  // Send message to player
  pDnodeActor->PlayerOut += TargetNameSave;
  pDnodeActor->PlayerOut += " is now level ";
  pDnodeActor->PlayerOut += LevelString;
  pDnodeActor->PlayerOut += ".";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  // Send message to target
  pDnodeTgt->PlayerOut += "\r\n";
  pDnodeTgt->PlayerOut += "&Y";
  pDnodeTgt->PlayerOut += PlayerNameSave;
  if (Level > pDnodeTgt->pPlayer->Level)
  { // Level up!
    pDnodeTgt->PlayerOut += " has advanced you to level ";
  }
  else
  { // Level down :(
    pDnodeTgt->PlayerOut += " has DEMOTED you to level ";
  }
  pDnodeTgt->PlayerOut += LevelString;
  pDnodeTgt->PlayerOut += "!";
  pDnodeTgt->PlayerOut += "&N";
  pDnodeTgt->PlayerOut += "\r\n";
  // Make it so
  pDnodeTgt->pPlayer->Level      = Level;
  pDnodeTgt->pPlayer->Experience = Player::CalcLevelExperience(Level);
  pDnodeTgt->pPlayer->Save();
  // Prompt
  pDnodeTgt->pPlayer->CreatePrompt();
  pDnodeTgt->PlayerOut += pDnodeTgt->pPlayer->GetOutput();
  // Restore the player as a bonus to being advanced
  DoRestore("restore " + pDnodeTgt->pPlayer->Name);
}

/***********************************************************
 * Afk command                                             *
 ***********************************************************/

void Communication::DoAfk()
{
  if (pDnodeActor->PlayerStateAfk)
  { // Player returning from AFK
    pDnodeActor->PlayerStateAfk = false;
    pDnodeActor->PlayerOut += "You are no longer Away From Keyboard";
    pDnodeActor->PlayerOut += "\r\n";
  }
  else
  { // Player going AFK
    pDnodeActor->PlayerOut += "You are now Away From Keyboard";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerStateAfk = true;
  }
  pDnodeActor->pPlayer->Save();
  // Create player prompt
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Assist command                                          *
 ***********************************************************/

void Communication::DoAssist()
{
  CString  AssistMsg;
  CString  MobileId;
  CString  PlayerNameCheck;
  CString  TargetNameCheck;
  CString  TargetNameSave;
  bool     TargetNotHere;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (IsFighting())
  { // Player is fighting, send msg, command is not done
    return;
  }
  if (Utility::WordCount(CmdStr) < 2)
  { // No object or target
    pDnodeActor->PlayerOut += "Assist whom?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  PlayerNameCheck = pDnodeActor->PlayerName;
  TargetNameCheck = Utility::GetWord(CmdStr, 2);
  TargetNameSave  = TargetNameCheck;
  PlayerNameCheck.MakeLower();
  TargetNameCheck.MakeLower();
  if (PlayerNameCheck == TargetNameCheck)
  { // Player is trying to assist themself
    pDnodeActor->PlayerOut += "You can't assist youself.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  //*********************
  //* Turning assist on *
  //*********************
  if (TargetNameCheck == "on")
  {
    pDnodeActor->pPlayer->AllowAssist = true;
    pDnodeActor->PlayerOut += "You are now accepting assists.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //**********************
  //* Turning assist off *
  //**********************
  if (TargetNameCheck == "off")
  {
    pDnodeActor->pPlayer->AllowAssist = false;
    pDnodeActor->PlayerOut += "You are now rejecting assists.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //******************
  //* Is target OK ? *
  //******************
  TargetNotHere = false;
  pDnodeTgt = GetTargetDnode(TargetNameCheck);
  if (!pDnodeTgt)
  { // Target is not online and/or not in 'playing' state
    TargetNotHere = true;
  }
  else
  { // Target is online and playing
    if (pDnodeActor->pPlayer->RoomId != pDnodeTgt->pPlayer->RoomId)
    { // Target is not in the same room
      TargetNotHere = true;
    }
  }
  if (TargetNotHere)
  { // Tell player that target is not here
    pDnodeActor->PlayerOut += TargetNameSave;
    pDnodeActor->PlayerOut += " is not here.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  if (!pDnodeTgt->PlayerStateFighting)
  { // Tell player that target is not fighting
    pDnodeActor->PlayerOut += pDnodeTgt->pPlayer->Name;
    pDnodeActor->PlayerOut += " is not fighting.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (!pDnodeTgt->pPlayer->AllowAssist)
  { // Tell player that target is not accepting assistance
    pDnodeActor->PlayerOut += pDnodeTgt->pPlayer->Name;
    pDnodeActor->PlayerOut += " is not accepting assistance.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //******************
  //* Send to player *
  //******************
  pDnodeActor->PlayerOut += "You begin assisting ";
  pDnodeActor->PlayerOut += pDnodeTgt->pPlayer->Name;
  pDnodeActor->PlayerOut += ".\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  //******************
  //* Send to target *
  //******************
  pDnodeTgt->PlayerOut += "\r\n";
  pDnodeTgt->PlayerOut += pDnodeActor->pPlayer->Name;
  pDnodeTgt->PlayerOut += " begins assisting you.\r\n";
  pDnodeTgt->pPlayer->CreatePrompt();
  pDnodeTgt->PlayerOut += pDnodeTgt->pPlayer->GetOutput();  
  //****************
  //* Send to room *
  //****************
  AssistMsg =  pDnodeActor->PlayerName;
  AssistMsg += " begins assisting ";
  AssistMsg += pDnodeTgt->pPlayer->Name;
  AssistMsg += ".";
  pDnodeSrc = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, AssistMsg);
  //**************************
  //* Make the assist happen *
  //**************************
  MobileId = Violence::GetPlayerMobMobileId(pDnodeTgt->PlayerName);
  Mobile::CreatePlayerMob(pDnodeActor->PlayerName, MobileId);
  pDnodeActor->PlayerStateFighting = true;
}

/***********************************************************
* Buy command                                              *
************************************************************/

void Communication::DoBuy()
{
  Object  *pObject;
  int      Cost;
  CString  Desc1;
  CString  ObjectId;
  CString  ObjectName;
  CString  RoomId;
  CString  TmpStr;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (IsFighting())
  { // Player is fighting, send msg, command is not done
    return;
  }
  RoomId = pDnodeActor->pPlayer->RoomId;
  if (!Shop::IsShop(RoomId))
  { // Room is not a shop
    pDnodeActor->PlayerOut += "Find a shop.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  ObjectName = Utility::GetWord(CmdStr, 2);
  if (ObjectName == "")
  { // No object given
    pDnodeActor->PlayerOut += "Buy what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (Utility::WordCount(CmdStr) > 2)
  { // Buy command not only takes 1 object
    pDnodeActor->PlayerOut += "The buy command must be followed by only one word.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pObject = Shop::IsShopObj(RoomId, ObjectName);
  if (!pObject)
  { // Object not in shop for player to buy
    pDnodeActor->PlayerOut += "That item cannot be bought here.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  ObjectId = pObject->ObjectId;
  Desc1    = pObject->Desc1;
  Cost     = pObject->Cost;
  delete pObject;
  if (pDnodeActor->pPlayer->Silver < Cost)
  { // Player cannot afford item
    pDnodeActor->PlayerOut += "You cannot afford that item.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //*******************
  //* Buy the object *
  //*******************
  // Add object to player's inventory
  Object::AddObjToPlayerInv(pDnodeActor, ObjectId);
  // Player receives some money
  pDnodeActor->pPlayer->SetMoney('-', Cost, "Silver");
  // Send messages
  sprintf(Buf, "%d", Cost);
  TmpStr = ConvertStringToCString(Buf);
  pDnodeActor->PlayerOut += "You buy ";
  pDnodeActor->PlayerOut += Desc1;
  pDnodeActor->PlayerOut += " for ";
  pDnodeActor->PlayerOut += TmpStr;
  pDnodeActor->PlayerOut += " piece(s) of silver.";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Chat command                                            *
 ***********************************************************/

void Communication::DoChat()
{
  CString AllMsg;
  CString ChatMsg;
  CString PlayerMsg;

  //********************
  //* Validate command *
  //********************
  ChatMsg = Utility::GetWords(CmdStr, 2);
  if (ChatMsg.GetLength() < 1)
  { // Player did not enter any chat
    pDnodeActor->PlayerOut += "You start to chat, but, about what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //*************
  //* Chat away *
  //*************
  PlayerMsg  = "&G";
  PlayerMsg += "You Chat: ";
  PlayerMsg += ChatMsg;
  PlayerMsg += "&N";
  PlayerMsg += "\r\n";
  AllMsg  = "\r\n";
  AllMsg += "&G";
  AllMsg += pDnodeActor->PlayerName;
  AllMsg += " chats: ";
  AllMsg += ChatMsg;
  AllMsg += "&N";
  AllMsg += "\r\n";
  SendToAll(PlayerMsg, AllMsg);
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
* Color command                                            *
************************************************************/

void Communication::DoColor()
{
  CString TmpStr;

  TmpStr = Utility::GetWord(CmdStr, 2);
  TmpStr.MakeLower();
  if (TmpStr == "on")
  { // Turn color on
    pDnodeActor->pPlayer->Color = true;
    pDnodeActor->pPlayer->Save();
    pDnodeActor->PlayerOut += "You will now see &RP&Gr&Ye&Bt&Mt&Cy&N &RC&Go&Yl&Bo&Mr&Cs&N.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (TmpStr == "off")
  { // Turn color off
    pDnodeActor->pPlayer->Color = false;
    pDnodeActor->pPlayer->Save();
    pDnodeActor->PlayerOut += "Color is &Moff&N.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (pDnodeActor->pPlayer->Color)
  { // Color is on
    pDnodeActor->PlayerOut += "&CColor&N is &Mon&N.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  }
  else
  { // Color is off
    pDnodeActor->PlayerOut += "Color is off.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  }
}

/***********************************************************
* Consider command                                         *
************************************************************/

void Communication::DoConsider()
{
  Mobile  *pMobile;
  CString  HintMsg;
  int      LevelDiff;
  CString  MobileName;
  CString  PlayerName;
  CString  PlayerNameCheck;
  CString  RoomId;
  CString  Target;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (Utility::WordCount(CmdStr) < 2)
  { // No target
    pDnodeActor->PlayerOut += "Consider whom or what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (Utility::WordCount(CmdStr) > 2)
  { // Two many targets
    pDnodeActor->PlayerOut += "Hmm, you are confused. Try 'help consider'.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  PlayerName      = pDnodeActor->PlayerName;
  RoomId          = pDnodeActor->pPlayer->RoomId;
  PlayerNameCheck = PlayerName;
  PlayerNameCheck.MakeLower();
  Target = Utility::GetWord(CmdStr, 2);
  MobileName = Target;
  Target.MakeLower();
  if (Target == PlayerNameCheck)
  { // Trying to kill self
    pDnodeActor->PlayerOut += "Consider yourself considered!";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (Player::IsPlayer(Target))
  { // Trying to kill another player
    pDnodeActor->PlayerOut += "Why consider another player? Player killing is not allowed.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pMobile = Mobile::IsMobInRoom(Target);
  if (!pMobile)
  { // Target mobile is not here
    pDnodeActor->PlayerOut += "There doesn't seem to be a(n) ";
    pDnodeActor->PlayerOut += MobileName;
    pDnodeActor->PlayerOut += " here.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  // Build message based on level difference
  LevelDiff = pMobile->Level - pDnodeActor->pPlayer->Level;
  if (LevelDiff < -6)
  {
    HintMsg = "&GDon't bother&N";
  }
  else
  if (LevelDiff == -6)
  {
    HintMsg = "&GWay too easy&N";
  }
  else
  if (LevelDiff == -5)
  {
    HintMsg = "&GVery easy&N";
  }
  else
  if (LevelDiff == -4)
  {
    HintMsg = "&CEasy&N";
  }
  else
  if (LevelDiff == -3)
  {
    HintMsg = "&CNo problem&N";
  }
  else
  if (LevelDiff == -2)
  {
    HintMsg = "&CA worthy opponent&N";
  }
  else
  if (LevelDiff == -1)
  {
    HintMsg = "&YYou might win&N";
  }
  else
  if (LevelDiff == 0)
  {
    HintMsg = "&YTough fight&N";
  }
  else
  if (LevelDiff == 1)
  {
    HintMsg = "&YLots of luck&N";
  }
  else
  if (LevelDiff == 2)
  {
    HintMsg = "&RBad idea&N";
  }
  else
  if (LevelDiff > 2)
  {
    HintMsg = "&RYOU ARE MAD&N";
  }

  //*****************
  //* Send messages *
  //*****************
  // Send message to player
  pDnodeActor->PlayerOut += "You consider ";
  pDnodeActor->PlayerOut += pMobile->Desc1;
  pDnodeActor->PlayerOut += ".";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += HintMsg;;
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  delete pMobile;
}

/***********************************************************
* Delete command                                           *
************************************************************/

void Communication::DoDelete()
{
  CString  AllMsg;
  CString  LogBuf;
  CString  Name;
  CString  Password;
  CString  Phrase;
  CString  PlayerFileName;
  CString  PlayerMsg;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (IsFighting())
  { // Player is fighting, send msg, command is not done
    return;
  }
  if (Utility::WordCount(CmdStr) < 3)
  {
    pDnodeActor->PlayerOut += "You must provide your name and password.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  Name     = Utility::GetWord(CmdStr, 2);
  Password = Utility::GetWord(CmdStr, 3);
  Phrase   = Utility::GetWords(CmdStr, 4);
  if (Name != pDnodeActor->PlayerName)
  {
    pDnodeActor->PlayerOut += "Your name was not entered correctly.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "Upper and lowercase letters must match.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (Password != ConvertStringToCString(pDnodeActor->pPlayer->Password))
  {
    pDnodeActor->PlayerOut += "Your password was not entered correctly.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "Upper and lowercase letters must match.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (Phrase == "")
  {
    pDnodeActor->PlayerOut += "If you really want delete your character,";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "add the phrase: MAKE IT SO";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "to the end of the command.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "Like this: delete <name> <password> MAKE IT SO";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "and you will be immediately DELETED!!";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (Phrase != "MAKE IT SO")
  {
    pDnodeActor->PlayerOut += "Ok, it seems that you provided a phrase";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "but it is _not_ 'MAKE IT SO'.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "The command must be like this: delete <name> <password> MAKE IT SO";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //*****************
  //* Delete player *
  //*****************
  pDnodeActor->PlayerStateBye = true;
  pDnodeActor->PlayerStatePlaying = false;
  GrpLeave();
  LogBuf  = pDnodeActor->PlayerName;
  LogBuf += " issued the DELETE command";
  Log::LogIt(LogBuf);
  // Delete Player file
  PlayerFileName =  PLAYER_DIR;
  PlayerFileName += pDnodeActor->PlayerName;
  PlayerFileName += ".txt";
  TRY
  {
    CFile::Remove(PlayerFileName);
  }
  CATCH (CFileException, e)
  { // It should be impossible for this delete to fail
    AfxMessageBox("Communication::DoDelete - Remove Player file failed", MB_ICONSTOP);
    _endthread();
  }
  END_CATCH
  // Delete PlayerEqu file
  PlayerFileName =  PLAYER_EQU_DIR;
  PlayerFileName += pDnodeActor->PlayerName;
  PlayerFileName += ".txt";
  TRY
  {
    CFile::Remove(PlayerFileName);
  }
  CATCH (CFileException, e)
  { // Don't care if delete fails
  }
  END_CATCH
  // Delete PlayerObj file
  PlayerFileName =  PLAYER_OBJ_DIR;
  PlayerFileName += pDnodeActor->PlayerName;
  PlayerFileName += ".txt";
  TRY
  {
    CFile::Remove(PlayerFileName);
  }
  CATCH (CFileException, e)
  { // Don't care if delete fails
  }
  END_CATCH
  // Delete PlayerRoom file
  PlayerFileName =  PLAYER_ROOM_DIR;
  PlayerFileName += pDnodeActor->PlayerName;
  PlayerFileName += ".txt";
  TRY
  {
    CFile::Remove(PlayerFileName);
  }
  CATCH (CFileException, e)
  { // Don't care if delete fails
  }
  END_CATCH
  // Send messages
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = NULL;
  PlayerMsg  = "You have been DELETED!!!";
  PlayerMsg += "\r\n";
  AllMsg  = "\r\n";
  AllMsg += pDnodeActor->PlayerName;
  AllMsg += " has just DELETED $pHimselfHerself.";
  AllMsg += "\r\n";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = NULL;
  AllMsg  = Utility::PronounSubstitute(AllMsg);
  pDnodeSrc = NULL;
  pDnodeTgt = NULL;
  SendToAll(PlayerMsg, AllMsg);
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Destroy command                                         *
 ***********************************************************/

void Communication::DoDestroy()
{
  Object  *pObject;
  CString  ObjectName;
  CString  TmpStr;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (Utility::WordCount(CmdStr) == 1)
  { // Invalid command format
    pDnodeActor->PlayerOut += "Destroy what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //****************************
  //* Does player have object? *
  //****************************
  TmpStr = Utility::GetWord(CmdStr, 2);
  ObjectName = TmpStr;
  TmpStr.MakeLower();
  pObject = Object::IsObjInPlayerInv(TmpStr);
  if (!pObject)
  {
    pDnodeActor->PlayerOut += "You don't have a(n) ";
    pDnodeActor->PlayerOut += ObjectName;
    pDnodeActor->PlayerOut += ".\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  //******************
  //* Destroy object *
  //******************
  // Remove object from player's inventory
  Object::RemoveObjFromPlayerInv(pObject->ObjectId, 1);
  // Send messages
  pDnodeActor->PlayerOut += "You help make the world cleaner by destroying ";
  pDnodeActor->PlayerOut += pObject->Desc1;
  pDnodeActor->PlayerOut += ".\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();        
  delete pObject;
}

/***********************************************************
 * Drink command                                           *
 ***********************************************************/

void Communication::DoDrink()
{
  Object  *pObject;
  CString  DrinkMsg;
  CString  ObjectName;
  CString  RoomId;
  string   sRoomId;
  CString  RoomName;
  CString  RoomType;
  CString  TmpStr;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (Utility::GetWord(CmdStr, 2) == "from")
  { // Toss out 'from', just extra verbage for player's benefit
    CmdStr.Delete(5,5);
  }
  if (Utility::WordCount(CmdStr) == 1)
  { // Invalid command format
    pDnodeActor->PlayerOut += "Drink from what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //*************************
  //* Is this a drink room? *
  //*************************
  RoomId   = pDnodeActor->pPlayer->RoomId;
  sRoomId = ConvertCStringToString(RoomId);
  if (Room::IsRoomType(sRoomId, "Drink"))
  { // Room contains something to drink
    string sRoomName;
    sRoomName = Room::GetRoomName(sRoomId);
    TmpStr = Utility::GetWord(CmdStr, 2);
    TmpStr.MakeLower();
    RoomName.MakeLower();
    if (Utility::IsWord(TmpStr, RoomName))
    { //*****************
      //* Player drinks *
      //*****************
      // Send messages
      pDnodeActor->PlayerOut += "You take a drink of clear &Ccool&N water.";
      pDnodeActor->PlayerOut += "\r\n";
      DrinkMsg  = pDnodeActor->PlayerName;
      DrinkMsg += " takes a drink of clear &Ccool&N water.";
      pDnodeSrc = pDnodeActor;
      pDnodeTgt = pDnodeActor;
      SendToRoom(pDnodeActor->pPlayer->RoomId, DrinkMsg);
      // Update thirst
      pDnodeActor->pPlayer->Drink(20);
      pDnodeActor->pPlayer->Save();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      // Prompt
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
  }
  //****************************
  //* Does player have object? *
  //****************************
  TmpStr = Utility::GetWord(CmdStr, 2);
  ObjectName = TmpStr;
  TmpStr.MakeLower();
  pObject = Object::IsObjInPlayerInv(TmpStr);
  if (!pObject)
  { // Object not found in player's inventory
    pDnodeActor->PlayerOut += "You don't have a(n) ";
    pDnodeActor->PlayerOut += ObjectName;
    pDnodeActor->PlayerOut += " in your inventory.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  //*******************
  //* Is object drink? *
  //*******************
  pObject->Type.MakeLower();
  if (pObject->Type != "drink")
  { // Object is not a drink
    pDnodeActor->PlayerOut += "You can't drink ";
    pDnodeActor->PlayerOut += pObject->Desc1;
    pDnodeActor->PlayerOut += ".";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  //****************
  //* Drink object *
  //****************
  // Send messages
  pDnodeActor->PlayerOut += "You drink from ";
  pDnodeActor->PlayerOut += pObject->Desc1;
  pDnodeActor->PlayerOut += ".";
  pDnodeActor->PlayerOut += "\r\n";
  DrinkMsg  = pDnodeActor->PlayerName;
  DrinkMsg += " drinks from ";
  DrinkMsg += pObject->Desc1;
  DrinkMsg += ".";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, DrinkMsg);
  // Drink and remove object from player's inventory
  pDnodeActor->pPlayer->Drink(pObject->DrinkPct);
  pDnodeActor->pPlayer->Save();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  Object::RemoveObjFromPlayerInv(pObject->ObjectId, 1);
  // Clean up and give prompt
  delete pObject;
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Drop command                                            *
 ***********************************************************/

void Communication::DoDrop()
{
  Object  *pObject;
  CString  DropMsg;
  CString  ObjectName;
  CString  TmpStr;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (Utility::WordCount(CmdStr) == 1)
  { // Invalid command format
    pDnodeActor->PlayerOut += "Drop what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //****************************
  //* Does player have object? *
  //****************************
  TmpStr = Utility::GetWord(CmdStr, 2);
  ObjectName = TmpStr;
  TmpStr.MakeLower();
  pObject = Object::IsObjInPlayerInv(TmpStr);
  if (!pObject)
  {
    pDnodeActor->PlayerOut += "You don't have a(n) ";
    pDnodeActor->PlayerOut += ObjectName;
    pDnodeActor->PlayerOut += " in your inventory.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  //***************
  //* Drop object *
  //***************
  // Remove object from player's inventory
  Object::RemoveObjFromPlayerInv(pObject->ObjectId, 1);
  // Send messages
  pDnodeActor->PlayerOut += "You drop ";
  pDnodeActor->PlayerOut += pObject->Desc1;
  pDnodeActor->PlayerOut += ".";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  DropMsg = pDnodeActor->PlayerName;
  DropMsg += " drops ";
  DropMsg += pObject->Desc1;
  DropMsg += ".";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, DropMsg);
  // Add object to room
  Object::AddObjToRoom(pDnodeActor->pPlayer->RoomId, pObject->ObjectId);
  delete pObject;
}

/***********************************************************
 * Eat command                                             *
 ***********************************************************/

void Communication::DoEat()
{
  Object  *pObject;
  CString  EatMsg;
  CString  ObjectName;
  CString  TmpStr;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (Utility::WordCount(CmdStr) == 1)
  { // Invalid command format
    pDnodeActor->PlayerOut += "Eat what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //****************************
  //* Does player have object? *
  //****************************
  TmpStr = Utility::GetWord(CmdStr, 2);
  ObjectName = TmpStr;
  TmpStr.MakeLower();
  pObject = Object::IsObjInPlayerInv(TmpStr);
  if (!pObject)
  {
    pDnodeActor->PlayerOut += "You don't have a(n) ";
    pDnodeActor->PlayerOut += ObjectName;
    pDnodeActor->PlayerOut += " in your inventory.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  //*******************
  //* Is object food? *
  //*******************
  pObject->Type.MakeLower();
  if (pObject->Type != "food")
  {
    pDnodeActor->PlayerOut += "You can't eat ";
    pDnodeActor->PlayerOut += pObject->Desc1;
    pDnodeActor->PlayerOut += ".";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  //**************
  //* Eat object *
  //**************
  // Send messages
  pDnodeActor->PlayerOut += "You eat ";
  pDnodeActor->PlayerOut += pObject->Desc1;
  pDnodeActor->PlayerOut += ".";
  pDnodeActor->PlayerOut += "\r\n";
  EatMsg  = pDnodeActor->PlayerName;
  EatMsg += " eats ";
  EatMsg += pObject->Desc1;
  EatMsg += ".";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, EatMsg);
  // Eat and remove object from player's inventory
  pDnodeActor->pPlayer->Eat(pObject->FoodPct);
  pDnodeActor->pPlayer->Save();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  Object::RemoveObjFromPlayerInv(pObject->ObjectId, 1);
  // Clean up and give prompt
  delete pObject;
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Emote command                                           *
 ***********************************************************/

void Communication::DoEmote()
{
  CString EmoteMsg;
  CString TmpStr;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  EmoteMsg = Utility::GetWords(CmdStr, 2);
  if (EmoteMsg.GetLength() < 1)
  { // Player did not enter anything to say
    pDnodeActor->PlayerOut += "You try to show emotion, but fail.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //*******************
  //* Emote something *
  //*******************
  
  pDnodeActor->PlayerOut += "&C";
  pDnodeActor->PlayerOut += pDnodeActor->PlayerName;
  pDnodeActor->PlayerOut += " ";
  pDnodeActor->PlayerOut += EmoteMsg;
  pDnodeActor->PlayerOut += "&N";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  TmpStr = "&C";
  TmpStr += pDnodeActor->PlayerName;
  TmpStr += " ";
  TmpStr += EmoteMsg;
  TmpStr += "&N";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, TmpStr);
}

/***********************************************************
 * Equipment command                                       *
 ***********************************************************/

void Communication::DoEquipment()
{
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  Object::ShowPlayerEqu(pDnodeActor);
}

/***********************************************************
 * Examine command                                         *
 ***********************************************************/

void Communication::DoExamine()
{
  Object  *pObject;
  bool     ObjectFound;
  CString  ObjectName;
  CString  ObjectType;
  CString  TmpStr;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (IsFighting())
  { // Player is fighting, send msg, command is not done
    return;
  }
  if (Utility::WordCount(CmdStr) == 1)
  { // Invalid command format
    pDnodeActor->PlayerOut += "Examine what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //***********************************************
  //* Ok, object, object ... where is the object? *
  //***********************************************
  ObjectFound = false;
  TmpStr = Utility::GetWord(CmdStr, 2);
  ObjectName = TmpStr;
  TmpStr.MakeLower();
  // Check room
  pObject = Object::IsObjInRoom(TmpStr);
  if (pObject)
  { // Object is in the room
    ObjectFound = true;
  }
  else
  { // Check player inventory
    pObject = Object::IsObjInPlayerInv(TmpStr);
    if (pObject)
    { // Object is in player's inventory
      ObjectFound = true;
    }
    else
    { // Check player equipment
      pObject = Object::IsObjInPlayerEqu(TmpStr);
      if (pObject)
      { // Object is in player's equipment
        ObjectFound = true;
      }
    }
  }
  if (!ObjectFound)
  { // Object can't be found
    pDnodeActor->PlayerOut += "There doesn't seem to be a(n) ";
    pDnodeActor->PlayerOut += ObjectName;
    pDnodeActor->PlayerOut += " here.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //************************************
  //* Ojbect was found, now examine it *
  //************************************
  // Send messages
  pDnodeActor->PlayerOut += "You examine ";
  pDnodeActor->PlayerOut += pObject->Desc1;
  pDnodeActor->PlayerOut += ".";
  pDnodeActor->PlayerOut += "\r\n";
  // Examine object
  pDnodeActor->PlayerOut += "Object type: ";
  pDnodeActor->PlayerOut += pObject->Type;
  pDnodeActor->PlayerOut += "\r\n";
  ObjectType = pObject->Type;
  ObjectType.MakeLower();
  if (ObjectType == "weapon")
  { // Object is a weapon
    pDnodeActor->PlayerOut += "Weapon type: ";
    pDnodeActor->PlayerOut += pObject->WeaponType;
    pDnodeActor->PlayerOut += "\r\n";
  }
  pObject->ExamineObj(pObject->ObjectId);
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  delete pObject;
}

/***********************************************************
 * Flee command                                            *
 ***********************************************************/

void Communication::DoFlee()
{
  int         CandidateCount;
  CString     CandidateList;
  int         CandidateTarget;
  CFileStatus FileStatus;
  CString     FleeMsg;
  CString     MobileId;
  CString     MobileIdSave;
  CString     MobPlayerFileName;
  CString     MudCmdIsExit;
  CString     PlayerName1;
  CString     PlayerName2;
  CString     RoomIdBeforeFleeing;
  CString     Target;
  CString     TmpStr;

  //********************
  //* Validate command *
  //********************
  if (!pDnodeActor->PlayerStateFighting)
  {
    pDnodeActor->PlayerOut += "You can't flee, you aren't fighting.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  TmpStr = Utility::GetWord(CmdStr, 2);
  if (TmpStr == "")
  { // No direction given
    pDnodeActor->PlayerOut += "Aimless fleeing is not allowed.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  RoomIdBeforeFleeing = pDnodeActor->pPlayer->RoomId;
  MudCmdIsExit = "go";
  string sMudCmdIsExit;
  sMudCmdIsExit = ConvertCStringToString(MudCmdIsExit);
  if (!Room::IsExit(sMudCmdIsExit))
  { // Direction given is not valid
    pDnodeActor->PlayerOut += "Flee where?\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //************************************
  //* Player has been moved, they fled *
  //************************************
  // Let everyone in room know they fled
  FleeMsg += "&R";
  FleeMsg += pDnodeActor->PlayerName;
  FleeMsg += " has fled for $pHisHers life!!";
  FleeMsg += "&N";
  FleeMsg = Utility::PronounSubstitute(FleeMsg);
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(RoomIdBeforeFleeing, FleeMsg);
  // Let player know they succeeded in fleeing
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "&R";
  pDnodeActor->PlayerOut += "You have sucessfully fled for your life!!";
  pDnodeActor->PlayerOut += "&N";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  //***********
  //* Cleanup *
  //***********
  pDnodeActor->PlayerStateFighting = false;
  if (pDnodeActor->pPlayer->pPlayerFollowers[0] != NULL)
  { // Player is following someone, stop following
    DoFollow(pDnodeActor, "follow none");
  }
  PlayerName1 = pDnodeActor->PlayerName;
  // Get mobile id for mob that fleeing player was fighting
  MobileIdSave = Violence::GetPlayerMobMobileId(PlayerName1);
  // Delete PlayerMob file -- player is no longer attacking mob
  Mobile::DeletePlayerMob(PlayerName1);
  // See if a mob is whacking player
  MobPlayerFileName =  MOB_PLAYER_DIR;
  MobPlayerFileName += PlayerName1;
  MobPlayerFileName += ".txt";
  if (!CFile::GetStatus(MobPlayerFileName, FileStatus))
  { // If MobPlayer does not exist, then no mob is fighting player
    return;
  }
  //***************************
  //* Make mob switch targets *
  //***************************
  // Delete fighting mobiles from MobPlayer file
  Descriptor::SetpDnodeCursorFirst();
  while (!Descriptor::EndOfDnodeList())
  { // Loop thru all connections
    pDnodeOthers = Descriptor::GetDnode();
    if (pDnodeOthers->PlayerStateFighting)
    { // Players who are fighting
      if (RoomIdBeforeFleeing == pDnodeOthers->pPlayer->RoomId)
      { // In the same room
        PlayerName2 = pDnodeOthers->PlayerName;
        MobileId = Violence::GetPlayerMobMobileId(PlayerName2);
        Mobile::DeleteMobPlayer(PlayerName1, MobileId);
        if (MobileId == MobileIdSave)
        { // Add player to candidate list for MobileIdSave
          CandidateList += PlayerName2;
          CandidateList += " ";
        }
      }
    }
    Descriptor::SetpDnodeCursorNext();
  }
  // Re-position pDnodeCursor
  RepositionDnodeCursor();
  // Put mobiles that are not fighting back in room
  Mobile::PutMobBackInRoom(PlayerName1, RoomIdBeforeFleeing);
  // Player is gone, so delete MobPlayer completely
  Mobile::DeleteMobPlayer(PlayerName1, "file");
  // Select a new target for MobileIdSave
  if (CandidateList.GetLength() == 0)
  { // No available target for MobileIdSave
    return;
  }
  CandidateCount  = Utility::WordCount(CandidateList);
  CandidateTarget = Utility::GetRandomNumber(CandidateCount);
  Target          = Utility::GetWord(CandidateList, CandidateTarget);
  Mobile::CreateMobPlayer(Target, MobileIdSave);
}

/***********************************************************
 * Follow command                                          *
 ***********************************************************/

void Communication::DoFollow(Dnode *pDnode, CString CmdStr1)
{
  Dnode   *pDnodeGrpLdr; // Group leader
  Dnode   *pDnodeGrpMem; // Group member
  int      i;
  int      j;
  CString  Target;
  bool     TargetInGroup;
  CString  TmpStr;

  CmdStr = CmdStr1;
  i      = 0;
  j      = 0;
  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  Target = Utility::GetWord(CmdStr, 2);
  TmpStr = Target;
  TmpStr.MakeLower();
  if (Target == "")
  { // Follow with no target
    pDnode->PlayerOut += "Who would you like to follow?\r\n";
    pDnode->pPlayer->CreatePrompt();
    pDnode->PlayerOut += pDnode->pPlayer->GetOutput();
    return;
  }
  //******************
  //* Stop following *
  //******************
  if (TmpStr == "none")
  { // Player wants to stop following
    if (pDnode->pPlayer->pPlayerFollowers[0] == NULL)
    { // Player is not following anyone
      pDnode->PlayerOut += "Ok. But alas, you were not following anyone";
      pDnode->PlayerOut += "\r\n";
      pDnode->pPlayer->CreatePrompt();
      pDnode->PlayerOut += pDnode->pPlayer->GetOutput();
      return;
    }
    for (i = 1; i < GRP_LIMIT; i++)
    { // Search thru player-being-followed's list of followers
      if (pDnode->pPlayer == pDnode->pPlayer->pPlayerFollowers[0]->pPlayerFollowers[i])
      { // Found player, set to null
        pDnode->pPlayer->pPlayerFollowers[0]->pPlayerFollowers[i] = NULL;
        j = i; // Save player's subscript
        break;
      }
    }
    // Compact the list of followers, so new followers are at the end
    for (i = j; i < GRP_LIMIT-1; i++) // j is subscript of follower who has stopped following
    {
      pDnode->pPlayer->pPlayerFollowers[0]->pPlayerFollowers[i]   = pDnode->pPlayer->pPlayerFollowers[0]->pPlayerFollowers[i+1];
      pDnode->pPlayer->pPlayerFollowers[0]->pPlayerFollowers[i+1] = NULL;
    }
    pDnode->PlayerOut += "You stop following ";
    pDnode->PlayerOut += pDnode->pPlayer->pPlayerFollowers[0]->Name;
    pDnode->PlayerOut += ".\r\n";
    pDnode->pPlayer->CreatePrompt();
    pDnode->PlayerOut += pDnode->pPlayer->GetOutput();
    pDnode->pPlayer->pPlayerFollowers[0] = NULL;
    return;
  }
  //******************
  //* List followers *
  //******************
  if (TmpStr == "list")
  {
    if (pDnode->pPlayer->pPlayerFollowers[0] == NULL)
    {
      pDnode->PlayerOut += "You are not following anyone.\r\n";
    }
    else
    {
      pDnode->PlayerOut += "You are following ";
      pDnode->PlayerOut += pDnode->pPlayer->pPlayerFollowers[0]->Name;
      pDnode->PlayerOut += ".\r\n";
    }
    for (i = 1; i < GRP_LIMIT; i++)
    {
      if (pDnode->pPlayer->pPlayerFollowers[i] != NULL)
      {
        if (i == 1)
        {
          pDnode->PlayerOut += "Followers\r\n";
          pDnode->PlayerOut += "---------\r\n";
        }
        pDnode->PlayerOut += pDnode->pPlayer->pPlayerFollowers[i]->Name;
        pDnode->PlayerOut += "\r\n";
      }
    }
    pDnode->pPlayer->CreatePrompt();
    pDnode->PlayerOut += pDnode->pPlayer->GetOutput();
    return;
  }
  //*******************************
  //* Player is already following *
  //*******************************
  if (pDnode->pPlayer->pPlayerFollowers[0] != NULL)
  {
    pDnode->PlayerOut += "You are already following ";
    pDnode->PlayerOut += pDnode->pPlayer->pPlayerFollowers[0]->Name;
    pDnode->PlayerOut += ".\r\n";
    pDnode->pPlayer->CreatePrompt();
    pDnode->PlayerOut += pDnode->pPlayer->GetOutput();
    return;
  }
  //******************
  //* Target online? *
  //******************
  pDnodeTgt = GetTargetDnode(Target);
  if (!pDnodeTgt)
  { // Target not online
    pDnode->PlayerOut += Target;
    pDnode->PlayerOut += " is not online.\r\n";
    pDnode->pPlayer->CreatePrompt();
    pDnode->PlayerOut += pDnode->pPlayer->GetOutput();
    return;
  }
  //***************************
  //* Can not follow yourself *
  //***************************
  if (pDnode == pDnodeTgt)
  { // Player trying to follow themself
    pDnode->PlayerOut += "You can not follow yourself.\r\n";
    pDnode->pPlayer->CreatePrompt();
    pDnode->PlayerOut += pDnode->pPlayer->GetOutput();
    return;
  }
  //*************************************
  //* Player and target must be grouped *
  //*************************************
  if (pDnode->pPlayer->pPlayerGrpMember[0] == NULL)
  { // Player is not in a group
    pDnode->PlayerOut += "You must be in a group before you can follow.";
    pDnode->PlayerOut += "\r\n";
    pDnode->pPlayer->CreatePrompt();
    pDnode->PlayerOut += pDnode->pPlayer->GetOutput();
    return;
  }
  pDnodeGrpLdr = GetTargetDnode(pDnode->pPlayer->pPlayerGrpMember[0]->Name);
  TargetInGroup = false;
  for (i = 0; i < GRP_LIMIT; i++)
  { // For each member of leader's group including the leader
    if (pDnodeGrpLdr->pPlayer->pPlayerGrpMember[i] == NULL)
    { // Done looping through group members
      break;
    }
    // Get group member's dnode
    pDnodeGrpMem = GetTargetDnode(pDnodeGrpLdr->pPlayer->pPlayerGrpMember[i]->Name);
    if (pDnodeGrpMem == pDnodeTgt)
    { // Target member found
      TargetInGroup = true;
    }
  }
  if (!TargetInGroup)
  { // Target is not grouped with player
    pDnode->PlayerOut += Target;
    pDnode->PlayerOut += " is not in the group.\r\n";
    pDnode->pPlayer->CreatePrompt();
    pDnode->PlayerOut += pDnode->pPlayer->GetOutput();
    return;
  }
  //************************************
  //* Player can not follow a follower *
  //************************************
  for (i = 1; i < GRP_LIMIT; i++)
  {
    if (pDnodeTgt->pPlayer == pDnode->pPlayer->pPlayerFollowers[i])
    {
      pDnode->PlayerOut += "Can not! ";
      pDnode->PlayerOut += pDnodeTgt->pPlayer->Name;
      pDnode->PlayerOut += " is following YOU.\r\n";
      pDnode->pPlayer->CreatePrompt();
      pDnode->PlayerOut += pDnode->pPlayer->GetOutput();
      return;
    }
  }
  //***********************
  //* Follow - make it so *
  //***********************
  // Message to player
  pDnode->PlayerOut += "You begin to follow ";
  pDnode->PlayerOut +=  pDnodeTgt->PlayerName;
  pDnode->PlayerOut += ".\r\n";
  pDnode->pPlayer->CreatePrompt();
  pDnode->PlayerOut += pDnode->pPlayer->GetOutput();
  // Message to player being followed
  pDnodeTgt->PlayerOut += "\r\n";
  pDnodeTgt->PlayerOut +=  pDnode->PlayerName;
  pDnodeTgt->PlayerOut += " begins to follow you.";
  pDnodeTgt->PlayerOut += "\r\n";
  pDnodeTgt->pPlayer->CreatePrompt();
  pDnodeTgt->PlayerOut += pDnodeTgt->pPlayer->GetOutput();
  // Make 'em follow
  pDnode->pPlayer->pPlayerFollowers[0] = pDnodeTgt->pPlayer;
  for (i = 1; i < GRP_LIMIT; i++)
  { // Loop through target's list of followers to find an empty slot
    if (pDnodeTgt->pPlayer->pPlayerFollowers[i] == NULL)
    { // Found a slot for the new follower
      break;
    }
  }
  pDnodeTgt->pPlayer->pPlayerFollowers[i] = pDnode->pPlayer;
}

/***********************************************************
 * Get command                                             *
 ***********************************************************/

void Communication::DoGet()
{
  Object  *pObject;
  CString  GetMsg;
  CString  ObjectName;
  CString  TmpStr;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (Utility::WordCount(CmdStr) == 1)
  { // Invalid command format
    pDnodeActor->PlayerOut += "Get what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //**********************************************
  //* See if object is in room and can be gotten *
  //**********************************************
  TmpStr = Utility::GetWord(CmdStr, 2);
  ObjectName = TmpStr;
  TmpStr.MakeLower();
  pObject = Object::IsObjInRoom(TmpStr);
  if (!pObject)
  {
    pDnodeActor->PlayerOut += "There doesn't seem to be a(n) ";
    pDnodeActor->PlayerOut += ObjectName;
    pDnodeActor->PlayerOut += " here.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  if (pObject->Type == "NoTake" && !pDnodeActor->pPlayer->Admin)
  { // Only administrators can 'get' a 'notake' object
    pDnodeActor->PlayerOut += "You may not take that.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    delete pObject;
    return;
  }
  //**********************
  //* So take the object *
  //**********************
  // Remove object from room
  Object::RemoveObjFromRoom(pObject->ObjectId);
  // Send messages
  pDnodeActor->PlayerOut += "You get ";
  pDnodeActor->PlayerOut += pObject->Desc1;
  pDnodeActor->PlayerOut += ".\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  GetMsg = pDnodeActor->PlayerName;
  GetMsg += " gets ";
  GetMsg += pObject->Desc1;
  GetMsg += ".";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, GetMsg);
  // Add object to player's inventory
  Object::AddObjToPlayerInv(pDnodeTgt, pObject->ObjectId);
  delete pObject;
}

/***********************************************************
 * Give command                                            *
 ***********************************************************/

void Communication::DoGive()
{
  Object  *pObject;
  CString  GiveMsg;
  CString  ObjectName;
  CString  PlayerName;
  CString  TargetName;
  bool     TargetNotHere;
  CString  TmpStr;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (Utility::WordCount(CmdStr) < 2)
  { // No object or target
    pDnodeActor->PlayerOut += "Give what and to whom?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (Utility::WordCount(CmdStr) < 3)
  { // No target
    pDnodeActor->PlayerOut += "Give it to whom?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //****************************
  //* Does player have object? *
  //****************************
  TmpStr = Utility::GetWord(CmdStr, 2);
  ObjectName = TmpStr;
  TmpStr.MakeLower();
  pObject = Object::IsObjInPlayerInv(TmpStr);
  if (!pObject)
  { // Player does not have object
    pDnodeActor->PlayerOut += "You don't have a(n) ";
    pDnodeActor->PlayerOut += ObjectName;
    pDnodeActor->PlayerOut += " in your inventory.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  //****************
  //* Is target Ok *
  //****************
  TargetNotHere = false;
  TmpStr = Utility::GetWord(CmdStr, 3);
  TargetName = TmpStr;
  TmpStr.MakeLower();
  PlayerName = pDnodeActor->PlayerName;
  PlayerName.MakeLower();
  if (PlayerName == TmpStr)
  { // Player is trying to give something to themself
    pDnodeActor->PlayerOut += "Giving something to youself is just plain silly!\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  pDnodeTgt = GetTargetDnode(TmpStr);
  if (!pDnodeTgt)
  { // Target is not online and/or not in 'playing' state
    TargetNotHere = true;
  }
  else
  { // Target is online and playing
    if (pDnodeActor->pPlayer->RoomId != pDnodeTgt->pPlayer->RoomId)
    { // Target is not in the same room
      TargetNotHere = true;
    }
  }
  if (TargetNotHere)
  { // Tell player that target is not here
    pDnodeActor->PlayerOut += TargetName;
    pDnodeActor->PlayerOut += " is not here.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  if (pDnodeTgt->pPlayer->Position == "sleep")
  { // Tell player that target is sleeping
    pDnodeActor->PlayerOut += pDnodeTgt->pPlayer->Name;
    pDnodeActor->PlayerOut += " is sleeping.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //******************
  //* Send to player *
  //******************
  pDnodeActor->PlayerOut += "You give ";
  pDnodeActor->PlayerOut += pObject->Desc1;
  pDnodeActor->PlayerOut += " to ";
  pDnodeActor->PlayerOut += pDnodeTgt->pPlayer->Name;
  pDnodeActor->PlayerOut += ".\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  //******************
  //* Send to target *
  //******************
  pDnodeTgt->PlayerOut += "\r\n";
  pDnodeTgt->PlayerOut += pDnodeActor->pPlayer->Name;
  pDnodeTgt->PlayerOut += " gives you ";
  pDnodeTgt->PlayerOut += pObject->Desc1;
  pDnodeTgt->PlayerOut += ".\r\n";
  pDnodeTgt->pPlayer->CreatePrompt();
  pDnodeTgt->PlayerOut += pDnodeTgt->pPlayer->GetOutput();  
  //****************
  //* Send to room *
  //****************
  GiveMsg =  pDnodeActor->PlayerName;
  GiveMsg += " gives ";
  GiveMsg += pObject->Desc1;
  GiveMsg += " to ";
  GiveMsg += pDnodeTgt->pPlayer->Name;
  GiveMsg += ".";
  pDnodeSrc = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, GiveMsg);
  //*****************************
  //* Transfer object ownership *
  //*****************************
  Object::RemoveObjFromPlayerInv(pObject->ObjectId, 1);
  Object::AddObjToPlayerInv(pDnodeTgt, pObject->ObjectId);
  delete pObject;
}

/***********************************************************
 * Go command                                              *
 ***********************************************************/

void Communication::DoGo()
{
  CString MudCmdIsExit;
  CString TmpStr;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (IsFighting())
  { // Player is fighting, send msg, command is not done
    return;
  }
  if (pDnodeActor->pPlayer->Position == "sit")
  { // Player is sitting
    pDnodeActor->PlayerOut += "You must be standing before you can move.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  TmpStr = Utility::GetWord(CmdStr, 2);
  if (TmpStr == "")
  { // No direction given
    pDnodeActor->PlayerOut += "Aimless wandering is not allowed.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //***************
  //* Try to move *
  //***************
  MudCmdIsExit = "go";
  string sMudCmdIsExit;
  sMudCmdIsExit = ConvertCStringToString(MudCmdIsExit);
  if (Room::IsExit(sMudCmdIsExit))
  { // Player has been moved
    return;
  }
  //********************************
  //* Direction given is not valid *
  //********************************
  pDnodeActor->PlayerOut += "Go where?\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Goto command                                            *
 ***********************************************************/

void Communication::DoGoTo()
{
  CString GoToMsg;
  CString RoomId;
  string  sRoomId;
  CString csRoomId;

  //********************
  //* Validate command *
  //********************
  RoomId = Utility::GetWord(CmdStr, 2);
  if (RoomId == "")
  {
    pDnodeActor->PlayerOut += "A destination is needed.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  sRoomId = ConvertCStringToString(RoomId);
  if (!Room::IsRoom(sRoomId))
  {
    pDnodeActor->PlayerOut += "Go to where?\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //*************
  //* GoTo Room *
  //*************
  // Send GoTo departure message
  GoToMsg = pDnodeActor->pPlayer->GoToDepart;
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, GoToMsg);
  // GoTo room
  // TODO This seems like double talk with the RoomId, even after CString is completely removed
  sRoomId = ConvertCStringToString(RoomId);
  sRoomId = Room::GetRoomId(sRoomId);
  csRoomId = ConvertStringToCString(sRoomId);
  pDnodeActor->pPlayer->RoomId = csRoomId;
  DoLook("");
  // Send GoTo arrival message
  GoToMsg = pDnodeActor->pPlayer->GoToArrive;
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, GoToMsg);
}

/***********************************************************
* GoToArrive command                                       *
************************************************************/

void Communication::DoGoToArrive()
{
  CString GoToArrive;
  CString TmpStr;

  TmpStr = Utility::GetWord(CmdStr, 2);
  TmpStr.MakeLower();
  if (TmpStr == "")
  { // Player entered 'gotoarrive' by itself
    if (pDnodeActor->pPlayer->GoToArrive == "")
    { // Player has no arrival message
      pDnodeActor->PlayerOut += "You do not have an arrival message";
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
    else
    { // Show player's arrival message
      pDnodeActor->PlayerOut += "Your arrival message is: ";
      pDnodeActor->PlayerOut +=  pDnodeActor->pPlayer->GoToArrive;
      pDnodeActor->PlayerOut += "&N"; // In case arrive msg is messed up
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
  }
  if (TmpStr == "none")
  { // Player entered 'gotoarrive none'
    if (pDnodeActor->pPlayer->GoToArrive == "")
    { // Player has no arrival message
      pDnodeActor->PlayerOut += "You did not have an arrival message and you still do not have an arrival message";
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
    else
    {
      pDnodeActor->pPlayer->GoToArrive = "";
      pDnodeActor->PlayerOut += "Your arrival message has been removed.\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
  }
  TmpStr = Utility::GetWords(CmdStr, 2);
  GoToArrive = TmpStr;
  // Strip out color codes so arrival message length can be checked
  TmpStr.Replace("&N", "");
  TmpStr.Replace("&K", "");
  TmpStr.Replace("&R", "");
  TmpStr.Replace("&G", "");
  TmpStr.Replace("&Y", "");
  TmpStr.Replace("&B", "");
  TmpStr.Replace("&M", "");
  TmpStr.Replace("&C", "");
  TmpStr.Replace("&W", "");
  if (TmpStr.GetLength() > 60)
  {
    pDnodeActor->PlayerOut += "Arrival message must be less than 61 characters, color codes do not count.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pDnodeActor->pPlayer->GoToArrive = GoToArrive;
  pDnodeActor->PlayerOut += "Your arrival message has been set.\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
* GoToDepart command                                       *
************************************************************/

void Communication::DoGoToDepart()
{
  CString GoToDepart;
  CString TmpStr;

  TmpStr = Utility::GetWord(CmdStr, 2);
  TmpStr.MakeLower();
  if (TmpStr == "")
  { // Player entered 'gotodepart' by itself
    if (pDnodeActor->pPlayer->GoToDepart == "")
    { // Player has no departure message
      pDnodeActor->PlayerOut += "You do not have a departure message";
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
    else
    { // Show player's departure message
      pDnodeActor->PlayerOut += "Your departure message is: ";
      pDnodeActor->PlayerOut +=  pDnodeActor->pPlayer->GoToDepart;
      pDnodeActor->PlayerOut += "&N"; // In case depart msg is messed up
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
  }
  if (TmpStr == "none")
  { // Player entered 'gotodepart none'
    if (pDnodeActor->pPlayer->GoToDepart == "")
    { // Player has no departure message
      pDnodeActor->PlayerOut += "You did not have an departure message and you still do not have an departure message";
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
    else
    {
      pDnodeActor->pPlayer->GoToDepart = "";
      pDnodeActor->PlayerOut += "Your departure message has been removed.\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
  }
  TmpStr = Utility::GetWords(CmdStr, 2);
  GoToDepart = TmpStr;
  // Strip out color codes so arrival message length can be checked
  TmpStr.Replace("&N", "");
  TmpStr.Replace("&K", "");
  TmpStr.Replace("&R", "");
  TmpStr.Replace("&G", "");
  TmpStr.Replace("&Y", "");
  TmpStr.Replace("&B", "");
  TmpStr.Replace("&M", "");
  TmpStr.Replace("&C", "");
  TmpStr.Replace("&W", "");
  if (TmpStr.GetLength() > 60)
  {
    pDnodeActor->PlayerOut += "Departure message must be less than 61 characters, color codes do not count.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pDnodeActor->pPlayer->GoToDepart = GoToDepart;
  pDnodeActor->PlayerOut += "Your departure message has been set.\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Group command                                           *
 ***********************************************************/

void Communication::DoGroup()
{
  Dnode   *pDnodeGrpLdr; // Group leader
  int      i;
  int      j;
  bool     GrpFull;
  CString  PlayerNameCheck;
  CString  TargetNameCheck;
  CString  TargetNameSave;
  CString  TmpStr;

  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  PlayerNameCheck = pDnodeActor->PlayerName;
  TargetNameCheck = Utility::GetWord(CmdStr, 2);
  TargetNameSave  = TargetNameCheck;
  PlayerNameCheck.MakeLower();
  TargetNameCheck.MakeLower();
  //************************
  //* Group with no target *
  //************************
  if (TargetNameCheck.GetLength() < 1)
  { // No target given
      pDnodeActor->PlayerOut += "Group with whom?\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
  }
  //*************************
  //* List members of group *
  //*************************
  if (TargetNameCheck == "list")
  { // List group
    if (pDnodeActor->pPlayer->pPlayerGrpMember[0] == NULL)
    { // Player is not in a group
      pDnodeActor->PlayerOut += "You are not in a group.\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
    else
    { // Player is in a group, show members
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->pPlayerGrpMember[0]->Name;
      pDnodeActor->PlayerOut += " \r\n";
      j = pDnodeActor->pPlayer->pPlayerGrpMember[0]->Name.GetLength();
      for (i = 1; i < j + 1; i++)
      {
        pDnodeActor->PlayerOut += "-";
      }
      for (i = 1; i < GRP_LIMIT; i++)
      { // List group members
        if (pDnodeActor->pPlayer->pPlayerGrpMember[0]->pPlayerGrpMember[i] != NULL)
        {
          pDnodeActor->PlayerOut += " \r\n";
          pDnodeActor->PlayerOut += pDnodeActor->pPlayer->pPlayerGrpMember[0]->pPlayerGrpMember[i]->Name;
        }
      }
      pDnodeActor->PlayerOut += " \r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
  }
  //***********************
  //* Turning grouping on *
  //***********************
  if (TargetNameCheck == "on")
  {
    pDnodeActor->pPlayer->AllowGroup = true;
    pDnodeActor->PlayerOut += "You are now accepting requests to group.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //************************
  //* Turning grouping off *
  //************************
  if (TargetNameCheck == "off")
  {
    pDnodeActor->pPlayer->AllowGroup = false;
    pDnodeActor->PlayerOut += "You are now rejecting requests to group.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //*******************************
  //* Player is leaving the group *
  //*******************************
  if (TargetNameCheck == "none")
  {
    if (pDnodeActor->pPlayer->pPlayerGrpMember[0] == NULL)
    { // Player is not in a group
      pDnodeActor->PlayerOut += "You are not in a group.\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
    GrpLeave();
    return;
  }
  //*********************************************************
  //* Trying to create a new group or trying to add members *
  //*********************************************************
  if (TargetNameCheck == PlayerNameCheck)
  { // Trying to group with self
    if (pDnodeActor->pPlayer->pPlayerGrpMember[0] == NULL)
    { // Player is not in a group
      pDnodeActor->PlayerOut += "One is a lonely number, try grouping with another player.\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
    else
    { // Player is in a group
      pDnodeActor->PlayerOut += "One is a lonely number, but wait, you are already in group.\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
  }
  pDnodeGrpLdr = GetTargetDnode(TargetNameCheck);
  if (!pDnodeGrpLdr)
  { // New group member ... not online
    pDnodeActor->PlayerOut += TargetNameSave;
    pDnodeActor->PlayerOut += " is not online.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (pDnodeActor->pPlayer->pPlayerGrpMember[0] == pDnodeGrpLdr->pPlayer)
  { // Player is trying to group with their group's leader
    pDnodeActor->PlayerOut += "You are already grouped with ";
    pDnodeActor->PlayerOut += pDnodeGrpLdr->PlayerName;
    pDnodeActor->PlayerOut += ", who is the leader.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (pDnodeActor->pPlayer->RoomId != pDnodeGrpLdr->pPlayer->RoomId)
  { // New group member is not in same room as leader
    pDnodeActor->PlayerOut += pDnodeGrpLdr->PlayerName;
    pDnodeActor->PlayerOut += " is not here.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (pDnodeActor->pPlayer->pPlayerGrpMember[0] != NULL)
  { // Player is in a group
    if (pDnodeActor->pPlayer != pDnodeActor->pPlayer->pPlayerGrpMember[0])
    { // But is not the leader, only leader may add members
      pDnodeActor->PlayerOut += "You are not the leader. Leader must add members.\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
  }
  if (pDnodeGrpLdr->pPlayer->pPlayerGrpMember[0] != NULL)
  { // New group member already in a group
    pDnodeActor->PlayerOut += pDnodeGrpLdr->PlayerName;
    pDnodeActor->PlayerOut += " is already in a group.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  GrpFull = true;
  for (i = 0; i < GRP_LIMIT; i++)
  { // Is group full 
    if (pDnodeActor->pPlayer->pPlayerGrpMember[i] == NULL)
    { // Found an empty member slot
      j = i;
      GrpFull = false;
      break;
    }
  }
  if (GrpFull)
  { // Group is full
    pDnodeActor->PlayerOut += "Your group is full, maximum of ";
    sprintf(Buf, "%d", GRP_LIMIT);
    TmpStr = ConvertStringToCString(Buf);
    TmpStr.TrimLeft();
    TmpStr.TrimRight();
    pDnodeActor->PlayerOut+= TmpStr;
    pDnodeActor->PlayerOut += " members allowed.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (!pDnodeGrpLdr->pPlayer->AllowGroup)
  { // New group member is not accepting group requests
    pDnodeActor->PlayerOut += pDnodeGrpLdr->PlayerName;
    pDnodeActor->PlayerOut += " is not accepting requests to group.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //***********************************
  //* Ok, done checking ... group 'em *
  //***********************************
  if (pDnodeActor->pPlayer->pPlayerGrpMember[0] == NULL)
  { // Forming new group
    pDnodeActor->pPlayer->pPlayerGrpMember[0] = pDnodeActor->pPlayer;
    pDnodeGrpLdr->pPlayer->pPlayerGrpMember[0] = pDnodeActor->pPlayer;
    pDnodeActor->pPlayer->pPlayerGrpMember[1] = pDnodeGrpLdr->pPlayer;
    pDnodeActor->PlayerOut += "You have formed a new group with ";
    pDnodeActor->PlayerOut += pDnodeGrpLdr->PlayerName;
    pDnodeActor->PlayerOut += " as your first member.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  }
  else
  { // Adding a member to an existing group
    pDnodeActor->pPlayer->pPlayerGrpMember[i] = pDnodeGrpLdr->pPlayer;
    pDnodeGrpLdr->pPlayer->pPlayerGrpMember[0] = pDnodeActor->pPlayer;
    pDnodeActor->PlayerOut += pDnodeGrpLdr->PlayerName;
    pDnodeActor->PlayerOut += " has been added to your group.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  }
  pDnodeGrpLdr->PlayerOut += "\r\n";
  pDnodeGrpLdr->PlayerOut += "You have joined a group, ";
  pDnodeGrpLdr->PlayerOut += pDnodeActor->PlayerName;
  pDnodeGrpLdr->PlayerOut += " is the leader.\r\n";
  pDnodeGrpLdr->pPlayer->CreatePrompt();
  pDnodeGrpLdr->PlayerOut += pDnodeGrpLdr->pPlayer->GetOutput();
}

/***********************************************************
 * Gsay command                                            *
 ***********************************************************/

void Communication::DoGsay()
{
  Dnode   *pDnodeGrpLdr; // Group leader
  Dnode   *pDnodeGrpMem; // Group members
  CString  GsayMsg;
  int      i;

  //********************
  //* Validate command *
  //********************
  if (pDnodeActor->pPlayer->pPlayerGrpMember[0] == NULL)
  { // Player is not in a group
    pDnodeActor->PlayerOut += "You are not in a group.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  GsayMsg = Utility::GetWords(CmdStr, 2);
  if (GsayMsg.GetLength() < 1)
  { // Player typed gsay but did not type a message
    pDnodeActor->PlayerOut += "Are you trying to say something to the group?\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //*****************
  //* Send the gsay *
  //*****************
  pDnodeActor->PlayerOut += "&C";
  pDnodeActor->PlayerOut += "You say to the group: ";
  pDnodeActor->PlayerOut += GsayMsg;
  pDnodeActor->PlayerOut += "&N";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  // Get group leader
  pDnodeGrpLdr = GetTargetDnode(pDnodeActor->pPlayer->pPlayerGrpMember[0]->Name);
  for (i = 0; i < GRP_LIMIT; i++)
  { // For each member of leader's group including the leader
    if (pDnodeGrpLdr->pPlayer->pPlayerGrpMember[i] == NULL)
    { // Done looping through group members
      return;
    }
    // Send gsay to other group members
    pDnodeGrpMem = GetTargetDnode(pDnodeGrpLdr->pPlayer->pPlayerGrpMember[i]->Name);
    if (pDnodeActor == pDnodeGrpMem)
    { // Do not send gsay to originating player
      continue;
    }
    pDnodeGrpMem->PlayerOut += "\r\n";
    pDnodeGrpMem->PlayerOut += "&C";
    pDnodeGrpMem->PlayerOut += pDnodeActor->PlayerName;
    pDnodeGrpMem->PlayerOut += " says to the group: ";
    pDnodeGrpMem->PlayerOut += GsayMsg;
    pDnodeGrpMem->PlayerOut += "&N";
    pDnodeGrpMem->PlayerOut += "\r\n";
    pDnodeGrpMem->pPlayer->CreatePrompt();
    pDnodeGrpMem->PlayerOut += pDnodeGrpMem->pPlayer->GetOutput();
  }
}

/***********************************************************
 * Hail command                                            *
 ***********************************************************/

void Communication::DoHail()
{
  Mobile  *pMobile;
  CString  HailMsg;
  CString  MobileId;
  CString  MobileName;
  CString  MobileMsg;
  CString  PlayerName;
  CString  PlayerNameCheck;
  CString  RoomId;
  CString  Target;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (IsFighting())
  { // Player is fighting, send msg, command is not done
    return;
  }
  if (Utility::WordCount(CmdStr) < 2)
  { // No target
    pDnodeActor->PlayerOut += "You need a target.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (Utility::WordCount(CmdStr) > 2)
  { // Two many targets
    pDnodeActor->PlayerOut += "Only one target at a time.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  PlayerName      = pDnodeActor->PlayerName;
  RoomId          = pDnodeActor->pPlayer->RoomId;
  PlayerNameCheck = PlayerName;
  PlayerNameCheck.MakeLower();
  Target = Utility::GetWord(CmdStr, 2);
  MobileName = Target;
  Target.MakeLower();
  if (Target == PlayerNameCheck)
  { // Trying to kill self
    pDnodeActor->PlayerOut += "Hailing yourself is just plain silly.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pMobile = Mobile::IsMobInRoom(Target);
  if (!pMobile)
  { // Target mobile is not here
    pDnodeActor->PlayerOut += "Try hailing an NPC that is in this room.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  //*****************
  //* Send messages *
  //*****************
  // Send message to player
  pDnodeActor->PlayerOut += "&W";
  pDnodeActor->PlayerOut += "You hail ";
  pDnodeActor->PlayerOut += pMobile->Desc1;
  pDnodeActor->PlayerOut += "!";
  pDnodeActor->PlayerOut += "&N";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  // Send message to room
  HailMsg = "&W";
  HailMsg += PlayerName;
  HailMsg += " hails ";
  HailMsg += pMobile->Desc1;
  HailMsg += ".";
  HailMsg += "&N";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(RoomId, HailMsg);
  MobileMsg = pMobile->MobTalk();
  // Stip last \r\n from message, SendToRoom adds a \r\n
  MobileMsg = MobileMsg.Left(MobileMsg.GetLength()-2);
  pDnodeSrc = NULL;
  pDnodeTgt = NULL;
  SendToRoom(RoomId, MobileMsg);
  delete pMobile;
}

/***********************************************************
 * Help command                                            *
 ***********************************************************/

void Communication::DoHelp()
{
  if (Help::IsHelp())
  { // Help was found and sent to player
    return;
  }
  // No help entry found
  CmdStr = "help notfound";
  if (Help::IsHelp())
  { // Help notfound entry was found and sent to player
    return;
  }
  // Ok, if we are here, then there really is no help
  pDnodeActor->PlayerOut += "No help topic found.\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Inventory command                                       *
 ***********************************************************/

void Communication::DoInventory()
{
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  Object::ShowPlayerInv();
}

/***********************************************************
* Invisible command                                        *
************************************************************/

void Communication::DoInvisible()
{
  CString TmpStr;

  TmpStr = Utility::GetWord(CmdStr, 2);
  TmpStr.MakeLower();
  if (TmpStr == "on")
  { // Turn Invisible on
    pDnodeActor->pPlayer->Invisible = true;
    pDnodeActor->PlayerStateInvisible = true;
    pDnodeActor->pPlayer->Save();
    pDnodeActor->PlayerOut += "You are invisible.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (TmpStr == "off")
  { // Turn Invisible off
    pDnodeActor->pPlayer->Invisible = false;
    pDnodeActor->PlayerStateInvisible = false;
    pDnodeActor->pPlayer->Save();
    pDnodeActor->PlayerOut += "You are visible.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (pDnodeActor->pPlayer->Invisible)
  { // Invisible is on
    pDnodeActor->PlayerOut += "Invisible is on.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  }
  else
  { // Invisible is off
    pDnodeActor->PlayerOut += "Invisible is off.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  }
}

/***********************************************************
 * Kill command                                            *
 ***********************************************************/

void Communication::DoKill()
{
  Mobile  *pMobile;
  CString  KillMsg;
  CString  MobileId;
  CString  MobileName;
  CString  PlayerName;
  CString  PlayerNameCheck;
  CString  RoomId;
  string   sRoomId;
  CString  RoomType;
  CString  Target;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (pDnodeActor->pPlayer->Position == "sit")
  { // Player is sitting
    pDnodeActor->PlayerOut += "You must stand up before starting a fight!";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  RoomId = pDnodeActor->pPlayer->RoomId;
  sRoomId = ConvertCStringToString(RoomId);
  if (Room::IsRoomType(sRoomId, "NoFight"))
  { // No fighting is allowed in this room
    pDnodeActor->PlayerOut += "You are not allowed to fight here.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (pDnodeActor->PlayerStateFighting)
  { // Player is fighting
    pDnodeActor->PlayerOut += "You can only fight one opponent at a time!";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (Utility::WordCount(CmdStr) < 2)
  { // No target
    pDnodeActor->PlayerOut += "You need a target.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (Utility::WordCount(CmdStr) > 2)
  { // Two many targets
    pDnodeActor->PlayerOut += "Only one target at a time.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  PlayerName      = pDnodeActor->PlayerName;
  PlayerNameCheck = PlayerName;
  PlayerNameCheck.MakeLower();
  Target = Utility::GetWord(CmdStr, 2);
  MobileName = Target;
  Target.MakeLower();
  if (Target == PlayerNameCheck)
  { // Trying to kill self
    pDnodeActor->PlayerOut += "That would be just awful.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (Player::IsPlayer(Target))
  { // Trying to kill another player
    pDnodeActor->PlayerOut += "Don't even think about it, player killing is not allowed.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pMobile = Mobile::IsMobInRoom(Target);
  if (!pMobile)
  { // Target mobile is not here
    pDnodeActor->PlayerOut += "There doesn't seem to be a(n) ";
    pDnodeActor->PlayerOut += MobileName;
    pDnodeActor->PlayerOut += " here.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  //**************
  //* One Whack! *
  //**************
  if (pDnodeActor->pPlayer->OneWhack)
  { // Admininstrator has set OneWhack to yes
    pDnodeActor->PlayerStateFighting = false;
    pDnodeActor->PlayerOut += "&R";
    pDnodeActor->PlayerOut += "One WHACK and ";
    pDnodeActor->PlayerOut += pMobile->Desc1;
    pDnodeActor->PlayerOut += " is dead!\r\n";
    pDnodeActor->PlayerOut += "&N";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    // Send kill message to room
    KillMsg = "&R";
    KillMsg += PlayerName;
    KillMsg += " kills ";
    KillMsg += pMobile->Desc1;
    KillMsg += " with one WHACK!";
    KillMsg += "&N";
    pDnodeSrc = pDnodeActor;
    pDnodeTgt = pDnodeActor;
    SendToRoom(RoomId, KillMsg);
    // Remove mobile from room
    Mobile::RemoveMobFromRoom(RoomId, pMobile->MobileId);
    delete pMobile;
    return;
  }
  //*****************
  //* Send messages *
  //*****************
  // Send message to player
  pDnodeActor->PlayerOut += "&R";
  pDnodeActor->PlayerOut += "You start a fight with ";
  pDnodeActor->PlayerOut += pMobile->Desc1;
  pDnodeActor->PlayerOut += "!";
  pDnodeActor->PlayerOut += "&N";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  // Send message to room
  KillMsg = "&R";
  KillMsg += PlayerName;
  KillMsg += " starts a fight with ";
  KillMsg += pMobile->Desc1;
  KillMsg += "!";
  KillMsg += "&N";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(RoomId, KillMsg);
  //*****************
  //* Start a fight *
  //*****************
  if (!pMobile->Hurt)
  { //  Mobile not hurt
    pMobile->GetNextMobNbr();
    pMobile->CreateMobStatsFile(RoomId);
    MobileId = pMobile->MobileId;
    Mobile::RemoveMobFromRoom(RoomId, MobileId);
    MobileId = pMobile->MobileId + "." + pMobile->MobNbr;
  }
  else
  { // Mobile is hurt
    MobileId = pMobile->MobileId + "." + pMobile->MobNbr;
    Mobile::RemoveMobFromRoom(RoomId, MobileId);
  }
  Mobile::UpdateMobInWorld(MobileId, "add"); // Keep Mob InWorld count correct
  // Set player and mobile to fight
  Mobile::CreatePlayerMob(PlayerName, MobileId);
  Mobile::CreateMobPlayer(PlayerName, MobileId);
  delete pMobile;
  pDnodeActor->PlayerStateFighting = true;
}

/***********************************************************
* List command                                              *
************************************************************/

void Communication::DoList()
{
  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (IsFighting())
  { // Player is fighting, send msg, command is not done
    return;
  }
  if (!Shop::IsShop(pDnodeActor->pPlayer->RoomId))
  { // Room is not a shop
    pDnodeActor->PlayerOut += "Find a shop.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  Shop::ListObjects();
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Load command                                            *
 ***********************************************************/

void Communication::DoLoad()
{
  Mobile  *pMobile;
  Object  *pObject;
  CString  LoadMsg;
  CString  MobileId;
  CString  ObjectId;
  CString  TmpStr;

  //********************
  //* Validate command *
  //********************
  if (Utility::WordCount(CmdStr) == 1)
  { // Invalid command format
    pDnodeActor->PlayerOut += "Load what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }  
  if (Utility::WordCount(CmdStr) != 3)
  { // Invalid command format
    pDnodeActor->PlayerOut += "Usage: load obj{ect}|mob{ile} <target>";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  TmpStr = Utility::GetWord(CmdStr, 2);
  TmpStr.MakeLower();
  TmpStr = Utility::TranslateWord(TmpStr);
  if (Utility::IsNotWord(TmpStr,"object mobile"))
  { // obj or mob must be specified
    pDnodeActor->PlayerOut += "2nd parm must be obj{ect}|mob{ile}\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //***************
  //* Load object *
  //***************
  if (TmpStr == "object")
  { // Loading an object
    ObjectId = Utility::GetWord(CmdStr, 3);
    pObject = Object::IsObject(ObjectId);
    if (!pObject)
    { // Object does not exist
      pDnodeActor->PlayerOut += "Object not found.\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
    delete pObject;
    Object::AddObjToPlayerInv(pDnodeActor, ObjectId);
    pDnodeActor->PlayerOut += "Load successful\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //*****************
  //* Load a mobile *
  //*****************
  if (TmpStr == "mobile")
  { // Loading an mobile
    MobileId = Utility::GetWord(CmdStr, 3);
    MobileId.MakeLower();
    pMobile = Mobile::IsMobValid(MobileId);
    if (!pMobile)
    { // Mobile does not exist
      pDnodeActor->PlayerOut += "Mobile not found.\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
    Mobile::AddMobToRoom(pDnodeActor->pPlayer->RoomId, MobileId);
    World::SpawnMobileNoMove(MobileId);
    pDnodeActor->PlayerOut += "Load successful\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    LoadMsg = pMobile->Desc1;
    LoadMsg += " suddenly appears!";
    pDnodeSrc = pDnodeActor;
    pDnodeTgt = pDnodeActor;
    SendToRoom(pDnodeActor->pPlayer->RoomId, LoadMsg);
    delete pMobile;
    return;
  }
}

/***********************************************************
 * Logon                                                   *
 ***********************************************************/

void Communication::DoLogon()
{
  if (pDnodeActor->PlayerStateWaitNewCharacter)
  { // New character 'y-n' prompt
    pDnodeActor->PlayerStateWaitNewCharacter = false;
    LogonWaitNewCharacter();
    return;
  }
  if (pDnodeActor->PlayerStateWaitName)
  { // Name prompt
    pDnodeActor->PlayerStateWaitName = false;
    LogonWaitName();
    return;
  }
  if (pDnodeActor->PlayerStateWaitNameConfirmation)
  { // Name confirmation prompt
    pDnodeActor->PlayerStateWaitNameConfirmation = false;
    LogonWaitNameConfirmation();
    return;
  }
  if (pDnodeActor->PlayerStateWaitPassword)
  { // Password prompt
    pDnodeActor->PlayerStateWaitPassword = false;
    LogonWaitPassword(); 
    return;
  }
  if (pDnodeActor->PlayerStateWaitMaleFemale)
  { // Sex prompt
    pDnodeActor->PlayerStateWaitMaleFemale = false;
    LogonWaitMaleFemale();
    return;
  }
}

/***********************************************************
 * Look command                                            *
 ***********************************************************/

void Communication::DoLook(CString CmdStr1)
{
  Mobile  *pMobile;
  CString  Desc1FirstLetter;
  CString  Desc1TheRest;
  bool     IsPlayer;
  CString  MudCmdIsExit;
  string   sMudCmdIsExit;
  CString  TargetName;
  CString  TmpStr;

  CmdStr = CmdStr1;
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  TmpStr = Utility::GetWord(CmdStr, 2);
  //*****************
  //* Just looking? *
  //*****************
  if (TmpStr == "")
  { // Just look
    Room::ShowRoom(pDnodeActor);
    return;
  }
  //**********************
  //* Is it a room exit? *
  //**********************
  MudCmdIsExit = "look";
  sMudCmdIsExit = ConvertCStringToString(MudCmdIsExit);
  if (Room::IsExit(sMudCmdIsExit))
  { // Look room exit
    return;
  }
  //*******************
  //* Is it a player? *
  //*******************
  IsPlayer = true;
  TargetName = TmpStr;
  TargetName.MakeLower();
  pDnodeTgt = GetTargetDnode(TargetName);
  if (!pDnodeTgt)
  { // Target is not online and/or not in 'playing' state
    IsPlayer = false;
  }
  else
  { // Target is online and playing
    if (pDnodeActor->pPlayer->RoomId != pDnodeTgt->pPlayer->RoomId)
    { // Target is not in the same room
      IsPlayer = false;
    }
  }
  if (IsPlayer)
  { // Show player
    Object::ShowPlayerEqu(pDnodeTgt);
    return;
  }
  //*******************
  //* Is it a mobile? *
  //*******************
  pMobile = Mobile::IsMobInRoom(TargetName);
  if (pMobile)
  { // Player is looking at a mob
    TmpStr = Utility::MakeFirstLower(pMobile->Desc1);
    pDnodeActor->PlayerOut += "You look at ";
    pDnodeActor->PlayerOut += TmpStr;
    pDnodeActor->PlayerOut += ".";
    pDnodeActor->PlayerOut += "\r\n";
    pMobile->ExamineMob(pMobile->MobileId);
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    delete pMobile;
    return;
  }
  // Nothing found to look at
  pDnodeActor->PlayerOut += "If it's an object, use examine, otherwise <shrug>.";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Money command                                           *
 ***********************************************************/

void Communication::DoMoney()
{
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  pDnodeActor->pPlayer->ShowMoney();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Motd command                                            *
 ***********************************************************/

void Communication::DoMotd()
{
  CStdioFile MotdFile;
  CString    MotdFileName;
  CString    Stuff;
  int        Success;

  // Read Motd file
  MotdFileName = MOTD_DIR;
  MotdFileName += "Motd";
  MotdFileName += ".txt";
  Success = MotdFile.Open(MotdFileName,
                   CFile::modeRead |
                   CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Communication::DoMotd - Open Motd file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  MotdFile.ReadString(Stuff);
  while (Stuff != "End of Motd")
  {
    Stuff += "\r\n";
    pDnodeActor->PlayerOut += Stuff;
    MotdFile.ReadString(Stuff);
  }
  MotdFile.Close();
  if (pDnodeActor->PlayerStatePlaying)
  {
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  }
}

/***********************************************************
* OneWhack command                                         *
************************************************************/

void Communication::DoOneWhack()
{
  CString TmpStr;

  TmpStr = Utility::GetWord(CmdStr, 2);
  TmpStr.MakeLower();
  if (TmpStr == "on")
  { // Turn OneWhack on
    pDnodeActor->pPlayer->OneWhack = true;
    pDnodeActor->pPlayer->Save();
    pDnodeActor->PlayerOut += "You will obliterate enemies with One Whack.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (TmpStr == "off")
  { // Turn OneWhack off
    pDnodeActor->pPlayer->OneWhack = false;
    pDnodeActor->pPlayer->Save();
    pDnodeActor->PlayerOut += "You will now have to fight for you life.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (pDnodeActor->pPlayer->OneWhack)
  { // OneWhack is on
    pDnodeActor->PlayerOut += "OneWhack is on.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  }
  else
  { // OneWhack is off
    pDnodeActor->PlayerOut += "OneWhack is off.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  }
}

/***********************************************************
 * Password command                                        *
 ***********************************************************/

void Communication::DoPassword()
{
  CString Password;
  CString NewPassword1;
  CString NewPassword2;

  //********************
  //* Validate command *
  //********************
  if (IsFighting())
  { // Player is fighting, send msg, command is not done
    return;
  }
  if (Utility::WordCount(CmdStr) != 4)
  {
    pDnodeActor->PlayerOut += "Password command requires: ";
    pDnodeActor->PlayerOut += "Password NewPassword NewPassword";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  Password     = Utility::GetWord(CmdStr, 2);
  NewPassword1 = Utility::GetWord(CmdStr, 3);
  NewPassword2 = Utility::GetWord(CmdStr, 4);
  if (Password != ConvertStringToCString(pDnodeActor->pPlayer->Password))
  {
    pDnodeActor->PlayerOut += "Password does not match current password.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (NewPassword1 != NewPassword2)
  {
    pDnodeActor->PlayerOut += "New passwords do not match.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //*******************
  //* Change password *
  //*******************
  pDnodeActor->pPlayer->Password = NewPassword1;
  pDnodeActor->pPlayer->Save();
  pDnodeActor->PlayerOut += "Your password has been changed.";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Played command                                          *
 ***********************************************************/

void Communication::DoPlayed()
{
  char   Buffer[100];
  int    Days;
  int    Hours;
  int    Minutes;
  int    Seconds;
  int    n;
  string BirthDay;
  string PlayerAge;
  string TimePlayed;
  time_t BornSec;
  time_t NowSec;
  time_t PlayerAgeSec;
  time_t TimePlayedSec;

  pDnodeActor->pPlayer->Save(); // Save() updates TimePlayed
  NowSec        = time(0);
  BornSec       = pDnodeActor->pPlayer->Born;
  PlayerAgeSec  = NowSec - BornSec;
  TimePlayedSec = pDnodeActor->pPlayer->TimePlayed;
  // Birthday
  BirthDay = ctime(&BornSec);
  // Age
  n        = (int) PlayerAgeSec;
  Days     = n / (24 * 3600);
  n        = n % (24 * 3600);
  Hours    = n / 3600;
  n       %= 3600;
  Minutes  = n / 60;
  n       %= 60;
  Seconds  = n;
  sprintf(Buffer, "Your age: %d days, %d hours, %d minutes, %d seconds", Days, Hours, Minutes, Seconds);
  PlayerAge = Buffer;
  // TimePlayed
  n        = (int)TimePlayedSec;
  Days     = n / (24 * 3600);
  n        = n % (24 * 3600);
  Hours    = n / 3600;
  n       %= 3600;
  Minutes  = n / 60;
  n       %= 60;
  Seconds  = n;
  sprintf(Buffer, "You've played: %d days, %d hours, %d minutes, %d seconds", Days, Hours, Minutes, Seconds);
  TimePlayed = Buffer;

  pDnodeActor->PlayerOut += PlayerAge.c_str();
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += TimePlayed.c_str();
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "Your birthday is: ";
  pDnodeActor->PlayerOut += BirthDay.c_str();
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Quit command                                            *
 ***********************************************************/

void Communication::DoQuit()
{
  CString AllMsg;
  CString LogBuf;
  CString PlayerMsg;
  
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (IsFighting())
  { // Player is fighting, send msg, command is not done
    return;
  }
  pDnodeActor->PlayerStateBye = true;
  pDnodeActor->PlayerStatePlaying = false;
  pDnodeActor->pPlayer->Save();
  GrpLeave();
  LogBuf  = pDnodeActor->PlayerName;
  LogBuf += " issued the QUIT command";
  Log::LogIt(LogBuf);
  PlayerMsg  = "\r\n";
  PlayerMsg += "Bye Bye!";
  PlayerMsg += "\r\n";
  AllMsg  = "\r\n";
  AllMsg += pDnodeActor->PlayerName;
  AllMsg += " has left the game.";
  AllMsg += "\r\n";
  SendToAll(PlayerMsg, AllMsg);
}

/***********************************************************
 * Refresh command                                         *
 ***********************************************************/

void Communication::DoRefresh()
{
  CString TmpStr;

  if (Utility::WordCount(CmdStr) > 2)
  { // Invalid command format
    pDnodeActor->PlayerOut += "You may refresh only one thing at time.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  TmpStr = Utility::GetWord(CmdStr, 2);
  if (TmpStr.GetLength() == 0)
  { // Player did not provide an target to be refreshed
    pDnodeActor->PlayerOut += "Refresh what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  TmpStr.MakeLower();
  if (TmpStr == "commands")
  { // Refresh commands array
    CommandArrayLoad();
    pDnodeActor->PlayerOut += "Commands have been refreshed.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();     
    return;
  }
  // Refresh target not valid
  pDnodeActor->PlayerOut += "Refresh what??";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Remove command                                          *
 ***********************************************************/

void Communication::DoRemove()
{
  Object  *pObject;
  CString  ObjectName;
  CString  RemoveMsg;
  CString  TmpStr;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (Utility::WordCount(CmdStr) > 2)
  { // Invalid command format, like 'remove shirt pants'
    pDnodeActor->PlayerOut += "You may remove only one item at time.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  TmpStr = Utility::GetWord(CmdStr, 2);
  if (TmpStr.GetLength() == 0)
  { // Player did not provide an object to be removed
    pDnodeActor->PlayerOut += "Remove what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  // Get pointer to object
  ObjectName = TmpStr;
  TmpStr.MakeLower();
  pObject = Object::IsObjInPlayerEqu(TmpStr);
  if (!pObject)
  { // Object not in equipment
    pDnodeActor->PlayerOut += "You don't have a(n) ";
    pDnodeActor->PlayerOut += ObjectName;
    pDnodeActor->PlayerOut += " equipped.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  // Decrease player's ArmorClass
  pDnodeActor->pPlayer->ArmorClass -= pObject->ArmorValue;
  // Remove object from player's equipment
  Object::RemoveObjFromPlayerEqu(pObject->ObjectId);
  // Send remove message to player
  pDnodeActor->PlayerOut += "You remove ";
  pDnodeActor->PlayerOut += pObject->Desc1;
  pDnodeActor->PlayerOut += ".\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  // Send remove message to room
  RemoveMsg = pDnodeActor->PlayerName;
  RemoveMsg += " removes ";
  RemoveMsg += pObject->Desc1;
  RemoveMsg += ".";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, RemoveMsg);
  // Add object to player's inventory
  Object::AddObjToPlayerInv(pDnodeTgt, pObject->ObjectId);
  TmpStr = pObject->Type;
  TmpStr.MakeLower();
  if (TmpStr == "weapon")
  { // Now player has no weapon
    pDnodeActor->pPlayer->WeaponDamage = PLAYER_DMG_HAND;
    pDnodeActor->pPlayer->WeaponDesc1  = "a pair of bare hands";
    pDnodeActor->pPlayer->WeaponType   = "Hand";
  }
  delete pObject;
}

/***********************************************************
 * Restore command                                         *
 ***********************************************************/

void Communication::DoRestore(CString CmdStr1)
{
  CString  LookupName;
  CString  PlayerName;
  bool     TargetFound;
  CString  TargetName;
  CString  TargetNameSave;

  //********************
  //* Validate command *
  //********************
  TargetFound     = false;
  PlayerName      = pDnodeActor->PlayerName;
  TargetName      = Utility::GetWord(CmdStr, 2);
  TargetNameSave  = TargetName;
  PlayerName.MakeLower();
  TargetName.MakeLower();
  if (TargetName.GetLength() < 1)
  { // No target, assume self
    TargetName = PlayerName;
  }
  if (TargetName == PlayerName)
  { // Admin is restore themself
    pDnodeActor->PlayerOut += "You restore yourself!\r\n";
    pDnodeActor->pPlayer->HitPoints = pDnodeActor->pPlayer->Level * PLAYER_HPT_PER_LEVEL;
    pDnodeActor->pPlayer->Hunger    = 0;
    pDnodeActor->pPlayer->Thirst    = 0;
    pDnodeActor->pPlayer->Save();
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pDnodeTgt = GetTargetDnode(TargetName);
  if (!pDnodeTgt)
  { // Tell player ... not found
    pDnodeActor->PlayerOut += TargetNameSave;
    pDnodeActor->PlayerOut += " is not online.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  // Restore the player
  pDnodeTgt->pPlayer->HitPoints = pDnodeTgt->pPlayer->Level * PLAYER_HPT_PER_LEVEL;
  pDnodeTgt->pPlayer->Hunger    = 0;
  pDnodeTgt->pPlayer->Thirst    = 0;
  pDnodeTgt->pPlayer->Save();
  //****************************
  //* Send the restore message *
  //****************************
  PlayerName = pDnodeActor->PlayerName;
  TargetName = pDnodeTgt->PlayerName;
  // Send restore message to player
  pDnodeActor->PlayerOut += "You have restored ";
  pDnodeActor->PlayerOut += TargetName;
  pDnodeActor->PlayerOut += ".";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  // Send restore message to target
  pDnodeTgt->PlayerOut += "\r\n";  
  pDnodeTgt->PlayerOut += "&Y";
  pDnodeTgt->PlayerOut += PlayerName;
  pDnodeTgt->PlayerOut += " has restored you!";
  pDnodeTgt->PlayerOut += "&N";
  pDnodeTgt->PlayerOut += "\r\n";
  pDnodeTgt->pPlayer->CreatePrompt();
  pDnodeTgt->PlayerOut += pDnodeTgt->pPlayer->GetOutput();
}

/***********************************************************
 * RoomInfo command                                        *
 ***********************************************************/

void Communication::DoRoomInfo()
{
  CString TmpStr;

  TmpStr = Utility::GetWord(CmdStr, 2);
  TmpStr.MakeLower();
  if (TmpStr == "on")
  {
    pDnodeActor->pPlayer->RoomInfo = true;
    pDnodeActor->PlayerOut += "You will now see hidden room info.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (TmpStr == "off")
  {
    pDnodeActor->pPlayer->RoomInfo = false;
    pDnodeActor->PlayerOut += "You will no longer see hidden room info.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pDnodeActor->PlayerOut += "Try on or off.\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Save command                                            *
 ***********************************************************/

void Communication::DoSave()
{
  pDnodeActor->pPlayer->Save();
  pDnodeActor->PlayerOut += "Saved!";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Say command                                             *
 ***********************************************************/

void Communication::DoSay()
{
  CString SayMsg;
  CString TmpStr;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  SayMsg = Utility::GetWords(CmdStr, 2);
  if (SayMsg.GetLength() < 1)
  { // Player did not enter anything to say
    pDnodeActor->PlayerOut += "You try to speak, but no words come out of your mouth.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (pDnodeActor->PlayerStateInvisible)
  { // Player can't speak while invisible
    pDnodeActor->PlayerOut += "No talking while you are invisible.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //*****************
  //* Say something *
  //*****************
  pDnodeActor->PlayerOut += "&W";
  pDnodeActor->PlayerOut += "You say: ";
  pDnodeActor->PlayerOut += SayMsg;
  pDnodeActor->PlayerOut += "&N";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  TmpStr = "&W";
  TmpStr += pDnodeActor->PlayerName;
  TmpStr += " says: ";
  TmpStr += SayMsg;
  TmpStr += "&N";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, TmpStr);
}

/***********************************************************
* Sell command                                             *
************************************************************/

void Communication::DoSell()
{
  Object  *pObject;
  int      Cost;
  CString  Desc1;
  int      InvCountInt;
  CString  InvCountStr;
  CString  ObjectId;
  CString  ObjectName;
  CString  RoomId;
  int      SellCountInt;
  CString  SellCountStr;
  CString  TmpStr;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (IsFighting())
  { // Player is fighting, send msg, command is not done
    return;
  }
  RoomId = pDnodeActor->pPlayer->RoomId;
  if (!Shop::IsShop(RoomId))
  { // Room is not a shop
    pDnodeActor->PlayerOut += "Find a shop.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  ObjectName = Utility::GetWord(CmdStr, 2);
  if (ObjectName == "")
  { // No object given
    pDnodeActor->PlayerOut += "Sell what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pObject = Object::IsObjInPlayerInv(ObjectName);
  if (!pObject)
  { // Player doesn't have object
    pDnodeActor->PlayerOut += "You don't have a(n) ";
    pDnodeActor->PlayerOut += ObjectName;
    pDnodeActor->PlayerOut += " in your inventory.";
    pDnodeActor->PlayerOut +="\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  ObjectId    = pObject->ObjectId;
  Desc1       = pObject->Desc1;
  Cost        = pObject->Cost;
  InvCountStr = pObject->Count;
  pObject     = Shop::IsShopObj(RoomId, ObjectName);
  if (!pObject)
  { // Player cannot sell object to this shop
    pDnodeActor->PlayerOut += "That item cannot be sold here.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  delete pObject;
  //********************
  //* Check sell count *
  //********************
  InvCountInt = atoi(InvCountStr);
  SellCountStr = Utility::GetWord(CmdStr, 3);
  SellCountStr.MakeLower(); // In case player typed 'all'
  if (SellCountStr == "")
  { // Player did not specify a sell count
    SellCountInt = 1;
  }
  else
  { // Player might be selling more than 1 item
    if (SellCountStr == "all")
    { // Player is selling all items
      SellCountInt = InvCountInt;
    }
    else
    { // Compare player InvCount against SellCountInt
      SellCountInt = atoi(SellCountStr);
      if (SellCountInt <= 0)
      { // Player entered a negative or zero amount to sell
        pDnodeActor->PlayerOut += "You cannot sell less that 1 item";
        pDnodeActor->PlayerOut += "\r\n";
        pDnodeActor->pPlayer->CreatePrompt();
        pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
        return;
      }
      else
      if (SellCountInt > InvCountInt)
      { // Player is trying sell more than they have
        pDnodeActor->PlayerOut += "You don't have that many ";
        pDnodeActor->PlayerOut += ObjectName;
        pDnodeActor->PlayerOut += "s";
        pDnodeActor->PlayerOut += "\r\n";
        pDnodeActor->pPlayer->CreatePrompt();
        pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
        return;
      }
    }
  }
  //*******************
  //* Sell the object *
  //*******************
  // Remove object from player's inventory
  Object::RemoveObjFromPlayerInv(ObjectId, SellCountInt);
  // Player receives some money
  Cost = Cost * SellCountInt;
  pDnodeActor->pPlayer->SetMoney('+', Cost, "Silver");
  // Send messages
  sprintf(Buf, "%d", SellCountInt);
  TmpStr = ConvertStringToCString(Buf);
  TmpStr = "(" + TmpStr + ") ";
  pDnodeActor->PlayerOut += TmpStr;
  pDnodeActor->PlayerOut += "You sell ";
  pDnodeActor->PlayerOut += Desc1;
  pDnodeActor->PlayerOut += " for ";
  sprintf(Buf, "%d", Cost);
  TmpStr = ConvertStringToCString(Buf);
  pDnodeActor->PlayerOut += TmpStr;
  pDnodeActor->PlayerOut += " piece(s) of silver.";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Show command                                            *
 ***********************************************************/

void Communication::DoShow()
{
  CString    CommandCheckResult;
  CStdioFile HelpFile;
  CString    HelpFileName;
  CString    HelpText;
  CStdioFile SocialFile;
  CString    SocialFileName;
  CString    SocialText;
  int        Success;
  CString    TmpStr;
  CString    MudCmdChk;
  CString    ValCmdInfo;

  //********************
  //* Validate command *
  //********************
  if (Utility::WordCount(CmdStr) > 2)
  { // Invalid command format
    pDnodeActor->PlayerOut += "You may show only one thing at time.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  TmpStr = Utility::GetWord(CmdStr, 2);
  if (TmpStr.GetLength() == 0)
  { // Player did not provide a target to be shown
    pDnodeActor->PlayerOut += "Show what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  TmpStr.MakeLower();
  if (Utility::IsNotWord(TmpStr, "commands socials help"))
  { // Show target not valid
    pDnodeActor->PlayerOut += "Show what??";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //*****************
  //* Show commands *
  //*****************
  if (TmpStr == "commands")
  {
    pDnodeActor->PlayerOut += "Commands";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "--------";
    pDnodeActor->PlayerOut += "\r\n";
    for (auto ValidCmd : ValidCmds)
    { // For each string in the ValidCmds vector
      ValCmdInfo = ConvertStringToCString(ValidCmd);
      MudCmdChk  = Utility::GetWord(ValCmdInfo, 1);
      CommandCheckResult = CommandCheck(MudCmdChk);
      if (CommandCheckResult == "Ok")
      { // Mud command is Ok for this player
        pDnodeActor->PlayerOut += MudCmdChk;
        pDnodeActor->PlayerOut += "\r\n";
      }
      else
      if (Utility::GetWord(CommandCheckResult, 1) == "Level")
      { // Mud command is Ok for this player, but level restricted
        pDnodeActor->PlayerOut += MudCmdChk;
        pDnodeActor->PlayerOut += " acquired at level(";
        pDnodeActor->PlayerOut += Utility::GetWord(CommandCheckResult, 2);
        pDnodeActor->PlayerOut += ")";
        pDnodeActor->PlayerOut += "\r\n";
      }
    }
  }
  //*************
  //* Show help *
  //*************
  if (TmpStr == "help")
  {
    pDnodeActor->PlayerOut += "Help Topics";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "-----------";
    pDnodeActor->PlayerOut += "\r\n";
    HelpFileName =  HELP_DIR;
    HelpFileName += "Help.txt";
    Success = HelpFile.Open(HelpFileName,
                 CFile::modeRead |
                 CFile::typeText);
    if(!Success)
    { // Help file open failed
      pDnodeActor->PlayerOut += "No help is available, you are on your own!";
      pDnodeActor->PlayerOut += "\r\n";
    }
    else
    { // Help file is open
      HelpFile.ReadString(HelpText); // Skip first line
      HelpFile.ReadString(HelpText);
      while (HelpText != "End of Help")
      { // Read the whole file
        if (HelpText.Left(5) == "Help:")
        { // Found a help topic
          pDnodeActor->PlayerOut += HelpText.Right(HelpText.GetLength() - 5);
          pDnodeActor->PlayerOut += "\r\n";
        }
        HelpFile.ReadString(HelpText);
      }
      HelpFile.Close();
    }
  }
  //****************
  //* Show socials *
  //****************
  if (TmpStr == "socials")
  {
    pDnodeActor->PlayerOut += "Socials";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "-------";
    pDnodeActor->PlayerOut += "\r\n";
    SocialFileName =  SOCIAL_DIR;
    SocialFileName += "Social.txt";
    Success = SocialFile.Open(SocialFileName,
                   CFile::modeRead |
                   CFile::typeText);
    if(!Success)
    { // Social file open failed
      pDnodeActor->PlayerOut += "No socials are available, how boring!";
      pDnodeActor->PlayerOut += "\r\n";
    }
    else
    { // Social file is open
      SocialFile.ReadString(SocialText);
      while (SocialText != "End of Socials")
      { // Read the whole file
        if (SocialText.Left(9) == "Social : ")
        { // Found a help topic
          pDnodeActor->PlayerOut += SocialText.Right(SocialText.GetLength() - 9);
          pDnodeActor->PlayerOut += "\r\n";
        }
        SocialFile.ReadString(SocialText);
      }
      SocialFile.Close();
    }
  }
  // Prompt
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Sit command                                             *
 ***********************************************************/

void Communication::DoSit()
{
  CString SitMsg;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (IsFighting())
  { // Player is fighting, send msg, command is not done
    return;
  }
  if (pDnodeActor->pPlayer->Position == "sit")
  { // Player is already sitting
    pDnodeActor->PlayerOut += "You are already sitting down.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //************
  //* Sit down *
  //************
  pDnodeActor->pPlayer->Position = "sit";
  pDnodeActor->pPlayer->Save();
  pDnodeActor->PlayerOut += "You sit down.";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  SitMsg = pDnodeActor->PlayerName + " sits down.";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, SitMsg);
}

/***********************************************************
 * Sleep command                                           *
 ***********************************************************/

void Communication::DoSleep()
{
  CString SleepMsg;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (IsFighting())
  { // Player is fighting, send msg, command is not done
    return;
  }
  if (pDnodeActor->pPlayer->Position != "sit")
  { // Player must sitting before sleeping
    pDnodeActor->PlayerOut += "You must be sitting before you can sleep.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //***************
  //* Fall asleep *
  //***************
  pDnodeActor->pPlayer->Position = "sleep";
  pDnodeActor->pPlayer->Save();
  pDnodeActor->PlayerOut += "You go to sleep.";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  SleepMsg = pDnodeActor->PlayerName + " goes to sleep.";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, SleepMsg);
}

/***********************************************************
 * Stand command                                           *
 ***********************************************************/

void Communication::DoStand()
{
  CString StandMsg;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (IsFighting())
  { // Player is fighting, send msg, command is not done
    return;
  }
  if (pDnodeActor->pPlayer->Position == "stand")
  { // Player is already standing
    pDnodeActor->PlayerOut += "You are already standing.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //************
  //* Stand up *
  //************
  pDnodeActor->pPlayer->Position = "stand";
  pDnodeActor->pPlayer->Save();
  pDnodeActor->PlayerOut += "You stand up.";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  StandMsg = pDnodeActor->PlayerName + " stands up.";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, StandMsg);
}

/***********************************************************
 * Status command                                          *
 ***********************************************************/

void Communication::DoStatus()
{
  pDnodeActor->pPlayer->ShowStatus();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Stop command                                            *
 ***********************************************************/

void Communication::DoStop()
{
  CString LogBuf;
  CString GoGoGoFileName;
  CString StopItFileName;

  StateStopping = true;
  LogBuf  = pDnodeActor->PlayerName;
  LogBuf += " issued the STOP command";
  Log::LogIt(LogBuf);
  pDnodeActor->PlayerOut += "Stop command issued!\r\n";
  GoGoGoFileName  = CONTROL_DIR;
  GoGoGoFileName += "GoGoGo";
  StopItFileName  = CONTROL_DIR;
  StopItFileName += "StopIt";
  CFile::Rename(GoGoGoFileName, StopItFileName);
}

/***********************************************************
 * Tell command                                            *
 ***********************************************************/

void Communication::DoTell()
{
  CString  PlayerName;
  bool     TargetFound;
  CString  TargetName;
  CString  TargetNameSave;
  CString  TellMsg;

  //********************
  //* Validate command *
  //********************
  TargetFound     = false;
  PlayerName      = pDnodeActor->PlayerName;
  TargetName      = Utility::GetWord(CmdStr, 2);
  TargetNameSave  = TargetName;
  PlayerName.MakeLower();
  TargetName.MakeLower();
  if (TargetName == PlayerName)
  {
    pDnodeActor->PlayerOut += "Seems silly to tell yourself something!\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (TargetName.GetLength() < 1)
  {
    pDnodeActor->PlayerOut += "Tell who?\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  TellMsg = Utility::GetWords(CmdStr, 3);
  if (TellMsg.GetLength() < 1)
  {
    pDnodeActor->PlayerOut += "Um, tell ";
    pDnodeActor->PlayerOut += TargetNameSave;
    pDnodeActor->PlayerOut += " what?";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pDnodeTgt = GetTargetDnode(TargetName);
  if (!pDnodeTgt)
  { // Tell player ... not found
    pDnodeActor->PlayerOut += TargetNameSave;
    pDnodeActor->PlayerOut += " is not online.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //*****************
  //* Send the tell *
  //*****************
  PlayerName = pDnodeActor->PlayerName;
  TargetName = pDnodeTgt->PlayerName;
  // Send tell message to player
  pDnodeActor->PlayerOut += "&M";
  pDnodeActor->PlayerOut += "You tell ";
  pDnodeActor->PlayerOut += TargetName;
  pDnodeActor->PlayerOut += ": ";
  pDnodeActor->PlayerOut += TellMsg;
  pDnodeActor->PlayerOut += "&N";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  // Send tell message to target
  pDnodeTgt->PlayerOut += "&M";
  pDnodeTgt->PlayerOut += "\r\n";  
  pDnodeTgt->PlayerOut += PlayerName;
  pDnodeTgt->PlayerOut += " tells you: ";
  pDnodeTgt->PlayerOut += TellMsg;
  pDnodeTgt->PlayerOut += "&N";
  pDnodeTgt->PlayerOut += "\r\n";
  pDnodeTgt->pPlayer->CreatePrompt();
  pDnodeTgt->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
* Time command                                             *
************************************************************/

void Communication::DoTime()
{
  CString DisplayCurrentTime;

  // Server time
  time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
  string s(30, '\0');
  strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S", localtime(&now));
  DisplayCurrentTime = ConvertStringToCString(s);
  pDnodeActor->PlayerOut += "Current server time is: ";
  pDnodeActor->PlayerOut += DisplayCurrentTime;
  pDnodeActor->PlayerOut += "\r\n";
  // Game time
  pDnodeActor->PlayerOut += "Current game time is: ";
  pDnodeActor->PlayerOut += ConvertStringToCString(pCalendar->GetTime());
  pDnodeActor->PlayerOut += "\r\n";
  // Prompt
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
* Title command                                            *
************************************************************/

void Communication::DoTitle()
{
  CString Title;
  CString TmpStr;

  TmpStr = Utility::GetWord(CmdStr, 2);
  TmpStr.MakeLower();
  if (TmpStr == "")
  { // Player entered 'title' by itself
    if (pDnodeActor->pPlayer->Title == "")
    { // Player has no title
      pDnodeActor->PlayerOut += "You do not have a title";
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
    else
    { // Show player's title
      pDnodeActor->PlayerOut += "Your title is: ";
      pDnodeActor->PlayerOut +=  pDnodeActor->pPlayer->Title;
      pDnodeActor->PlayerOut += "&N"; // In case title is messed up
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
  }
  if (TmpStr == "none")
  { // Player entered 'title none'
    if (pDnodeActor->pPlayer->Title == "")
    { // Player has no title
      pDnodeActor->PlayerOut += "You did not have a title and you still do not have a title";
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
    else
    {
      pDnodeActor->pPlayer->Title = "";
      pDnodeActor->PlayerOut += "Your title has been removed.\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
  }
  TmpStr = Utility::GetWords(CmdStr, 2);
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
  if (TmpStr.GetLength() > 40)
  {
    pDnodeActor->PlayerOut += "Title must be less than 41 characters, color codes do not count.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pDnodeActor->pPlayer->Title = Title;
  pDnodeActor->pPlayer->Save();
  pDnodeActor->PlayerOut += "Your title has been set.\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
* Train command                                            *
************************************************************/

void Communication::DoTrain()
{
  int     IncreaseDecrease;
  CString MinusSign;
  int     SkillPointsUsed;
  int     SkillPointsRemaining;
  CString TmpStr;
  CString UnTrainCost;
  CString WeaponType;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (IsFighting())
  { // Player is fighting, send msg, command is not done
    return;
  }
  // Get command words
  WeaponType  = Utility::GetWord(CmdStr, 2);
  MinusSign   = Utility::GetWord(CmdStr, 3);
  UnTrainCost = Utility::GetWord(CmdStr, 4);
  WeaponType.MakeLower();
  // Calculate skill points used and remaining
  SkillPointsUsed  = 0;
  SkillPointsUsed += pDnodeActor->pPlayer->SkillAxe;
  SkillPointsUsed += pDnodeActor->pPlayer->SkillClub;
  SkillPointsUsed += pDnodeActor->pPlayer->SkillDagger;
  SkillPointsUsed += pDnodeActor->pPlayer->SkillHammer;
  SkillPointsUsed += pDnodeActor->pPlayer->SkillSpear;
  SkillPointsUsed += pDnodeActor->pPlayer->SkillStaff;
  SkillPointsUsed += pDnodeActor->pPlayer->SkillSword;
  SkillPointsRemaining = PLAYER_SKILL_PER_LEVEL * pDnodeActor->pPlayer->Level - SkillPointsUsed;
  // Do some more checking
  if (Utility::WordCount(CmdStr) > 4)
  { // Invalid command format
    pDnodeActor->PlayerOut += "Train command syntax error, try'er again.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (WeaponType != "")
  { // WeaponType specified
    if (Utility::IsNotWord(WeaponType, "axe club dagger hammer spear staff sword"))
    { // But it was the invalid
      pDnodeActor->PlayerOut += "Please specify a valid weapon type.";
      pDnodeActor->PlayerOut += "\r\n";
      return;
    }
    else
    { // Player is trying to train, check skill points remaining
      if (SkillPointsRemaining == 0 && MinusSign == "")
      { // No skill points available
        pDnodeActor->PlayerOut += "Sorry, you have no skill points remaining.";
        pDnodeActor->PlayerOut += "\r\n";
        pDnodeActor->pPlayer->CreatePrompt();
        pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
        return;
      }
    }
  }
  if (MinusSign != "")
  { // Third word was specified
    if (MinusSign != "-")
    { // But it was not a minus sign
      pDnodeActor->PlayerOut += "If third parameter is specified, it must be a minus sign.";
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
  }
  if (MinusSign == "-")
  { // Player wishes to untrain a skill
    if (UnTrainCost == "")
    { // But did not specify the cost
      pDnodeActor->PlayerOut += "Untraining a skill will cost ";
      pDnodeActor->PlayerOut += UNTRAIN_COST;
      pDnodeActor->PlayerOut += " silver";
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->PlayerOut += "Specify this amount after the minus sign.";
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
    else
    { // Cost specified, but was it the correct cost?
      if (UnTrainCost != UNTRAIN_COST)
      { // Wrong cost
        pDnodeActor->PlayerOut += "You must specify the correct amount! ";
        pDnodeActor->PlayerOut += UNTRAIN_COST;
        pDnodeActor->PlayerOut += " silver please.";
        pDnodeActor->PlayerOut += "\r\n";
        pDnodeActor->pPlayer->CreatePrompt();
        pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
        return;
      }
      else
      { // Player has specified everything correctly for untrain, check money
        if (pDnodeActor->pPlayer->Silver < atoi(UNTRAIN_COST))
        { // Not enough money
          pDnodeActor->PlayerOut += "You do not have ";
          pDnodeActor->PlayerOut += UNTRAIN_COST;
          pDnodeActor->PlayerOut += " pieces of silver!";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->pPlayer->CreatePrompt();
          pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
          return;
        }
        // Check skills
        if (WeaponType == "axe" && pDnodeActor->pPlayer->SkillAxe == 0)
        { // No axe skill
          pDnodeActor->PlayerOut += "You have no ";
          pDnodeActor->PlayerOut += WeaponType;
          pDnodeActor->PlayerOut += " skill!";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->pPlayer->CreatePrompt();
          pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
          return;
        }
        if (WeaponType == "club" && pDnodeActor->pPlayer->SkillClub == 0)
        { // No club skill
          pDnodeActor->PlayerOut += "You have no ";
          pDnodeActor->PlayerOut += WeaponType;
          pDnodeActor->PlayerOut += " skill!";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->pPlayer->CreatePrompt();
          pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
          return;
        }
        if (WeaponType == "dagger" && pDnodeActor->pPlayer->SkillDagger == 0)
        { // No dagger skill
          pDnodeActor->PlayerOut += "You have no ";
          pDnodeActor->PlayerOut += WeaponType;
          pDnodeActor->PlayerOut += " skill!";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->pPlayer->CreatePrompt();
          pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
          return;
        }
        if (WeaponType == "hammer" && pDnodeActor->pPlayer->SkillHammer == 0)
        { // No hammer skill
          pDnodeActor->PlayerOut += "You have no ";
          pDnodeActor->PlayerOut += WeaponType;
          pDnodeActor->PlayerOut += " skill!";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->pPlayer->CreatePrompt();
          pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
          return;
        }
        if (WeaponType == "spear" && pDnodeActor->pPlayer->SkillSpear == 0)
        { // No spear skill
          pDnodeActor->PlayerOut += "You have no ";
          pDnodeActor->PlayerOut += WeaponType;
          pDnodeActor->PlayerOut += " skill!";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->pPlayer->CreatePrompt();
          pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
          return;
        }
        if (WeaponType == "staff" && pDnodeActor->pPlayer->SkillStaff == 0)
        { // No staff skill
          pDnodeActor->PlayerOut += "You have no ";
          pDnodeActor->PlayerOut += WeaponType;
          pDnodeActor->PlayerOut += " skill!";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->pPlayer->CreatePrompt();
          pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
          return;
        }
        if (WeaponType == "sword" && pDnodeActor->pPlayer->SkillSword == 0)
        { // No sword skill
          pDnodeActor->PlayerOut += "You have no ";
          pDnodeActor->PlayerOut += WeaponType;
          pDnodeActor->PlayerOut += " skill!";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->pPlayer->CreatePrompt();
          pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
          return;
        }
      }
    }
  }
  if (WeaponType == "")
  { // Show player's skills summary
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "Skills Summary";
    pDnodeActor->PlayerOut += "\r\n";
    // Axe
    pDnodeActor->PlayerOut += "Axe:    ";
    sprintf(Buf, "%3d", pDnodeActor->pPlayer->SkillAxe);
    TmpStr = ConvertStringToCString(Buf);
    pDnodeActor->PlayerOut += TmpStr;
    pDnodeActor->PlayerOut += "\r\n";
    // Club
    pDnodeActor->PlayerOut += "Club:   ";
    sprintf(Buf, "%3d", pDnodeActor->pPlayer->SkillClub);
    TmpStr = ConvertStringToCString(Buf);
    pDnodeActor->PlayerOut += TmpStr;
    pDnodeActor->PlayerOut += "\r\n";
    // Dagger
    pDnodeActor->PlayerOut += "Dagger: ";
    sprintf(Buf, "%3d", pDnodeActor->pPlayer->SkillDagger);
    TmpStr = ConvertStringToCString(Buf);
    pDnodeActor->PlayerOut += TmpStr;
    pDnodeActor->PlayerOut += "\r\n";
    // Hammer
    pDnodeActor->PlayerOut += "Hammer: ";
    sprintf(Buf, "%3d", pDnodeActor->pPlayer->SkillHammer);
    TmpStr = ConvertStringToCString(Buf);
    pDnodeActor->PlayerOut += TmpStr;
    pDnodeActor->PlayerOut += "\r\n";
    // Spear
    pDnodeActor->PlayerOut += "Spear:  ";
    sprintf(Buf, "%3d", pDnodeActor->pPlayer->SkillSpear);
    TmpStr = ConvertStringToCString(Buf);
    pDnodeActor->PlayerOut += TmpStr;
    pDnodeActor->PlayerOut += "\r\n";
    // Staff
    pDnodeActor->PlayerOut += "Staff:  ";
    sprintf(Buf, "%3d", pDnodeActor->pPlayer->SkillStaff);
    TmpStr = ConvertStringToCString(Buf);
    pDnodeActor->PlayerOut += TmpStr;
    pDnodeActor->PlayerOut += "\r\n";
    // Sword
    pDnodeActor->PlayerOut += "Sword:  ";
    sprintf(Buf, "%3d", pDnodeActor->pPlayer->SkillSword);
    TmpStr = ConvertStringToCString(Buf);
    pDnodeActor->PlayerOut += TmpStr;
    pDnodeActor->PlayerOut += "\r\n";
    // Skill points used
    pDnodeActor->PlayerOut += "Skill points used:      ";
    sprintf(Buf, "%4d", SkillPointsUsed);
    TmpStr = ConvertStringToCString(Buf);
    pDnodeActor->PlayerOut += TmpStr;
    pDnodeActor->PlayerOut += "\r\n";
    // Skill points remaining
    pDnodeActor->PlayerOut += "Skill points remaining: ";
    sprintf(Buf, "%4d", SkillPointsRemaining);
    TmpStr = ConvertStringToCString(Buf);
    pDnodeActor->PlayerOut += TmpStr;
    pDnodeActor->PlayerOut += "\r\n";
    // Prompt
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (MinusSign != "-")
  { // Training a skill
    IncreaseDecrease = +1;
  }
  else
  { // UnTraining a skill
    IncreaseDecrease = -1;
  }
  // Ok, so train or untrain them already
  if (WeaponType == "axe")
  {
    pDnodeActor->pPlayer->SkillAxe += IncreaseDecrease;
  }
  else
  if (WeaponType == "club")
  {
    pDnodeActor->pPlayer->SkillClub += IncreaseDecrease;
  }
  else
  if (WeaponType == "dagger")
  {
    pDnodeActor->pPlayer->SkillDagger += IncreaseDecrease;
  }
  else
  if (WeaponType == "hammer")
  {
    pDnodeActor->pPlayer->SkillHammer += IncreaseDecrease;
  }
  else
  if (WeaponType == "spear")
  {
    pDnodeActor->pPlayer->SkillSpear += IncreaseDecrease;
  }
  else
  if (WeaponType == "staff")
  {
    pDnodeActor->pPlayer->SkillStaff += IncreaseDecrease;
  }
  else
  if (WeaponType == "sword")
  {
    pDnodeActor->pPlayer->SkillSword += IncreaseDecrease;
  }
  if (MinusSign != "-")
  { // Training
    pDnodeActor->PlayerOut += "You have improved your ";
    pDnodeActor->PlayerOut += WeaponType;
    pDnodeActor->PlayerOut += " skill!";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  }
  else
  { // UnTraining
    pDnodeActor->PlayerOut += "Your ";
    pDnodeActor->PlayerOut += WeaponType;
    pDnodeActor->PlayerOut += " skill has decreased at a cost of ";
    pDnodeActor->PlayerOut += UNTRAIN_COST;
    pDnodeActor->PlayerOut += " silver.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    pDnodeActor->pPlayer->Silver -= atoi(UNTRAIN_COST);
  }
  pDnodeActor->pPlayer->Save();
}

/***********************************************************
 * Wake command                                            *
 ***********************************************************/

void Communication::DoWake()
{
  CString WakeMsg;

  //********************
  //* Validate command *
  //********************
  if (pDnodeActor->pPlayer->Position != "sleep")
  {
    pDnodeActor->PlayerOut += "You are already awake.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //***********
  //* Wake up *
  //***********
  pDnodeActor->pPlayer->Position = "sit";
  pDnodeActor->pPlayer->Save();
  pDnodeActor->PlayerOut += "You awake and sit up.";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  WakeMsg = pDnodeActor->PlayerName + " awakens and sits up.";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, WakeMsg);
}

/***********************************************************
 * Wear command                                            *
 ***********************************************************/

void Communication::DoWear()
{
  Object  *pObject;
  CString  ObjectName;
  CString  TmpStr;
  bool     WearFailed;
  CString  WearMsg;
  CString  WearPosition;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (Utility::WordCount(CmdStr) == 1)
  { // Invalid command format
    pDnodeActor->PlayerOut += "Wear what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  // Get pointer to object
  TmpStr = Utility::GetWord(CmdStr, 2);
  ObjectName = TmpStr;
  TmpStr.MakeLower();
  pObject = Object::IsObjInPlayerInv(TmpStr);
  if (!pObject)
  { // Player does not have object in inventory
    pDnodeActor->PlayerOut += "You don't have a(n) ";
    pDnodeActor->PlayerOut += ObjectName;
    pDnodeActor->PlayerOut += ".\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  pObject->Type.MakeLower();
  if (pObject->Type != "armor")
  { // Player can't wear stuff that is NOT armor
    pDnodeActor->PlayerOut += "You can't wear ";
    pDnodeActor->PlayerOut += pObject->Desc1;
    pDnodeActor->PlayerOut += ".\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    delete pObject;
    return;
  }
  // Handle wear positions that require left or right
  if (Utility::IsWord(pObject->WearPosition,"ear wrist finger ankle"))
  { // Object must be worn using left and right
    TmpStr = Utility::GetWord(CmdStr, 3);
    TmpStr.MakeLower();
    if (Utility::IsNotWord(TmpStr,"left right"))
    { // Player did not specify left or right
      pDnodeActor->PlayerOut += "You must specify left or right";
      pDnodeActor->PlayerOut += ".\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      delete pObject;
      return;
    }
    pObject->WearPosition += TmpStr;
  }
  //***************
  //* Wear object *
  //***************
  // Add object to player's equipment
  WearFailed = Object::AddObjToPlayerEqu(pObject->WearPosition, pObject->ObjectId);
  if (WearFailed)
  { // Already wearing an object in that wear position
    pDnodeActor->PlayerOut += "You fail to wear ";
    pDnodeActor->PlayerOut += pObject->Desc1;
    pDnodeActor->PlayerOut += ".";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    delete pObject;
    return;
  }
  // Increase player's ArmorClass
  pDnodeActor->pPlayer->ArmorClass += pObject->ArmorValue;
  // Remove object from player's inventory
  Object::RemoveObjFromPlayerInv(pObject->ObjectId, 1);
  // Send messages
  pDnodeActor->PlayerOut += "You wear ";
  pDnodeActor->PlayerOut += pObject->Desc1;
  pDnodeActor->PlayerOut += ".";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  WearMsg = pDnodeActor->PlayerName + " wears " + pObject->Desc1 + ".";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, WearMsg);
  delete pObject;
}

/***********************************************************
 * Where command                                           *
 ***********************************************************/

void Communication::DoWhere()
{
  CString  SearchId;

  if (Utility::WordCount(CmdStr) != 2)
  { // Invalid command format
    pDnodeActor->PlayerOut += "Nothing given to search for!";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  SearchId = Utility::GetWord(CmdStr, 2);
  SearchId.MakeLower();
  // Find Players
  pDnodeTgt = GetTargetDnode(SearchId);
  if (pDnodeTgt)
  { // Target is online and in 'playing' state
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += pDnodeTgt->PlayerName;
    pDnodeActor->PlayerOut += " is in ";
    pDnodeActor->PlayerOut += pDnodeTgt->pPlayer->RoomId;
    pDnodeActor->PlayerOut += "\r\n";
  }
  else
  if (Mobile::IsMobValid(SearchId))
  { // Find Mobiles
    Mobile::WhereMob(SearchId);
  }
  else
  if (Object::IsObject(SearchId))
  { // Find Objects
    Object::WhereObj(SearchId);
  }
  else
  { // Could not find it
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "No ";
    pDnodeActor->PlayerOut += SearchId;
    pDnodeActor->PlayerOut += " found.";
    pDnodeActor->PlayerOut += "\r\n";
  }
  // Prompt
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Who command                                             *
 ***********************************************************/

void Communication::DoWho()
{
  CString  DisplayName;
  CString  DisplayLevel;

  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "&C";
  pDnodeActor->PlayerOut += "Players online";
  pDnodeActor->PlayerOut += "&N";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "--------------";
  pDnodeActor->PlayerOut += "\r\n";
  // List all players
  Descriptor::SetpDnodeCursorFirst();
  while (!Descriptor::EndOfDnodeList())
  { // Loop thru all connections
    pDnodeOthers = Descriptor::GetDnode();
    if (pDnodeOthers->PlayerStatePlaying)
    { // Who are 'playing'
      if (pDnodeOthers->PlayerStateInvisible)
      { // Player is invisible, no show
        Descriptor::SetpDnodeCursorNext();
        continue;
      }
      sprintf(Buf, "%-15s", (LPCSTR) pDnodeOthers->PlayerName);
      DisplayName = ConvertStringToCString(Buf);
      sprintf(Buf, "%3d", pDnodeOthers->pPlayer->Level);
      DisplayLevel = ConvertStringToCString(Buf);
      pDnodeActor->PlayerOut += DisplayName;
      pDnodeActor->PlayerOut += " ";
      pDnodeActor->PlayerOut += DisplayLevel;
      pDnodeActor->PlayerOut += " ";
      if (pDnodeOthers->PlayerStateAfk)
      { // Player is AFK
        pDnodeActor->PlayerOut += "&Y";
        pDnodeActor->PlayerOut += "AFK ";
        pDnodeActor->PlayerOut += "&N";
      }
      else
      { // Player is not AFK
        pDnodeActor->PlayerOut += "    ";
      }
      pDnodeActor->PlayerOut += pDnodeOthers->pPlayer->Title;
      pDnodeActor->PlayerOut += "&N";
      pDnodeActor->PlayerOut += "\r\n";
    }
    Descriptor::SetpDnodeCursorNext();
  }
  // Re-position pDnodeCursor
  RepositionDnodeCursor();
  // Create player prompt
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Wield command                                           *
 ***********************************************************/

void Communication::DoWield()
{
  Object  *pObject;
  CString  ObjectName;
  CString  TmpStr;
  CString  WearPosition;
  bool     WieldFailed;
  CString  WieldMsg;

  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (Utility::WordCount(CmdStr) == 1)
  { // Invalid command format
    pDnodeActor->PlayerOut += "Wield what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  //************************* 
  //* Get pointer to object *
  //*************************
  TmpStr = Utility::GetWord(CmdStr, 2);
  ObjectName = TmpStr;
  TmpStr.MakeLower();
  pObject = Object::IsObjInPlayerInv(TmpStr);
  if (!pObject)
  { // Player does not have object in inventory
    pDnodeActor->PlayerOut += "You don't have a(n) ";
    pDnodeActor->PlayerOut += ObjectName;
    pDnodeActor->PlayerOut += ".\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  //*******************
  //* Is it a weapon? *
  //*******************
  TmpStr = pObject->Type;
  TmpStr.MakeLower();
  if (TmpStr != "weapon")
  { // Player is trying to wield something that is not a weapon
    pDnodeActor->PlayerOut += "Try wielding a weapon.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  //********************
  //* Wield the weapon *
  //********************
  // Add object to player's equipment
  WieldFailed = Object::AddObjToPlayerEqu(pObject->WearPosition, pObject->ObjectId);
  if (WieldFailed)
  { // Already wielding a weapon
    pDnodeActor->PlayerOut += "You are already wielding a weapon";
    pDnodeActor->PlayerOut += ".\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    delete pObject;
    return;
  }
  // Remove object from player's inventory
  Object::RemoveObjFromPlayerInv(pObject->ObjectId, 1);
  // Send messages
  pDnodeActor->PlayerOut += "You wield ";
  pDnodeActor->PlayerOut += pObject->Desc1;
  pDnodeActor->PlayerOut += ".";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  // Send wear message to room
  WieldMsg  = pDnodeActor->PlayerName;
  WieldMsg += " wields " ;
  WieldMsg += pObject->Desc1; 
  WieldMsg += ".";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, WieldMsg);
  // Set player's weapon info
  pDnodeActor->pPlayer->WeaponDamage = pObject->WeaponDamage;
  pDnodeActor->pPlayer->WeaponDesc1  = pObject->Desc1;
  pDnodeActor->pPlayer->WeaponType   = pObject->WeaponType;
  pDnodeActor->pPlayer->Save();
  delete pObject;
}

/***********************************************************
* Groups - Calculate group experience, if any              *
************************************************************/

void Communication::GrpExperience(int MobileExpPoints, int MobileLevel)
{
  Dnode   *pDnode;
  Dnode   *pDnodeGrpLdr; // Group leader
  Dnode   *pDnodeGrpMem; // Group members
  Player  *pPlayer;      // Group member
  int      ExpPoints;
  float    fGrpLimit;
  float    fGrpMemberCount;
  CString  GainLoose;
  int      GrpMemberCount;
  int      i;
  double   LevelTotal;
  int      PlayerExpPct;
  CString  TmpStr;

  // Count group members
  GrpMemberCount = 0;
  LevelTotal     = 0;
  for (i = 0; i < GRP_LIMIT; i++)
  { // For each group member
    pPlayer = pDnodeActor->pPlayer->pPlayerGrpMember[0]->pPlayerGrpMember[i];
    if (pPlayer != NULL)
    { // Found a member
      GrpMemberCount++;
      LevelTotal += pPlayer->Level;
    }
  }
  // Award experience
  fGrpMemberCount = (float) GrpMemberCount;
  fGrpLimit       = (float) GRP_LIMIT;
  MobileExpPoints += int(MobileExpPoints*(fGrpMemberCount/fGrpLimit*MGBP/100.0));

  pDnodeGrpLdr = GetTargetDnode(pDnodeActor->pPlayer->pPlayerGrpMember[0]->Name);
  for (i = 0; i < GRP_LIMIT; i++)
  { // For each member of leader's group including the leader
    if (pDnodeGrpLdr->pPlayer->pPlayerGrpMember[i] == NULL)
    { // Done looping through group members
      return;
    }
    // Get group member's Dnode
    pDnodeGrpMem = GetTargetDnode(pDnodeGrpLdr->pPlayer->pPlayerGrpMember[i]->Name);
    if (pDnodeActor->pPlayer->RoomId != pDnodeGrpMem->pPlayer->RoomId)
    { // Group members who are not in the same room do not get exp
      continue;
    }
    // Calculate experience
    if (MobileExpPoints >= 0)
    { // Player gains xp
      PlayerExpPct = int(pDnodeGrpMem->pPlayer->Level/LevelTotal*100.0);
      ExpPoints    = int(MobileExpPoints*(PlayerExpPct/100.0));
      ExpPoints    = Player::CalcAdjustedExpPoints(pDnodeGrpMem->pPlayer->Level, MobileLevel, ExpPoints);
      GainLoose    = "gain";
    }
    else
    { // Player loses xp
      ExpPoints = MobileExpPoints * pDnodeGrpMem->pPlayer->Level;
      GainLoose = "loose";
    }
    // Send experience message 
    if (ExpPoints >= 0)
    { // Player gains xp
      sprintf(Buf, "%d",ExpPoints);
      TmpStr = ConvertStringToCString(Buf);
    }
    else
    { // Player looses xp
      sprintf(Buf, "%d",ExpPoints * -1);
      TmpStr = ConvertStringToCString(Buf);
    }
    pDnodeGrpMem->PlayerOut += "\r\n";
    pDnodeGrpMem->PlayerOut += "&Y";
    pDnodeGrpMem->PlayerOut += "You ";
    pDnodeGrpMem->PlayerOut += GainLoose;
    pDnodeGrpMem->PlayerOut += " ";
    pDnodeGrpMem->PlayerOut += TmpStr;
    pDnodeGrpMem->PlayerOut += " points of Group Experience!";
    pDnodeGrpMem->PlayerOut += "&N";
    pDnodeGrpMem->PlayerOut += "\r\n";
    // Gain some experience
    pDnode = pDnodeGrpMem;
    pDnodeGrpMem->pPlayer->GainExperience(pDnode, ExpPoints);
    // Save player
    pDnodeGrpMem->pPlayer->Save();
    // Player prompt
    pDnodeGrpMem->pPlayer->CreatePrompt();
    pDnodeGrpMem->PlayerOut += pDnodeGrpMem->pPlayer->GetOutput();
  }
}

/***********************************************************
* Groups - Player is leaving the group                     *
************************************************************/

void Communication::GrpLeave()
{
  if (pDnodeActor->pPlayer->pPlayerGrpMember[0] == NULL)
  { // Player is not in a group
    return;
  }
  if (pDnodeActor->pPlayer == pDnodeActor->pPlayer->pPlayerGrpMember[0])
  { // Player is group leader, disband the whole group
    GrpLeaveLeader();
  }
  else
  { // Player is a group member
    GrpLeaveMember();
  }
}

/***********************************************************
* Groups - Leader is leaving - Disband the whole group     *
************************************************************/

void Communication::GrpLeaveLeader()
{
  Dnode *pDnodeGrpMem; // Other group members
  int    i;
  int    j;

  // Player is group leader, disband the whole group
  for (i = 1; i < GRP_LIMIT; i++)
  { // For each group member
    if (pDnodeActor->pPlayer->pPlayerGrpMember[i] != NULL)
    { // Get member's dnode before member's pointer is nulled
      pDnodeGrpMem = GetTargetDnode(pDnodeActor->pPlayer->pPlayerGrpMember[i]->Name);
      // Null member's leader pointer
      pDnodeActor->pPlayer->pPlayerGrpMember[i]->pPlayerGrpMember[0] = NULL;
      // Null member's pointer
      pDnodeActor->pPlayer->pPlayerGrpMember[i] = NULL;
      // Let the group members know that group is disbanded
      pDnodeGrpMem->PlayerOut += "\r\n";
      pDnodeGrpMem->PlayerOut += "The group has been disbanded.\r\n";
      pDnodeGrpMem->pPlayer->CreatePrompt();
      pDnodeGrpMem->PlayerOut += pDnodeGrpMem->pPlayer->GetOutput();
      // Member now has no group, remove any followers of this member
      for (j = 0; j < GRP_LIMIT; j++)
      {
        pDnodeGrpMem->pPlayer->pPlayerFollowers[j] = NULL;
        pDnodeActor->pPlayer->pPlayerFollowers[j]  = NULL;
      }
    }
  }
  // Leader now has no group, remove any followers
  for (i = 0; i < GRP_LIMIT; i++)
  {
    pDnodeActor->pPlayer->pPlayerFollowers[i]  = NULL;
  }
  // Complete the disbanding of the whole group
  pDnodeActor->pPlayer->pPlayerGrpMember[0] = NULL;
  pDnodeActor->PlayerOut += "Your group has been disbanded.\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
* Groups - Member is leaving - Remove them from the group  *
************************************************************/

void Communication::GrpLeaveMember()
{
  Dnode   *pDnodeGrpLdr; // Group leader
  Dnode   *pDnodeGrpMem; // Other group members
  int      i;
  int      j;
  bool     GrpEmpty;

  if (pDnodeActor->pPlayer->pPlayerFollowers[0] != NULL)
  { // Player is following someone
    DoFollow(pDnodeActor, "follow none");
  }
  for (i = 1; i < GRP_LIMIT; i++)
  { // Loop thru leader's member list to find player
    if (pDnodeActor->pPlayer->pPlayerGrpMember[0]->pPlayerGrpMember[i] == pDnodeActor->pPlayer)
    { // Remove player from leader's group
      pDnodeActor->pPlayer->pPlayerGrpMember[0]->pPlayerGrpMember[i] = NULL;
      j = i; // Save player's subscript
      break;
    }
  }
  // Complete the disbanding and let the player know
  pDnodeActor->PlayerOut += "You have left the group.\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  // Let group leader know when a member leaves the group
  pDnodeGrpLdr = GetTargetDnode(pDnodeActor->pPlayer->pPlayerGrpMember[0]->Name);
  pDnodeGrpLdr->PlayerOut += "\r\n";
  pDnodeGrpLdr->PlayerOut += pDnodeActor->PlayerName;
  pDnodeGrpLdr->PlayerOut += " has left your group.\r\n";
  GrpEmpty = true;
  for (i = 1; i < GRP_LIMIT; i++)
  { // For each member of leader's group
    if (pDnodeGrpLdr->pPlayer->pPlayerGrpMember[i] != NULL)
    { // Let other group members know that player has left
      GrpEmpty = false;
      pDnodeGrpMem = GetTargetDnode(pDnodeGrpLdr->pPlayer->pPlayerGrpMember[i]->Name);
      pDnodeGrpMem->PlayerOut += "\r\n";
      pDnodeGrpMem->PlayerOut += pDnodeActor->PlayerName;
      pDnodeGrpMem->PlayerOut += " has left the group.\r\n";
      pDnodeGrpMem->pPlayer->CreatePrompt();
      pDnodeGrpMem->PlayerOut += pDnodeGrpMem->pPlayer->GetOutput();
      if (pDnodeActor->pPlayer == pDnodeGrpMem->pPlayer->pPlayerFollowers[0])
      { // Another group member was following player
        DoFollow(pDnodeGrpMem, "follow none");
      }
    }
  }
  pDnodeActor->pPlayer->pPlayerGrpMember[0] = NULL;
  if (GrpEmpty)
  { // Player was the last in the group, let the leader know
    pDnodeGrpLdr->pPlayer->pPlayerGrpMember[0] = NULL;
    pDnodeGrpLdr->PlayerOut += "Your group has disbanded.\r\n";
    // Leader has no group, remove any followers
    for (i = 0; i < GRP_LIMIT; i++)
    {
      pDnodeGrpLdr->pPlayer->pPlayerFollowers[i] = NULL;
    }
  }
  pDnodeGrpLdr->pPlayer->CreatePrompt();
  pDnodeGrpLdr->PlayerOut += pDnodeGrpLdr->pPlayer->GetOutput();
  // Compact the list of members, so new members are at the end
  for (i = j; i < GRP_LIMIT-1; i++) // j is subscript of member who is leaving
  {
    pDnodeGrpLdr->pPlayer->pPlayerGrpMember[i] = pDnodeGrpLdr->pPlayer->pPlayerGrpMember[i+1];
    pDnodeGrpLdr->pPlayer->pPlayerGrpMember[i+1] = NULL;
  }
  // When a member leaves a group, remove any followers
  for (i = 0; i < GRP_LIMIT; i++)
  {
    pDnodeActor->pPlayer->pPlayerFollowers[i] = NULL;
  }
}

/***********************************************************
 * Logon greeting                                          *
 ***********************************************************/

void Communication::LogonGreeting()
{
  CStdioFile GreetingFile;
  CString    GreetingFileName;
  CString    Stuff;
  int        Success;

  // Read greeting file
  GreetingFileName = GREETING_DIR;
  GreetingFileName += "Greeting";
  GreetingFileName += ".txt";
  Success = GreetingFile.Open(GreetingFileName,
                   CFile::modeRead |
                   CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Communication::LogonGreeting - Open Greeting file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  pDnodeActor->PlayerOut += "Version ";
  pDnodeActor->PlayerOut += VERSION;
  pDnodeActor->PlayerOut += "\r\n";
  GreetingFile.ReadString(Stuff);
  while (Stuff != "End of Greeting")
  {
    Stuff += "\r\n";
    pDnodeActor->PlayerOut += Stuff;
    GreetingFile.ReadString(Stuff);
  }
  GreetingFile.Close();
}

/***********************************************************
 * Logon wait male female                                  *
 ***********************************************************/

void Communication::LogonWaitMaleFemale()
{
  CString  AllMsg;
  CString  LogBuf;
  CString  PlayerMsg;

  CmdStr.MakeUpper();
  if (!(CmdStr.FindOneOf("MF") == 0 && CmdStr.GetLength() == 1))
  { // Not M or F
    pDnodeActor->PlayerStateWaitMaleFemale = true;
    pDnodeActor->PlayerOut += "You must enter a M or F.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "Sex of this character M-F?";
    pDnodeActor->PlayerOut += "\r\n";
  }
  else
  { // M or F entered, save them, let them play
    DoMotd();
    pDnodeActor->pPlayer = new Player();
    pDnodeActor->pPlayer->Name = pDnodeActor->PlayerName;
    pDnodeActor->pPlayer->Password = pDnodeActor->PlayerPassword;
    pDnodeActor->pPlayer->Sex = CmdStr;
    pDnodeActor->pPlayer->Born = GetTimeSeconds();
    pDnodeActor->pPlayer->Save();
    pDnodeActor->pPlayer->SessionTime = GetTimeSeconds();
    pDnodeActor->PlayerStateLoggingOn = false;
    pDnodeActor->PlayerStatePlaying = true;
    pDnodeActor->pPlayer->Save();
    PlayerMsg  = "\r\n";
    PlayerMsg += "May your travels be safe.";
    PlayerMsg += "\r\n";
    PlayerMsg += "\r\n";
    AllMsg  = "\r\n";
    AllMsg += "Please welcome new player ";
    AllMsg += pDnodeActor->PlayerName;
    AllMsg += ".";
    AllMsg += "\r\n";
    SendToAll(PlayerMsg, AllMsg);
    Room::ShowRoom(pDnodeActor);
    LogBuf  = "New player ";
    LogBuf += pDnodeActor->PlayerName;
    Log::LogIt(LogBuf);
    pDnodeActor->pPlayer->Save();
  }
}

/***********************************************************
 * Logon wait name                                         *
 ***********************************************************/

void Communication::LogonWaitName()
{
  // Fix name so first letter is upper case, rest are lower case
  pDnodeActor->PlayerName = Utility::MakeFirstUpper(CmdStr);
  if (pDnodeActor->PlayerNewCharacter == "Y")
  { // New player
    pDnodeActor->PlayerStateWaitNameConfirmation = true;
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "You wish to be known as ";
    pDnodeActor->PlayerOut += pDnodeActor->PlayerName;
    pDnodeActor->PlayerOut += "? Y-N";
    pDnodeActor->PlayerOut += "\r\n";
  }
  else
  { // Returning player
    if (!Player::IsPlayer(pDnodeActor->PlayerName))
    { // Name not found on file
      pDnodeActor->PlayerStateWaitName = true;
      pDnodeActor->PlayerOut += pDnodeActor->PlayerName;
      pDnodeActor->PlayerOut += " is not a citizen of this realm.";
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->PlayerOut += "Name?";
      pDnodeActor->PlayerOut += "\r\n";
    }
    else
    { // Name found on file
      pDnodeActor->PlayerStateWaitPassword = true;
      if (pDnodeActor->PlayerStateWaitPassword)
      { // Prompt for password
        pDnodeActor->PlayerOut += "\r\n";
        pDnodeActor->PlayerOut += "Password?";
        pDnodeActor->PlayerOut += "\r\n";
        pDnodeActor->pPlayer = new Player(); 
        pDnodeActor->pPlayer->Name = pDnodeActor->PlayerName;
        pDnodeActor->pPlayer->ParsePlayerStuff();
        pDnodeActor->PlayerPassword = pDnodeActor->pPlayer->Password;
      }
    }
  }
}

/***********************************************************
 * Logon wait name confirmation                            *
 ***********************************************************/

void Communication::LogonWaitNameConfirmation()
{
  CmdStr.MakeUpper();
  if (!(CmdStr.FindOneOf("YN") == 0 && CmdStr.GetLength() == 1))
  { // Not Y or N ... try again
    pDnodeActor->PlayerStateWaitNameConfirmation = true;
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "You must enter a Y or N.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "You wish to be known as ";
    pDnodeActor->PlayerOut += pDnodeActor->PlayerName;
    pDnodeActor->PlayerOut += "? Y-N";
    pDnodeActor->PlayerOut += "\r\n";
  }
  else
  { // Y or N entered
    if (CmdStr == "N")
    { // N ... changed their mind ... try again
      pDnodeActor->PlayerStateWaitName = true;
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->PlayerOut += "Ok then, Try again.";
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->PlayerOut += "Name?";
      pDnodeActor->PlayerOut += "\r\n";
    }
    else
    { // Y entered ... they like the name
      if (!Player::IsNameValid(pDnodeActor->PlayerName))      { // Name is invalid ... try again
        pDnodeActor->PlayerStateWaitName = true;
        pDnodeActor->PlayerOut += pDnodeActor->PlayerName;
        pDnodeActor->PlayerOut += " is not an acceptable name in this realm\r\n";
        pDnodeActor->PlayerOut += "\r\n";
        pDnodeActor->PlayerOut += "Name?";
        pDnodeActor->PlayerOut += "\r\n";
      }
      else
      { // Name is valid
        if (Player::IsPlayer(pDnodeActor->PlayerName))
        { // Name aleady used ... try again
          pDnodeActor->PlayerStateWaitName = true;
          pDnodeActor->PlayerOut += pDnodeActor->PlayerName;
          pDnodeActor->PlayerOut += " belongs to an exiting character";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->PlayerOut += "Try a different name";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->PlayerOut += "Name?";
          pDnodeActor->PlayerOut += "\r\n";
        }
        else
        { // New player with valid name that has not been used
          pDnodeActor->PlayerStateWaitPassword = true;
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->PlayerOut += "Password?";
          pDnodeActor->PlayerOut += "\r\n";
        }
      }
    }
  }
}

/***********************************************************
 * Logon wait new character                                *
 ***********************************************************/

void Communication::LogonWaitNewCharacter()
{
  CmdStr.MakeUpper();
  if (!(CmdStr.FindOneOf("YN") == 0 && CmdStr.GetLength() == 1))
  { // Not Y or N ... try again
    pDnodeActor->PlayerStateWaitNewCharacter = true;
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "You must enter a Y or N.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "Create a new character Y-N?";
    pDnodeActor->PlayerOut += "\r\n";
  }
  else
  { // Y or N entered ... store response for use later
    pDnodeActor->PlayerStateWaitName = true;
    pDnodeActor->PlayerNewCharacter = CmdStr;
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "Name?";
    pDnodeActor->PlayerOut += "\r\n";
  }
}

/***********************************************************
 * Logon wait password                                     *
 ***********************************************************/

void Communication::LogonWaitPassword()
{
  CString  AllMsg;
  CString  LogBuf;
  CString  PlayerMsg;

  if (pDnodeActor->PlayerPassword == ConvertCStringToString(CmdStr))
  { // Password matches
    if (pDnodeActor->PlayerNewCharacter == "Y")
    { // Password matches and a new player, get sex
      pDnodeActor->PlayerStateWaitMaleFemale = true;
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->PlayerOut +=  pDnodeActor->PlayerName;
      pDnodeActor->PlayerOut += ", remember your password.";
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->PlayerOut += "You must know it log in again.";
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->PlayerOut += "Sex of this character M-F?";
      pDnodeActor->PlayerOut += "\r\n";
    }
    else
    { // Password matches and returning player, let them play
      // Reconnecting?
      Descriptor::SetpDnodeCursorFirst();
      while (!Descriptor::EndOfDnodeList())
      { // Loop thru all connections
        pDnodeOthers = Descriptor::GetDnode();
        if (pDnodeActor != pDnodeOthers)
        { // Check other connections
          if (pDnodeActor->PlayerName == pDnodeOthers->PlayerName)
          { // Reconnect character 
            pDnodeActor->PlayerStateLoggingOn = false;
            pDnodeActor->PlayerStatePlaying = true;
            pDnodeActor->pPlayer = pDnodeOthers->pPlayer;
            pDnodeActor->pPlayer->SessionTime = GetTimeSeconds();
            pDnodeActor->PlayerOut += "\r\n";
            pDnodeActor->PlayerOut += "You take control of ";
            pDnodeActor->PlayerOut += pDnodeOthers->PlayerName;
            pDnodeActor->PlayerOut += ".";
            pDnodeActor->PlayerOut += "\r\n";
            pDnodeActor->PlayerOut += "Reconnection successful.";
            pDnodeActor->PlayerOut += "\r\n";
            pDnodeActor->pPlayer->CreatePrompt();
            pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
            LogBuf  = "Reconnecting player ";
            LogBuf += pDnodeActor->PlayerName;
            Log::LogIt(LogBuf);
            pDnodeActor->pPlayer->Save();
            // Clean up old connection
            pDnodeOthers->PlayerStateBye = true;
            pDnodeOthers->PlayerStateReconnecting = true;
            pDnodeOthers->PlayerOut += "\r\n";
            pDnodeOthers->PlayerOut += "&R";
            pDnodeOthers->PlayerOut += "Disconnecting - multiple login dectected";
            pDnodeOthers->PlayerOut += "&N";
            pDnodeOthers->PlayerOut += "\r\n ";
          }
        }
        Descriptor::SetpDnodeCursorNext();
      }
      // Re-position pDnodeCursor
      RepositionDnodeCursor();
      if (pDnodeActor->PlayerStateLoggingOn)
      { // Not reconnecting
        DoMotd();
        pDnodeActor->PlayerStateLoggingOn = false;
        pDnodeActor->PlayerStatePlaying = true;
        pDnodeActor->pPlayer->SessionTime = GetTimeSeconds();
        PlayerMsg  = "\r\n";
        PlayerMsg += "May your travels be safe.";
        PlayerMsg += "\r\n";
        PlayerMsg += "\r\n";
        AllMsg  = "\r\n";
        AllMsg += pDnodeActor->PlayerName;
        AllMsg += " has entered the game.";
        AllMsg += "\r\n";
        SendToAll(PlayerMsg, AllMsg);
        Room::ShowRoom(pDnodeActor);
        LogBuf  = "Returning player ";
        LogBuf += pDnodeActor->PlayerName;
        Log::LogIt(LogBuf);
        pDnodeActor->pPlayer->Save();
      }
    }
  }
  else
  { // Password does not match
    if (pDnodeActor->PlayerNewCharacter == "Y")
    { // New player
      pDnodeActor->PlayerWrongPasswordCount++;
      pDnodeActor->PlayerStateWaitPassword = true;
      if (pDnodeActor->PlayerWrongPasswordCount == 1)
      { // First password entered
        pDnodeActor->PlayerPassword = ConvertCStringToString(CmdStr);
        pDnodeActor->PlayerOut += "\r\n";
        pDnodeActor->PlayerOut += "Retype Password:";
        pDnodeActor->PlayerOut += "\r\n";
      }
      else
      {
        if (pDnodeActor->PlayerWrongPasswordCount < 4)
        { // Can't seem to type the same password ... doh!
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->PlayerOut += "Retyped Password does not match";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->PlayerOut += "Try again";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->PlayerOut += "Password?";
          pDnodeActor->PlayerOut += "\r\n";
        }
        else
        { // Mis-matched password entered 3 times, boot them off!
          pDnodeActor->PlayerStateBye = true;
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->PlayerOut += "Wrong password entered 3 times";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->PlayerOut += "You have been disconnected.";
          pDnodeActor->PlayerOut += "\r\n";
        }
      }
    }
    else
    { // Returning player
      pDnodeActor->PlayerWrongPasswordCount++;
      if (pDnodeActor->PlayerWrongPasswordCount < 3)
      { // Tries < 3
        pDnodeActor->PlayerStateWaitPassword = true;
        pDnodeActor->PlayerOut += "\r\n";
        pDnodeActor->PlayerOut += "Wrong password.";
        pDnodeActor->PlayerOut += "\r\n";
        pDnodeActor->PlayerOut += "\r\n";
        pDnodeActor->PlayerOut += "Password?";
        pDnodeActor->PlayerOut += "\r\n";
      }
      else
      { // Wrong password entered 3 times, log it, boot them off!
        pDnodeActor->PlayerStateBye = true;
        pDnodeActor->PlayerOut += "\r\n";
        pDnodeActor->PlayerOut += "Wrong password entered 3 times";
        pDnodeActor->PlayerOut += "\r\n";
        pDnodeActor->PlayerOut += "\r\n";
        pDnodeActor->PlayerOut += "You have been disconnected";
        pDnodeActor->PlayerOut += "\r\n";
        LogBuf = "Password failure for ";
        LogBuf += pDnodeActor->PlayerName;
        Log::LogIt(LogBuf);
      }
    }
  }
}

/***********************************************************
 * Reposition Dnode cursor                                 *
 ***********************************************************/

void Communication::RepositionDnodeCursor()
{
  // Dnode cursor must be repositioned after each 'looping thru all connections'
  Descriptor::SetpDnodeCursorFirst();
  while (!Descriptor::EndOfDnodeList())
  { // Loop thru all connections
    pDnodeOthers = Descriptor::GetDnode();
    if (pDnodeActor == pDnodeOthers)
    { // pDnodeCursor is now correctly positioned
      break;
    }
    Descriptor::SetpDnodeCursorNext();
  }
}

/***********************************************************
 * New connection                                          *
 ***********************************************************/

void Communication::SockNewConnection()
{
  unsigned long       FionbioParm;
  CString             LogBuf;
  int                 Result;
  struct sockaddr_in  Sock{};
  int                 SocketHandle;
  int                 SocketSize;
  CString             IpAddress;
  CString             TmpStr;
    
  FionbioParm = 1;
  SocketSize  = sizeof(Sock);
  // Return a new socket for a newly created connection (pg 63)
  SocketHandle = accept(ListenSocket, (struct sockaddr *)&Sock, &SocketSize);
  if (!SocketHandle)
  {
    sprintf(Buf, "%s", strerror(errno));
    LogBuf = "Communication::SockNewConnection - Error: accept: " + ConvertStringToCString(Buf);
    Log::LogIt(LogBuf);
    AfxMessageBox("Communication::SockNewConnection - Error: accept", MB_ICONSTOP);
    _endthread();
  }
  IpAddress = inet_ntoa(Sock.sin_addr);
  // Make socket nonblocking (pg 286)
  Result = ioctlsocket(ListenSocket, FIONBIO, &FionbioParm);
  if (Result != 0)
  {
    sprintf(Buf, "%s", strerror(errno));
    LogBuf = "Communication::SockNewConnection - Error: ioctlsocket " + ConvertStringToCString(Buf);
    Log::LogIt(LogBuf);
    AfxMessageBox("Communication::NewConection - Error: ioctlsocket", MB_ICONSTOP);
    _endthread();
  }
  sprintf(Buf, "%d", SocketHandle);
  TmpStr = ConvertStringToCString(Buf);
  LogBuf  = "New connection with socket handle ";
  LogBuf += TmpStr;
  LogBuf += " and address ";
  LogBuf += IpAddress;
  Log::LogIt(LogBuf);
  pDnodeActor = new Dnode(SocketHandle, IpAddress);
  Descriptor::AppendIt();
  StateConnections = true;
}

/***********************************************************
 * Send message                                            *
 ***********************************************************/

void Communication::SockSend(const char *arg)
{
  int Length;
  int Written;

  if (!arg || arg[0] == '\0')
  {
    return;
  }
  Length = strlen(arg);
  Written = ::send(pDnodeActor->DnodeFd, arg, Length, 0);
  if (Written == Length)
  { // Everything was sent
    pDnodeActor->PlayerOut = "";
  }
  else
  { // Some was not sent
    pDnodeActor->PlayerOut = pDnodeActor->PlayerOut.Right(Length-Written);
  }
}

/***********************************************************
 * Update player statistics that are 'tick' dependant      *
 ***********************************************************/

void Communication::UpdatePlayerStats()
{
  int     GainReducePct;
  int     HitPoints;
  int     HitPointsGain;
  int     HitPointsMax;
  int     HungerPct;
  int     Level;
  CString Position;
  int     ThirstPct;

  HitPointsGain = 0;
  HitPoints     = pDnodeActor->pPlayer->HitPoints;
  Level         = pDnodeActor->pPlayer->Level;
  Position      = pDnodeActor->pPlayer->Position;
  HitPointsMax  = Level * PLAYER_HPT_PER_LEVEL;
  HungerPct     = pDnodeActor->pPlayer->Hunger;
  ThirstPct     = pDnodeActor->pPlayer->Thirst;
  GainReducePct = int(floor((HungerPct+ThirstPct)/(200/MGRP)));
  if (HitPoints < HitPointsMax)
  { // Hit points have fallen below maximun
    if (Position == "stand")
    { // Additional hit points gained while standing
      HitPointsGain = Level * HPT_GAIN_STAND;
    }
    else
    if (Position == "sit")
    { // Additional hit points gained while siting
      HitPointsGain = Level * HPT_GAIN_SIT;
    }
    else
    if (Position == "sleep")
    { // Additional hit points gained while sleeping
      HitPointsGain = Level * HPT_GAIN_SLEEP;
    }
    HitPointsGain -= int(ceil(HitPointsGain*(GainReducePct/100.0)));
    HitPoints += HitPointsGain;
    if (HitPoints > HitPointsMax)
    { // Prevent hit points from exceeding the maximum
      HitPoints = HitPointsMax;
    }
    pDnodeActor->pPlayer->HitPoints = HitPoints;
  }
}

/***********************************************************
* Violence, as in ... WHACK 'em!                           *
************************************************************/

void Communication::Violence()
{
  ViolencePlayer();
  if (pDnodeActor->PlayerStateFighting)
  {
    ViolenceMobile();
  }
}

/***********************************************************
* Mobile's turn to do some damage                          *
************************************************************/

void Communication::ViolenceMobile()
{
  int     DamageToPlayer;
  CString HealthPct;
  int     HitPoints;
  int     HitPointsMax;
  int     i;
  CString MobileAttack;
  CString MobileBeenWhacked;
  int     MobileDamage;
  CString MobileDesc1;
  CString MobileId;
  CString MobileIdCheck;
  int     PAC;
  CString PlayerBeenWhacked;
  CString TmpStr;

  i = 0;
  i++;
  MobileId = Violence::GetMobPlayerMobileId(pDnodeActor->PlayerName, i);
  while (MobileId != "No more mobiles")
  { // For each mob whacking the player
    PAC               = pDnodeActor->pPlayer->ArmorClass;
    MobileAttack      = Violence::GetMobileAttack(MobileId);
    MobileDamage      = Violence::GetMobileDamage(MobileId);
    MobileDesc1       = Violence::GetMobileDesc1(MobileId);
    DamageToPlayer    = Violence::CalcDamageToPlayer(MobileDamage, PAC);
    PlayerBeenWhacked = Violence::WhackPlayer(MobileDesc1, MobileAttack, DamageToPlayer);
    pDnodeActor->pPlayer->HitPoints -= DamageToPlayer;
    HitPoints = pDnodeActor->pPlayer->HitPoints;
    // Calculate health percentage
    HitPointsMax = pDnodeActor->pPlayer->Level * PLAYER_HPT_PER_LEVEL;
    HealthPct = Violence::CalcHealthPct(HitPoints, HitPointsMax);
    // Add heath pct to PlayerBeenWhacked
    PlayerBeenWhacked.Insert(0, " ");
    PlayerBeenWhacked.Insert(0, HealthPct);
    pDnodeActor->PlayerOut += PlayerBeenWhacked;
    pDnodeActor->PlayerOut += "\r\n";
    if (HitPoints <= 0)
    { // Player is dead, how sad
      ViolencePlayerDied(MobileDesc1);
      return;
    }
    i++;
    MobileId = Violence::GetMobPlayerMobileId(pDnodeActor->PlayerName, i);
  }
  // Player is still alive!
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
* Mobile has died                                          *
************************************************************/

void Communication::ViolenceMobileDied(CString MobileBeenWhacked,
                                       CString MobileDesc1,
                                       CString MobileId)
{
  CString  DeadMsg;
  int      ExpPoints;
  CString  GainLoose;
  int      MobileExpPoints;
  CString  MobileExpPointsLevel;
  CString  MobileIdCheck;
  int      MobileLevel;
  CString  MobileLoot;
  CString  TmpStr;

  MobileExpPointsLevel = Violence::GetMobileExpPointsLevel(MobileId);
  MobileExpPoints      = atoi(Utility::GetWord(MobileExpPointsLevel, 1));
  MobileLevel          = atoi(Utility::GetWord(MobileExpPointsLevel, 2));
  MobileLoot           = Violence::GetMobileLoot(MobileId);
  // Send dead mob message to player
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += MobileBeenWhacked;
  // Let others in room know that the mobile is DEAD!
  DeadMsg =  "&R";
  DeadMsg += pDnodeActor->PlayerName;
  DeadMsg += " has vanquished ";
  DeadMsg += MobileDesc1;
  DeadMsg += "!";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, DeadMsg);
  // Calculate experience distribution
  if (pDnodeActor->pPlayer->pPlayerGrpMember[0] != NULL)
  { // Player is in a group, award group experience
    GrpExperience(MobileExpPoints, MobileLevel);
  }
  else
  { // Calculate adjusted experience
    if (MobileExpPoints >= 0)
    { // Player gains xp
      ExpPoints = Player::CalcAdjustedExpPoints(pDnodeActor->pPlayer->Level, MobileLevel, MobileExpPoints);
      GainLoose = "Gain";
    }
    else
    { // Player looses xp
      ExpPoints = MobileExpPoints * pDnodeActor->pPlayer->Level;
      GainLoose = "Loose";
    }
    // Send experience message to player 
    if (ExpPoints >= 0)
    { // Player gains xp
      sprintf(Buf, "%d",ExpPoints);
      TmpStr = ConvertStringToCString(Buf);
    }
    else
    { // Player looses xp
      sprintf(Buf, "%d",ExpPoints * -1);
      TmpStr = ConvertStringToCString(Buf);
    }
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "&Y";
    pDnodeActor->PlayerOut += "You ";
    pDnodeActor->PlayerOut += GainLoose;
    pDnodeActor->PlayerOut += " ";
    pDnodeActor->PlayerOut += TmpStr;
    pDnodeActor->PlayerOut += " points of Experience!";
    pDnodeActor->PlayerOut += "&N";
    pDnodeActor->PlayerOut += "\r\n";
    // Gain some experience
    pDnodeActor->pPlayer->GainExperience(pDnodeActor, ExpPoints);
    // Save player
    pDnodeActor->pPlayer->Save();
    // Player prompt
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  }
  // Fight done, clean up
  Mobile::DeletePlayerMob(pDnodeActor->PlayerName);
  Mobile::DeleteMobPlayer(pDnodeActor->PlayerName, MobileId);
  Mobile::DeleteMobStats(MobileId);
  pDnodeActor->PlayerStateFighting = false;
  Mobile::UpdateMobInWorld(MobileId, "remove");
  //***************************************************
  //* Stop other players who are whacking this mobile *
  //***************************************************
  Descriptor::SetpDnodeCursorFirst();
  while (!Descriptor::EndOfDnodeList())
  { // Loop thru all connections
    pDnodeOthers = Descriptor::GetDnode();
    if (pDnodeOthers->PlayerStateFighting)
    { // Players who are fighting
      MobileIdCheck = Violence::GetPlayerMobMobileId(pDnodeOthers->PlayerName);
      if (MobileId == MobileIdCheck)
      { // The same mobile
        Mobile::DeletePlayerMob(pDnodeOthers->PlayerName);
        Mobile::DeleteMobPlayer(pDnodeOthers->PlayerName, MobileId);
        pDnodeOthers->PlayerStateFighting = false;
      }
    }
    Descriptor::SetpDnodeCursorNext();
  }
  // Re-position pDnodeCursor
  RepositionDnodeCursor();
  //********************
  // Hand out the loot *
  //********************
  ViolenceMobileLoot(MobileLoot);
  ViolenceMobileMore();
}

/***********************************************************
* Hand out the loot                                        *
************************************************************/

void Communication::ViolenceMobileLoot(CString Loot)
{
  Dnode      *pDnodeGrpMem;
  Player     *pPlayerGrpLdr;
  int         i;
  bool        LootFlag;
  CStdioFile  MobileLootFile;
  CString     MobileLootFileName;
  bool        NoLoot;
  CString     Stuff;
  int         Success;

  MobileLootFileName  = LOOT_DIR;
  MobileLootFileName += Loot;
  MobileLootFileName += ".txt";

  Success = MobileLootFile.Open(MobileLootFileName,
                     CFile::modeRead |
                     CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Communication::ViolenceMobileLoot - Error opening mobile loot file, it may not exist", MB_ICONSTOP);
    _endthread();
  }
  NoLoot = true;
  MobileLootFile.ReadString(Stuff);
  while (Stuff != "")
  {
    LootFlag = ViolenceMobileLootHandOut(Stuff);
    if (LootFlag)
    { // Ok, player got some loot, so set NoLoot to false
      NoLoot = false;
    }
    MobileLootFile.ReadString(Stuff);
  }
  MobileLootFile.Close();
  if (NoLoot)
  { // Player got no loot
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "&Y";
    pDnodeActor->PlayerOut += "You didn't get any loot.";
    pDnodeActor->PlayerOut += "&N";
  }
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  // If player is in a group, send other group member their prompt
  pPlayerGrpLdr = pDnodeActor->pPlayer->pPlayerGrpMember[0];
  if (pPlayerGrpLdr != NULL)
  { // Player is in a group
    for (i = 0; i < GRP_LIMIT; i++)
    { // Loop thru leader's member list
      if (pPlayerGrpLdr->pPlayerGrpMember[i] != NULL)
      { // Group member
        if (pPlayerGrpLdr->pPlayerGrpMember[i] != pDnodeActor->pPlayer)
        { // Group member that is not the member who got the loot
          pDnodeGrpMem = GetTargetDnode(pPlayerGrpLdr->pPlayerGrpMember[i]->Name);
          if (NoLoot)
          { // Player got no loot
            pDnodeGrpMem->PlayerOut += "\r\n";
            pDnodeGrpMem->PlayerOut += pDnodeActor->PlayerName;
            pDnodeGrpMem->PlayerOut += " didn't get any loot.";
          }
          pDnodeGrpMem->PlayerOut += "\r\n";
          pDnodeGrpMem->pPlayer->CreatePrompt();
          pDnodeGrpMem->PlayerOut += pDnodeGrpMem->pPlayer->GetOutput();
        }
      }
      else
      { // No more members to process
        break;
      }
    }
  }
}

/***********************************************************
* Hand out the loot - for real this time                   *
************************************************************/

bool Communication::ViolenceMobileLootHandOut(CString Loot)
{
  Object  *pObject;
  Dnode   *pDnodeGrpMem;
  Player  *pPlayerGrpLdr;
  int      Chance;
  int      Count;
  bool     GotLoot;
  int      i;
  CString  LogBuf;
  CString  ObjectId;
  int      Percent;

  GotLoot  = false;
  Count    = atoi(Utility::GetWord(Loot, 1));
  Percent  = atoi(Utility::GetWord(Loot, 2));
  ObjectId = Utility::GetWord(Loot, 3);
  for (i = 1; i <= Count; i++)
  { // For each object, for example twice if '2 60 RatEar' is specified
    Chance = Utility::GetRandomNumber(100);
    if (Chance < Percent)
    { // Random number came up less than 'percent chance of getting loot'
      pObject = Object::IsObject(ObjectId);
      if (!pObject)
      { // Object does not exist, Log it
        LogBuf += "Loot object not found";
        LogBuf += " ";
        LogBuf += ObjectId;
        Log::LogIt(LogBuf);
      }
      // Tell player what they got
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->PlayerOut += "You loot ";
      pDnodeActor->PlayerOut += pObject->Desc1;
      pDnodeActor->PlayerOut += ".";
      // If player is in a group, let other group members know what they looted
      pPlayerGrpLdr = pDnodeActor->pPlayer->pPlayerGrpMember[0];
      if (pPlayerGrpLdr != NULL)
      { // Player is in a group
        for (i = 0; i < GRP_LIMIT; i++)
        { // Loop thru leader's member list
          if (pPlayerGrpLdr->pPlayerGrpMember[i] != NULL)
          { // Group member
            if (pPlayerGrpLdr->pPlayerGrpMember[i] != pDnodeActor->pPlayer)
            { // Group member that is not the member who got the loot
              pDnodeGrpMem = GetTargetDnode(pPlayerGrpLdr->pPlayerGrpMember[i]->Name);
              pDnodeGrpMem->PlayerOut += "\r\n";
              pDnodeGrpMem->PlayerOut += pDnodeActor->PlayerName;
              pDnodeGrpMem->PlayerOut += " looted ";
              pDnodeGrpMem->PlayerOut += pObject->Desc1;
              pDnodeGrpMem->PlayerOut += ".";
            }
          }
          else
          { // No more members to process
            break;
          }
        }
      }
      delete pObject;
      Object::AddObjToPlayerInv(pDnodeActor, ObjectId);
      GotLoot = true;
    }
  }
  return GotLoot;
}

/***********************************************************
* More mobiles to fight?                                   *
************************************************************/

void Communication::ViolenceMobileMore()
{
  CString MobileId;

  MobileId = Violence::GetMobPlayerMobileId(pDnodeActor->PlayerName, 1);
  if (MobileId == "No more mobiles")
  {
    return;
  }
  Mobile::CreatePlayerMob(pDnodeActor->pPlayer->Name, MobileId);
  pDnodeActor->PlayerStateFighting = true;
}

/***********************************************************
* Player's turn to do some damage                          *
************************************************************/

void Communication::ViolencePlayer()
{
  int     DamageToMobile;
  CString DeadOrAlive;
  int     MaxDamageToMobile;
//int     MobileArmor; currently not being used
  CString MobileBeenWhacked;
  CString MobileDesc1;
  CString MobileId;
  int     WeaponSkill;
  CString WeaponType;

  WeaponSkill       = pDnodeActor->pPlayer->GetWeaponSkill();
  WeaponType        = pDnodeActor->pPlayer->WeaponType;
  MaxDamageToMobile = pDnodeActor->pPlayer->WeaponDamage;
  MobileId          = Violence::GetPlayerMobMobileId(pDnodeActor->PlayerName);
//MobileArmor       = Violence::GetMobileArmor(MobileId);
  MobileDesc1       = Violence::GetMobileDesc1(MobileId);
  DamageToMobile    = Violence::CalcDamageToMobile(MaxDamageToMobile, WeaponSkill);
  MobileBeenWhacked = Violence::WhackMobile(MobileId, DamageToMobile, MobileDesc1, WeaponType);
  // Player has whacked the mobile
  DeadOrAlive = Utility::GetWord(MobileBeenWhacked, 1);
  MobileBeenWhacked = Utility::DeleteWord(MobileBeenWhacked, 1);
  DeadOrAlive.MakeLower();
  if (DeadOrAlive == "alive")
  { // Mobile is not dead, Send fight messages to player
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += MobileBeenWhacked;
    pDnodeActor->PlayerOut += "\r\n";
  }
  else
  { // Mobile is dead
    ViolenceMobileDied(MobileBeenWhacked, MobileDesc1, MobileId);
  }
}

/***********************************************************
* Player has died, sad but true                            *
************************************************************/

void Communication::ViolencePlayerDied(CString MobileDesc1)
{
  int      CandidateCount;
  CString  CandidateList;
  int      CandidateTarget;
  CString  DeadMsg;
  CString  MobileId;
  CString  MobileIdSave;
  CString  RoomIdBeforeDying;
  CString  Target;
  CString  TmpStr;

  pDnodeActor->pPlayer->HitPoints = 0;
  // Tell player of their demise
  pDnodeActor->PlayerOut += "&R";
  pDnodeActor->PlayerOut += "You have been vanquished by";
  pDnodeActor->PlayerOut += " ";
  pDnodeActor->PlayerOut += MobileDesc1;
  pDnodeActor->PlayerOut += "!!!";
  pDnodeActor->PlayerOut += "\r\n";
  // Loose experience?
  if (pDnodeActor->pPlayer->Level > PLAYER_LOOSES_EXP_LEVEL)
  { // Player is loosing some experience
    pDnodeActor->pPlayer->Experience -= Utility::GetRandomNumber(pDnodeActor->pPlayer->Level * 5 * MOB_EXP_PER_LEVEL);
    if (pDnodeActor->pPlayer->Experience < 0)
    { // Stop experience from going negative
      pDnodeActor->pPlayer->Experience = 0;
    }
    pDnodeActor->PlayerOut += "&Y";
    pDnodeActor->PlayerOut += "You lost some experience.";
    pDnodeActor->PlayerOut += "&N";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "\r\n";
  }
  else
  { // No experience was lost
    sprintf(Buf, "%d", PLAYER_LOOSES_EXP_LEVEL);
    TmpStr = ConvertStringToCString(Buf);
    pDnodeActor->PlayerOut += "&Y";
    pDnodeActor->PlayerOut += "After level ";
    pDnodeActor->PlayerOut += TmpStr;
    pDnodeActor->PlayerOut += ", you will lose experience.";
    pDnodeActor->PlayerOut += "&N";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "\r\n";
  }
  // Tell players in room of player's demise
  DeadMsg  = "&R";
  DeadMsg += pDnodeActor->PlayerName;
  DeadMsg += " has been vanquished by ";
  DeadMsg += MobileDesc1;
  DeadMsg += "!";
  DeadMsg += "\r\n";
  DeadMsg += pDnodeActor->PlayerName;
  DeadMsg += " has been taken to a safe place to recover.";
  DeadMsg += "&N";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, DeadMsg);
  RoomIdBeforeDying = pDnodeActor->pPlayer->RoomId;
  // Move player to a safe room, stop the fight
  pDnodeActor->pPlayer->RoomId = SAFE_ROOM;
  Room::ShowRoom(pDnodeActor);  
  pDnodeActor->PlayerStateFighting = false;
  // Get mobile id for mob that dead player was fighting
  MobileIdSave = Violence::GetPlayerMobMobileId(pDnodeActor->PlayerName);
  // Delete PlayerMob file
  Mobile::DeletePlayerMob(pDnodeActor->PlayerName);
  //********************************************************
  //* Delete fighting mobiles from MobPlayer file          *
  //********************************************************
  Descriptor::SetpDnodeCursorFirst();
  while (!Descriptor::EndOfDnodeList())
  { // Loop thru all connections
    pDnodeOthers = Descriptor::GetDnode();
    if (pDnodeOthers->PlayerStateFighting)
    { // Players who are fighting
      if (RoomIdBeforeDying == pDnodeOthers->pPlayer->RoomId)
      { // In the same room
        MobileId = Violence::GetPlayerMobMobileId(pDnodeOthers->PlayerName);
        Mobile::DeleteMobPlayer(pDnodeActor->PlayerName, MobileId);
        if (MobileId == MobileIdSave)
        { // Add player to candidate list for MobileIdSave
          CandidateList += pDnodeOthers->PlayerName;
          CandidateList += " ";
        }
      }
    }
    Descriptor::SetpDnodeCursorNext();
  }
  // Re-position pDnodeCursor
  RepositionDnodeCursor();
  // Put mobiles that are not fighting back in room
  Mobile::PutMobBackInRoom(pDnodeActor->PlayerName, RoomIdBeforeDying);
  // Player is gone, so delete MobPlayer completely
  Mobile::DeleteMobPlayer(pDnodeActor->PlayerName, "file");
  // Select a new target for MobileIdSave
  if (CandidateList.GetLength() == 0)
  { // MobileIdSave's target is still in room, nothing to do
    return;
  }
  CandidateCount  = Utility::WordCount(CandidateList);
  CandidateTarget = Utility::GetRandomNumber(CandidateCount);
  Target          = Utility::GetWord(CandidateList, CandidateTarget);
  Mobile::CreateMobPlayer(Target, MobileIdSave);
}