/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   Descriptor.h                                     *
* Usage:  Define Descriptor class                          *
* Author: Steve Bryant (khorlane@www.holyquest.org)        *
*                                                          *
* This program belongs to Stephen L. Bryant.               *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "stdafx.h"
#include "Dnode.h"

/***********************************************************
* Define Descriptor class                                  *
************************************************************/

class Descriptor
{

// Public functions
  public:
    Descriptor();
    ~Descriptor();
    Dnode         *GetDnode()           {return pDnodeCursor;}
    void          AppendIt(Dnode *);
    bool          DeleteNode();
    bool          EndOfDnodeList()      {return pDnodeCursor->DnodeFd ? false : true;}
    void          SetpDnodeCursorFirst(){pDnodeCursor = pDnodeHead->pDnodeNext;}
    void          SetpDnodeCursorNext() {pDnodeCursor = pDnodeCursor->pDnodeNext;}
    void          SetpDnodeCursorPrev() {pDnodeCursor = pDnodeCursor->pDnodePrev;}

// Protected variables
  protected:
    Dnode         *pDnodeCursor;
    Dnode         *pDnodeHead;
};

#endif