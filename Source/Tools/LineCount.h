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
    void            CountLines(string SourceCodeDir, string SourceCodeFileName);
    void            OpenLineCount();
    void            GetSourceCodeFiles(string ParentDir, string SourceCodeDir, string WildCard);

// Private variables
  private:
    char            Buffer[100];
    ofstream        LineCountFile;
    string          LineCountFileName;
    int             LineCountTotal1;
    int             LineCountTotal2;
    int             LineCountTotal3;
    ifstream        SourceCodeFile;
    string          SourceCodeFileExtension;
    string          SourceCodeFileName;
    string          SourceCodeFileNameSave;
    string          Stuff;
    string          TmpStr;
};

#endif