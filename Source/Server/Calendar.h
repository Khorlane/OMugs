/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Calendar.h                                       *
* Usage:  Define Calendar class                            *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef	CALENDAR_H
#define CALENDAR_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Log.h"
#include "Utility.h"

/***********************************************************
* Define Calendar class                                    *
************************************************************/

class Calendar
{

// Public functions
  public:
    Calendar();
    ~Calendar();
    void            AdvanceTime();
    CString         GetTime();

// Private functions
  private:
    void            CloseCalendarFile();
    void            GetStartTime();
    void            OpenCalendarFile();
    void            LoadDayNamesArray();
    void            LoadDayOfMonthArray();
    void            LoadHourNamesArray();
    void            LoadMonthNamesArray();
    void            SaveTime();

// Private variables
    char            Buffer[100];
    ifstream        CalendarFileInp;
    bool            CalendarFileIsOpen;
    string          CalendarFileName;
    ofstream        CalendarFileOut;
    int             Day;
    CStringArray    DayOfMonth;
    CStdioFile      DayOfMonthFile;
    CString         DayOfMonthFileName;
    int             DayOfWeek;
    CStringArray    DayNames;
    CStdioFile      DayNamesFile;
    CString         DayNamesFileName;
    CFileStatus     FileStatus;
    CString         FormattedDateTime;
    int             Hour;
    CStringArray    HourNames;
    CStdioFile      HourNamesFile;
    CString         HourNamesFileName;
    string          LogBuf;
    int             Month;
    CStringArray    MonthNames;
    CStdioFile      MonthNamesFile;
    CString         MonthNamesFileName;
    CString         Stuff;
    int             Success;
    time_t          TimeToAdvanceHour;
    int             Year;
};

#endif