# OMugs

OMugs is an Online Multi-User Game Server. It is a MUD server written from
scratch in C++ and is a member of the combat-oriented family of MUDs. The
concepts used to build OMugs are by no means original, but the implementation
is original to this codebase.

Coding of OMugs began in 2002 and the first release went live on March 7, 2003.
By the end of 2003, the codebase was complete and supported 61 commands.

The OMugs project began after spending considerable time creating a game using
the CircleMUD server. There are a number of similarities between OMugs and
CircleMUD and DIKU, because CircleMUD is a derivative of DIKU. See
www.circlemud.org and www.dikumud.com for more information about those MUD
servers. Also see `Beginning.doc` in the `Doc` directory for more information
about the beginnings of OMugs.

## Current Status

OMugs is now a command-line server. Project development remains centered on
Microsoft Visual Studio 2026, using WSL for Linux compilation and testing. The
intended operational environment is a remote Linux server.

## Project Layout

`Debug\`

Contains the object files for the project, plus other files Visual Studio
creates and uses to manage the project.

`Doc\`

Contains documentation for OMugs.

`Library\`

Contains files that are read by OMugs and used to define the world.

`Res\`

Contains legacy application icon and cursor resources that are no longer part
of the active command-line server build.

`Running\`

Contains the runtime directory skeleton and required control seed files.
Most files created and maintained by OMugs while the server runs are ignored.

`Source\Server\`

Contains the source code for the Online Multi-User Game Server, including the
command-line entry point.

`Source\Osi\`

Contains OSI script parsing and runtime support.

`Source\Tools\`

Contains the source code for non-UI tools, like validation.

`Tmp\`

Temporary files. This directory should be empty most of the time.

`Utility\`

Miscellaneous tools, including the room generator and the Who Is Online XML
generator for the website.

`WebSite\`

The HolyQuest website.

`HomeDir.txt`

Contains one line that specifies the OMugs home directory. For the current WSL
debug configuration this is `/mnt/c/OMugs/`.

All other directories and files are Visual Studio or Git related.

## License

This project is licensed under the [Unlicense](LICENSE), which releases the
code into the public domain.
