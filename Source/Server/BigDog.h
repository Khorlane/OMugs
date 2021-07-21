/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   BigDog.h                                         *
* Usage:  Starting point for all OMugs stuff.              *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef BIGDOG_H 
#define BIGDOG_H 

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Calendar.h"
#include "Communication.h"
#include "Config.h"
#include "Descriptor.h"
#include "Dnode.h"
#include "Log.h"
#include "Social.h"
#include "World.h"
#include "Validate.h"
#include "WhoIsOnline.h"

/***********************************************************
* Globals                                                  *
************************************************************/

string ConvertCStringToString(CString Str1)
{
  return (LPCTSTR)Str1;
}

CString ConvertStringToCString(string Str1)
{
  return Str1.c_str();
}

string StrLeft(string Str1, int Len)
{
  return Str1.substr(0, Len);
}

void StrReplace(string &str, const string &from, const string &to)
 {
  if (from.empty())
    return;
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
  }
}

string StrRight(string Str1, int Len)
{
  if (Str1 == "")
  {
    return "";
  }
  return Str1.substr(Str1.length() - Len, Len);
}

string StrTrimLeft(string Str1)
{
  if (Str1 == "")
  {
    return "";
  }
  const auto First = Str1.find_first_not_of(' ');
  return Str1.substr(First, Str1.length());
}

string StrTrimRight(string Str1)
{
  const auto Last = Str1.find_last_not_of(' ');
  return Str1.substr(0, Last + 1);
}

string StrMakeLower(string Str1)
{
  transform(Str1.begin(), Str1.end(), Str1.begin(),
    [](unsigned char c) { return tolower(c); });
  return Str1;
}

string StrGetWord(string Str1, int WordNbr)
{
  int    i;
  string Word;
  stringstream iss(Str1);
  i = 0;
  while (iss >> Word)
  {
    i++;
    if (i == WordNbr)
      return Word;
  }
  return "";
}

bool FileExist(string Name)
{
  bool Exist;

  ifstream f(Name.c_str());
  if (f.is_open())
  {
    Exist = true;
    f.close();
  }
  else
  {
    Exist = false;
  }
  return Exist;
}

bool ChgDir(string Dir)
{
  return _chdir(Dir.c_str());
}

void Rename(string File1, string File2)
{
  rename(File1.c_str(), File2.c_str());
}

#endif