/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Calendar.cpp                                     *
* Usage:  Maintains the game calendar                      *
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
#include "Calendar.h"

/***********************************************************
* Calendar constructor                                     *
************************************************************/

Calendar::Calendar()
{
  time_t Timer;

  DEBUGIT(1);
  time(&Timer);
  TimeToAdvanceHour = Timer + REAL_MINUTES_PER_HOUR * 60;
  Year      = 1;
  Month     = 1;
  Day       = 1;
  Hour      = 1;
  DayOfWeek = 1;
  OpenCalendarFile();
  if (CalendarFileIsOpen)
  { // Get start time
    GetStartTime();
  }
  LoadDayNamesArray();
  LoadDayOfMonthArray();
  LoadHourNamesArray();
  LoadMonthNamesArray();
}

/***********************************************************
* Calendar destructor                                      *
************************************************************/

Calendar::~Calendar()
{
  DEBUGIT(1);
  SaveTime();
}

////////////////////////////////////////////////////////////
// Public functions                                       //
////////////////////////////////////////////////////////////

/***********************************************************
* Advance time                                             *
************************************************************/

void Calendar::AdvanceTime()
{
  time_t NowSec;

  DEBUGIT(5);
  time(&NowSec);
  if (NowSec < TimeToAdvanceHour)
  { // Not time to advance the hour
    return;
  }
  TimeToAdvanceHour = NowSec + REAL_MINUTES_PER_HOUR * 60;
  Hour++;
  if (Hour > HOURS_PER_DAY)
  {
    Day++;
    Hour = 1;
    DayOfWeek++;
    if (DayOfWeek > DAYS_PER_WEEK)
    {
      DayOfWeek = 1;
    }
  }
  if (Day > DAYS_PER_MONTH)
  {
    Month++;
    Day = 1;
  }
  if (Month > MONTHS_PER_YEAR)
  {
    Year++;
    Month = 1;
  }
  SaveTime();
}

/***********************************************************
* Get time                                                 *
************************************************************/

string Calendar::GetTime()
{
  DEBUGIT(1);
  FormattedDateTime = "";

  Stuff = DayNames[DayOfWeek-1];
  FormattedDateTime += Stuff;
  FormattedDateTime += ", ";

  Stuff = MonthNames[Month-1];
  FormattedDateTime += Stuff;
  FormattedDateTime += " ";

  Stuff = DayOfMonth[Day-1];
  FormattedDateTime += Stuff;
  FormattedDateTime += ", ";

  sprintf(Buffer, "%d ", Year);
  Stuff = Buffer;
  FormattedDateTime += Stuff;

  Stuff = HourNames[Hour-1];
  FormattedDateTime += Stuff;

  return FormattedDateTime;
}

////////////////////////////////////////////////////////////
// Private functions                                      //
////////////////////////////////////////////////////////////

/***********************************************************
* Close calendar file                                      *
************************************************************/

void Calendar::CloseCalendarFile()
{
  DEBUGIT(1);
  CalendarFileInp.close();
}

/***********************************************************
* Get start time                                           *
************************************************************/

void Calendar::GetStartTime()
{
  DEBUGIT(1);
  getline(CalendarFileInp, Stuff);
  CloseCalendarFile();
  Year      = stoi(StrGetWord(Stuff, 1));
  Month     = stoi(StrGetWord(Stuff, 2));
  Day       = stoi(StrGetWord(Stuff, 3));
  Hour      = stoi(StrGetWord(Stuff, 4));
  DayOfWeek = stoi(StrGetWord(Stuff, 5));
  if (Year <= 0)
  { // Invalid year
    Year = 1;
    LogBuf = "Calendar::GetStartTime - Year forced to 1";
    LogIt(LogBuf);
  }
  if (Month <= 0)
  { // Invalid month
    Month = 1;
    LogBuf = "Calendar::GetStartTime - Month forced to 1";
    LogIt(LogBuf);
  }
  if (Day <= 0)
  { // Invalid day
    Day = 1;
    LogBuf = "Calendar::GetStartTime - Day forced to 1";
    LogIt(LogBuf);
  }
  if (Hour <= 0)
  { // Invalid hour
    Hour = 1;
    LogBuf = "Calendar::GetStartTime - Hour forced to 1";
    LogIt(LogBuf);
  }
  if (DayOfWeek <= 0)
  { // Invalid day of week
    Hour = 1;
    LogBuf = "Calendar::GetStartTime - Day of Week forced to 1";
    LogIt(LogBuf);
  }
  LogBuf = "Start date and time is: ";
  sprintf(Buffer, "Year: %d Month: %d Day: %d Hour: %d Day of Week: %d", Year, Month, Day, Hour, DayOfWeek);
  Stuff = Buffer;
  LogBuf += Stuff;
  LogIt(LogBuf);
}

/***********************************************************
* Open calendar file                                       *
************************************************************/

