/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Config.h                                         *
* Usage:  Configuration header file                        *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include <direct.h>         // _chdir()
#include <process.h>        // _endthread()
#include <bitset>           // Allows bit manipulation
#include <math.h>           // Math functions like pow, ceil, floor
#include <afxdao.h>         // Allows access to ACCESS databases
#include <iostream>         // std::getline
#include <fstream>          // std::fstream
#include <string>           // std::string
#include <algorithm>        // Functions used on ranges of elements (see StrMakeLower)
#include <cctype>           // String functions like tolower, toupper
#include <sstream>          // String 'stream' functions (see StrGetWord)
#include <time.h>           // Time functions
#include <vector>           // Vector stuff
#include <filesystem>       // Needed to get file system info, like all files in a directory

using namespace std;

/***********************************************************
* Macros                                                   *
************************************************************/

#define VERSION                 "2022.08.25"
#define DEBUGIT(x)              if (DEBUG_LVL >= x) LogIt((string)__FUNCTION__)
#define DEBUG_LVL               1      // 0 to turn off, 1 or more to turn on

#define MAC                    300.0f  // Maximum Armor Class
#define MDRP                    60.0f  // Maximum Damage Reduction Percent
#define MGBP                    80.0f  // Maximum Group Bonus Percent
#define MGRP                    50.0f  // Maximum Gain Reduction Percent

#define MOB_ARM_PER_LEVEL          5   // Currently not used
#define MOB_DMG_PER_LEVEL          9   // Damage per level 
#define MOB_EXP_PER_LEVEL         50   // Experiece per level
#define MOB_HPT_PER_LEVEL         31   // Hit points per level
#define MOB_MOVE_PCT              30   // Percent chance mob will move

#define PLAYER_DMG_HAND            5   // Bare hand damage
#define PLAYER_DMG_PCT            75   // Random damage deduction
#define PLAYER_EXP_PER_LEVEL    1000   // Experiece per level factor
#define PLAYER_HPT_PER_LEVEL      31   // Hit points per level
#define PLAYER_LOOSES_EXP_LEVEL    5   // Level when death causes xp lose
#define PLAYER_SKILL_PER_LEVEL     3   // Skill points gained per level

// Defines tick duration: 100 = 10 ticks per second = game wakes up ~10 times a second
#define MILLI_SECONDS_TO_SLEEP   100

#define EVENT_TICK                50   // 5    seconds
#define FIGHT_TICK                17   // 1.7  seconds
#define HUNGER_THIRST_TICK       540   // 54   seconds
#define INPUT_TICK               300   // 30   seconds
#define MOB_HEAL_TICK            900   // 90   seconds
#define STATS_TICK               101   // 10.1 seconds
#define WHO_IS_ONLINE_TICK       165   // 16.5 seconds

#define REAL_MINUTES_PER_HOUR      4   // Game time advances 1 hour every 4 minutes
#define HOURS_PER_DAY             24   // Game hours in each game day
#define DAYS_PER_WEEK              7   // Game days in each game week
#define DAYS_PER_MONTH            30   // Game weeks in each game month
#define MONTHS_PER_YEAR           12   // Game months in each game year

#define GRP_LIMIT                  4   // Maximum group members
#define HPT_GAIN_STAND             2   // Hit points gained while standing
#define HPT_GAIN_SIT               5   // Hit points gained while sitting
#define HPT_GAIN_SLEEP             9   // Hit points gained while sleeping
#define HUNGER_THIRST_LEVEL        3   // No hunger or thirst until this level
#define MAX_INPUT_LENGTH        1024   // Maximun string length received from player
#define MAX_ROOMS               8192   // Must be a multiple of 8
#define MAX_ROOMS_CHAR          MAX_ROOMS/8 // 8 rooms represented by each byte
#define PORT_NBR                7373   // Listening on this port for connections - might need to kill Windows Service 'iphlpsvc' if you use port 7777

#define SAFE_ROOM               "JesseSquare8"
#define START_ROOM              "Welcome226"
#define UNTRAIN_COST            "250"  // Cost to UnTrain a skill 

#define GREETING_DIR            ".\\Library\\"
#define HELP_DIR                ".\\Library\\"
#define MOTD_DIR                ".\\Library\\"
#define SOCIAL_DIR              ".\\Library\\"
#define VALID_CMDS_DIR          ".\\Library\\"
#define VALID_NAMES_DIR         ".\\Library\\"

