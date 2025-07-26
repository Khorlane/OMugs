/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   LineCount.h                                      *
* Usage:  Read OMugs source code and count lines           *
* Author: Steve Bryant                                     *
************************************************************/

#ifndef LINE_COUNT_H
#define LINE_COUNT_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include <string>           // For std::string
#include <fstream>          // For file operations (ifstream, ofstream)
#include <filesystem>       // For std::filesystem
#include <thread>           // For _endthread
#include <cstring>          // For strlen
#include <algorithm>        // For std::copy
#include <cstdio>           // For sprintf
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
};

#endif