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
    string          GetTime();

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
    vector<string>  DayOfMonth;
    ifstream        DayOfMonthFile;
    string          DayOfMonthFileName;
    int             DayOfWeek;
    vector<string>  DayNames;
    ifstream        DayNamesFile;
    string          DayNamesFileName;
    string          FormattedDateTime;
    int             Hour;
    vector<string>  HourNames;
    ifstream        HourNamesFile;
    string          HourNamesFileName;
    string          LogBuf;
    int             Month;
    vector<string>  MonthNames;
    ifstream        MonthNamesFile;
    string          MonthNamesFileName;
    string          Stuff;
    int             Success;
    time_t          TimeToAdvanceHour;
    int             Year;
};

inline Calendar    *pCalendar;

#endif