OMugs - Online Multi-User Game Server

Author: Steve Bryant (stevebryant@www.holyquest.org)

OMugs belongs to Stephen L. Bryant.
It is considered a trade secret and is not to be divulged or used by
parties who have not received written authorization from the owner.

OMugs is a 'mud' server and is a member of the combat-oriented family
of muds. The concepts used to build OMugs are by no means original.
However, the means by which those concepts are implemented are
completely original. In other words, the author wrote the OMugs
codebase from scratch.

The OMugs project began after spending considerable time creating a
game using the CircleMud server. Therefore, there are a number of
similarities between OMugs and CircleMUD and DIKU as well, because
CircleMUD is a deriviative of DIKU. See www.circlemud.org and
www.dikumud.com for more information about those mud servers. Also
see Beginning.doc in the Doc directory for more information about
the beginnings of OMugs.

The OMugs project development and operating environment is Microsoft
Windows. The code is written using Visual Studio 2022 and the
documentation is done using EditPlus and Word.

The remainder of this file is a summary of what is in the files and
directories that constitute the the OMugs project.

Debug\
    Contains the *.obj files for the project, plus a number of
    other files Visual Studio creates and uses to manage the project.

Doc\
    Contains documentation for OMugs.

Library\
    Contains files that are read by OMugs and are used to define the
    'world'.
    
Res\
    Contains legacy application icon and cursor resources that are no
    longer part of the active command-line server build.

Running\
    Contains files created and maintained by OMugs.

Source\Server
    Contains the source code for the Online Multi-User Game Server,
    including the command-line entry point.

Source\Tools
    Contains the source code for non-UI tools, like validation.

Tmp\
    Temporary stuff, should be empty most of the time.

Utility\
    Misc stuff like a room generator, 'who' xml generator for the
    website's 'who is online' page.

WebSite\
    The HolyQuest website.

HomeDir.txt
    Contains one line that specifies the home directory for the mud
    server and requires double backslashes.
    For example: C:\\OMugs\\

ReadMe.txt
    This file.

All other directories and files are Visual Studio or git related.
