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

void    AddObjToPlayerInv(Dnode *pDnodeTgt1, string ObjectId);  // Object
Dnode  *GetDnode();                                             // Descriptor
Mobile *IsMobInRoom(CString MobileName);                        // Mobile
Mobile *IsMobValid(string MobileId);                            // Mobile
void    ShowRoom(Dnode *pDnode);                                // Room
void    ShowPlayerEqu(Dnode *pDnodeTgt1);                       // Object

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
  return GetTargetDnode(ConvertCStringToString(TargetName));
}
Dnode *Communication::GetTargetDnode(string TargetName)
{
  Dnode   *pDnodeLookup;
  bool     TargetFound;
  string   LookupName;

  TargetFound = false;
  TargetName = StrMakeLower(TargetName);
  // Get Dnode pointer for target player
  SetpDnodeCursorFirst();
  while (!EndOfDnodeList())
  { // Loop thru all connections
    pDnodeLookup = pDnodeCursor;
// TODO - steve - Can the line above replace the line below ???    
    pDnodeLookup = GetDnode();
    LookupName = pDnodeLookup->PlayerName;
    LookupName = StrMakeLower(LookupName);
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
  string  FightingMsg;

  if (!pDnodeActor->PlayerStateFighting)
  {
    return false;
  }
  // Player is fighting, generate random message
  RandomNumber = GetRandomNumber(5);
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
  pDnodeActor->PlayerOut += ConvertStringToCString(FightingMsg);
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
  string  SleepingMsg;

  if (pDnodeActor->pPlayer->Position != "sleep")
  {
    return false;
  }
  // Player is sleeping, generate random message
  RandomNumber = GetRandomNumber(5);
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
  pDnodeActor->PlayerOut += ConvertStringToCString(SleepingMsg);
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  return true;
}

/***********************************************************
* Send output to all players                               *
************************************************************/
void Communication::SendToAll(string PlayerMsg, string AllMsg)
{
  SendToAll(ConvertStringToCString(PlayerMsg), ConvertStringToCString(AllMsg));
}
void Communication::SendToAll(CString PlayerMsg, string AllMsg)
{
  SendToAll(PlayerMsg, ConvertStringToCString(AllMsg));
}
void Communication::SendToAll(CString PlayerMsg, CString AllMsg)
{
  SetpDnodeCursorFirst();
  while (!EndOfDnodeList())
  { // Loop thru all connections
    pDnodeOthers = GetDnode();
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
          SetpDnodeCursorNext();
          continue;
        }
        pDnodeOthers->PlayerOut += AllMsg;
        pDnodeOthers->pPlayer->CreatePrompt();
        pDnodeOthers->PlayerOut += pDnodeOthers->pPlayer->GetOutput();
      }
    }
    SetpDnodeCursorNext();
  }
  // Re-position pDnodeCursor
  RepositionDnodeCursor();
}

