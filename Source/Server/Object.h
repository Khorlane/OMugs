/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Object.h                                         *
* Usage:  Define Object class                              *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
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
    static  bool    AddObjToPlayerEqu(CString WearPosition, CString ObjectId);
    static  void    AddObjToPlayerInv(Dnode *pDnodeTgt, string ObjectId);
    static  void    AddObjToRoom(CString RoomId, CString ObjectId);
    static  int     CalcPlayerArmorClass();
    static  void    IsObjInPlayerEqu(CString ObjectName);
    static  void    IsObjInPlayerInv(CString ObjectName);
    static  void    IsObjInRoom(CString ObjectName);
    static  void    IsObject(CString ObjectId);
    static  void    RemoveObjFromPlayerEqu(CString ObjectId);
    static  void    RemoveObjFromPlayerInv(string ObjectId, int Count);
    static  void    RemoveObjFromRoom(CString ObjectId);
    static  void    ShowPlayerEqu(Dnode *pDnodeTgt);
    static  void    ShowPlayerInv();
    static  void    ShowObjsInRoom(Dnode *pDnode);
    static  void    WhereObj(CString ObjectIdSearch);
    static  void    WhereObjPlayerEqu(CString ObjectIdSearch);
    static  void    WhereObjPlayerObj(CString ObjectIdSearch);
    static  void    WhereObjRoomObj(CString ObjectIdSearch);

// Public functions
  public:
    Object(CString ObjectId);
    ~Object();
    void            ExamineObj(CString ObjectId);

// Private functions
  private:
    void            CloseFile();
    void            OpenFile(CString ObjectId);
    void            ParseStuff();
    void            ReadLine();

// Public variables
  public:
    int             ArmorValue;
    CString         ArmorWear;
    int             ContainerCapacity;
    int             Cost;
    CString         Count;
    CString         Desc1;
    CString         Desc2;
    CString         Desc3;
    int             DrinkPct;
    int             FoodPct;
    int             LightHours;
    CStdioFile      ObjectFile;
    CString         ObjectId;
    CString         Names;
    CString         Stuff;
    CString         Type;
    CString         WeaponType;
    int             WeaponDamage;
    CString         WearPosition;
    int             Weight;
};

inline Object      *pObject;

#endif