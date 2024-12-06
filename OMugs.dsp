# Microsoft Developer Studio Project File - Name="OMugs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=OMugs - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OMugs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OMugs.mak" CFG="OMugs - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OMugs - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "OMugs - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OMugs - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "OMugs - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "C:\OMugs\Source\WinApp" /I "C:\OMugs\Source\Server" /I "C:\OMugs\Source\Osi" /I "C:\OMugs\Source\Tools" /I "C:\OMugs\Source\Sqlite3" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fr /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 sqlite3.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /pdbtype:sept /libpath:"C:\Omugs\Source\Sqlite3"

!ENDIF 

# Begin Target

# Name "OMugs - Win32 Release"
# Name "OMugs - Win32 Debug"
# Begin Group "Server Source Files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\Source\Server\BigDog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Calendar.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Communication.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Descriptor.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Dnode.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Help.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Mobile.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Object.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Player.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Room.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Shop.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Social.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Utility.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Violence.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Server\World.cpp
# End Source File
# End Group
# Begin Group "Server Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\Source\Server\BigDog.h
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Calendar.h
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Color.h
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Communication.h
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Config.h
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Descriptor.h
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Dnode.h
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Help.h
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Log.h
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Mobile.h
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Object.h
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Player.h
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Room.h
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Shop.h
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Social.h
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Utility.h
# End Source File
# Begin Source File

SOURCE=.\Source\Server\Violence.h
# End Source File
# Begin Source File

SOURCE=.\Source\Server\World.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\Res\cursor2.cur
# End Source File
# Begin Source File

SOURCE=.\Res\cursor3.cur
# End Source File
# Begin Source File

SOURCE=.\res\OMugs.ico
# End Source File
# Begin Source File

SOURCE=.\res\OMugs.rc2
# End Source File
# Begin Source File

SOURCE=.\res\OMugsDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Group "WinApp Source Files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\Source\WinApp\BuildMobiles.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinApp\BuildMobilesList.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinApp\BuildObjects.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinApp\BuildObjectsList.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinApp\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinApp\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinApp\OMugs.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinApp\OMugs.rc
# End Source File
# Begin Source File

SOURCE=.\Source\WinApp\OMugsDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinApp\OMugsView.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinApp\StdAfx.cpp

!IF  "$(CFG)" == "OMugs - Win32 Release"

!ELSEIF  "$(CFG)" == "OMugs - Win32 Debug"

# ADD CPP /Yc"StdAfx.h"

!ENDIF 

# End Source File
# End Group
# Begin Group "WinApp Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\Source\WinApp\BuildMobiles.h
# End Source File
# Begin Source File

SOURCE=.\Source\WinApp\BuildMobilesList.h
# End Source File
# Begin Source File

SOURCE=.\Source\WinApp\BuildObjects.h
# End Source File
# Begin Source File

SOURCE=.\Source\WinApp\BuildObjectsList.h
# End Source File
# Begin Source File

SOURCE=.\Source\WinApp\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\Source\WinApp\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Source\WinApp\OMugs.h
# End Source File
# Begin Source File

SOURCE=.\Source\WinApp\OMugsDoc.h
# End Source File
# Begin Source File

SOURCE=.\Source\WinApp\OMugsView.h
# End Source File
# Begin Source File

SOURCE=.\Source\WinApp\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Source\WinApp\StdAfx.h
# End Source File
# End Group
# Begin Group "Tools Source Files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\Source\Tools\GenerateRooms.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\LineCount.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\Validate.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\WhoIsOnline.cpp
# End Source File
# End Group
# Begin Group "Tools Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\Source\Tools\GenerateRooms.h
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\LineCount.h
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\Validate.h
# End Source File
# Begin Source File

SOURCE=.\Source\Tools\WhoIsOnline.h
# End Source File
# End Group
# Begin Group "Osi Source Files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\Source\Osi\Buffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Osi\Executor.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Osi\Icode.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Osi\Parser.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Osi\RunStack.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Osi\Scanner.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Osi\Symbol.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Osi\Token.cpp
# End Source File
# End Group
# Begin Group "Osi Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\Source\Osi\Buffer.h
# End Source File
# Begin Source File

SOURCE=.\Source\Osi\Executor.h
# End Source File
# Begin Source File

SOURCE=.\Source\Osi\Icode.h
# End Source File
# Begin Source File

SOURCE=.\Source\Osi\Parser.h
# End Source File
# Begin Source File

SOURCE=.\Source\Osi\RunStack.h
# End Source File
# Begin Source File

SOURCE=.\Source\Osi\Scanner.h
# End Source File
# Begin Source File

SOURCE=.\Source\Osi\Symbol.h
# End Source File
# Begin Source File

SOURCE=.\Source\Osi\Token.h
# End Source File
# End Group
# Begin Group "Sqlite3"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\Source\Sqlite3\sqlite3.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
