/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Validate.h                                       *
* Usage:  Validate Rooms, Objects, Mobiles, etc            *
* Author: Steve Bryant                                     *
************************************************************/

#ifndef VALIDATE_H 
#define VALIDATE_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include <string>           // For std::string
#include <fstream>          // For file operations (ifstream, ofstream)
#include <filesystem>       // For std::filesystem
#include <thread>           // For _endthread
#include <algorithm>        // For std::copy
#include <cstdio>           // For sprintf
#include <cstring>          // For strlen
#include "Config.h"
#include "Utility.h"

/***********************************************************
* Define Validate class                                    *
************************************************************/

class Validate
{

// Public functions
  public:
    Validate();
    ~Validate();
    void    static  LogValErr(string Message, string FileName);
    bool    static  ValidateIt(string ValidationType);
    void    static  ValidateAll();
    void    static  ValidateLibraryLoot();
    void    static  ValidateLibraryMobiles();
    void    static  ValidateLibraryObjects();
    void    static  ValidateLibraryRooms();
    void    static  ValidateLibraryShops();
    void    static  ValidateLibraryWorldMobiles();
    void    static  ValidateRunningPlayers();
    void    static  ValidateRunningPlayersPlayerEqu();
    void    static  ValidateRunningPlayersPlayerObj();
    void    static  ValidateRunningRoomMob();
    void    static  ValidateRunningRoomObj();
};
#endif