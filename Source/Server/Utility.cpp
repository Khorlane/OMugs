/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Utility.cpp                                      *
* Usage:  General purpose utility stuff                    *
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
#include "Utility.h"

/***********************************************************
* Globals                                                  *
************************************************************/

////////////////////////////////////////////////////////////
// Public functions static                                //
////////////////////////////////////////////////////////////

/***********************************************************
 * Calculate a percentage                                  *
 ***********************************************************/

int Utility::CalcPct(int Dividend, int Divisor)
{
  double  x;
  double  y;
  double  z;

  x = Dividend;
  y = Divisor;
  z = x / y * 100.0;
  return int(z);
}

/***********************************************************
 * Delete a word plus one blank after from a string        *
 ***********************************************************/

CString Utility::DeleteWord(CString String, int WordNbr)
{
  int     Count;
  int     Len;
  CString PosLen;
  int     Pos;

  if (WordNbr < 1)
  { // That is just plain silly
    return String;
  }
  Count = WordCount(String);
  if (WordNbr > Count)
  { // WordNbr is greater than the number of words in String
    return String;
  }
  PosLen = GetWordPosLen(String, WordNbr);
  Pos    = atoi(GetWord(PosLen, 1));
  Len    = atoi(GetWord(PosLen, 2));
  if (WordNbr < Count)
  { // Not the last word, so delete the word and the blank after the word
    String.Delete(Pos-1, Len+1);
  }
  else
  { // This is the last word, so delete the word and the blank before the word
    String.Delete(Pos-2, Len+1);
  }
  return String;
}

/***********************************************************
 * Insert commas into a nueric string                      *
 ***********************************************************/

CString Utility::FormatCommas(CString String)
{
  int i; // For loop control
  int j; // Insert comma position
  int x; // Original string length
  int y; // Number of commas to be inserted
  int z; // Remainder after dividing by 3

  x = String.GetLength();
  if (x < 4)
  { // No commas needed
    return String;
  }
  y = x/3;
  z = x%3;
  if (z == 0)
  { // String length is evenly divisable by 3
    y--;
  }
  j = x - 3;
  for (i = y; i > 0; i--)
  { // Insert the commas
    String.Insert(j,',');
    j = j - 3;
  }
  return String;
}

/***********************************************************
 * Get home directory                                      *
 ***********************************************************/

