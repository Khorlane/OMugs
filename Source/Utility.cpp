/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   Utility.cpp                                      *
* Usage:  General purpose utility stuff                    *
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

/************************************************************
 * Get a word from CmdStr                                   *
 ************************************************************/

CString GetWord(CString CmdStr, int WordNbr)
{
  bool    Found;  // Toggle in case WordNbr requested doesn't exist
  CString Word;   // Return value
  int     i;      // Start positon for Find
  int     j;      // For loop control
  int     x;      // Position of the blank before the word
  int     y;      // Position of the blank after the word
  int     z;      // Length of CmdStr

  Found = true;
  CmdStr.TrimLeft();
  CmdStr.TrimRight();
  CmdStr  = " "     + CmdStr;
  CmdStr  = CmdStr  + " ";
  i       = 0;
  z       = CmdStr.GetLength();
  for (j = 1; j <= WordNbr; j++)
  {
    x = CmdStr.Find(' ', i);
    i = x + 1;
    y = CmdStr.Find(' ', i);
    i = y;
    if (i >= z)
    {
      Found = false;
      break;
    }
  }
  if (Found)
  {
    Word = CmdStr.Mid(x+1, y-x-1);
  }
  else
  {
    Word = "";
  }
  return Word;
}

/************************************************************
 * Get words starting with word number specified            *
 ************************************************************/

CString GetWords(CString CmdStr, int WordNbr)
{
  bool    Found;  // Toggle in case WordNbr requested doesn't exist
  CString Words;  // Return value
  int     i;      // Start positon for Find
  int     j;      // For loop control
  int     x;      // Position of the blank before the word
  int     y;      // Position of the blank after the word
  int     z;      // Length of CmdStr

  Found = true;
  CmdStr.TrimLeft();
  CmdStr.TrimRight();
  CmdStr  = " "     + CmdStr;
  CmdStr  = CmdStr  + " ";
  i       = 0;
  z       = CmdStr.GetLength();
  for (j = 1; j <= WordNbr; j++)
  {
    x = CmdStr.Find(' ', i);
    i = x + 1;
    y = CmdStr.Find(' ', i);
    i = y;
    if (i >= z)
    {
      Found = false;
      break;
    }
  }
  if (Found)
  {
    Words = CmdStr.Mid(x+1, z-(x+2));
  }
  else
  {
    Words = "";
  }
  return Words;
}