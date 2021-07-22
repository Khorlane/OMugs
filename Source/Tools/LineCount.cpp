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

#include "stdafx.h" // This is only here to speed up compiles
#include "LineCount.h"

/***********************************************************
* Globals                                                  *
************************************************************/

/***********************************************************
* LineCount constructor                                    *
************************************************************/

LineCount::LineCount()
{
  LineCountTotal3 = 0;
  OpenLineCount();
  GetSourceCodeFiles(SOURCE_DIR,  "Server", ".cpp");
  GetSourceCodeFiles(SOURCE_DIR,  "Server", ".h");
  GetSourceCodeFiles(SOURCE_DIR,  "Osi",    ".cpp");
  GetSourceCodeFiles(SOURCE_DIR,  "Osi",    ".h");
  GetSourceCodeFiles(SOURCE_DIR,  "Tools",  ".cpp");
  GetSourceCodeFiles(SOURCE_DIR,  "Tools",  ".h");
  GetSourceCodeFiles(SOURCE_DIR,  "WinApp", ".cpp");
  GetSourceCodeFiles(SOURCE_DIR,  "WinApp", ".h");
  // Write grand total
  sprintf(Buffer, "%5d", LineCountTotal3);
  Stuff = Buffer;
  Stuff += " ";
  Stuff += "Grand Total";
  Stuff += "\n";
  copy(Stuff.begin(), Stuff.end(), Buffer);
  Buffer[Stuff.size()] = '\0';
  LineCountFile.write(Buffer, strlen(Buffer));
  LineCountFile.close();
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
  LineCountFile.close();
}

/***********************************************************
 * Count source code lines                                 *
 ***********************************************************/

void LineCount::CountLines(string SourceCodeDir, string SourceCodeFileName)
{
  LineCountTotal1 = 0;
  SourceCodeFileNameSave = SourceCodeFileName;
  // Open source code file
  SourceCodeFileName = SourceCodeDir + SourceCodeFileName;
  SourceCodeFile.open(SourceCodeFileName);
  if(!SourceCodeFile.is_open())
  { // We don't care, just return
    return;
  }
  getline(SourceCodeFile, Stuff);
  while (Stuff != "End of Source")
  {
    LineCountTotal1++;
    getline(SourceCodeFile, Stuff);
    if (SourceCodeFile.peek() == EOF)
    { // End of file reached
      LineCountTotal1++;
      Stuff = "End of Source";
    }
  }
  SourceCodeFile.close();
  // Write results
  sprintf(Buffer, "%5d", LineCountTotal1);
  TmpStr = Buffer;
  Stuff  = TmpStr + " " + SourceCodeFileNameSave;
  Stuff += "\n";
  copy(Stuff.begin(), Stuff.end(), Buffer);
  Buffer[Stuff.size()] = '\0';
  LineCountFile.write(Buffer, strlen(Buffer));
}

/***********************************************************
 * Get source code files                                   *
 ***********************************************************/

void LineCount::GetSourceCodeFiles(string ParentDir, string SourceCodeDir, string FileExtension)
{
  LineCountTotal2 = 0;
  // Write directory name
  Stuff  = SourceCodeDir;
  Stuff += " Directory";
  Stuff += "\n";
  copy(Stuff.begin(), Stuff.end(), Buffer);
  Buffer[Stuff.size()] = '\0';
  LineCountFile.write(Buffer, strlen(Buffer));
  SourceCodeDir  = ParentDir + SourceCodeDir + "\\";
  for (const auto &entry : filesystem::directory_iterator(SourceCodeDir))
  { // For each source code file
    SourceCodeFileExtension = entry.path().extension().string();
    if (SourceCodeFileExtension != FileExtension)
    {
      continue;
    }
    SourceCodeFileName = entry.path().filename().string();
    CountLines(SourceCodeDir, SourceCodeFileName);
    LineCountTotal2 += LineCountTotal1;
  }
  // Write results
  sprintf(Buffer, "%5d", LineCountTotal2);
  Stuff = Buffer;
  Stuff += " Total";
  Stuff += "\n\n";
  copy(Stuff.begin(), Stuff.end(), Buffer);
  Buffer[Stuff.size()] = '\0';
  LineCountFile.write(Buffer, strlen(Buffer));
  LineCountTotal3 += LineCountTotal2;
}

/***********************************************************
 * Open LineCount file                                      *
 ***********************************************************/

void LineCount::OpenLineCount()
{
  LineCountFileName  = DOC_DIR;
  LineCountFileName += "LineCount.txt";
  LineCountFile.open(LineCountFileName);
  if(!LineCountFile.is_open())
  { // Create file failed
    AfxMessageBox("LineCount::OpenLineCount - OpenLineCount - Create LineCount file failed");
    _endthread();;
  }
  Stuff  = "OMugs Source Code Line Count Report";
  Stuff += "\n";
  Stuff += "-----------------------------------";
  Stuff += "\n\n";
  copy(Stuff.begin(), Stuff.end(), Buffer);
  Buffer[Stuff.size()] = '\0';
  LineCountFile.write(Buffer, strlen(Buffer));
}