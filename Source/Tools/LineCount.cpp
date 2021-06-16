/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   LineCount.cpp                                    *
* Usage:  Read OMugs source code and count lines           *
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

#include "stdafx.h"
#include "LineCount.h"

/***********************************************************
* Globals                                                  *
************************************************************/

extern string  HomeDir;

/***********************************************************
* LineCount constructor                                    *
************************************************************/

LineCount::LineCount()
{
  LineCountTotal3 = 0;
  OpenLineCount();
  GetSourceCodeFiles(SOURCE_DIR,  "Server", "*.cpp");
  GetSourceCodeFiles(SOURCE_DIR,  "Server", "*.h");
  GetSourceCodeFiles(SOURCE_DIR,  "Osi",    "*.cpp");
  GetSourceCodeFiles(SOURCE_DIR,  "Osi",    "*.h");
  GetSourceCodeFiles(SOURCE_DIR,  "Tools",  "*.cpp");
  GetSourceCodeFiles(SOURCE_DIR,  "Tools",  "*.h");
  GetSourceCodeFiles(SOURCE_DIR,  "WinApp", "*.cpp");
  GetSourceCodeFiles(SOURCE_DIR,  "WinApp", "*.h");
  // Write grand total
  Stuff.Format("%5d", LineCountTotal3);
  Stuff += " ";
  Stuff += "Grand Total";
  Stuff += "\n";
  LineCountFile.WriteString(Stuff);
  CloseLineCount();
}

/***********************************************************
* LineCount destructor                                     *
************************************************************/

LineCount::~LineCount()
{
}

/***********************************************************
 * Close LineCount file                                     *
 ***********************************************************/

void LineCount::CloseLineCount()
{
  LineCountFile.Close();
}

/***********************************************************
 * Count source code lines                                 *
 ***********************************************************/

void LineCount::CountLines(CString SourceCodeDir, CString SourceCodeFileName)
{
  LineCountTotal1 = 0;
  SourceCodeFileNameSave = SourceCodeFileName;
  // Open source code file
  SourceCodeFileName = SourceCodeDir + SourceCodeFileName;
  Success = SourceCodeFile.Open(SourceCodeFileName,
                     CFile::modeRead |
                     CFile::typeText);
  if(!Success)
  { // We don't care, just return
    return;
  }
  SourceCodeFile.ReadString(Stuff);
  while (Stuff != "End of Source")
  {
    LineCountTotal1++;
    SourceCodeFile.ReadString(Stuff);
    if (SourceCodeFile.GetPosition() == SourceCodeFile.GetLength())
    { // End of file reached
      LineCountTotal1++;
      Stuff = "End of Source";
    }
  }
  SourceCodeFile.Close();
  // Write results
  TmpStr.Format("%5d", LineCountTotal1);
  Stuff  = TmpStr + " " + SourceCodeFileNameSave;
  Stuff += "\n";
  LineCountFile.WriteString(Stuff);
}

/***********************************************************
 * Get source code files                                   *
 ***********************************************************/

void LineCount::GetSourceCodeFiles(CString ParentDir, CString SourceCodeDir, CString WildCard)
{
  LineCountTotal2 = 0;
  // Write directory name
  Stuff  = SourceCodeDir;
  Stuff += " Directory";
  Stuff += "\n";
  LineCountFile.WriteString(Stuff);
  // Change to home directory so that following change to source directory will work
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("LineCount::GetSourceCodeFiles - Change to home directory failed");
    _endthread();;
  }
  // Change to source code directory to get file list
  SourceCodeDir  = ParentDir + SourceCodeDir + "\\";
  if (ChgDir((LPCTSTR)SourceCodeDir))
  { // Change directory failed
    AfxMessageBox("LineCount::GetSourceCodeFiles - Change to source directory failed");
    _endthread();
  }
  MoreFiles = FileList.FindFile(WildCard);
  // Change back to home directory
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("LineCount::GetSourceCodeFiles - Change to home directory failed (1)");
    _endthread();
  }
  // FileList now contains a list of source code files
  while (MoreFiles)
  { // For each source code file
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    { // Skip directories
      continue;
    }
    SourceCodeFileName = FileList.GetFileName();
    CountLines(SourceCodeDir, SourceCodeFileName);
    LineCountTotal2 += LineCountTotal1;
  }
  // Write results
  Stuff.Format("%5d", LineCountTotal2);
  Stuff += " Total";
  Stuff += "\n\n";
  LineCountFile.WriteString(Stuff);
  LineCountTotal3 += LineCountTotal2;
  // Change to home directory
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("LineCount::GetSourceCodeFiles - Change to home directory failed (2)");
    _endthread();;
  }
}

/***********************************************************
 * Open LineCount file                                      *
 ***********************************************************/

void LineCount::OpenLineCount()
{
  LineCountFileName  = DOC_DIR;
  LineCountFileName += "LineCount.txt";
  Success = LineCountFile.Open(LineCountFileName,
                           CFile::modeCreate    |
                           CFile::modeWrite     |
                           CFile::typeText);
  if(!Success)
  { // Create file failed
    AfxMessageBox("LineCount::OpenLineCount - OpenLineCount - Create LineCount file failed");
    _endthread();;
  }
  Stuff  = "OMugs Source Code Line Count Report";
  Stuff += "\n";
  Stuff += "-----------------------------------";
  Stuff += "\n\n";
  LineCountFile.WriteString(Stuff);
}