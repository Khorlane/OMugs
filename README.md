# OMugs

OMugs is an Online Multi-User Game Server. It is a MUD server written from scratch in C++ and is a member of the combat-oriented family of MUDs. The concepts used to build OMugs are by no means original, but the implementation of those concepts, namely OMugs, is original.

Coding of OMugs began in 2002 and the first release went live on March 7, 2003. By early 2004, the project had completed 62 documented server components.

The OMugs project began after spending considerable time creating a game using the CircleMUD server. See www.circlemud.org for more information about that MUD server. Also see `Beginning.md` in the `Doc` directory for more information
about the beginnings of OMugs.

OMugs ran only on Windows until 2026. In 2021, work began to replace Microsoft Foundation Classes (MFC) with standard C++. In 2026, OMugs became a command-line server, and Winsock was replaced with POSIX sockets as part of the Linux port.

## Current Status

OMugs is a command-line server that runs on Linux. Development uses Microsoft Visual Studio 2026 on Windows 11, with WSL for Linux builds and testing. The server is deployed to a separate Linux host.

## Mapper Protocol

HolyQuest offers GMCP (Telnet option 201). A client that accepts it receives `HolyQuest.Room` after login or reconnect and whenever the game shows the current room, including after movement and `look`:

```text
HolyQuest.Room {"id":"JesseSquare8","name":"Jesse Square","exits":{"E":"SpringGateStreet7","S":"ValleyGateAvenue12","W":"PalaceWay9"}}
```

Use `id` as the mapper's room key and `name` as the display label. `exits` maps each outgoing exit to its destination room ID. Compass and vertical directions use `N`, `NE`, `E`, `SE`, `S`, `SW`, `W`, `NW`, `U`, or `D`; named exits such as `ArmorShop` keep their names. No reverse exits are implied. A room with no exits sends an empty object.

Plain Telnet clients still receive the usual game text. The in-game `roominfo on/off` setting controls only the visible room text, not GMCP.

## Project Layout

`Doc\`

Contains documentation for OMugs.

`Library\`

Contains files that are read by OMugs and used to define the world.

`Running\`

Contains the runtime directories and files.

`Source\Server\`

Contains the Online Multi-User Game Server (OMugs) source code.

`Source\Osi\`

Contains OSI (OMugs Script Interpreter) source code.

`Source\Tools\`

Contains tools source code.

`Utility\`

Miscellaneous mud admin scripts.

`WebSite\`

The HolyQuest website.

`HomeDir.txt`

Contains a single line that specifies the OMugs home directory.

## License

This project is licensed under the [Unlicense](LICENSE), which releases the code into the public domain.
