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

#include "stdafx.h"         // precompiled headers
#include "Calendar.h"

/***********************************************************
* Calendar constructor                                     *
************************************************************/

Calendar::Calendar()
{
  TimeToAdvanceHour = CTime::GetCurrentTime() + REAL_MINUTES_PER_HOUR * 60;
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
  if (CTime::GetCurrentTime() < TimeToAdvanceHour)
  { // Not time to advance the hour
    return;
  }
  TimeToAdvanceHour = CTime::GetCurrentTime() + REAL_MINUTES_PER_HOUR * 60;
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

CString Calendar::GetTime()
{
  FormattedDateTime = "";

  Stuff = DayNames.GetAt(DayOfWeek-1);
  FormattedDateTime += Stuff;
  FormattedDateTime += ", ";

  Stuff = MonthNames.GetAt(Month-1);
  FormattedDateTime += Stuff;
  FormattedDateTime += " ";

  Stuff = DayOfMonth.GetAt(Day-1);
  FormattedDateTime += Stuff;
  FormattedDateTime += ", ";

  Stuff.Format("%d, ", Year);
  FormattedDateTime += Stuff;

  Stuff = HourNames.GetAt(Hour-1);
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
  CalendarFile.Close();
}

/***********************************************************
* Get start time                                           *
************************************************************/

void Calendar::GetStartTime()
{
  CalendarFile.ReadString(Stuff);
  CloseCalendarFile();
  Year      = atoi(Utility::GetWord(Stuff, 1));
  Month     = atoi(Utility::GetWord(Stuff, 2));
  Day       = atoi(Utility::GetWord(Stuff, 3));
  Hour      = atoi(Utility::GetWord(Stuff, 4));
  DayOfWeek = atoi(Utility::GetWord(Stuff, 5));
  if (Year <= 0)
  { // Invalid year
    Year = 1;
    LogBuf = "Calendar::GetStartTime - Year forced to 1";
    Log::LogIt(LogBuf);
  }
  if (Month <= 0)
  { // Invalid month
    Month = 1;
    LogBuf = "Calendar::GetStartTime - Month forced to 1";
    Log::LogIt(LogBuf);
  }
  if (Day <= 0)
  { // Invalid day
    Day = 1;
    LogBuf = "Calendar::GetStartTime - Day forced to 1";
    Log::LogIt(LogBuf);
  }
  if (Hour <= 0)
  { // Invalid hour
    Hour = 1;
    LogBuf = "Calendar::GetStartTime - Hour forced to 1";
    Log::LogIt(LogBuf);
  }
  if (DayOfWeek <= 0)
  { // Invalid day of week
    Hour = 1;
    LogBuf = "Calendar::GetStartTime - Day of Week forced to 1";
    Log::LogIt(LogBuf);
  }
  LogBuf = "Start date and time is: ";
  Stuff.Format("Year: %d Month: %d Day: %d Hour: %d Day of Week: %d", Year, Month, Day, Hour, DayOfWeek);
  LogBuf += Stuff;
  Log::LogIt(LogBuf);
}

/***********************************************************
* Open calendar file                                       *
************************************************************/

void Calendar::OpenCalendarFile()
{
  CalendarFileIsOpen = false;
  CalendarFileName = CONTROL_DIR;
  CalendarFileName += "Calendar.txt";
  if (!CFile::GetStatus(CalendarFileName, FileStatus))
  { // Calendar file does not exist
    LogBuf = "Calendar file not found.";
    Log::LogIt(LogBuf);
	  LogBuf = "Forcing start date to Year: 1 Month: 1 Day: 1 Hour: 1 Day of Week: 1";
    Log::LogIt(LogBuf);
	  return;
  }
  Success = CalendarFile.Open(CalendarFileName,
                   CFile::modeRead |
                   CFile::typeText);
  if(Success)
  { // Open was successful
    CalendarFileIsOpen = true;
  }
  else
  { // Open failed, something is bad wrong
    AfxMessageBox("Calendar::OpenCalendarFile - Failed", MB_ICONSTOP);
    _endthread();
  }
}

/***********************************************************
* Load DayNames array                                      *
************************************************************/

void Calendar::LoadDayNamesArray()
{
  DayNamesFileName =  DAY_NAMES_DIR;
  DayNamesFileName += "DayNames.txt";
  Success = DayNamesFile.Open(DayNamesFileName,
                   CFile::modeRead |
                   CFile::typeText);
  if(!Success)
  { // Open failed
    AfxMessageBox("Calendar::LoadDayNamesArray - Open Day Names file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  DayNames.RemoveAll();
  DayNamesFile.ReadString(Stuff);
  while (Stuff != "")
  { // Read all day names
    DayNames.Add(Stuff);
    DayNamesFile.ReadString(Stuff);
  }
  DayNamesFile.Close();
  LogBuf = "DayNames array loaded";
  Log::LogIt(LogBuf);
}

/***********************************************************
* Load DayOfMonth array                                    *
************************************************************/

void Calendar::LoadDayOfMonthArray()
{
  DayOfMonthFileName =  DAY_NAMES_DIR;
  DayOfMonthFileName += "DayOfMonth.txt";
  Success = DayOfMonthFile.Open(DayOfMonthFileName,
                     CFile::modeRead |
                     CFile::typeText);
  if(!Success)
  { // Open failed
    AfxMessageBox("Calendar::LoadDayOfMonthArray - Open Day Of Month file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  DayOfMonth.RemoveAll();
  DayOfMonthFile.ReadString(Stuff);
  while (Stuff != "")
  { // Read all day of month
    DayOfMonth.Add(Stuff);
    DayOfMonthFile.ReadString(Stuff);
  }
  DayOfMonthFile.Close();
  LogBuf = "DayOfMonth array loaded";
  Log::LogIt(LogBuf);
}

/***********************************************************
* Load HourNames array                                    *
************************************************************/

void Calendar::LoadHourNamesArray()
{
  HourNamesFileName =  HOUR_NAMES_DIR;
  HourNamesFileName += "HourNames.txt";
  Success = HourNamesFile.Open(HourNamesFileName,
                     CFile::modeRead |
                     CFile::typeText);
  if(!Success)
  { // Open failed
    AfxMessageBox("Calendar::LoadHourNamesArray - Open Hour Names file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  HourNames.RemoveAll();
  HourNamesFile.ReadString(Stuff);
  while (Stuff != "")
  { // Read all hour names
    HourNames.Add(Stuff);
    HourNamesFile.ReadString(Stuff);
  }
  HourNamesFile.Close();
  LogBuf = "HourNames array loaded";
  Log::LogIt(LogBuf);
}

/***********************************************************
* Load MonthNames array                                    *
************************************************************/

void Calendar::LoadMonthNamesArray()
{
  MonthNamesFileName =  MONTH_NAMES_DIR;
  MonthNamesFileName += "MonthNames.txt";
  Success = MonthNamesFile.Open(MonthNamesFileName,
                     CFile::modeRead |
                     CFile::typeText);
  if(!Success)
  { // Open failed
    AfxMessageBox("Calendar::LoadMonthNamesArray - Open Month Names file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  MonthNames.RemoveAll();
  MonthNamesFile.ReadString(Stuff);
  while (Stuff != "")
  { // Read all month names
    MonthNames.Add(Stuff);
    MonthNamesFile.ReadString(Stuff);
  }
  MonthNamesFile.Close();
  LogBuf = "MonthNames array loaded";
  Log::LogIt(LogBuf);
}

/***********************************************************
* Save time                                                *
************************************************************/

void Calendar::SaveTime()
{
  Success = CalendarFile.Open(CalendarFileName,
                   CFile::modeCreate |
                   CFile::modeWrite  |
                   CFile::typeText);
  if(!Success)
  { // Open failed
    AfxMessageBox("Calendar::SaveTime open file - Failed", MB_ICONSTOP);
    _endthread();
  }
  Stuff.Format("%d %d %d %d %d", Year, Month, Day, Hour, DayOfWeek);
  CalendarFile.WriteString(Stuff);
  CloseCalendarFile();
}