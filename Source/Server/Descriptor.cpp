/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Descriptor.cpp                                   *
* Usage:  Maintains a linked list of connection data       *
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

#include "stdafx.h"         // precompiled headers
#include "Descriptor.h"

/***********************************************************
* Globals                                                  *
************************************************************/

/***********************************************************
 * Descriptor constructor                                  *
 ***********************************************************/

Descriptor::Descriptor()
{
}

/***********************************************************
 * Descriptor destructor                                   *
 ***********************************************************/

Descriptor::~Descriptor()
{
}

////////////////////////////////////////////////////////////
// Public functions static                                //
////////////////////////////////////////////////////////////

/***********************************************************
 * Append new connection to connection list                *
 ***********************************************************/

void Descriptor::AppendIt()
{
  pDnodeActor->pDnodePrev                 = pDnodeHead->pDnodePrev;
  pDnodeHead->pDnodePrev->pDnodeNext  = pDnodeActor;
  pDnodeHead->pDnodePrev              = pDnodeActor;
  pDnodeActor->pDnodeNext                 = pDnodeHead;
}

/***********************************************************
 * Clear descriptor linked list                            *
 ***********************************************************/

void Descriptor::ClearDescriptor()
{
  delete pDnodeHead;
  pDnodeHead = NULL;
}
/***********************************************************
 * Delete connection from connection list and close socket *
 ***********************************************************/

bool Descriptor::DeleteNode()
{
  Dnode *pDnode;
  int    Result;

  if (!pDnodeCursor->DnodeFd)
  {
    return false;
  }
  // Close the socket (pg 70)
  Result = ::closesocket(pDnodeCursor->DnodeFd);
  if (Result != 0)
  {
    AfxMessageBox("Descriptor::DeleteNode - Error: closesocket", MB_ICONSTOP);
    _endthread();
  }
  pDnodeActor = NULL;
  pDnode = pDnodeCursor->pDnodePrev;
  delete pDnodeCursor;
  pDnodeCursor = pDnode;
  return true;
}

/***********************************************************
 * End of Dnode list?                                      *
 ***********************************************************/

bool Descriptor::EndOfDnodeList()
{
  return pDnodeCursor->DnodeFd ? false : true;
}

/***********************************************************
 * Get Dnode pointer                                       *
 ***********************************************************/

Dnode *Descriptor::GetDnode()
{
  return pDnodeCursor;
}

/***********************************************************
 * Initialize descriptor pointers                          *
 ***********************************************************/

void Descriptor::InitDescriptor()
{
  pDnodeHead              = new Dnode(0, "");
  pDnodeHead->pDnodeNext  = pDnodeHead;
  pDnodeHead->pDnodePrev  = pDnodeHead;
  pDnodeCursor            = pDnodeHead;
}

/***********************************************************
 * Set Dnode pointer to the first Dnode in the list        *
 ***********************************************************/

void Descriptor::SetpDnodeCursorFirst()
{
  pDnodeCursor = pDnodeHead->pDnodeNext;
}

/***********************************************************
 * Set Dnode pointer to the next Dnode in the list         *
 ***********************************************************/

void Descriptor::SetpDnodeCursorNext()
{
  pDnodeCursor = pDnodeCursor->pDnodeNext;
}

/***********************************************************
 * Set Dnode pointer to the previous Dnode in the list     *
 ***********************************************************/

void Descriptor::SetpDnodeCursorPrev()
{
  pDnodeCursor = pDnodeCursor->pDnodePrev;
}