/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   Descriptor.cpp                                   *
* Usage:  Maintain a linked list of connections            *
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
#include "Descriptor.h"

/************************************************************
 * Descriptor constructor                                   *
 ************************************************************/

Descriptor::Descriptor()
{
  pDnodeHead              = new Dnode(0);
  pDnodeHead->pDnodeNext  = pDnodeHead;
  pDnodeHead->pDnodePrev  = pDnodeHead;
  pDnodeCursor            = pDnodeHead;
}

/************************************************************
 * Append new connection to connection list                 *
 ************************************************************/

void Descriptor::AppendIt(Dnode *pDnode1)
{
  pDnode1->pDnodePrev                 = pDnodeHead->pDnodePrev;
  pDnodeHead->pDnodePrev->pDnodeNext  = pDnode1;
  pDnodeHead->pDnodePrev              = pDnode1;
  pDnode1->pDnodeNext                 = pDnodeHead;
}

/************************************************************
 * Delete connection from connection list and close socket  *
 ************************************************************/

bool Descriptor::DeleteNode()
{
  Dnode *pDnode1;

  if (!pDnodeCursor->DnodeFd)
  {
    return false;
  }
  closesocket(pDnodeCursor->DnodeFd);
  pDnode1 = pDnodeCursor->pDnodePrev;
  delete pDnodeCursor;
  pDnodeCursor = pDnode1;
  return true;
}

/************************************************************
 * Descriptor destructor                                    *
 ************************************************************/

Descriptor::~Descriptor()
{
}