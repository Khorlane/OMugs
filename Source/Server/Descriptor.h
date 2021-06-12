/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Descriptor.h                                     *
* Usage:  Define Descriptor class                          *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Dnode.h"

/***********************************************************
* Globals                                                  *
************************************************************/

extern Dnode *pDnodeCursor;
extern Dnode *pDnodeHead;

/***********************************************************
* Define Descriptor class                                  *
************************************************************/

class Descriptor
{

// Public functions static
  public:
    Descriptor();
    ~Descriptor();
    void    static  AppendIt();
    void    static  ClearDescriptor();
    bool    static  DeleteNode();
    bool    static  EndOfDnodeList();
    Dnode   static *GetDnode();
    void    static  InitDescriptor();
    void    static  SetpDnodeCursorFirst();
    void    static  SetpDnodeCursorNext();
    void    static  SetpDnodeCursorPrev();
};

#endif