#define DAY_NAMES_DIR           ".\\Library\\Calendar\\"
#define DAY_OF_MONTH_DIR        ".\\Library\\Calendar\\"
#define HOUR_NAMES_DIR          ".\\Library\\Calendar\\"
#define MONTH_NAMES_DIR         ".\\Library\\Calendar\\"

#define LOOT_DIR                ".\\Library\\Loot\\"
#define MOBILES_DIR             ".\\Library\\Mobiles\\"
#define OBJECTS_DIR             ".\\Library\\Objects\\"
#define ROOMS_DIR               ".\\Library\\Rooms\\"
#define SCRIPTS_DIR             ".\\Library\\Scripts\\"
#define SHOPS_DIR               ".\\Library\\Shops\\"
#define SQL_DIR                 ".\\Library\\Sql\\"
#define TALK_DIR                ".\\Library\\Talk\\"
#define WORLD_MAP_DIR           ".\\Library\\World\\Map\\"
#define WORLD_MOBILES_DIR       ".\\Library\\World\\Mobiles\\"

#define CONTROL_DIR             ".\\Running\\Control\\"
#define CONTROL_EVENTS_DIR      ".\\Running\\Control\\Events\\"
#define CONTROL_MOB_INWORLD_DIR ".\\Running\\Control\\Mobiles\\InWorld\\"
#define CONTROL_MOB_NOMOVE_DIR  ".\\Running\\Control\\Mobiles\\NoMove\\"
#define CONTROL_MOB_SPAWN_DIR   ".\\Running\\Control\\Mobiles\\Spawn\\"
#define LOG_DIR                 ".\\Running\\Log\\"
#define PLAYER_DIR              ".\\Running\\Players\\"
#define PLAYER_EQU_DIR          ".\\Running\\Players\\PlayerEqu\\"
#define PLAYER_OBJ_DIR          ".\\Running\\Players\\PlayerObj\\"
#define PLAYER_ROOM_DIR         ".\\Running\\Players\\PlayerRoom\\"
#define ROOM_MOB_DIR            ".\\Running\\RoomMob\\"
#define ROOM_OBJ_DIR            ".\\Running\\RoomObj\\"

#define MOB_PLAYER_DIR          ".\\Running\\Violence\\MobPlayer\\"
#define PLAYER_MOB_DIR          ".\\Running\\Violence\\PlayerMob\\"
#define MOB_STATS_ARM_DIR       ".\\Running\\Violence\\MobStats\\Armor\\"
#define MOB_STATS_ATK_DIR       ".\\Running\\Violence\\MobStats\\Attack\\"
#define MOB_STATS_DMG_DIR       ".\\Running\\Violence\\MobStats\\Damage\\"
#define MOB_STATS_DSC_DIR       ".\\Running\\Violence\\MobStats\\Desc1\\"
#define MOB_STATS_EXP_DIR       ".\\Running\\Violence\\MobStats\\ExpPoints\\"
#define MOB_STATS_HPT_DIR       ".\\Running\\Violence\\MobStats\\HitPoints\\"
#define MOB_STATS_LOOT_DIR      ".\\Running\\Violence\\MobStats\\Loot\\"
#define MOB_STATS_ROOM_DIR      ".\\Running\\Violence\\MobStats\\Room\\"

#define DOC_DIR                 ".\\Doc\\"
#define SOURCE_DIR              ".\\Source\\"
#define TMP_DIR                 ".\\Tmp\\"
#define UTILITY_DIR             ".\\Utility\\"
#define WEB_SITE_DIR            ".\\WebSite\\"

/***********************************************************
* Osi - OMugs Script Interpreter                           *
************************************************************/

#define EOF_CHAR                0x7F   // End of file character
#define ICODE_SEGSIZE           4096
#define MAX_DIGITS              4      // Maximun number of digits
#define MAX_RES_WORDS           25     // Maximum number of reserved words
#define RUN_STACK_SIZE          4      // Run time stack size

// Osi Character codes
enum EnumCharCode
{
  ccLetter,
  ccDigit,
  ccSpecial,
  ccQuote,
  ccWhiteSpace,
  ccEndOfFile,
  ccError
};

// Osi Token codes
enum EnumTokenCode
{
  tcDummy,
  tcBraceL,
  tcBraceR,
  tcDeclareInt,
  tcDeclareFloat,
  tcDeclareString,
  tcElse,
  tcEndOfFile,
  tcEqual,
  tcEqualEqual,
  tcError,
  tcGe,
  tcGt,
  tcIdentifier,
  tcIf,
  tcLe,
  tcLt,
  tcMinus,
  tcNe,
  tcNumber,
  tcParenL,
  tcParenR,
  tcPlus,
  tcScript,
  tcSlash,
  tcStar,
  tcString,
  tcWhile
};