void Calendar::OpenCalendarFile()
{
  DEBUGIT(1);
  CalendarFileIsOpen = false;
  CalendarFileName   = CONTROL_DIR;
  CalendarFileName  += "Calendar.txt";
  CalendarFileInp.open(CalendarFileName);
  if (!CalendarFileInp.is_open())
  { // Calendar file does not exist
    LogBuf = "Calendar file not found.";
    LogIt(LogBuf);
    LogBuf = "Forcing start date to Year: 1 Month: 1 Day: 1 Hour: 1 Day of Week: 1";
    LogIt(LogBuf);
    return;
  }
  // Open was successful
  CalendarFileIsOpen = true;
}

/***********************************************************
* Load DayNames array                                      *
************************************************************/

void Calendar::LoadDayNamesArray()
{
  DEBUGIT(1);
  DayNamesFileName =  DAY_NAMES_DIR;
  DayNamesFileName += "DayNames.txt";
  DayNamesFile.open(DayNamesFileName);
  if(!DayNamesFile.is_open())
  { // Open failed
    AfxMessageBox("Calendar::LoadDayNamesArray - Open Day Names file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  DayNames.clear();
  getline(DayNamesFile, Stuff);
  while (DayNamesFile.peek() != EOF)
  { // Read all day names
    DayNames.push_back(Stuff);
    getline(DayNamesFile, Stuff);
  }
  DayNames.push_back(Stuff);
  DayNamesFile.close();
  LogBuf = "DayNames array loaded";
  LogIt(LogBuf);
}

/***********************************************************
* Load DayOfMonth array                                    *
************************************************************/

void Calendar::LoadDayOfMonthArray()
{
  DEBUGIT(1);
  DayOfMonthFileName  =  DAY_NAMES_DIR;
  DayOfMonthFileName += "DayOfMonth.txt";
  DayOfMonthFile.open(DayOfMonthFileName);
  if(!DayOfMonthFile.is_open())
  { // Open failed
    AfxMessageBox("Calendar::LoadDayOfMonthArray - Open Day Of Month file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  DayOfMonth.clear();
  getline(DayOfMonthFile, Stuff);
  while (DayOfMonthFile.peek() != EOF)
  { // Read all day of month
    DayOfMonth.push_back(Stuff);
    getline(DayOfMonthFile, Stuff);
  }
  DayOfMonth.push_back(Stuff);
  DayOfMonthFile.close();
  LogBuf = "DayOfMonth array loaded";
  LogIt(LogBuf);
}

/***********************************************************
* Load HourNames array                                    *
************************************************************/

void Calendar::LoadHourNamesArray()
{
  DEBUGIT(1);
  HourNamesFileName =  HOUR_NAMES_DIR;
  HourNamesFileName += "HourNames.txt";
  HourNamesFile.open(HourNamesFileName);
  if(!HourNamesFile.is_open())
  { // Open failed
    AfxMessageBox("Calendar::LoadHourNamesArray - Open Hour Names file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  HourNames.clear();
  getline(HourNamesFile, Stuff);
  while (HourNamesFile.peek() != EOF)
  { // Read all hour names
    HourNames.push_back(Stuff);
    getline(HourNamesFile, Stuff);
  }
  HourNames.push_back(Stuff);
  HourNamesFile.close();
  LogBuf = "HourNames array loaded";
  LogIt(LogBuf);
}

/***********************************************************
* Load MonthNames array                                    *
************************************************************/

void Calendar::LoadMonthNamesArray()
{
  DEBUGIT(1);
  MonthNamesFileName =  MONTH_NAMES_DIR;
  MonthNamesFileName += "MonthNames.txt";
  MonthNamesFile.open(MonthNamesFileName);
  if (!MonthNamesFile.is_open())
  { // Open failed
    AfxMessageBox("Calendar::LoadMonthNamesArray - Open Month Names file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  MonthNames.clear();
  getline(MonthNamesFile, Stuff);
  while (MonthNamesFile.peek() != EOF)
  { // Read all month names
    MonthNames.push_back(Stuff);
    getline(MonthNamesFile, Stuff);
  }
  MonthNames.push_back(Stuff);
  MonthNamesFile.close();
  LogBuf = "MonthNames array loaded";
  LogIt(LogBuf);
}

/***********************************************************
* Save time                                                *
************************************************************/

void Calendar::SaveTime()
{
  DEBUGIT(1);
  CalendarFileName  = CONTROL_DIR;
  CalendarFileName += "Calendar.txt";
  CalendarFileOut.open(CalendarFileName);
  if (!CalendarFileOut.is_open())
  { // Open failed
    LogBuf = "Calendar::SaveTime - Open calendar file - Failed";
    LogIt(LogBuf);
    _endthread();
  }
  sprintf(Buffer, "%d %d %d %d %d", Year, Month, Day, Hour, DayOfWeek);
  CalendarFileOut.write(Buffer, strlen(Buffer));
  CalendarFileOut.close();
}