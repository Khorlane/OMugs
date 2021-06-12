/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   LineCount.h                                      *
* Usage:  Read OMugs source code and count lines           *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef LINE_COUNT_H
#define LINE_COUNT_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"

/***********************************************************
* Define LineCount class                                   *
************************************************************/

class LineCount
{

// Public functions
  public:
    LineCount();
    ~LineCount();

// Private functions
  private:
    void            CloseLineCount();
    void            CountLines(CString SourceCodeDir, CString SourceCodeFileName);
    void            OpenLineCount();
    void            GetSourceCodeFiles(CString ParentDir, CString SourceCodeDir, CString WildCard);

// Private variables
  private:
    CFileFind       FileList;
    CStdioFile      LineCountFile;
    CString         LineCountFileName;
    int             LineCountTotal1;
    int             LineCountTotal2;
    int             LineCountTotal3;
    BOOL            MoreFiles;
    CStdioFile      SourceCodeFile;
    CString         SourceCodeFileName;
    CString         SourceCodeFileNameSave;
    CString         Stuff;
    int             Success;
    CString         TmpStr;
};

#endif