// Osi Token & Symbol data types
enum EnumDataType
{
  tyDummy,
  tyFloat,
  tyInteger,
  tyCharacter,
  tyString
};

// Osi Symbol types
enum EnumSymbolType
{
  syDummy,
  syVarFloat,
  syVarInteger,
  syVarCharacter,
  syVarString,
  syLitFloat,
  syLitInteger,
  syLitCharacter,
  syLitString
};

// Osi Token & Symbol data value
union EnumDataValue
{
  int     Integer;
  float   Real;
  char    Character;
  char   *pString;
};

// Osi reserverd words
struct ResWordStruct
{
  string        ResWord;
  EnumTokenCode TokenCode;
};

/***********************************************************
* Globals                                                  *
************************************************************/

// Functions
void    AddMobToRoom(CString RoomId, CString MobileId);
bool    AddObjToPlayerEqu(CString WearPosition, CString ObjectId);
void    AddObjToRoom(CString RoomId, CString ObjectId);
void    AdvanceTime();
void    AppendIt();
void    AppTestCode();
int     CalcAdjustedExpPoints(int PlayerLevel, int MobileLevel, int ExpPoints);
int     CalcDamageToMobile(int Damage, int WeaponSkill);
int     CalcDamageToPlayer(int Damage, int PAC);
CString CalcHealthPct(int HitPoints, int HitPointsMax);
float   CalcLevelExperience(int Level);
int     CalcPct(int Dividend, int Divisor);
int     CalcPlayerArmorClass();
bool    ChgDir(string Dir);
void    ClearDescriptor();
void    CloseLogFile();
string  ConvertCStringToString(CString Str1);
CString ConvertStringToCString(string  Str1);
int     CountMob(CString MobileId);
void    CreateMobPlayer(CString PlayerName, CString MobileId);
void    CreateMobStatsFileWrite(CString Directory, CString MobileIdForFight, CString Stuff);
void    CreatePlayerMob(CString PlayerName, CString MobileId);
void    DeleteMobPlayer(CString PlayerName, CString MobileId);
void    DeleteMobStats(CString MobileId);
bool    DeleteNode();
void    DeletePlayerMob(CString PlayerName);
CString DeleteWord(CString String, int WordNbr);
bool    EndOfDnodeList();
void    Events();
bool    FileExist(string Name);
CString FormatCommas(CString String);
int     GetCount();
string  GetHomeDir();
CString GetMobDesc1(CString MobileId);
CString GetMobPlayerMobileId(CString PlayerName, int i);
int     GetMobileArmor(CString MobileId);
CString GetMobileAttack(CString MobileId);
int     GetMobileDamage(CString MobileId);
CString GetMobileDesc1(CString MobileId);
CString GetMobileExpPointsLevel(CString MobileId);
CString GetMobileLoot(CString MobileId);
CString GetMobileRoom(CString MobileId);
CString GetPlayerMobMobileId(CString PlayerName);
int     GetRandomNumber(int Limit);
string  GetRoomId(string RoomId);
string  GetRoomName(string RoomId);
CString GetSqlStmt(CString SqlStmtId);
int     GetTimeSeconds();
string  GetValidMobRoomExits(string RoomId);
CString GetWord(CString String, int WordNbr);
CString GetWords(CString String, int StartWordNbr);
void    HealMobiles();
void    InitDescriptor();
bool    IsExit(string MudCmdIsExit);
bool    IsHelp();
bool    IsMobileIdInRoom(CString RoomId, CString MobileId);
bool    IsNameValid(CString Name);
bool    IsNotWord(CString Word, CString WordList);
void    IsObjInPlayerEqu(CString ObjectName);
void    IsObjInPlayerInv(CString ObjectName);
void    IsObjInRoom(CString ObjectName);
void    IsObject(CString ObjectId);
bool    IsPlayer(CString PlayerName);
bool    IsRoom(string RoomId);
bool    IsRoomType(string RoomId, string RoomType);
bool    IsShop(CString RoomId);
void    IsShopObj(CString RoomId, CString ObjectName);
bool    IsSleeping();
bool    IsSocial();
bool    IsWord(CString Word, CString WordList);
void    ListObjects();
void    LogIt(CString LogBuf);
void    LogIt(string LogBuf);
void    OpenLogFile();
void    Osi(CString ScriptType, CString ScriptId);
void    PrintIt(string Message);
string  PronounSubstitute(string MsgText);
void    PutMobBackInRoom(CString PlayerName, CString RoomId);
void    RemoveMobFromRoom(CString RoomId, CString MobileId);
void    RemoveObjFromPlayerEqu(CString ObjectId);
void    RemoveObjFromPlayerInv(CString ObjectId, int Count);
void    RemoveObjFromRoom(CString ObjectId);
int     Rename(string File1, string File2);
void    SendToRoom(CString TargetRoomId, CString MsgText);
void    SendToRoom(string TargetRoomId, string MsgText);
void    SetpDnodeCursorFirst();
void    SetpDnodeCursorNext();
void    ShowPlayerInv();
void    SockCheckForNewConnections();
void    SockClosePort(int PortNbr);
void    SockOpenPort(int PortNbr);
void    SockRecv();
void    SpawnMobileNoMove(CString MobileId);
int     StrCountChar(CString Str1, char c);
int     StrCountChar(string Str1, char c);
int     StrCountWords(CString Str1);
int     StrCountWords(string Str1);
CString StrDelete(CString Str1, int Position, int Length);
string  StrDelete(string Str1, int Position, int Length);
CString StrDeleteWord(CString Str1, int WordNbr);
string  StrDeleteWord(string Str1, int WordNbr);
int     StrFind(CString HayStack, const char *Needle);
int     StrFind(CString HayStack, CString Needle);
int     StrFind(string HayStack, string Needle);
int     StrFindFirstChar(CString Str1, char c);
int     StrFindFirstChar(string Str1, char c);
int     StrFindOneOf(CString HayStack, string Needle);
int     StrFindOneOf(string HayStack, string Needle);
char    StrGetAt(CString Str1, int Position);
char    StrGetAt(string Str1, int Position);
int     StrGetLength(CString Str1);
int     StrGetLength(string Str1);
CString StrGetWord(CString Str1, int WordNbr);
string  StrGetWord(string Str1, int WordNbr);
string  StrGetWords(string Str1, int WordNbr);
CString StrInsert(CString Str1, int Position, CString Str2);
string  StrInsert(string Str1, int Position, string str2);
CString StrInsertChar(CString Str1, int Position, char c);
string  StrInsertChar(string Str1, int Position, char c);
CString StrLeft(CString Str1, int Len);
string  StrLeft(string Str1, int Len);
CString StrMakeFirstLower(CString Str1);
string  StrMakeFirstLower(string Str1);
CString StrMakeFirstUpper(CString Str1);
string  StrMakeFirstUpper(string Str1);
CString StrMakeLower(CString Str1);
string  StrMakeLower(string Str1);
CString StrMakeUpper(CString Str1);
string  StrMakeUpper(string Str1);
CString StrRemove(CString Str1, char c);
string  StrRemove(string Str1, char c);
void    StrReplace(CString &str, const string &from, const string &to);
void    StrReplace(string &str, const string &from, const string &to);
CString StrRight(CString Str1, int Len);
string  StrRight(string Str1, int Len);
CString StrSetAt(CString Str1, int Position, char c);
string  StrSetAt(string Str1, int Position, char c);
CString StrSqueeze(CString Str1);
string  StrSqueeze(string Str1);
CString StrTrimLeft(CString Str1);
string  StrTrimLeft(string Str1);
CString StrTrimRight(CString Str1);
string  StrTrimRight(string Str1);
string  StrVectorGetAt(vector<string> &StrVector1, int Position);
void    StrVectorSetAt(vector<string> &StrVector1, int Position, string Str1);
CString TranslateWord(CString Word);
void    UpdateMobInWorld(CString MobileId, CString AddRemove);
bool    ValidateIt(CString ValidationType);
CString WhackMobile(CString PlayerName, int DamageToMobile, CString MobileDesc1, CString WeaponType);
CString WhackPlayer(CString MobileDesc1, CString MobileAttack, int DamageToPlayer);
void    WhereMob(CString MobileIdSearch);
void    WhereObj(CString ObjectIdSearch);
int     WordCount(CString String);

// Variables
inline char           Buf[256];
inline EnumCharCode   CharCodeMap[128];
inline CString        CmdStr;
inline unsigned char  CurrentLineNumber;
inline string         HomeDir;
inline CString        MudCmd;
inline float          PACMN;                // Percent Armor Class Magic Number
inline string         ScriptFileName;
inline bool           StateConnections;
inline bool           StateRunning;
inline bool           StateStopping;
inline bool           ValErr;

#endif