string Utility::GetHomeDir()
{
  ifstream   HomeDirFile;
  string     HomeDirFileName;
  string     Buffer;

  // Read HomeDir file - must be in same dir as OMugs.exe
  HomeDirFileName  = "HomeDir";
  HomeDirFileName += ".txt";
  HomeDirFile.open(HomeDirFileName);
  if(!HomeDirFile.is_open())
  {
    AfxMessageBox("Utility::GetHomeDir - Open HomeDir file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  getline(HomeDirFile, Buffer);
  HomeDir = Buffer;
  HomeDirFile.close();
  return HomeDir.c_str();
}

/***********************************************************
 * Get a random number                                     *
 ***********************************************************/

int Utility::GetRandomNumber(int Limit)
{
  int RandomNumber1;
  int RandomNumber2;
  int RandomNumber3;
  
  RandomNumber1 = rand();
  RandomNumber2 = int((RAND_MAX/Limit));
  RandomNumber3 = int(RandomNumber1 / RandomNumber2) + 1;
  if (RandomNumber3 > Limit)
  {
    RandomNumber3 = Limit;
  }
  return RandomNumber3;
}

/***********************************************************
 * Get a SQL statement                                     *
 ***********************************************************/

CString Utility::GetSqlStmt(CString SqlStmtId)
{
  int        RipOutMoreSpaces;
  CString    SqlStmt;
  CStdioFile SqlStmtFile;
  CString    SqlStmtFileName;
  CString    Stuff;
  int        Success;

  RipOutMoreSpaces = 0;
  // Read mobile stats Desc1 file
  SqlStmtFileName = SQL_DIR;
  SqlStmtFileName += SqlStmtId;
  SqlStmtFileName += ".txt";
  Success = SqlStmtFile.Open(SqlStmtFileName,
                  CFile::modeRead |
                  CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Utility::GetSqlStmt - Open SqlStmt file failed", MB_ICONSTOP);
    _endthread();
  }
  SqlStmtFile.ReadString(Stuff);
  while (Stuff != "")
  { // Read SQL statement
    SqlStmt += Stuff;
    SqlStmt += " ";
    SqlStmtFile.ReadString(Stuff);
  }
  SqlStmtFile.Close();
  do
  { // Rip out all extra spaces
    RipOutMoreSpaces = SqlStmt.Replace("  "," ");
  }
  while (RipOutMoreSpaces);
  return SqlStmt;
}

/***********************************************************
 * Get a word from String                                  *
 ***********************************************************/

CString Utility::GetWord(CString String, int WordNbr)
{
  bool    Found;  // Toggle in case WordNbr requested doesn't exist
  CString Word;   // Return value
  int     i;      // Start positon for Find
  int     j;      // For loop control
  int     x;      // Position of the blank before the word
  int     y;      // Position of the blank after the word
  int     z;      // Length of String

  i = 0;
  j = 0;
  x = 0;
  y = 0;
  z = 0;
  if (WordCount(String) < WordNbr)
  { // WordNbr is greater than the number of words in String
    Word = "";
    return Word;
  }
  Found = true;
  i = 1;
  while (i)
  {
    i = String.Replace("  ", " ");
  }
  String.TrimLeft();
  String.TrimRight();
  String  = String  + " ";
  String  = " "     + String;
  i       = 0;
  z       = String.GetLength();
  for (j = 1; j <= WordNbr; j++)
  {
    x = String.Find(' ', i);
    i = x + 1;
    y = String.Find(' ', i);
    i = y;
    if (i >= z)
    {
      Found = false;
      break;
    }
  }
  if (Found)
  {
    Word = String.Mid(x+1, y-x-1);
  }
  else
  {
    Word = "";
  }
  return Word;
}

/***********************************************************
 * Get the position and length of a word from String       *
 ***********************************************************/

CString Utility::GetWordPosLen(CString String, int WordNbr)
{
  bool    Found;  // Toggle in case WordNbr requested doesn't exist
  CString Word;   // Return value
  int     i;      // Start positon for Find
  int     j;      // For loop control
  int     x;      // Position of the blank before the word
  int     y;      // Position of the blank after the word
  int     z;      // Length of String

  i = 0;
  j = 0;
  x = 0;
  y = 0;
  z = 0;
  if (WordCount(String) < WordNbr)
  { // WordNbr is greater than the number of words in String
    Word = "0 0";
    return Word;
  }
  Found = true;
  i = 1;
  while (i)
  {
    i = String.Replace("  ", " ");
  }
  String.TrimLeft();
  String.TrimRight();
  String  = String  + " ";
  String  = " "     + String;
  i       = 0;
  z       = String.GetLength();
  for (j = 1; j <= WordNbr; j++)
  {
    x = String.Find(' ', i);
    i = x + 1;
    y = String.Find(' ', i);
    i = y;
    if (i >= z)
    {
      Found = false;
      break;
    }
  }
  if (Found)
  {
    sprintf(Buf, "%d %d", x+1, y-x-1);
    Word = ConvertStringToCString(Buf);
  }
  else
  {
    Word = "0 0";
  }
  return Word;
}

/***********************************************************
 * Get words starting with word number specified           *
 ***********************************************************/

CString Utility::GetWords(CString String, int StartWordNbr)
{
  bool    Found;  // Toggle in case WordNbr requested doesn't exist
  CString Words;  // Return value
  int     i;      // Start positon for Find
  int     j;      // For loop control
  int     x;      // Position of the blank before the word
  int     y;      // Position of the blank after the word
  int     z;      // Length of String

  i = 0;
  j = 0;
  x = 0;
  y = 0;
  z = 0;
  Found = true;
  String.TrimLeft();
  String.TrimRight();
  String  = " "     + String;
  String  = String  + " ";
  i       = 0;
  z       = String.GetLength();
  for (j = 1; j <= StartWordNbr; j++)
  {
    x = String.Find(' ', i);
    i = x + 1;
    y = String.Find(' ', i);
    i = y;
    if (i >= z)
    {
      Found = false;
      break;
    }
  }
  if (Found)
  {
    Words = String.Mid(x+1, z-(x+2));
    Words.TrimLeft();
    Words.TrimRight();
  }
  else
  {
    Words = "";
  }
  return Words;
}

/***********************************************************
 * Is word 'not in' word list?                             *
 ***********************************************************/

bool Utility::IsNotWord(CString Word, CString WordList)
{
  bool    Found;
  int     i;
  int     n;
  CString String;

  Found = false;
  if (Word.GetLength() == 0)
  { // Word is null, so it can't be in word list
    return true;
  }
  n = WordCount(WordList);
  for (i = 1; i <= n; i++)
  {
    String = GetWord(WordList, i);
    if (Word == String)
    {
      Found = true;
    }
  }
  if (Found)
  { // Word was found in word list
    return false;
  }
  else
  { // Word was not found in word list
    return true;
  }
}

/***********************************************************
 * Is word 'in' word list?                                 *
 ***********************************************************/

bool Utility::IsWord(CString Word, CString WordList)
{
  bool    Found;
  int     i;
  int     n;
  CString String;

  Found = false;
  if (Word.GetLength() == 0)
  { // Word is null, so it can't be in word list
    return false;
  }
  n = WordCount(WordList);
  for (i = 1; i <= n; i++)
  {
    String = GetWord(WordList, i);
    if (Word == String)
    {
      Found = true;
    }
  }
  if (Found)
  { // Word was found in word list
    return true;
  }
  else
  { // Word was not found in word list
    return false;
  }
}

/***********************************************************
 * Make first letter lowercase                             *
 ***********************************************************/

CString Utility::MakeFirstLower(CString String)
{
  CString FirstLetter;
  CString TheRest;

  FirstLetter = String.Left(1);
  TheRest     = String.Right(String.GetLength()-1);
  FirstLetter.MakeLower();
  String = FirstLetter + TheRest;
  return String;
}

/***********************************************************
 * Make first letter uppercase                             *
 ***********************************************************/

CString Utility::MakeFirstUpper(CString String)
{
  CString FirstLetter;
  CString TheRest;

  FirstLetter = String.Left(1);
  TheRest     = String.Right(String.GetLength()-1);
  FirstLetter.MakeUpper();
  String = FirstLetter + TheRest;
  return String;
}

/***********************************************************
* Substitute $thingies to more meaningful pronouns         *
************************************************************/

CString Utility::PronounSubstitute(CString MsgText)
{
  CString PronounHeShe;
  CString PronounHimHer;
  CString PronounHisHers;
  CString PronounHimselfHerself;

  if (pDnodeSrc->pPlayer->Sex == "M")
  { // Set male player pronouns
    PronounHeShe          = "he";
    PronounHimHer         = "him";
    PronounHisHers        = "his";
    PronounHimselfHerself = "himself";
  }
  else
  { // Set female player pronouns
    PronounHeShe          = "she";
    PronounHimHer         = "her";
    PronounHisHers        = "her"; // was 'hers' but I think it should 'her'???
    PronounHimselfHerself = "herself";
  }
  // Make the player substitutions
  MsgText.Replace("$P",               pDnodeSrc->PlayerName);
  MsgText.Replace("$pHeShe",          PronounHeShe);
  MsgText.Replace("$pHimHer",         PronounHimHer);
  MsgText.Replace("$pHisHers",        PronounHisHers);
  MsgText.Replace("$pHimselfHerself", PronounHimselfHerself);
  if (pDnodeTgt)
  { // There is a valid target
    if (pDnodeTgt->pPlayer->Sex == "M")
    { // Set male target pronouns
      PronounHeShe          = "he";
      PronounHimHer         = "him";
      PronounHisHers        = "his";
      PronounHimselfHerself = "himself";
    }
    else
    { // Set female target pronouns
      PronounHeShe          = "she";
      PronounHimHer         = "her";
      PronounHisHers        = "her"; // was 'hers' but I think it should 'her'???
      PronounHimselfHerself = "herself";
    }
    // Make the target substitutions
    MsgText.Replace("$T",               pDnodeTgt->PlayerName);
    MsgText.Replace("$tHeShe",          PronounHeShe);
    MsgText.Replace("$tHimHer",         PronounHimHer);
    MsgText.Replace("$tHisHers",        PronounHisHers);
    MsgText.Replace("$tHimselfHerself", PronounHimselfHerself);
  }
  return MsgText;
}

/***********************************************************
 * Translate a word                                        *
 ***********************************************************/

CString Utility::TranslateWord(CString Word)
{
  if (Word == "n")            return "go north";
  if (Word == "s")            return "go south";
  if (Word == "e")            return "go east";
  if (Word == "w")            return "go west";

  if (Word == "ne")           return "go northeast";
  if (Word == "se")           return "go southeast";
  if (Word == "sw")           return "go southwest";
  if (Word == "nw")           return "go northwest";

  if (Word == "u")            return "go up";
  if (Word == "d")            return "go down";

  if (Word == "con")          return "consider";
  if (Word == "des")          return "destroy";
  if (Word == "em")           return "emote";
  if (Word == "eq")           return "equipment";
  if (Word == "i")            return "inventory";
  if (Word == "k")            return "kill";
  if (Word == "l")            return "look";

  if (Word == "obj")          return "object";
  if (Word == "mob")          return "mobile";

  if (Word == "north")        return "go north";
  if (Word == "south")        return "go south";
  if (Word == "east")         return "go east";
  if (Word == "west")         return "go west";

  if (Word == "northeast")    return "go northeast";
  if (Word == "southeast")    return "go southeast";
  if (Word == "southwest")    return "go southwest";
  if (Word == "northwest")    return "go northwest";

  if (Word == "up")           return "go up";
  if (Word == "down")         return "go down";

  if (Word == "head")         return "01";
  if (Word == "earleft")      return "02";
  if (Word == "earright")     return "03";
  if (Word == "neck")         return "04";
  if (Word == "shoulders")    return "05";
  if (Word == "chest")        return "06";
  if (Word == "back")         return "07";
  if (Word == "arms")         return "08";
  if (Word == "wristleft")    return "09";
  if (Word == "wristright")   return "10";
  if (Word == "hands")        return "11";
  if (Word == "fingerleft")   return "12";
  if (Word == "fingerright")  return "13";
  if (Word == "shield")       return "14";
  if (Word == "waist")        return "15";
  if (Word == "legs")         return "16";
  if (Word == "ankleleft")    return "17";
  if (Word == "ankleright")   return "18";
  if (Word == "feet")         return "19";
  if (Word == "wielded")      return "20";
  if (Word == "light")        return "21";

  if (Word == "01")           return "Worn on head:            ";
  if (Word == "02")           return "Worn in left ear:        ";
  if (Word == "03")           return "Worn in right ear:       ";
  if (Word == "04")           return "Worn around neck:        ";
  if (Word == "05")           return "Worn on shoulders:       ";
  if (Word == "06")           return "Worn on chest:           ";
  if (Word == "07")           return "Worn on back:            ";
  if (Word == "08")           return "Worn on arms:            ";
  if (Word == "09")           return "Worn on left wrist       ";
  if (Word == "10")           return "Worn on right wrist      ";
  if (Word == "11")           return "Worn on hands:           ";
  if (Word == "12")           return "Worn on left finger:     ";
  if (Word == "13")           return "Worn on right finger:    ";
  if (Word == "14")           return "Worn as a shield:        ";
  if (Word == "15")           return "Worn around waist:       ";
  if (Word == "16")           return "Worn on legs:            ";
  if (Word == "17")           return "Worn around left ankle:  ";
  if (Word == "18")           return "Worn around right ankle: ";
  if (Word == "19")           return "Worn on feet:            ";
  if (Word == "20")           return "Weapon wielded:          ";
  if (Word == "21")           return "Using as a light:        ";

  if (Word == "hand")         return "slap";
  if (Word == "axe")          return "chop";
  if (Word == "club")         return "bash";
  if (Word == "dagger")       return "stab";
  if (Word == "hammer")       return "pound";
  if (Word == "spear")        return "pierce";
  if (Word == "staff")        return "whack";
  if (Word == "sword")        return "slash";

  return Word;
}

/***********************************************************
 * Count the number of words                               *
 ***********************************************************/

int Utility::WordCount(CString String)
{
  int i;
  int n;

  if (String.GetLength() == 0)
  { // No words ... length is zero
    return 0;
  }
  i = 1;
  while (i)
  {
    i = String.Replace("  ", " ");
  }
  String.TrimLeft();
  String.TrimRight();
  n = String.Remove(' ');
  n++;
  return n;
}