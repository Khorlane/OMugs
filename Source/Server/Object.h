/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Object.h                                         *
* Usage:  Define Object class                              *
* Author: Steve Bryant                                     *
************************************************************/

#ifndef OBJECT_H
#define OBJECT_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Utility.h"

/***********************************************************
* Define Object class                                      *
************************************************************/

class Object
{

// Public functions static
  public:
    static  bool    AddObjToPlayerEqu(string WearPosition, string ObjectId);
    static  void    AddObjToPlayerInv(Dnode *pDnodeTgt, string ObjectId);
    static  void    AddObjToRoom(string RoomId, string ObjectId);
    static  int     CalcPlayerArmorClass();
    static  void    IsObjInPlayerEqu(string ObjectName);
    static  void    IsObjInPlayerInv(string ObjectName);
    static  void    IsObjInRoom(string ObjectName);
    static  void    IsObject(string ObjectId);
    static  void    RemoveObjFromPlayerEqu(string ObjectId);
    static  void    RemoveObjFromPlayerInv(string ObjectId, int Count);
    static  void    RemoveObjFromRoom(string ObjectId);
    static  void    ShowPlayerEqu(Dnode *pDnodeTgt);
    static  void    ShowPlayerInv();
    static  void    ShowObjsInRoom(Dnode *pDnode);
    static  void    WhereObj(string ObjectIdSearch);
    static  void    WhereObjPlayerEqu(string ObjectIdSearch);
    static  void    WhereObjPlayerObj(string ObjectIdSearch);
    static  void    WhereObjRoomObj(string ObjectIdSearch);

// Public functions
  public:
    Object(string ObjectId);
    ~Object();
    void            ExamineObj(string ObjectId);

// Private functions
  private:
    void            CloseFile();
    void            OpenFile(string ObjectId);
    void            ParseStuff();
    void            ReadLine();

// Public variables
  public:
    int             ArmorValue;
    string          ArmorWear;
    int             ContainerCapacity;
    int             Cost;
    string          Count;
    string          Desc1;
    string          Desc2;
    string          Desc3;
    int             DrinkPct;
    int             FoodPct;
    int             LightHours;
    ifstream        ObjectFile;
    string          ObjectId;
    string          Names;
    string          Type;
    string          WeaponType;
    int             WeaponDamage;
    string          WearPosition;
    int             Weight;
};

inline Object      *pObject;

#endif