/***********************************************************
* Send output to other players in the same room as player  *
************************************************************/
void Communication::SendToRoom(string TargetRoomId, string MsgText)
{
  SendToRoom(ConvertStringToCString(TargetRoomId), ConvertStringToCString(MsgText));
}
void Communication::SendToRoom(CString TargetRoomId, string MsgText)
{
  SendToRoom(TargetRoomId, ConvertStringToCString(MsgText));
}
void Communication::SendToRoom(CString TargetRoomId, CString MsgText)
{
  string   LookupRoomId;

  // Send output to all players in the room
  SetpDnodeCursorFirst();
  while (!EndOfDnodeList())
  { // Loop thru all connections
    pDnodeOthers = GetDnode();
    if (pDnodeOthers->PlayerStatePlaying)
    { // Player is logged in and playing
      LookupRoomId = pDnodeOthers->pPlayer->RoomId;
      if (pDnodeSrc != pDnodeOthers)
      { // It's not the originating player
        if (pDnodeTgt != pDnodeOthers)
        { // It's not the target player
          if (ConvertCStringToString(TargetRoomId) == LookupRoomId)
          { // Others who are in the same room
            if (pDnodeOthers->pPlayer->Position != "sleep")
            { // and are not sleeping
              if (pDnodeSrc)
              { // Player sending a message, not an NPC
                if (pDnodeSrc->PlayerStateInvisible)
                { // Player is invisible, no message is sent
                  SetpDnodeCursorNext();
                  continue;
                }
              }
              if (pDnodeOthers->PlayerStateInvisible)
              { // Player is invisible, no message is sent
                SetpDnodeCursorNext();
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
    SetpDnodeCursorNext();
  }
  // Re-position pDnodeCursor
  RepositionDnodeCursor();
}

/***********************************************************
* Show players in a given room                             *
************************************************************/

void Communication::ShowPlayersInRoom(Dnode *pDnode)
{
  string   LookupRoomId;
  string   TargetRoomId;

  TargetRoomId = pDnode->pPlayer->RoomId;
  // Show players in same room as player
  SetpDnodeCursorFirst();
  while (!EndOfDnodeList())
  { // Loop thru all connections
    pDnodeOthers = GetDnode();
    if (pDnode != pDnodeOthers)
    { // It's another player
      if (pDnodeOthers->PlayerStatePlaying)
      { // who is playing
        LookupRoomId = pDnodeOthers->pPlayer->RoomId;
        if (TargetRoomId == LookupRoomId)
        { // and is in the same room
          if (pDnodeOthers->PlayerStateInvisible)
          { // Player is invisible, no show
            SetpDnodeCursorNext();
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
    SetpDnodeCursorNext();
  }
  // Re-position pDnodeCursor
  RepositionDnodeCursor();
}

/***********************************************************
 * Check for new connections                               *
 ***********************************************************/

void Communication::SockCheckForNewConnections()
{
  string                LogBuf;
  int                   SocketCount;
  static struct timeval TimeOut{};

  DEBUGIT(5);
  TimeOut.tv_sec = 0;
  TimeOut.tv_usec = 1;
  FD_ZERO(&InpSet);
  FD_ZERO(&OutSet);
  FD_ZERO(&ExcSet);
  FD_SET(ListenSocket, &InpSet);
  // Check status of connections
  SetpDnodeCursorFirst();
  while (!EndOfDnodeList())
  { // Loop thru all connections
    pDnodeActor = GetDnode();
    FD_SET(pDnodeActor->DnodeFd, &InpSet);
    FD_SET(pDnodeActor->DnodeFd, &OutSet);
    FD_SET(pDnodeActor->DnodeFd, &ExcSet);
    SetpDnodeCursorNext();
  }
  // Detect socket state (pg 159)
  SocketCount = select(-1, &InpSet, &OutSet, &ExcSet, &TimeOut);
  if (SocketCount == -1)
  { // Something is wrong
    sprintf(Buf,"%s", strerror(errno));
    LogBuf = "Communication::SockCheckForNewConnections: select: " + (string)Buf;
    LogIt(LogBuf);
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
  string  LogBuf;
  int     Result;

  DEBUGIT(1);
  // Close the socket (pg 70)
  Result = ::closesocket(ListenSocket);
  if (Result!= 0)
  {
    AfxMessageBox("Communication::~Communication - Error: closesocket", MB_ICONSTOP);
    _endthread();
  }
  sprintf(Buf, "%d", Port);
  LogBuf = "Closed port " + (string)Buf;;
  LogIt(LogBuf);
}

/***********************************************************
 * Open port                                               *
 ***********************************************************/

void Communication::SockOpenPort(int Port)
{
  unsigned long FionbioParm;
  struct        linger      ld{};
  string        LogBuf;
  int           OptionValue;
  int           Result;
  struct        sockaddr_in sa{};
  WORD          VersionRequested;
  WSADATA       WsaData;

  DEBUGIT(1);
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
    LogBuf = "WinSock not available!: " + (string)Buf;
    LogIt(LogBuf);
    PrintIt("Communication::SockOpenPort - WinSock not available");
    exit(1);
  }
  // Establish a streaming socket (pg 51)
  ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (ListenSocket == SOCKET_ERROR)
  {
    sprintf(Buf, "%s", strerror(errno));
    LogBuf = "Communication:SockOpenPort - Error: initializing socket: " + (string)Buf;
    LogIt(LogBuf);
    PrintIt("Communication::SockOpenPort - Error: initializing socket");
    exit(1);
  }
  // Enable reuse of local socket name (pg 305)
  Result = setsockopt(ListenSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&OptionValue, sizeof(OptionValue));
  if (Result != 0)
  {
    sprintf(Buf, "%s", strerror(errno));
    LogBuf = "Communication:SockOpenPort - Error: setsockopt: SO_REUSEADDR: " + (string)Buf;
    LogIt(LogBuf);
    PrintIt("Communication::SockOpenPort - Error: setsockopt: SO_REUSEADDR");
    exit(1);
  }
  // Establish underlying TCP/IP buffer size (pg 305)
  Result = setsockopt(ListenSocket, SOL_SOCKET, SO_SNDBUF, (char *) &OptionValue, sizeof(OptionValue));
  if (Result != 0)
  {
    sprintf(Buf, "%s", strerror(errno));
    LogBuf = "Communication:SockOpenPort - Error: setsockopt: SO_SNDBUF: " + (string)Buf;
    LogIt(LogBuf);
    PrintIt("Communication::SockOpenPort - Error: setsockopt: SO_SNDBUF");
    exit(1);
  }
  // Disable linger and set timeout to zero (pg 301)
  Result = setsockopt(ListenSocket, SOL_SOCKET, SO_LINGER, (char *) &ld, sizeof(ld));
  if (Result != 0)
  {
    sprintf(Buf, "%s", strerror(errno));
    LogBuf = "Communication:SockOpenPort - Error: setsockopt: SO_LINGER: " + (string)Buf;
    LogIt(LogBuf);
    PrintIt("Communication::SockOpenPort - Error: setsockopt: SO_LINGER");
    exit(1);
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
    LogBuf = "Communication:SockOpenPort - Error: bind: " + (string)Buf;
    LogIt(LogBuf);
    PrintIt("Communication::SockOpenPort - Error: bind");
    exit(1);
  }
  // Make socket nonblocking (pg 286)
  Result = ioctlsocket(ListenSocket, FIONBIO, &FionbioParm);
  if (Result != 0)
  {
    sprintf(Buf, "%s", strerror(errno));
    LogBuf = "Communication:SockOpenPort - Error: ioctlsocket: " + (string)Buf;
    LogIt(LogBuf);
    PrintIt("Communication::SockOpenPort - Error: ioctlsocket");
    exit(1);
  }
  // Listen on port and limit pending connections (pg 60)
  Result = listen(ListenSocket, 20);
  if (Result != 0)
  {
    sprintf(Buf, "%s", strerror(errno));
    LogBuf = "Communication:SockOpenPort - Error: listen: " + (string)Buf;
    LogIt(LogBuf);
    ::closesocket(ListenSocket);
    PrintIt("Communication::SockOpenPort - Error: listen");
    exit(1);
  }
  sprintf(Buf, "%d", Port);
  LogBuf = "Listening on port " + (string)Buf;
  LogIt(LogBuf);
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
  string   LogBuf;
  int      RecvByteCount;

  DEBUGIT(5);
  //***********************
  //* Service connections *
  //***********************
  SetpDnodeCursorFirst();
  while (!EndOfDnodeList())
  { // Loop thru all connections
    pDnodeActor = GetDnode();
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
        LogBuf = "Time out during logon on descriptor " + (string)Buf;;
        LogIt(LogBuf);
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
      SetpDnodeCursorFirst();
      while (!EndOfDnodeList())
      { // Loop thru all connections
        pDnodeOthers = GetDnode();
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
          LogIt(LogBuf);
        }
        SetpDnodeCursorNext();
      }
      // Re-position pDnodeCursor
      RepositionDnodeCursor();
    }
    //**********************
    //* Send player output *
    //**********************
    if (StrGetLength(pDnodeActor->PlayerOut) > 0)
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
      if (DeleteNode())
      { // When connection is deleted from list, log it
        sprintf(Buf, "%d", DnodeFdSave);
        LogBuf = "Closed connection on descriptor " + ConvertStringToCString(Buf);;
        LogIt(LogBuf);
        ConnectionCount = GetCount();
        if (ConnectionCount == 1)
        { // Connection count is one means no players are connected
          if (StateStopping)
          { // OMugs is stopping
            StateRunning = false;
          }
        }
      }
      // Skip to next dnode, this player's dnode has been deleted
      SetpDnodeCursorNext();
      continue;
    }
    //************************
    //* Process player input *
    //************************
    if (!StateStopping)
    { // Game is not shutting down
      LineFeedPosition = StrFindOneOf(pDnodeActor->PlayerInp, "\r\n");
      if (LineFeedPosition > -1)
      { // Found a newline, parse the command
        if (pDnodeActor->PlayerName != "Ixaka" && pDnodeActor->PlayerName != "Kwam")
        { // Log the player's input
          LogBuf = pDnodeActor->PlayerIpAddress;
          LogBuf += " ";
          if (pDnodeActor->pPlayer)
          {
            LogBuf += pDnodeActor->pPlayer->RoomId;
            LogBuf += " ";
          }
          LogBuf += pDnodeActor->PlayerInp;
          StrReplace(LogBuf, "\r", " ");
          StrReplace(LogBuf, "\n", " ");
          LogIt(LogBuf);
        }
        CommandParse();
      }
    }
    // Get the next Dnode to process
    SetpDnodeCursorNext();
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
  sPlayerOut = pDnodeActor->PlayerOut;
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
  string     LogBuf;
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
  LogIt(LogBuf);
}

/***********************************************************
 * Check command authorization, level, and validity        *
 ***********************************************************/

string Communication::CommandCheck(string MudCmdChk)
{
  string  CommandCheckResult;
  string  ValCmd;
  string  ValCmdInfo;
  string  WhoCanDo;

  CommandCheckResult = "Not Found";
  for (auto& ValidCmd : ValidCmds)
  { // For each string in the ValidCmds vector
    ValCmdInfo = ValidCmd;
    ValCmd     = StrGetWord(ValCmdInfo, 1);
    WhoCanDo   = StrGetWord(ValCmdInfo, 2);
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
      if (stoi(WhoCanDo) > pDnodeActor->pPlayer->Level)
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
  string   BadCommandMsg;
  int      CmdStrLength;
  string   CommandCheckResult;
  string   LogBuf;
  string   MudCmdChk;
  bool     MudCmdOk;
  int      PositionOfNewline;
  int      RandomNumber;

  //**************************
  // Get next command string *
  //**************************
  CmdStr = ConvertStringToCString(pDnodeActor->PlayerInp);
  CmdStrLength = StrGetLength(CmdStr);
  PositionOfNewline = StrFindOneOf(CmdStr, "\r\n");
  if (PositionOfNewline < 0)
  { // No newline found, skip out
    return;
  }
  CmdStr = StrLeft(CmdStr, PositionOfNewline);
  pDnodeActor->PlayerInp = StrRight(pDnodeActor->PlayerInp, CmdStrLength - PositionOfNewline);
  pDnodeActor->PlayerInp = StrTrimLeft(pDnodeActor->PlayerInp);
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
  MudCmd = StrGetWord(CmdStr, 1);
  MudCmd = StrMakeLower(MudCmd);
  // Translate 'n' into 'go north'
  MudCmd = TranslateWord(MudCmd);
  if (StrCountWords(MudCmd) == 2)
  { // Re-get MudCmd. In the case of 'go north', MudCmd is 'go'
    CmdStr = MudCmd;
    MudCmd = StrGetWord(CmdStr, 1);
    MudCmd = StrMakeLower(MudCmd);
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
  if (StrGetWord(CommandCheckResult, 1) == "Level")
  { // Level restriction on command
    pDnodeActor->PlayerOut += "You must attain level ";
    pDnodeActor->PlayerOut += ConvertStringToCString(StrGetWord(CommandCheckResult, 2));
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
    if (IsSocial())
    { // Yep, it was a social
      return;
    }
  }
  //**************
  // Bad command *
  //**************
  if (!MudCmdOk)
  { // Not a valid cmd and it is not a social
    RandomNumber = GetRandomNumber(5);
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
    pDnodeActor->PlayerOut += ConvertStringToCString(BadCommandMsg);
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
    DoFollow(pDnodeActor, ConvertCStringToString(CmdStr));
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
    DoLook(ConvertCStringToString(CmdStr));
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
    DoRestore(ConvertCStringToString(CmdStr));
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
  MudCmd  = StrMakeFirstUpper(MudCmd);
  LogBuf  = MudCmd;
  LogBuf += " is in command array, but Do";
  LogBuf += MudCmd;
  LogBuf += " is not coded.";
  LogIt(LogBuf);
}

/***********************************************************
 * Advance command                                         *
 ***********************************************************/

void Communication::DoAdvance()
{
  int      Level;
  string   LevelString;
  string   LogBuf;
  string   PlayerName;
  string   PlayerNameSave;
  string   TargetName;
  string   TargetNameSave;
  string   TmpStr;

  DEBUGIT(1);
  PlayerName      = pDnodeActor->PlayerName;
  TargetName      = StrGetWord(CmdStr, 2);
  PlayerNameSave  = PlayerName;
  TargetNameSave  = TargetName;
  PlayerName = StrMakeLower(PlayerName);
  TargetName = StrMakeLower(TargetName);
  Level = atoi(StrGetWord(CmdStr, 3));
  sprintf(Buf, "%d", Level);
  LevelString = (string)Buf;
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
    pDnodeActor->PlayerOut += ConvertStringToCString(TargetNameSave);
    pDnodeActor->PlayerOut += " is not online.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (Level == pDnodeTgt->pPlayer->Level)
  { // Advance to same level ... that's just plain silly
    pDnodeActor->PlayerOut += ConvertStringToCString(TargetNameSave);
    pDnodeActor->PlayerOut += " is already at level ";
    pDnodeActor->PlayerOut += ConvertStringToCString(LevelString);
    pDnodeActor->PlayerOut += ".";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (Level == 0)
  { // Advance to level 0 ... not valid
    pDnodeActor->PlayerOut += ConvertStringToCString(TargetNameSave);
    pDnodeActor->PlayerOut += " cannot be advanced to level ";
    pDnodeActor->PlayerOut += ConvertStringToCString(LevelString);
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
  LogIt(LogBuf);
  // Send message to player
  pDnodeActor->PlayerOut += ConvertStringToCString(TargetNameSave);
  pDnodeActor->PlayerOut += " is now level ";
  pDnodeActor->PlayerOut += ConvertStringToCString(LevelString);
  pDnodeActor->PlayerOut += ".";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  // Send message to target
  pDnodeTgt->PlayerOut += "\r\n";
  pDnodeTgt->PlayerOut += "&Y";
  pDnodeTgt->PlayerOut += ConvertStringToCString(PlayerNameSave);
  if (Level > pDnodeTgt->pPlayer->Level)
  { // Level up!
    pDnodeTgt->PlayerOut += " has advanced you to level ";
  }
  else
  { // Level down :(
    pDnodeTgt->PlayerOut += " has DEMOTED you to level ";
  }
  pDnodeTgt->PlayerOut += ConvertStringToCString(LevelString);
  pDnodeTgt->PlayerOut += "!";
  pDnodeTgt->PlayerOut += "&N";
  pDnodeTgt->PlayerOut += "\r\n";
  // Make it so
  pDnodeTgt->pPlayer->Level      = Level;
  pDnodeTgt->pPlayer->Experience = CalcLevelExperience(Level);
  pDnodeTgt->pPlayer->Save();
  // Prompt
  pDnodeTgt->pPlayer->CreatePrompt();
  pDnodeTgt->PlayerOut += pDnodeTgt->pPlayer->GetOutput();
  // Restore the player as a bonus to being advanced
  DoRestore("restore " + ConvertCStringToString(pDnodeTgt->pPlayer->Name));
}

/***********************************************************
 * Afk command                                             *
 ***********************************************************/

void Communication::DoAfk()
{
  DEBUGIT(1);
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
  string   AssistMsg;
  string   MobileId;
  string   PlayerNameCheck;
  string   TargetNameCheck;
  string   TargetNameSave;
  bool     TargetNotHere;

  DEBUGIT(1);
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
  if (StrCountWords(CmdStr) < 2)
  { // No object or target
    pDnodeActor->PlayerOut += "Assist whom?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  PlayerNameCheck = pDnodeActor->PlayerName;
  TargetNameCheck = StrGetWord(CmdStr, 2);
  TargetNameSave  = TargetNameCheck;
  PlayerNameCheck = StrMakeLower(PlayerNameCheck);
  TargetNameCheck = StrMakeLower(TargetNameCheck);
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
    pDnodeActor->PlayerOut += ConvertStringToCString(TargetNameSave);
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
  MobileId = GetPlayerMobMobileId(ConvertCStringToString(pDnodeTgt->PlayerName));
  CreatePlayerMob(ConvertCStringToString(pDnodeActor->PlayerName), MobileId);
  pDnodeActor->PlayerStateFighting = true;
}

/***********************************************************
* Buy command                                              *
************************************************************/

void Communication::DoBuy()
{
  int      Cost;
  string   Desc1;
  string   ObjectId;
  string   ObjectName;
  string   RoomId;
  string   TmpStr;

  DEBUGIT(1);
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
  RoomId = ConvertCStringToString(pDnodeActor->pPlayer->RoomId);
  if (!IsShop(RoomId))
  { // Room is not a shop
    pDnodeActor->PlayerOut += "Find a shop.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  ObjectName = StrGetWord(CmdStr, 2);
  if (ObjectName == "")
  { // No object given
    pDnodeActor->PlayerOut += "Buy what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (StrCountWords(CmdStr) > 2)
  { // Buy command not only takes 1 object
    pDnodeActor->PlayerOut += "The buy command must be followed by only one word.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pObject = NULL;
  IsShopObj(RoomId, ObjectName); // Sets pObject
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
  pObject = NULL;
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
  AddObjToPlayerInv(pDnodeActor, ObjectId);
  // Player receives some money
  pDnodeActor->pPlayer->SetMoney('-', Cost, "Silver");
  // Send messages
  sprintf(Buf, "%d", Cost);
  TmpStr = ConvertStringToCString(Buf);
  pDnodeActor->PlayerOut += "You buy ";
  pDnodeActor->PlayerOut += ConvertStringToCString(Desc1);
  pDnodeActor->PlayerOut += " for ";
  pDnodeActor->PlayerOut += ConvertStringToCString(TmpStr);
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
  string  AllMsg;
  string  ChatMsg;
  string  PlayerMsg;

  DEBUGIT(1);
  //********************
  //* Validate command *
  //********************
  ChatMsg = GetWords(CmdStr, 2);
  if (StrGetLength(ChatMsg) < 1)
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
  SendToAll(ConvertStringToCString(PlayerMsg), ConvertStringToCString(AllMsg));
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
* Color command                                            *
************************************************************/

void Communication::DoColor()
{
  string TmpStr;

  DEBUGIT(1);
  TmpStr = StrGetWord(CmdStr, 2);
  TmpStr = StrMakeLower(TmpStr);
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
  string   HintMsg;
  int      LevelDiff;
  string   MobileName;
  string   PlayerName;
  string   PlayerNameCheck;
  string   RoomId;
  string   Target;

  DEBUGIT(1);
  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (StrCountWords(CmdStr) < 2)
  { // No target
    pDnodeActor->PlayerOut += "Consider whom or what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (StrCountWords(CmdStr) > 2)
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
  PlayerNameCheck = StrMakeLower(PlayerNameCheck);
  Target = StrGetWord(CmdStr, 2);
  MobileName = Target;
  Target = StrMakeLower(Target);
  if (Target == PlayerNameCheck)
  { // Trying to kill self
    pDnodeActor->PlayerOut += "Consider yourself considered!";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (IsPlayer(ConvertStringToCString(Target)))
  { // Trying to kill another player
    pDnodeActor->PlayerOut += "Why consider another player? Player killing is not allowed.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pMobile = IsMobInRoom(ConvertStringToCString(Target));
  if (!pMobile)
  { // Target mobile is not here
    pDnodeActor->PlayerOut += "There doesn't seem to be a(n) ";
    pDnodeActor->PlayerOut += ConvertStringToCString(MobileName);
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
  pDnodeActor->PlayerOut += ConvertStringToCString(HintMsg);
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
  string   AllMsg;
  string   LogBuf;
  string   Name;
  string   Password;
  string   Phrase;
  string   PlayerFileName;
  string   PlayerMsg;

  DEBUGIT(1);
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
  if (StrCountWords(CmdStr) < 3)
  {
    pDnodeActor->PlayerOut += "You must provide your name and password.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  Name     = StrGetWord(CmdStr, 2);
  Password = StrGetWord(CmdStr, 3);
  Phrase   = GetWords(CmdStr, 4);
  if (Name != ConvertCStringToString(pDnodeActor->PlayerName))
  {
    pDnodeActor->PlayerOut += "Your name was not entered correctly.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += "Upper and lowercase letters must match.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (Password != pDnodeActor->pPlayer->Password)
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
  LogIt(LogBuf);
  // Delete Player file
  PlayerFileName =  PLAYER_DIR;
  PlayerFileName += pDnodeActor->PlayerName;
  PlayerFileName += ".txt";
  TRY
  {
    CFile::Remove(ConvertStringToCString(PlayerFileName));
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
    CFile::Remove(ConvertStringToCString(PlayerFileName));
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
    CFile::Remove(ConvertStringToCString(PlayerFileName));
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
    CFile::Remove(ConvertStringToCString(PlayerFileName));
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
  AllMsg    = PronounSubstitute(AllMsg);
  pDnodeSrc = NULL;
  pDnodeTgt = NULL;
  SendToAll(ConvertStringToCString(PlayerMsg), ConvertStringToCString(AllMsg));
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Destroy command                                         *
 ***********************************************************/

void Communication::DoDestroy()
{
  string   ObjectName;
  string   TmpStr;

  DEBUGIT(1);
  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (StrCountWords(CmdStr) == 1)
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
  TmpStr = StrGetWord(CmdStr, 2);
  ObjectName = TmpStr;
  TmpStr = StrMakeLower(TmpStr);
  pObject = NULL;
  IsObjInPlayerInv(TmpStr); // Sets pObject
  if (!pObject)
  {
    pDnodeActor->PlayerOut += "You don't have a(n) ";
    pDnodeActor->PlayerOut += ConvertStringToCString(ObjectName);
    pDnodeActor->PlayerOut += ".\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  //******************
  //* Destroy object *
  //******************
  // Remove object from player's inventory
  RemoveObjFromPlayerInv(pObject->ObjectId, 1);
  // Send messages
  pDnodeActor->PlayerOut += "You help make the world cleaner by destroying ";
  pDnodeActor->PlayerOut += ConvertStringToCString(pObject->Desc1);
  pDnodeActor->PlayerOut += ".\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();        
  delete pObject;
  pObject = NULL;
}

/***********************************************************
 * Drink command                                           *
 ***********************************************************/

void Communication::DoDrink()
{
  string   DrinkMsg;
  string   ObjectName;
  string   RoomId;
  string   RoomName;
  string   RoomType;
  string   TmpStr;

  DEBUGIT(1);
  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (StrGetWord(CmdStr, 2) == "from")
  { // Toss out 'from', just extra verbage for player's benefit
    CmdStr = StrDelete(CmdStr, 5, 5);
  }
  if (StrCountWords(CmdStr) == 1)
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
  if (IsRoomType(RoomId, "Drink"))
  { // Room contains something to drink
    RoomName = GetRoomName(RoomId);
    TmpStr   = StrGetWord(CmdStr, 2);
    TmpStr   = StrMakeLower(TmpStr);
    RoomName = StrMakeLower(RoomName);
    if (IsWord(ConvertStringToCString(TmpStr), ConvertStringToCString(RoomName)))
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
  TmpStr = StrGetWord(CmdStr, 2);
  ObjectName = TmpStr;
  TmpStr = StrMakeLower(TmpStr);
  pObject = NULL;
  IsObjInPlayerInv(TmpStr); // Sets pObject
  if (!pObject)
  { // Object not found in player's inventory
    pDnodeActor->PlayerOut += "You don't have a(n) ";
    pDnodeActor->PlayerOut += ConvertStringToCString(ObjectName);
    pDnodeActor->PlayerOut += " in your inventory.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  //*******************
  //* Is object drink? *
  //*******************
  pObject->Type = StrMakeLower(pObject->Type);
  if (pObject->Type != "drink")
  { // Object is not a drink
    pDnodeActor->PlayerOut += "You can't drink ";
    pDnodeActor->PlayerOut += ConvertStringToCString(pObject->Desc1);
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
  pDnodeActor->PlayerOut += ConvertStringToCString(pObject->Desc1);
  pDnodeActor->PlayerOut += ".";
  pDnodeActor->PlayerOut += "\r\n";
  DrinkMsg  = pDnodeActor->PlayerName;
  DrinkMsg += " drinks from ";
  DrinkMsg += ConvertStringToCString(pObject->Desc1);
  DrinkMsg += ".";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, DrinkMsg);
  // Drink and remove object from player's inventory
  pDnodeActor->pPlayer->Drink(pObject->DrinkPct);
  pDnodeActor->pPlayer->Save();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  RemoveObjFromPlayerInv(pObject->ObjectId, 1);
  // Clean up and give prompt
  delete pObject;
  pObject = NULL;
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Drop command                                            *
 ***********************************************************/

void Communication::DoDrop()
{
  string   DropMsg;
  string   ObjectName;
  string   TmpStr;

  DEBUGIT(1);
  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (StrCountWords(CmdStr) == 1)
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
  TmpStr = StrGetWord(CmdStr, 2);
  ObjectName = TmpStr;
  TmpStr = StrMakeLower(TmpStr);
  pObject = NULL;
  IsObjInPlayerInv(TmpStr); // Sets pObject
  if (!pObject)
  {
    pDnodeActor->PlayerOut += "You don't have a(n) ";
    pDnodeActor->PlayerOut += ConvertStringToCString(ObjectName);
    pDnodeActor->PlayerOut += " in your inventory.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  //***************
  //* Drop object *
  //***************
  // Remove object from player's inventory
  RemoveObjFromPlayerInv(pObject->ObjectId, 1);
  // Send messages
  pDnodeActor->PlayerOut += "You drop ";
  pDnodeActor->PlayerOut += ConvertStringToCString(pObject->Desc1);
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
  AddObjToRoom(ConvertCStringToString(pDnodeActor->pPlayer->RoomId), pObject->ObjectId);
  delete pObject;
  pObject = NULL;
}

/***********************************************************
 * Eat command                                             *
 ***********************************************************/

void Communication::DoEat()
{
  string   EatMsg;
  string   ObjectName;
  string   TmpStr;

  DEBUGIT(1);
  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (StrCountWords(CmdStr) == 1)
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
  TmpStr = StrGetWord(CmdStr, 2);
  ObjectName = TmpStr;
  TmpStr = StrMakeLower(TmpStr);
  pObject = NULL;
  IsObjInPlayerInv(TmpStr); // Sets pObject
  if (!pObject)
  {
    pDnodeActor->PlayerOut += "You don't have a(n) ";
    pDnodeActor->PlayerOut += ConvertStringToCString(ObjectName);
    pDnodeActor->PlayerOut += " in your inventory.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  //*******************
  //* Is object food? *
  //*******************
  pObject->Type = StrMakeLower(pObject->Type);
  if (pObject->Type != "food")
  {
    pDnodeActor->PlayerOut += "You can't eat ";
    pDnodeActor->PlayerOut += ConvertStringToCString(pObject->Desc1);
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
  pDnodeActor->PlayerOut += ConvertStringToCString(pObject->Desc1);
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
  RemoveObjFromPlayerInv(pObject->ObjectId, 1);
  // Clean up and give prompt
  delete pObject;
  pObject = NULL;
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Emote command                                           *
 ***********************************************************/

void Communication::DoEmote()
{
  string  EmoteMsg;
  string  TmpStr;

  DEBUGIT(1);
  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  EmoteMsg = GetWords(CmdStr, 2);
  if (StrGetLength(EmoteMsg) < 1)
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
  pDnodeActor->PlayerOut += ConvertStringToCString(EmoteMsg);
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
  DEBUGIT(1);
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  ShowPlayerEqu(pDnodeActor);
}

/***********************************************************
 * Examine command                                         *
 ***********************************************************/

void Communication::DoExamine()
{
  bool     ObjectFound;
  string   ObjectName;
  string   ObjectType;
  string   TmpStr;

  DEBUGIT(1);
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
  if (StrCountWords(CmdStr) == 1)
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
  TmpStr = StrGetWord(CmdStr, 2);
  ObjectName = TmpStr;
  TmpStr = StrMakeLower(TmpStr);
  // Check room
  pObject = NULL;
  IsObjInRoom(TmpStr); // Sets pObject
  if (pObject)
  { // Object is in the room
    ObjectFound = true;
  }
  else
  { // Check player inventory
    pObject = NULL;
    IsObjInPlayerInv(TmpStr); // Sets pObject
    if (pObject)
    { // Object is in player's inventory
      ObjectFound = true;
    }
    else
    { // Check player equipment
      pObject = NULL;
      IsObjInPlayerEqu(TmpStr); // Sets pObject
      if (pObject)
      { // Object is in player's equipment
        ObjectFound = true;
      }
    }
  }
  if (!ObjectFound)
  { // Object can't be found
    pDnodeActor->PlayerOut += "There doesn't seem to be a(n) ";
    pDnodeActor->PlayerOut += ConvertStringToCString(ObjectName);
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
  pDnodeActor->PlayerOut += ConvertStringToCString(pObject->Desc1);
  pDnodeActor->PlayerOut += ".";
  pDnodeActor->PlayerOut += "\r\n";
  // Examine object
  pDnodeActor->PlayerOut += "Object type: ";
  pDnodeActor->PlayerOut += ConvertStringToCString(pObject->Type);
  pDnodeActor->PlayerOut += "\r\n";
  ObjectType = pObject->Type;
  ObjectType = StrMakeLower(ObjectType);
  if (ObjectType == "weapon")
  { // Object is a weapon
    pDnodeActor->PlayerOut += "Weapon type: ";
    pDnodeActor->PlayerOut += ConvertStringToCString(pObject->WeaponType);
    pDnodeActor->PlayerOut += "\r\n";
  }
  pObject->ExamineObj(pObject->ObjectId);
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  delete pObject;
  pObject = NULL;
}

/***********************************************************
 * Flee command                                            *
 ***********************************************************/

void Communication::DoFlee()
{
  int         CandidateCount;
  string      CandidateList;
  int         CandidateTarget;
  CFileStatus FileStatus;
  string      FleeMsg;
  string      MobileId;
  string      MobileIdSave;
  string      MobPlayerFileName;
  string      MudCmdIsExit;
  string      PlayerName1;
  string      PlayerName2;
  string      RoomIdBeforeFleeing;
  string      Target;
  string      TmpStr;

  DEBUGIT(1);
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
  TmpStr = StrGetWord(CmdStr, 2);
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
  sMudCmdIsExit = MudCmdIsExit;
  if (!IsExit(sMudCmdIsExit))
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
  FleeMsg = PronounSubstitute(FleeMsg);
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
  MobileIdSave = GetPlayerMobMobileId(PlayerName1);
  // Delete PlayerMob file -- player is no longer attacking mob
  DeletePlayerMob(ConvertStringToCString(PlayerName1));
  // See if a mob is whacking player
  MobPlayerFileName =  MOB_PLAYER_DIR;
  MobPlayerFileName += PlayerName1;
  MobPlayerFileName += ".txt";
  if (!CFile::GetStatus(ConvertStringToCString(MobPlayerFileName), FileStatus))
  { // If MobPlayer does not exist, then no mob is fighting player
    return;
  }
  //***************************
  //* Make mob switch targets *
  //***************************
  // Delete fighting mobiles from MobPlayer file
  SetpDnodeCursorFirst();
  while (!EndOfDnodeList())
  { // Loop thru all connections
    pDnodeOthers = GetDnode();
    if (pDnodeOthers->PlayerStateFighting)
    { // Players who are fighting
      if (RoomIdBeforeFleeing == ConvertCStringToString(pDnodeOthers->pPlayer->RoomId))
      { // In the same room
        PlayerName2 = pDnodeOthers->PlayerName;
        MobileId = GetPlayerMobMobileId(PlayerName2);
        DeleteMobPlayer(PlayerName1, MobileId);
        if (MobileId == MobileIdSave)
        { // Add player to candidate list for MobileIdSave
          CandidateList += PlayerName2;
          CandidateList += " ";
        }
      }
    }
    SetpDnodeCursorNext();
  }
  // Re-position pDnodeCursor
  RepositionDnodeCursor();
  // Put mobiles that are not fighting back in room
  PutMobBackInRoom(ConvertStringToCString(PlayerName1), ConvertStringToCString(RoomIdBeforeFleeing));
  // Player is gone, so delete MobPlayer completely
  DeleteMobPlayer(PlayerName1, "file");
  // Select a new target for MobileIdSave
  if (StrGetLength(CandidateList) == 0)
  { // No available target for MobileIdSave
    return;
  }
  CandidateCount  = StrCountWords(CandidateList);
  CandidateTarget = GetRandomNumber(CandidateCount);
  Target          = StrGetWord(CandidateList, CandidateTarget);
  CreateMobPlayer(Target, MobileIdSave);
}

/***********************************************************
 * Follow command                                          *
 ***********************************************************/

void Communication::DoFollow(Dnode *pDnode, string CmdStr1)
{
  Dnode   *pDnodeGrpLdr; // Group leader
  Dnode   *pDnodeGrpMem; // Group member
  int      i;
  int      j;
  string   Target;
  bool     TargetInGroup;
  string   TmpStr;

  DEBUGIT(1);
  CmdStr = ConvertStringToCString(CmdStr1);
  i      = 0;
  j      = 0;
  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  Target = StrGetWord(CmdStr, 2);
  TmpStr = Target;
  TmpStr = StrMakeLower(TmpStr);
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
    pDnode->PlayerOut += ConvertStringToCString(Target);
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
    pDnode->PlayerOut += ConvertStringToCString(Target);
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
  string   GetMsg;
  string   ObjectName;
  string   TmpStr;

  DEBUGIT(1);
  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (StrCountWords(CmdStr) == 1)
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
  TmpStr = StrGetWord(CmdStr, 2);
  ObjectName = TmpStr;
  TmpStr = StrMakeLower(TmpStr);
  pObject = NULL;
  IsObjInRoom(TmpStr); // Sets pObject
  if (!pObject)
  {
    pDnodeActor->PlayerOut += "There doesn't seem to be a(n) ";
    pDnodeActor->PlayerOut += ConvertStringToCString(ObjectName);
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
    pObject = NULL;
    return;
  }
  //**********************
  //* So take the object *
  //**********************
  // Remove object from room
  RemoveObjFromRoom(pObject->ObjectId);
  // Send messages
  pDnodeActor->PlayerOut += "You get ";
  pDnodeActor->PlayerOut += ConvertStringToCString(pObject->Desc1);
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
  AddObjToPlayerInv(pDnodeTgt, pObject->ObjectId);
  delete pObject;
  pObject = NULL;
}

/***********************************************************
 * Give command                                            *
 ***********************************************************/

void Communication::DoGive()
{
  string   GiveMsg;
  string   ObjectName;
  string   PlayerName;
  string   TargetName;
  bool     TargetNotHere;
  string   TmpStr;

  DEBUGIT(1);
  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (StrCountWords(CmdStr) < 2)
  { // No object or target
    pDnodeActor->PlayerOut += "Give what and to whom?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (StrCountWords(CmdStr) < 3)
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
  TmpStr = StrGetWord(CmdStr, 2);
  ObjectName = TmpStr;
  TmpStr = StrMakeLower(TmpStr);
  pObject = NULL;
  IsObjInPlayerInv(TmpStr); // Sets pObject
  if (!pObject)
  { // Player does not have object
    pDnodeActor->PlayerOut += "You don't have a(n) ";
    pDnodeActor->PlayerOut += ConvertStringToCString(ObjectName);
    pDnodeActor->PlayerOut += " in your inventory.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  //****************
  //* Is target Ok *
  //****************
  TargetNotHere = false;
  TmpStr = StrGetWord(CmdStr, 3);
  TargetName = TmpStr;
  TmpStr = StrMakeLower(TmpStr);
  PlayerName = pDnodeActor->PlayerName;
  PlayerName = StrMakeLower(PlayerName);
  if (PlayerName == TmpStr)
  { // Player is trying to give something to themself
    pDnodeActor->PlayerOut += "Giving something to youself is just plain silly!\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  pDnodeTgt = GetTargetDnode(ConvertStringToCString(TmpStr));
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
    pDnodeActor->PlayerOut += ConvertStringToCString(TargetName);
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
  pDnodeActor->PlayerOut += ConvertStringToCString(pObject->Desc1);
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
  pDnodeTgt->PlayerOut += ConvertStringToCString(pObject->Desc1);
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
  RemoveObjFromPlayerInv(pObject->ObjectId, 1);
  AddObjToPlayerInv(pDnodeTgt, pObject->ObjectId);
  delete pObject;
  pObject = NULL;
}

/***********************************************************
 * Go command                                              *
 ***********************************************************/

void Communication::DoGo()
{
  string  MudCmdIsExit;
  string  TmpStr;

  DEBUGIT(1);
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
  TmpStr = StrGetWord(CmdStr, 2);
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
  sMudCmdIsExit = MudCmdIsExit;
  if (IsExit(sMudCmdIsExit))
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
  string  GoToMsg;
  string  RoomId;

  DEBUGIT(1);
  //********************
  //* Validate command *
  //********************
  RoomId = StrGetWord(CmdStr, 2);
  if (RoomId == "")
  {
    pDnodeActor->PlayerOut += "A destination is needed.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (!IsRoom(RoomId))
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
  pDnodeActor->pPlayer->RoomId = ConvertStringToCString(RoomId);
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
  string  GoToArrive;
  string  TmpStr;

  DEBUGIT(1);
  TmpStr = StrGetWord(CmdStr, 2);
  TmpStr = StrMakeLower(TmpStr);
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
  TmpStr = GetWords(CmdStr, 2);
  GoToArrive = TmpStr;
  // Strip out color codes so arrival message length can be checked
  StrReplace(TmpStr, "&N", "");
  StrReplace(TmpStr, "&K", "");
  StrReplace(TmpStr, "&R", "");
  StrReplace(TmpStr, "&G", "");
  StrReplace(TmpStr, "&Y", "");
  StrReplace(TmpStr, "&B", "");
  StrReplace(TmpStr, "&M", "");
  StrReplace(TmpStr, "&C", "");
  StrReplace(TmpStr, "&W", "");
  if (StrGetLength(TmpStr) > 60)
  {
    pDnodeActor->PlayerOut += "Arrival message must be less than 61 characters, color codes do not count.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pDnodeActor->pPlayer->GoToArrive = ConvertStringToCString(GoToArrive);
  pDnodeActor->PlayerOut += "Your arrival message has been set.\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
* GoToDepart command                                       *
************************************************************/

void Communication::DoGoToDepart()
{
  string  GoToDepart;
  string  TmpStr;

  DEBUGIT(1);
  TmpStr = StrGetWord(CmdStr, 2);
  TmpStr = StrMakeLower(TmpStr);
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
  TmpStr = GetWords(CmdStr, 2);
  GoToDepart = TmpStr;
  // Strip out color codes so arrival message length can be checked
  StrReplace(TmpStr, "&N", "");
  StrReplace(TmpStr, "&K", "");
  StrReplace(TmpStr, "&R", "");
  StrReplace(TmpStr, "&G", "");
  StrReplace(TmpStr, "&Y", "");
  StrReplace(TmpStr, "&B", "");
  StrReplace(TmpStr, "&M", "");
  StrReplace(TmpStr, "&C", "");
  StrReplace(TmpStr, "&W", "");
  if (StrGetLength(TmpStr) > 60)
  {
    pDnodeActor->PlayerOut += "Departure message must be less than 61 characters, color codes do not count.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pDnodeActor->pPlayer->GoToDepart = ConvertStringToCString(GoToDepart);
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
  string   PlayerNameCheck;
  string   TargetNameCheck;
  string   TargetNameSave;
  string   TmpStr;

  DEBUGIT(1);
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  PlayerNameCheck = pDnodeActor->PlayerName;
  TargetNameCheck = StrGetWord(CmdStr, 2);
  TargetNameSave  = TargetNameCheck;
  PlayerNameCheck = StrMakeLower(PlayerNameCheck);
  TargetNameCheck = StrMakeLower(TargetNameCheck);
  //************************
  //* Group with no target *
  //************************
  if (StrGetLength(TargetNameCheck) < 1)
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
      j = StrGetLength(pDnodeActor->pPlayer->pPlayerGrpMember[0]->Name);
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
  pDnodeGrpLdr = GetTargetDnode(ConvertStringToCString(TargetNameCheck));
  if (!pDnodeGrpLdr)
  { // New group member ... not online
    pDnodeActor->PlayerOut += ConvertStringToCString(TargetNameSave);
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
    TmpStr = StrTrimLeft(TmpStr);
    TmpStr = StrTrimRight(TmpStr);
    pDnodeActor->PlayerOut+= ConvertStringToCString(TmpStr);
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
  string   GsayMsg;
  int      i;

  DEBUGIT(1);
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
  GsayMsg = GetWords(CmdStr, 2);
  if (StrGetLength(GsayMsg) < 1)
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
  pDnodeActor->PlayerOut += ConvertStringToCString(GsayMsg);
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
    pDnodeGrpMem->PlayerOut += ConvertStringToCString(GsayMsg);
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
  string   HailMsg;
  string   MobileId;
  string   MobileName;
  string   MobileMsg;
  string   PlayerName;
  string   PlayerNameCheck;
  string   RoomId;
  string   Target;

  DEBUGIT(1);
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
  if (StrCountWords(CmdStr) < 2)
  { // No target
    pDnodeActor->PlayerOut += "You need a target.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (StrCountWords(CmdStr) > 2)
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
  PlayerNameCheck = StrMakeLower(PlayerNameCheck);
  Target = StrGetWord(CmdStr, 2);
  MobileName = Target;
  Target = StrMakeLower(Target);
  if (Target == PlayerNameCheck)
  { // Trying to kill self
    pDnodeActor->PlayerOut += "Hailing yourself is just plain silly.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pMobile = IsMobInRoom(ConvertStringToCString(Target));
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
  MobileMsg = StrLeft(MobileMsg, StrGetLength(MobileMsg) - 2);
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
  DEBUGIT(1);
  if (IsHelp())
  { // Help was found and sent to player
    return;
  }
  // No help entry found
  CmdStr = "help notfound";
  if (IsHelp())
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
  DEBUGIT(1);
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  ShowPlayerInv();
}

/***********************************************************
* Invisible command                                        *
************************************************************/

void Communication::DoInvisible()
{
  string  TmpStr;

  DEBUGIT(1);
  TmpStr = StrGetWord(CmdStr, 2);
  TmpStr = StrMakeLower(TmpStr);
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
  string   KillMsg;
  string   MobileId;
  string   MobileName;
  string   PlayerName;
  string   PlayerNameCheck;
  string   RoomId;
  string   sRoomId;
  string   RoomType;
  string   Target;

  DEBUGIT(1);
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
  if (IsRoomType(RoomId, "NoFight"))
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
  if (StrCountWords(CmdStr) < 2)
  { // No target
    pDnodeActor->PlayerOut += "You need a target.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (StrCountWords(CmdStr) > 2)
  { // Two many targets
    pDnodeActor->PlayerOut += "Only one target at a time.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  PlayerName      = pDnodeActor->PlayerName;
  PlayerNameCheck = PlayerName;
  PlayerNameCheck = StrMakeLower(PlayerNameCheck);
  Target = StrGetWord(CmdStr, 2);
  MobileName = Target;
  Target = StrMakeLower(Target);
  if (Target == PlayerNameCheck)
  { // Trying to kill self
    pDnodeActor->PlayerOut += "That would be just awful.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (IsPlayer(ConvertStringToCString(Target)))
  { // Trying to kill another player
    pDnodeActor->PlayerOut += "Don't even think about it, player killing is not allowed.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pMobile = IsMobInRoom(ConvertStringToCString(Target));
  if (!pMobile)
  { // Target mobile is not here
    pDnodeActor->PlayerOut += "There doesn't seem to be a(n) ";
    pDnodeActor->PlayerOut += ConvertStringToCString(MobileName);
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
    RemoveMobFromRoom(ConvertStringToCString(RoomId), pMobile->MobileId);
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
    pMobile->CreateMobStatsFile(ConvertStringToCString(RoomId));
    MobileId = pMobile->MobileId;
    RemoveMobFromRoom(ConvertStringToCString(RoomId), ConvertStringToCString(MobileId));
    MobileId = pMobile->MobileId + "." + pMobile->MobNbr;
  }
  else
  { // Mobile is hurt
    MobileId = pMobile->MobileId + "." + pMobile->MobNbr;
    RemoveMobFromRoom(ConvertStringToCString(RoomId), ConvertStringToCString(MobileId));
  }
  UpdateMobInWorld(ConvertStringToCString(MobileId), "add"); // Keep Mob InWorld count correct
  // Set player and mobile to fight
  CreatePlayerMob(PlayerName, MobileId);
  CreateMobPlayer(PlayerName, MobileId);
  delete pMobile;
  pDnodeActor->PlayerStateFighting = true;
}

/***********************************************************
* List command                                              *
************************************************************/

void Communication::DoList()
{
  DEBUGIT(1);
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
  if (!IsShop(ConvertCStringToString(pDnodeActor->pPlayer->RoomId)))
  { // Room is not a shop
    pDnodeActor->PlayerOut += "Find a shop.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  ListObjects();
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
 * Load command                                            *
 ***********************************************************/

void Communication::DoLoad()
{
  Mobile  *pMobile;
  string   LoadMsg;
  string   MobileId;
  string   ObjectId;
  string   TmpStr;

  DEBUGIT(1);
  //********************
  //* Validate command *
  //********************
  if (StrCountWords(CmdStr) == 1)
  { // Invalid command format
    pDnodeActor->PlayerOut += "Load what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }  
  if (StrCountWords(CmdStr) != 3)
  { // Invalid command format
    pDnodeActor->PlayerOut += "Usage: load obj{ect}|mob{ile} <target>";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  TmpStr = StrGetWord(CmdStr, 2);
  TmpStr = StrMakeLower(TmpStr);
  TmpStr = TranslateWord(ConvertStringToCString(TmpStr));
  if (IsNotWord(ConvertStringToCString(TmpStr),"object mobile"))
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
    ObjectId = StrGetWord(CmdStr, 3);
    pObject = NULL;
    IsObject(ObjectId); // Sets pObject
    if (!pObject)
    { // Object does not exist
      pDnodeActor->PlayerOut += "Object not found.\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
    delete pObject;
    pObject = NULL;
    AddObjToPlayerInv(pDnodeActor, ObjectId);
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
    MobileId = StrGetWord(CmdStr, 3);
    MobileId = StrMakeLower(MobileId);
    pMobile = IsMobValid(MobileId);
    if (!pMobile)
    { // Mobile does not exist
      pDnodeActor->PlayerOut += "Mobile not found.\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return;
    }
    AddMobToRoom(ConvertCStringToString(pDnodeActor->pPlayer->RoomId), MobileId);
    SpawnMobileNoMove(MobileId);
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
  DEBUGIT(1);
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

void Communication::DoLook(string CmdStr1)
{
  Mobile  *pMobile;
  string   Desc1FirstLetter;
  string   Desc1TheRest;
  bool     IsPlayer;
  string   MudCmdIsExit;
  string   TargetName;
  string   TmpStr;

  DEBUGIT(1);
  CmdStr = ConvertStringToCString(CmdStr1);
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  TmpStr = StrGetWord(CmdStr, 2);
  //*****************
  //* Just looking? *
  //*****************
  if (TmpStr == "")
  { // Just look
    ShowRoom(pDnodeActor);
    return;
  }
  //**********************
  //* Is it a room exit? *
  //**********************
  MudCmdIsExit = "look";
  if (IsExit(MudCmdIsExit))
  { // Look room exit
    return;
  }
  //*******************
  //* Is it a player? *
  //*******************
  IsPlayer = true;
  TargetName = TmpStr;
  TargetName = StrMakeLower(TargetName);
  pDnodeTgt = GetTargetDnode(ConvertStringToCString(TargetName));
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
    ShowPlayerEqu(pDnodeTgt);
    return;
  }
  //*******************
  //* Is it a mobile? *
  //*******************
  pMobile = IsMobInRoom(ConvertStringToCString(TargetName));
  if (pMobile)
  { // Player is looking at a mob
    TmpStr = StrMakeFirstLower(pMobile->Desc1);
    pDnodeActor->PlayerOut += "You look at ";
    pDnodeActor->PlayerOut += ConvertStringToCString(TmpStr);
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
  DEBUGIT(1);
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
  ifstream   MotdFile;
  string     MotdFileName;
  string     Stuff;

  DEBUGIT(1);
  // Read Motd file
  MotdFileName = MOTD_DIR;
  MotdFileName += "Motd";
  MotdFileName += ".txt";
  MotdFile.open(MotdFileName);
  if (!MotdFile.is_open())
  {
    AfxMessageBox("Communication::DoMotd - Open Motd file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  getline(MotdFile, Stuff);
  while (Stuff != "End of Motd")
  {
    Stuff += "\r\n";
    pDnodeActor->PlayerOut += ConvertStringToCString(Stuff);
    getline(MotdFile, Stuff);
  }
  MotdFile.close();
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
  string  TmpStr;

  DEBUGIT(1);
  TmpStr = StrGetWord(CmdStr, 2);
  TmpStr = StrMakeLower(TmpStr);
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
  string  Password;
  string  NewPassword1;
  string  NewPassword2;

  DEBUGIT(1);
  //********************
  //* Validate command *
  //********************
  if (IsFighting())
  { // Player is fighting, send msg, command is not done
    return;
  }
  if (StrCountWords(CmdStr) != 4)
  {
    pDnodeActor->PlayerOut += "Password command requires: ";
    pDnodeActor->PlayerOut += "Password NewPassword NewPassword";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  Password     = StrGetWord(CmdStr, 2);
  NewPassword1 = StrGetWord(CmdStr, 3);
  NewPassword2 = StrGetWord(CmdStr, 4);
  if (Password != pDnodeActor->pPlayer->Password)
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

  DEBUGIT(1);
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
  string  AllMsg;
  string  LogBuf;
  string  PlayerMsg;

  DEBUGIT(1);
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
  LogIt(LogBuf);
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
  string  TmpStr;

  DEBUGIT(1);
  if (StrCountWords(CmdStr) > 2)
  { // Invalid command format
    pDnodeActor->PlayerOut += "You may refresh only one thing at time.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  TmpStr = StrGetWord(CmdStr, 2);
  if (StrGetLength(TmpStr) == 0)
  { // Player did not provide an target to be refreshed
    pDnodeActor->PlayerOut += "Refresh what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  TmpStr = StrMakeLower(TmpStr);
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
  string   ObjectName;
  string   RemoveMsg;
  string   TmpStr;

  DEBUGIT(1);
  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (StrCountWords(CmdStr) > 2)
  { // Invalid command format, like 'remove shirt pants'
    pDnodeActor->PlayerOut += "You may remove only one item at time.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  TmpStr = StrGetWord(CmdStr, 2);
  if (StrGetLength(TmpStr) == 0)
  { // Player did not provide an object to be removed
    pDnodeActor->PlayerOut += "Remove what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  // Get pointer to object
  ObjectName = TmpStr;
  TmpStr = StrMakeLower(TmpStr);
  pObject = NULL;
  IsObjInPlayerEqu(TmpStr); // Sets pObject
  if (!pObject)
  { // Object not in equipment
    pDnodeActor->PlayerOut += "You don't have a(n) ";
    pDnodeActor->PlayerOut += ConvertStringToCString(ObjectName);
    pDnodeActor->PlayerOut += " equipped.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  // Decrease player's ArmorClass
  pDnodeActor->pPlayer->ArmorClass -= pObject->ArmorValue;
  // Remove object from player's equipment
  RemoveObjFromPlayerEqu(pObject->ObjectId);
  // Send remove message to player
  pDnodeActor->PlayerOut += "You remove ";
  pDnodeActor->PlayerOut += ConvertStringToCString(pObject->Desc1);
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
  AddObjToPlayerInv(pDnodeTgt, pObject->ObjectId);
  TmpStr = pObject->Type;
  TmpStr = StrMakeLower(TmpStr);
  if (TmpStr == "weapon")
  { // Now player has no weapon
    pDnodeActor->pPlayer->WeaponDamage = PLAYER_DMG_HAND;
    pDnodeActor->pPlayer->WeaponDesc1  = "a pair of bare hands";
    pDnodeActor->pPlayer->WeaponType   = "Hand";
  }
  delete pObject;
  pObject = NULL;
}

/***********************************************************
 * Restore command                                         *
 ***********************************************************/

void Communication::DoRestore(string CmdStr)
{
  string   LookupName;
  string   PlayerName;
  bool     TargetFound;
  string   TargetName;
  string   TargetNameSave;

  DEBUGIT(1);
  //********************
  //* Validate command *
  //********************
  TargetFound     = false;
  PlayerName      = pDnodeActor->PlayerName;
  TargetName      = StrGetWord(CmdStr, 2);
  TargetNameSave  = TargetName;
  PlayerName = StrMakeLower(PlayerName);
  TargetName = StrMakeLower(TargetName);
  if (StrGetLength(TargetName) < 1)
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
  pDnodeTgt = GetTargetDnode(ConvertStringToCString(TargetName));
  if (!pDnodeTgt)
  { // Tell player ... not found
    pDnodeActor->PlayerOut += ConvertStringToCString(TargetNameSave);
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
  pDnodeActor->PlayerOut += ConvertStringToCString(TargetName);
  pDnodeActor->PlayerOut += ".";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  // Send restore message to target
  pDnodeTgt->PlayerOut += "\r\n";  
  pDnodeTgt->PlayerOut += "&Y";
  pDnodeTgt->PlayerOut += ConvertStringToCString(PlayerName);
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
  string  TmpStr;

  DEBUGIT(1);
  TmpStr = StrGetWord(CmdStr, 2);
  TmpStr = StrMakeLower(TmpStr);
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
  DEBUGIT(1);
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
  string  SayMsg;
  string  TmpStr;

  DEBUGIT(1);
  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  SayMsg = GetWords(CmdStr, 2);
  if (StrGetLength(SayMsg) < 1)
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
  pDnodeActor->PlayerOut += ConvertStringToCString(SayMsg);
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
  int      Cost;
  string   Desc1;
  int      InvCountInt;
  string   InvCountStr;
  string   ObjectId;
  string   ObjectName;
  string   RoomId;
  int      SellCountInt;
  string   SellCountStr;
  string   TmpStr;

  DEBUGIT(1);
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
  RoomId = ConvertCStringToString(pDnodeActor->pPlayer->RoomId);
  if (!IsShop(RoomId))
  { // Room is not a shop
    pDnodeActor->PlayerOut += "Find a shop.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  ObjectName = StrGetWord(CmdStr, 2);
  if (ObjectName == "")
  { // No object given
    pDnodeActor->PlayerOut += "Sell what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pObject = NULL;
  IsObjInPlayerInv(ObjectName); // Sets pObject
  if (!pObject)
  { // Player doesn't have object
    pDnodeActor->PlayerOut += "You don't have a(n) ";
    pDnodeActor->PlayerOut += ConvertStringToCString(ObjectName);
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
  pObject     = NULL;
  IsShopObj(RoomId, ObjectName); // Sets pObject
  if (!pObject)
  { // Player cannot sell object to this shop
    pDnodeActor->PlayerOut += "That item cannot be sold here.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  delete pObject;
  pObject = NULL;
  //********************
  //* Check sell count *
  //********************
  InvCountInt = stoi(InvCountStr);
  SellCountStr = StrGetWord(CmdStr, 3);
  SellCountStr = StrMakeLower(SellCountStr); // In case player typed 'all'
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
      SellCountInt = stoi(SellCountStr);
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
        pDnodeActor->PlayerOut += ConvertStringToCString(ObjectName);
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
  RemoveObjFromPlayerInv(ObjectId, SellCountInt);
  // Player receives some money
  Cost = Cost * SellCountInt;
  pDnodeActor->pPlayer->SetMoney('+', Cost, "Silver");
  // Send messages
  sprintf(Buf, "%d", SellCountInt);
  TmpStr = ConvertStringToCString(Buf);
  TmpStr = "(" + TmpStr + ") ";
  pDnodeActor->PlayerOut += ConvertStringToCString(TmpStr);
  pDnodeActor->PlayerOut += "You sell ";
  pDnodeActor->PlayerOut += ConvertStringToCString(Desc1);
  pDnodeActor->PlayerOut += " for ";
  sprintf(Buf, "%d", Cost);
  TmpStr = ConvertStringToCString(Buf);
  pDnodeActor->PlayerOut += ConvertStringToCString(TmpStr);
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
  string     CommandCheckResult;
  ifstream   HelpFile;
  string     HelpFileName;
  string     HelpText;
  ifstream   SocialFile;
  string     SocialFileName;
  string     SocialText;
  string     TmpStr;
  string     MudCmdChk;
  string     ValCmdInfo;

  DEBUGIT(1);
  //********************
  //* Validate command *
  //********************
  if (StrCountWords(CmdStr) > 2)
  { // Invalid command format
    pDnodeActor->PlayerOut += "You may show only one thing at time.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  TmpStr = StrGetWord(CmdStr, 2);
  if (StrGetLength(TmpStr) == 0)
  { // Player did not provide a target to be shown
    pDnodeActor->PlayerOut += "Show what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  TmpStr = StrMakeLower(TmpStr);
  if (IsNotWord(ConvertStringToCString(TmpStr), "commands socials help"))
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
    for (auto& ValidCmd : ValidCmds)
    { // For each string in the ValidCmds vector
      ValCmdInfo = ConvertStringToCString(ValidCmd);
      MudCmdChk  = StrGetWord(ValCmdInfo, 1);
      CommandCheckResult = CommandCheck(MudCmdChk);
      if (CommandCheckResult == "Ok")
      { // Mud command is Ok for this player
        pDnodeActor->PlayerOut += ConvertStringToCString(MudCmdChk);
        pDnodeActor->PlayerOut += "\r\n";
      }
      else
      if (StrGetWord(CommandCheckResult, 1) == "Level")
      { // Mud command is Ok for this player, but level restricted
        pDnodeActor->PlayerOut += ConvertStringToCString(MudCmdChk);
        pDnodeActor->PlayerOut += " acquired at level(";
        pDnodeActor->PlayerOut += ConvertStringToCString(StrGetWord(CommandCheckResult, 2));
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
    HelpFile.open(HelpFileName);
    if (!HelpFile.is_open())
    { // Help file open failed
      pDnodeActor->PlayerOut += "No help is available, you are on your own!";
      pDnodeActor->PlayerOut += "\r\n";
    }
    else
    { // Help file is open
      getline(HelpFile, HelpText);    // Skip first line
      getline(HelpFile, HelpText);
      while (HelpText != "End of Help")
      { // Read the whole file
        if (StrLeft(HelpText, 5) == "Help:")
        { // Found a help topic
          pDnodeActor->PlayerOut += ConvertStringToCString(StrRight(HelpText, StrGetLength(HelpText) - 5));
          pDnodeActor->PlayerOut += "\r\n";
        }
        getline(HelpFile, HelpText);
      }
      HelpFile.close();
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
    SocialFile.open(SocialFileName);
    if (!SocialFile.is_open())
    { // Social file open failed
      pDnodeActor->PlayerOut += "No socials are available, how boring!";
      pDnodeActor->PlayerOut += "\r\n";
    }
    else
    { // Social file is open
      getline(SocialFile, SocialText);
      while (SocialText != "End of Socials")
      { // Read the whole file
        if (StrLeft(SocialText, 9) == "Social : ")
        { // Found a help topic
          pDnodeActor->PlayerOut += ConvertStringToCString(StrRight(SocialText, StrGetLength(SocialText) - 9));
          pDnodeActor->PlayerOut += "\r\n";
        }
        getline(SocialFile, SocialText);
      }
      SocialFile.close();
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
  string  SitMsg;

  DEBUGIT(1);
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
  string  SleepMsg;

  DEBUGIT(1);
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
  string  StandMsg;

  DEBUGIT(1);
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
  DEBUGIT(1);
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
  string  LogBuf;
  string  GoGoGoFileName;
  string  StopItFileName;

  DEBUGIT(1);
  StateStopping = true;
  LogBuf  = pDnodeActor->PlayerName;
  LogBuf += " issued the STOP command";
  LogIt(LogBuf);
  pDnodeActor->PlayerOut += "Stop command issued!\r\n";
  GoGoGoFileName  = CONTROL_DIR;
  GoGoGoFileName += "GoGoGo";
  StopItFileName  = CONTROL_DIR;
  StopItFileName += "StopIt";
  CFile::Rename(ConvertStringToCString(GoGoGoFileName), ConvertStringToCString(StopItFileName));
}

/***********************************************************
 * Tell command                                            *
 ***********************************************************/

void Communication::DoTell()
{
  string   PlayerName;
  bool     TargetFound;
  string   TargetName;
  string   TargetNameSave;
  string   TellMsg;

  DEBUGIT(1);
  //********************
  //* Validate command *
  //********************
  TargetFound     = false;
  PlayerName      = pDnodeActor->PlayerName;
  TargetName      = StrGetWord(CmdStr, 2);
  TargetNameSave  = TargetName;
  PlayerName = StrMakeLower(PlayerName);
  TargetName = StrMakeLower(TargetName);
  if (TargetName == PlayerName)
  {
    pDnodeActor->PlayerOut += "Seems silly to tell yourself something!\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (StrGetLength(TargetName) < 1)
  {
    pDnodeActor->PlayerOut += "Tell who?\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  TellMsg = GetWords(CmdStr, 3);
  if (StrGetLength(TellMsg) < 1)
  {
    pDnodeActor->PlayerOut += "Um, tell ";
    pDnodeActor->PlayerOut += ConvertStringToCString(TargetNameSave);
    pDnodeActor->PlayerOut += " what?";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pDnodeTgt = GetTargetDnode(ConvertStringToCString(TargetName));
  if (!pDnodeTgt)
  { // Tell player ... not found
    pDnodeActor->PlayerOut += ConvertStringToCString(TargetNameSave);
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
  pDnodeActor->PlayerOut += ConvertStringToCString(TargetName);
  pDnodeActor->PlayerOut += ": ";
  pDnodeActor->PlayerOut += ConvertStringToCString(TellMsg);
  pDnodeActor->PlayerOut += "&N";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  // Send tell message to target
  pDnodeTgt->PlayerOut += "&M";
  pDnodeTgt->PlayerOut += "\r\n";  
  pDnodeTgt->PlayerOut += ConvertStringToCString(PlayerName);
  pDnodeTgt->PlayerOut += " tells you: ";
  pDnodeTgt->PlayerOut += ConvertStringToCString(TellMsg);
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
  string  DisplayCurrentTime;

  DEBUGIT(1);
  // Server time
  time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
  string s(30, '\0');
  strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S", localtime(&now));
  DisplayCurrentTime = ConvertStringToCString(s);
  pDnodeActor->PlayerOut += "Current server time is: ";
  pDnodeActor->PlayerOut += ConvertStringToCString(DisplayCurrentTime);
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
  string  Title;
  string  TmpStr;

  DEBUGIT(1);
  TmpStr = StrGetWord(CmdStr, 2);
  TmpStr = StrMakeLower(TmpStr);
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
  TmpStr = GetWords(CmdStr, 2);
  Title = TmpStr;
  // Strip out color codes so Title length can be checked
  StrReplace(TmpStr, "&N", "");
  StrReplace(TmpStr, "&K", "");
  StrReplace(TmpStr, "&R", "");
  StrReplace(TmpStr, "&G", "");
  StrReplace(TmpStr, "&Y", "");
  StrReplace(TmpStr, "&B", "");
  StrReplace(TmpStr, "&M", "");
  StrReplace(TmpStr, "&C", "");
  StrReplace(TmpStr, "&W", "");
  if (StrGetLength(TmpStr) > 40)
  {
    pDnodeActor->PlayerOut += "Title must be less than 41 characters, color codes do not count.\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pDnodeActor->pPlayer->Title = ConvertStringToCString(Title);
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
  string  MinusSign;
  int     SkillPointsUsed;
  int     SkillPointsRemaining;
  string  TmpStr;
  string  UnTrainCost;
  string  WeaponType;

  DEBUGIT(1);
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
  WeaponType  = StrGetWord(CmdStr, 2);
  MinusSign   = StrGetWord(CmdStr, 3);
  UnTrainCost = StrGetWord(CmdStr, 4);
  WeaponType = StrMakeLower(WeaponType);
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
  if (StrCountWords(CmdStr) > 4)
  { // Invalid command format
    pDnodeActor->PlayerOut += "Train command syntax error, try'er again.";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  if (WeaponType != "")
  { // WeaponType specified
    if (IsNotWord(ConvertStringToCString(WeaponType), "axe club dagger hammer spear staff sword"))
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
          pDnodeActor->PlayerOut += ConvertStringToCString(WeaponType);
          pDnodeActor->PlayerOut += " skill!";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->pPlayer->CreatePrompt();
          pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
          return;
        }
        if (WeaponType == "club" && pDnodeActor->pPlayer->SkillClub == 0)
        { // No club skill
          pDnodeActor->PlayerOut += "You have no ";
          pDnodeActor->PlayerOut += ConvertStringToCString(WeaponType);
          pDnodeActor->PlayerOut += " skill!";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->pPlayer->CreatePrompt();
          pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
          return;
        }
        if (WeaponType == "dagger" && pDnodeActor->pPlayer->SkillDagger == 0)
        { // No dagger skill
          pDnodeActor->PlayerOut += "You have no ";
          pDnodeActor->PlayerOut += ConvertStringToCString(WeaponType);
          pDnodeActor->PlayerOut += " skill!";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->pPlayer->CreatePrompt();
          pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
          return;
        }
        if (WeaponType == "hammer" && pDnodeActor->pPlayer->SkillHammer == 0)
        { // No hammer skill
          pDnodeActor->PlayerOut += "You have no ";
          pDnodeActor->PlayerOut += ConvertStringToCString(WeaponType);
          pDnodeActor->PlayerOut += " skill!";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->pPlayer->CreatePrompt();
          pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
          return;
        }
        if (WeaponType == "spear" && pDnodeActor->pPlayer->SkillSpear == 0)
        { // No spear skill
          pDnodeActor->PlayerOut += "You have no ";
          pDnodeActor->PlayerOut += ConvertStringToCString(WeaponType);
          pDnodeActor->PlayerOut += " skill!";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->pPlayer->CreatePrompt();
          pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
          return;
        }
        if (WeaponType == "staff" && pDnodeActor->pPlayer->SkillStaff == 0)
        { // No staff skill
          pDnodeActor->PlayerOut += "You have no ";
          pDnodeActor->PlayerOut += ConvertStringToCString(WeaponType);
          pDnodeActor->PlayerOut += " skill!";
          pDnodeActor->PlayerOut += "\r\n";
          pDnodeActor->pPlayer->CreatePrompt();
          pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
          return;
        }
        if (WeaponType == "sword" && pDnodeActor->pPlayer->SkillSword == 0)
        { // No sword skill
          pDnodeActor->PlayerOut += "You have no ";
          pDnodeActor->PlayerOut += ConvertStringToCString(WeaponType);
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
    pDnodeActor->PlayerOut += ConvertStringToCString(TmpStr);
    pDnodeActor->PlayerOut += "\r\n";
    // Club
    pDnodeActor->PlayerOut += "Club:   ";
    sprintf(Buf, "%3d", pDnodeActor->pPlayer->SkillClub);
    TmpStr = ConvertStringToCString(Buf);
    pDnodeActor->PlayerOut += ConvertStringToCString(TmpStr);
    pDnodeActor->PlayerOut += "\r\n";
    // Dagger
    pDnodeActor->PlayerOut += "Dagger: ";
    sprintf(Buf, "%3d", pDnodeActor->pPlayer->SkillDagger);
    TmpStr = ConvertStringToCString(Buf);
    pDnodeActor->PlayerOut += ConvertStringToCString(TmpStr);
    pDnodeActor->PlayerOut += "\r\n";
    // Hammer
    pDnodeActor->PlayerOut += "Hammer: ";
    sprintf(Buf, "%3d", pDnodeActor->pPlayer->SkillHammer);
    TmpStr = ConvertStringToCString(Buf);
    pDnodeActor->PlayerOut += ConvertStringToCString(TmpStr);
    pDnodeActor->PlayerOut += "\r\n";
    // Spear
    pDnodeActor->PlayerOut += "Spear:  ";
    sprintf(Buf, "%3d", pDnodeActor->pPlayer->SkillSpear);
    TmpStr = ConvertStringToCString(Buf);
    pDnodeActor->PlayerOut += ConvertStringToCString(TmpStr);
    pDnodeActor->PlayerOut += "\r\n";
    // Staff
    pDnodeActor->PlayerOut += "Staff:  ";
    sprintf(Buf, "%3d", pDnodeActor->pPlayer->SkillStaff);
    TmpStr = ConvertStringToCString(Buf);
    pDnodeActor->PlayerOut += ConvertStringToCString(TmpStr);
    pDnodeActor->PlayerOut += "\r\n";
    // Sword
    pDnodeActor->PlayerOut += "Sword:  ";
    sprintf(Buf, "%3d", pDnodeActor->pPlayer->SkillSword);
    TmpStr = ConvertStringToCString(Buf);
    pDnodeActor->PlayerOut += ConvertStringToCString(TmpStr);
    pDnodeActor->PlayerOut += "\r\n";
    // Skill points used
    pDnodeActor->PlayerOut += "Skill points used:      ";
    sprintf(Buf, "%4d", SkillPointsUsed);
    TmpStr = ConvertStringToCString(Buf);
    pDnodeActor->PlayerOut += ConvertStringToCString(TmpStr);
    pDnodeActor->PlayerOut += "\r\n";
    // Skill points remaining
    pDnodeActor->PlayerOut += "Skill points remaining: ";
    sprintf(Buf, "%4d", SkillPointsRemaining);
    TmpStr = ConvertStringToCString(Buf);
    pDnodeActor->PlayerOut += ConvertStringToCString(TmpStr);
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
    pDnodeActor->PlayerOut += ConvertStringToCString(WeaponType);
    pDnodeActor->PlayerOut += " skill!";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  }
  else
  { // UnTraining
    pDnodeActor->PlayerOut += "Your ";
    pDnodeActor->PlayerOut += ConvertStringToCString(WeaponType);
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
  string  WakeMsg;

  DEBUGIT(1);
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
  string   ObjectName;
  string   TmpStr;
  bool     WearFailed;
  string   WearMsg;
  string   WearPosition;

  DEBUGIT(1);
  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (StrCountWords(CmdStr) == 1)
  { // Invalid command format
    pDnodeActor->PlayerOut += "Wear what?";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  // Get pointer to object
  TmpStr = StrGetWord(CmdStr, 2);
  ObjectName = TmpStr;
  TmpStr = StrMakeLower(TmpStr);
  pObject = NULL;
  IsObjInPlayerInv(TmpStr); // Sets pObject
  if (!pObject)
  { // Player does not have object in inventory
    pDnodeActor->PlayerOut += "You don't have a(n) ";
    pDnodeActor->PlayerOut += ConvertStringToCString(ObjectName);
    pDnodeActor->PlayerOut += ".\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    return;
  }
  pObject->Type = StrMakeLower(pObject->Type);
  if (pObject->Type != "armor")
  { // Player can't wear stuff that is NOT armor
    pDnodeActor->PlayerOut += "You can't wear ";
    pDnodeActor->PlayerOut += ConvertStringToCString(pObject->Desc1);
    pDnodeActor->PlayerOut += ".\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    delete pObject;
    pObject = NULL;
    return;
  }
  // Handle wear positions that require left or right
  if (IsWord(ConvertStringToCString(pObject->WearPosition), ConvertStringToCString("ear wrist finger ankle")))
  { // Object must be worn using left and right
    TmpStr = StrGetWord(CmdStr, 3);
    TmpStr = StrMakeLower(TmpStr);
    if (IsNotWord(ConvertStringToCString(TmpStr),"left right"))
    { // Player did not specify left or right
      pDnodeActor->PlayerOut += "You must specify left or right";
      pDnodeActor->PlayerOut += ".\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      delete pObject;
      pObject = NULL;
      return;
    }
    pObject->WearPosition += ConvertStringToCString(TmpStr);
  }
  //***************
  //* Wear object *
  //***************
  // Add object to player's equipment
  WearFailed = AddObjToPlayerEqu(pObject->WearPosition, pObject->ObjectId);
  if (WearFailed)
  { // Already wearing an object in that wear position
    pDnodeActor->PlayerOut += "You fail to wear ";
    pDnodeActor->PlayerOut += ConvertStringToCString(pObject->Desc1);
    pDnodeActor->PlayerOut += ".";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    delete pObject;
    pObject = NULL;
    return;
  }
  // Increase player's ArmorClass
  pDnodeActor->pPlayer->ArmorClass += pObject->ArmorValue;
  // Remove object from player's inventory
  RemoveObjFromPlayerInv(pObject->ObjectId, 1);
  // Send messages
  pDnodeActor->PlayerOut += "You wear ";
  pDnodeActor->PlayerOut += ConvertStringToCString(pObject->Desc1);
  pDnodeActor->PlayerOut += ".";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  WearMsg = pDnodeActor->PlayerName + " wears " + ConvertStringToCString(pObject->Desc1) + ".";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, WearMsg);
  delete pObject;
  pObject = NULL;
}

/***********************************************************
 * Where command                                           *
 ***********************************************************/

void Communication::DoWhere()
{
  string   SearchId;

  DEBUGIT(1);
  if (StrCountWords(CmdStr) != 2)
  { // Invalid command format
    pDnodeActor->PlayerOut += "Nothing given to search for!";
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  SearchId = StrGetWord(CmdStr, 2);
  SearchId = StrMakeLower(SearchId);
  // Find Players
  pDnodeTgt = GetTargetDnode(ConvertStringToCString(SearchId));
  if (pDnodeTgt)
  { // Target is online and in 'playing' state
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += pDnodeTgt->PlayerName;
    pDnodeActor->PlayerOut += " is in ";
    pDnodeActor->PlayerOut += pDnodeTgt->pPlayer->RoomId;
    pDnodeActor->PlayerOut += "\r\n";
  }
  else
  if (IsMobValid(SearchId))
  { // Find Mobiles
    WhereMob(ConvertStringToCString(SearchId));
  }
  else
  {
    pObject = NULL;
    IsObject(SearchId); // Sets pObject
    if (pObject)
    { // Find Objects
      WhereObj(SearchId);
    }
    else
    { // Could not find it
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->PlayerOut += "No ";
      pDnodeActor->PlayerOut += ConvertStringToCString(SearchId);
      pDnodeActor->PlayerOut += " found.";
      pDnodeActor->PlayerOut += "\r\n";
    }
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
  string   DisplayName;
  string   DisplayLevel;

  DEBUGIT(1);
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "&C";
  pDnodeActor->PlayerOut += "Players online";
  pDnodeActor->PlayerOut += "&N";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "--------------";
  pDnodeActor->PlayerOut += "\r\n";
  // List all players
  SetpDnodeCursorFirst();
  while (!EndOfDnodeList())
  { // Loop thru all connections
    pDnodeOthers = GetDnode();
    if (pDnodeOthers->PlayerStatePlaying)
    { // Who are 'playing'
      if (pDnodeOthers->PlayerStateInvisible)
      { // Player is invisible, no show
        SetpDnodeCursorNext();
        continue;
      }
      sprintf(Buf, "%-15s", (LPCSTR) pDnodeOthers->PlayerName);
      DisplayName = ConvertStringToCString(Buf);
      sprintf(Buf, "%3d", pDnodeOthers->pPlayer->Level);
      DisplayLevel = ConvertStringToCString(Buf);
      pDnodeActor->PlayerOut += ConvertStringToCString(DisplayName);
      pDnodeActor->PlayerOut += " ";
      pDnodeActor->PlayerOut += ConvertStringToCString(DisplayLevel);
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
    SetpDnodeCursorNext();
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
  string   ObjectName;
  string   TmpStr;
  string   WearPosition;
  bool     WieldFailed;
  string   WieldMsg;

  DEBUGIT(1);
  //********************
  //* Validate command *
  //********************
  if (IsSleeping())
  { // Player is sleeping, send msg, command is not done
    return;
  }
  if (StrCountWords(CmdStr) == 1)
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
  TmpStr = StrGetWord(CmdStr, 2);
  ObjectName = TmpStr;
  TmpStr = StrMakeLower(TmpStr);
  pObject = NULL;
  IsObjInPlayerInv(TmpStr); // Sets pObject
  if (!pObject)
  { // Player does not have object in inventory
    pDnodeActor->PlayerOut += "You don't have a(n) ";
    pDnodeActor->PlayerOut += ConvertStringToCString(ObjectName);
    pDnodeActor->PlayerOut += ".\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();      
    return;
  }
  //*******************
  //* Is it a weapon? *
  //*******************
  TmpStr = pObject->Type;
  TmpStr = StrMakeLower(TmpStr);
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
  WieldFailed = AddObjToPlayerEqu(pObject->WearPosition, pObject->ObjectId);
  if (WieldFailed)
  { // Already wielding a weapon
    pDnodeActor->PlayerOut += "You are already wielding a weapon";
    pDnodeActor->PlayerOut += ".\r\n";
    pDnodeActor->pPlayer->CreatePrompt();
    pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
    delete pObject;
    pObject = NULL;
    return;
  }
  // Remove object from player's inventory
  RemoveObjFromPlayerInv(pObject->ObjectId, 1);
  // Send messages
  pDnodeActor->PlayerOut += "You wield ";
  pDnodeActor->PlayerOut += ConvertStringToCString(pObject->Desc1);
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
  pDnodeActor->pPlayer->WeaponDesc1  = ConvertStringToCString(pObject->Desc1);
  pDnodeActor->pPlayer->WeaponType   = ConvertStringToCString(pObject->WeaponType);
  pDnodeActor->pPlayer->Save();
  delete pObject;
  pObject = NULL;
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
  string   GainLoose;
  int      GrpMemberCount;
  int      i;
  double   LevelTotal;
  int      PlayerExpPct;
  string   TmpStr;

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
      ExpPoints    = CalcAdjustedExpPoints(pDnodeGrpMem->pPlayer->Level, MobileLevel, ExpPoints);
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
    pDnodeGrpMem->PlayerOut += ConvertStringToCString(GainLoose);
    pDnodeGrpMem->PlayerOut += " ";
    pDnodeGrpMem->PlayerOut += ConvertStringToCString(TmpStr);
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
  ifstream   GreetingFile;
  string     GreetingFileName;
  string     Stuff;

  // Read greeting file
  GreetingFileName = GREETING_DIR;
  GreetingFileName += "Greeting";
  GreetingFileName += ".txt";
  GreetingFile.open(GreetingFileName);
  if(!GreetingFile.is_open())
  {
    AfxMessageBox("Communication::LogonGreeting - Open Greeting file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  pDnodeActor->PlayerOut += "Version ";
  pDnodeActor->PlayerOut += VERSION;
  pDnodeActor->PlayerOut += "\r\n";
  getline(GreetingFile, Stuff);
  while (Stuff != "End of Greeting")
  {
    Stuff += "\r\n";
    pDnodeActor->PlayerOut += ConvertStringToCString(Stuff);
    getline(GreetingFile, Stuff);
  }
  GreetingFile.close();
}

/***********************************************************
 * Logon wait male female                                  *
 ***********************************************************/

void Communication::LogonWaitMaleFemale()
{
  string   AllMsg;
  string   LogBuf;
  string   PlayerMsg;

  CmdStr = StrMakeUpper(CmdStr);
  if (!(StrFindOneOf(CmdStr, "MF") == 0 && StrGetLength(CmdStr) == 1))
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
    SendToAll(ConvertStringToCString(PlayerMsg), ConvertStringToCString(AllMsg));
    ShowRoom(pDnodeActor);
    LogBuf  = "New player ";
    LogBuf += pDnodeActor->PlayerName;
    LogIt(LogBuf);
    pDnodeActor->pPlayer->Save();
  }
}

/***********************************************************
 * Logon wait name                                         *
 ***********************************************************/

void Communication::LogonWaitName()
{
  // Fix name so first letter is upper case, rest are lower case
  pDnodeActor->PlayerName = StrMakeFirstUpper(CmdStr);
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
    if (!IsPlayer(pDnodeActor->PlayerName))
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
  CmdStr = StrMakeUpper(CmdStr);
  if (!(StrFindOneOf(CmdStr, "YN") == 0 && StrGetLength(CmdStr) == 1))
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
      if (!IsNameValid(pDnodeActor->PlayerName))      { // Name is invalid ... try again
        pDnodeActor->PlayerStateWaitName = true;
        pDnodeActor->PlayerOut += pDnodeActor->PlayerName;
        pDnodeActor->PlayerOut += " is not an acceptable name in this realm\r\n";
        pDnodeActor->PlayerOut += "\r\n";
        pDnodeActor->PlayerOut += "Name?";
        pDnodeActor->PlayerOut += "\r\n";
      }
      else
      { // Name is valid
        if (IsPlayer(pDnodeActor->PlayerName))
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
  CmdStr = StrMakeUpper(CmdStr);
  if (!(StrFindOneOf(CmdStr, "YN") == 0 && StrGetLength(CmdStr) == 1))
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
    pDnodeActor->PlayerNewCharacter = ConvertCStringToString(CmdStr);
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
  string   AllMsg;
  string   LogBuf;
  string   PlayerMsg;

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
      SetpDnodeCursorFirst();
      while (!EndOfDnodeList())
      { // Loop thru all connections
        pDnodeOthers = GetDnode();
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
            LogIt(LogBuf);
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
        SetpDnodeCursorNext();
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
        SendToAll(ConvertStringToCString(PlayerMsg), ConvertStringToCString(AllMsg));
        ShowRoom(pDnodeActor);
        LogBuf  = "Returning player ";
        LogBuf += pDnodeActor->PlayerName;
        LogIt(LogBuf);
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
        LogIt(LogBuf);
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
  SetpDnodeCursorFirst();
  while (!EndOfDnodeList())
  { // Loop thru all connections
    pDnodeOthers = GetDnode();
    if (pDnodeActor == pDnodeOthers)
    { // pDnodeCursor is now correctly positioned
      break;
    }
    SetpDnodeCursorNext();
  }
}

/***********************************************************
 * New connection                                          *
 ***********************************************************/

void Communication::SockNewConnection()
{
  unsigned long       FionbioParm;
  string              LogBuf;
  int                 Result;
  struct sockaddr_in  Sock{};
  int                 SocketHandle;
  int                 SocketSize;
  string              IpAddress;
  string              TmpStr;
    
  FionbioParm = 1;
  SocketSize  = sizeof(Sock);
  // Return a new socket for a newly created connection (pg 63)
  SocketHandle = accept(ListenSocket, (struct sockaddr *)&Sock, &SocketSize);
  if (!SocketHandle)
  {
    sprintf(Buf, "%s", strerror(errno));
    LogBuf = "Communication::SockNewConnection - Error: accept: " + (string)Buf;
    LogIt(LogBuf);
    AfxMessageBox("Communication::SockNewConnection - Error: accept", MB_ICONSTOP);
    _endthread();
  }
  IpAddress = inet_ntoa(Sock.sin_addr);
  // Make socket nonblocking (pg 286)
  Result = ioctlsocket(ListenSocket, FIONBIO, &FionbioParm);
  if (Result != 0)
  {
    sprintf(Buf, "%s", strerror(errno));
    LogBuf = "Communication::SockNewConnection - Error: ioctlsocket " + (string)Buf;
    LogIt(LogBuf);
    AfxMessageBox("Communication::NewConection - Error: ioctlsocket", MB_ICONSTOP);
    _endthread();
  }
  sprintf(Buf, "%d", SocketHandle);
  TmpStr = (string)Buf;
  LogBuf  = "New connection with socket handle ";
  LogBuf += TmpStr;
  LogBuf += " and address ";
  LogBuf += IpAddress;
  LogIt(LogBuf);
  pDnodeActor = new Dnode(SocketHandle, IpAddress);
  AppendIt();
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
  string  Position;
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
  string  HealthPct;
  int     HitPoints;
  int     HitPointsMax;
  int     i;
  string  MobileAttack;
  string  MobileBeenWhacked;
  int     MobileDamage;
  string  MobileDesc1;
  string  MobileId;
  string  MobileIdCheck;
  int     PAC;
  string  PlayerBeenWhacked;
  string  TmpStr;

  i = 0;
  i++;
  MobileId = GetMobPlayerMobileId(ConvertCStringToString(pDnodeActor->PlayerName), i);
  while (MobileId != "No more mobiles")
  { // For each mob whacking the player
    PAC               = pDnodeActor->pPlayer->ArmorClass;
    MobileAttack      = GetMobileAttack(MobileId);
    MobileDamage      = GetMobileDamage(MobileId);
    MobileDesc1       = GetMobileDesc1(MobileId);
    DamageToPlayer    = CalcDamageToPlayer(MobileDamage, PAC);
    PlayerBeenWhacked = WhackPlayer(MobileDesc1, MobileAttack, DamageToPlayer);
    pDnodeActor->pPlayer->HitPoints -= DamageToPlayer;
    HitPoints = pDnodeActor->pPlayer->HitPoints;
    // Calculate health percentage
    HitPointsMax = pDnodeActor->pPlayer->Level * PLAYER_HPT_PER_LEVEL;
    HealthPct = CalcHealthPct(HitPoints, HitPointsMax);
    // Add heath pct to PlayerBeenWhacked
    PlayerBeenWhacked = StrInsert(PlayerBeenWhacked, 0, " ");
    PlayerBeenWhacked = StrInsert(PlayerBeenWhacked, 0, HealthPct);
    pDnodeActor->PlayerOut += ConvertStringToCString(PlayerBeenWhacked);
    pDnodeActor->PlayerOut += "\r\n";
    if (HitPoints <= 0)
    { // Player is dead, how sad
      ViolencePlayerDied(MobileDesc1);
      return;
    }
    i++;
    MobileId = GetMobPlayerMobileId(ConvertCStringToString(pDnodeActor->PlayerName), i);
  }
  // Player is still alive!
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
* Mobile has died                                          *
************************************************************/

void Communication::ViolenceMobileDied(string MobileBeenWhacked,
                                       string MobileDesc1,
                                       string MobileId)
{
  string   DeadMsg;
  int      ExpPoints;
  string   GainLoose;
  int      MobileExpPoints;
  string   MobileExpPointsLevel;
  string   MobileIdCheck;
  int      MobileLevel;
  string   MobileLoot;
  string   TmpStr;

  MobileExpPointsLevel = (MobileId);
  MobileExpPoints      = stoi(StrGetWord(MobileExpPointsLevel, 1));
  MobileLevel          = stoi(StrGetWord(MobileExpPointsLevel, 2));
  MobileLoot           = GetMobileLoot(MobileId);
  // Send dead mob message to player
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += ConvertStringToCString(MobileBeenWhacked);
  // Let others in room know that the mobile is DEAD!
  DeadMsg =  "&R";
  DeadMsg += pDnodeActor->PlayerName;
  DeadMsg += " has vanquished ";
  DeadMsg += MobileDesc1;
  DeadMsg += "!";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(pDnodeActor->pPlayer->RoomId, ConvertStringToCString(DeadMsg));
  // Calculate experience distribution
  if (pDnodeActor->pPlayer->pPlayerGrpMember[0] != NULL)
  { // Player is in a group, award group experience
    GrpExperience(MobileExpPoints, MobileLevel);
  }
  else
  { // Calculate adjusted experience
    if (MobileExpPoints >= 0)
    { // Player gains xp
      ExpPoints = CalcAdjustedExpPoints(pDnodeActor->pPlayer->Level, MobileLevel, MobileExpPoints);
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
    pDnodeActor->PlayerOut += ConvertStringToCString(GainLoose);
    pDnodeActor->PlayerOut += " ";
    pDnodeActor->PlayerOut += ConvertStringToCString(TmpStr);
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
  DeletePlayerMob(pDnodeActor->PlayerName);
  DeleteMobPlayer(ConvertCStringToString(pDnodeActor->PlayerName), MobileId);
  DeleteMobStats(MobileId);
  pDnodeActor->PlayerStateFighting = false;
  UpdateMobInWorld(ConvertStringToCString(MobileId), "remove");
  //***************************************************
  //* Stop other players who are whacking this mobile *
  //***************************************************
  SetpDnodeCursorFirst();
  while (!EndOfDnodeList())
  { // Loop thru all connections
    pDnodeOthers = GetDnode();
    if (pDnodeOthers->PlayerStateFighting)
    { // Players who are fighting
      MobileIdCheck = GetPlayerMobMobileId(ConvertCStringToString(pDnodeOthers->PlayerName));
      if (MobileId == MobileIdCheck)
      { // The same mobile
        DeletePlayerMob(pDnodeOthers->PlayerName);
        DeleteMobPlayer(ConvertCStringToString(pDnodeOthers->PlayerName), MobileId);
        pDnodeOthers->PlayerStateFighting = false;
      }
    }
    SetpDnodeCursorNext();
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

void Communication::ViolenceMobileLoot(string Loot)
{
  Dnode      *pDnodeGrpMem;
  Player     *pPlayerGrpLdr;
  int         i;
  bool        LootFlag;
  ifstream    MobileLootFile;
  string      MobileLootFileName;
  bool        NoLoot;
  string      Stuff;

  MobileLootFileName  = LOOT_DIR;
  MobileLootFileName += Loot;
  MobileLootFileName += ".txt";

  MobileLootFile.open(MobileLootFileName);
  if(!MobileLootFile.is_open())
  {
    AfxMessageBox("Communication::ViolenceMobileLoot - Error opening mobile loot file, it may not exist", MB_ICONSTOP);
    _endthread();
  }
  NoLoot = true;
  getline(MobileLootFile, Stuff);
  while (Stuff != "")
  {
    LootFlag = ViolenceMobileLootHandOut(Stuff);
    if (LootFlag)
    { // Ok, player got some loot, so set NoLoot to false
      NoLoot = false;
    }
    getline(MobileLootFile, Stuff);
  }
  MobileLootFile.close();
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

bool Communication::ViolenceMobileLootHandOut(string Loot)
{
  Dnode   *pDnodeGrpMem;
  Player  *pPlayerGrpLdr;
  int      Chance;
  int      Count;
  bool     GotLoot;
  int      i;
  string   LogBuf;
  string   ObjectId;
  int      Percent;

  GotLoot  = false;
  Count    = stoi(StrGetWord(Loot, 1));
  Percent  = stoi(StrGetWord(Loot, 2));
  ObjectId = StrGetWord(Loot, 3);
  for (i = 1; i <= Count; i++)
  { // For each object, for example twice if '2 60 RatEar' is specified
    Chance = GetRandomNumber(100);
    if (Chance < Percent)
    { // Random number came up less than 'percent chance of getting loot'
      pObject = NULL;
      IsObject(ObjectId); // Sets pObject
      if (!pObject)
      { // Object does not exist, Log it
        LogBuf += "Loot object not found";
        LogBuf += " ";
        LogBuf += ObjectId;
        LogIt(LogBuf);
      }
      // Tell player what they got
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->PlayerOut += "You loot ";
      pDnodeActor->PlayerOut += ConvertStringToCString(pObject->Desc1);
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
              pDnodeGrpMem->PlayerOut += ConvertStringToCString(pObject->Desc1);
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
      pObject = NULL;
      AddObjToPlayerInv(pDnodeActor, ObjectId);
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
  string MobileId;

  MobileId = GetMobPlayerMobileId(ConvertCStringToString(pDnodeActor->PlayerName), 1);
  if (MobileId == "No more mobiles")
  {
    return;
  }
  CreatePlayerMob(ConvertCStringToString(pDnodeActor->pPlayer->Name), MobileId);
  pDnodeActor->PlayerStateFighting = true;
}

/***********************************************************
* Player's turn to do some damage                          *
************************************************************/

void Communication::ViolencePlayer()
{
  int     DamageToMobile;
  string  DeadOrAlive;
  int     MaxDamageToMobile;
  int     MobileArmor;
  string  MobileBeenWhacked;
  string  MobileDesc1;
  string  MobileId;
  int     WeaponSkill;
  string  WeaponType;

  WeaponSkill       = pDnodeActor->pPlayer->GetWeaponSkill();
  WeaponType        = ConvertCStringToString(pDnodeActor->pPlayer->WeaponType);
  MaxDamageToMobile = pDnodeActor->pPlayer->WeaponDamage;
  MobileId          = GetPlayerMobMobileId(ConvertCStringToString(pDnodeActor->PlayerName));
  MobileArmor       = GetMobileArmor(MobileId);
  MobileDesc1       = GetMobileDesc1(MobileId);
  DamageToMobile    = CalcDamageToMobile(MaxDamageToMobile, WeaponSkill);
  MobileBeenWhacked = WhackMobile(MobileId, DamageToMobile, MobileDesc1, WeaponType);
  // Player has whacked the mobile
  DeadOrAlive = StrGetWord(MobileBeenWhacked, 1);
  MobileBeenWhacked = StrDeleteWord(MobileBeenWhacked, 1);
  DeadOrAlive = StrMakeLower(DeadOrAlive);
  if (DeadOrAlive == "alive")
  { // Mobile is not dead, Send fight messages to player
    pDnodeActor->PlayerOut += "\r\n";
    pDnodeActor->PlayerOut += ConvertStringToCString(MobileBeenWhacked);
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

void Communication::ViolencePlayerDied(string MobileDesc1)
{
  int      CandidateCount;
  string   CandidateList;
  int      CandidateTarget;
  string   DeadMsg;
  string   MobileId;
  string   MobileIdSave;
  string   RoomIdBeforeDying;
  string   Target;
  string   TmpStr;

  pDnodeActor->pPlayer->HitPoints = 0;
  // Tell player of their demise
  pDnodeActor->PlayerOut += "&R";
  pDnodeActor->PlayerOut += "You have been vanquished by";
  pDnodeActor->PlayerOut += " ";
  pDnodeActor->PlayerOut += ConvertStringToCString(MobileDesc1);
  pDnodeActor->PlayerOut += "!!!";
  pDnodeActor->PlayerOut += "\r\n";
  // Loose experience?
  if (pDnodeActor->pPlayer->Level > PLAYER_LOOSES_EXP_LEVEL)
  { // Player is loosing some experience
    pDnodeActor->pPlayer->Experience -= GetRandomNumber(pDnodeActor->pPlayer->Level * 5 * MOB_EXP_PER_LEVEL);
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
    pDnodeActor->PlayerOut += ConvertStringToCString(TmpStr);
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
  SendToRoom(pDnodeActor->pPlayer->RoomId, ConvertStringToCString(DeadMsg));
  RoomIdBeforeDying = pDnodeActor->pPlayer->RoomId;
  // Move player to a safe room, stop the fight
  pDnodeActor->pPlayer->RoomId = SAFE_ROOM;
  ShowRoom(pDnodeActor);
  pDnodeActor->PlayerStateFighting = false;
  // Get mobile id for mob that dead player was fighting
  MobileIdSave = GetPlayerMobMobileId(ConvertCStringToString(pDnodeActor->PlayerName));
  // Delete PlayerMob file
  DeletePlayerMob(pDnodeActor->PlayerName);
  //********************************************************
  //* Delete fighting mobiles from MobPlayer file          *
  //********************************************************
  SetpDnodeCursorFirst();
  while (!EndOfDnodeList())
  { // Loop thru all connections
    pDnodeOthers = GetDnode();
    if (pDnodeOthers->PlayerStateFighting)
    { // Players who are fighting
      if (RoomIdBeforeDying == ConvertCStringToString(pDnodeOthers->pPlayer->RoomId))
      { // In the same room
        MobileId = GetPlayerMobMobileId(ConvertCStringToString(pDnodeOthers->PlayerName));
        DeleteMobPlayer(ConvertCStringToString(pDnodeActor->PlayerName), MobileId);
        if (MobileId == MobileIdSave)
        { // Add player to candidate list for MobileIdSave
          CandidateList += pDnodeOthers->PlayerName;
          CandidateList += " ";
        }
      }
    }
    SetpDnodeCursorNext();
  }
  // Re-position pDnodeCursor
  RepositionDnodeCursor();
  // Put mobiles that are not fighting back in room
  PutMobBackInRoom(pDnodeActor->PlayerName, ConvertStringToCString(RoomIdBeforeDying));
  // Player is gone, so delete MobPlayer completely
  DeleteMobPlayer(ConvertCStringToString(pDnodeActor->PlayerName), "file");
  // Select a new target for MobileIdSave
  if (StrGetLength(CandidateList) == 0)
  { // MobileIdSave's target is still in room, nothing to do
    return;
  }
  CandidateCount  = StrCountWords(CandidateList);
  CandidateTarget = GetRandomNumber(CandidateCount);
  Target          = StrGetWord(CandidateList, CandidateTarget);
  CreateMobPlayer(Target, MobileIdSave);
}