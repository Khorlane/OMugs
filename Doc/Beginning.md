# OMugs Project Beginning

## Introduction

This document describes the origin of OMugs, which is a mud server. In order to
understand some parts of this document, it is recommended that the reader
educate their self concerning muds and mud servers via the internet.

## Origin

The origin of the Online Multi-User Game Server, OMugs, goes back to the first
months of the year 1999. It was then that I first participated in an activity
known as mudding. The first mud I played was called The Inquisition and was, at
that time, a combat oriented mud. I enjoyed playing The Inquisition and became
interested in the computer program that facilitated so much fun. I learned that
the program was called a Mud Server and set out to explore this new, to me,
breed of programs.

## CircleMUD

My explorations, via the internet, soon took me to the CircleMUD website,
[www.circlemud.org](http://www.circlemud.org/). Almost all mud servers are
written and run on a Unix platform. I prefer a Windows platform, and was
pleased to see that CircleMUD had been ported to Windows and someone had been
so kind as to make a Microsoft Visual C++ workspace for CircleMUD. I promptly
downloaded CircleMUD and began exploring the code.

Of course, the CircleMUD server being a program and me being a programmer, I
immediately wanted to change something. The first thing to change was to add the
`.txt` extension on the file name for the log so that I could double-click the
file and it would automatically open the file using Notepad. I went on to make
a total of 39 changes to the CircleMUD code, carefully documenting each change.

For reasons described later in this document, I abandoned CircleMUD. It is a
great server and as of this writing continues to have a very dedicated
development staff, an active e-mail list, and numerous active muds using it.

## HolyQuest

The game that I created using the CircleMUD server was named HolyQuest by my
daughter, Sherry. My son, Chris, became interested in the game and assisted with
ideas for the world of HolyQuest. HolyQuest's theme is ancient Israel around
the time of King David. Creating HolyQuest required the complete removal of the
stock CircleMUD world and replacing it with my own original world. In June of
2000, HolyQuest was placed online and anyone in the world with internet access
could connect, although the world of HolyQuest only consisted of Jerusalem and
Snake Rock.

## World Editor

The building of the HolyQuest world, albeit small, was quite time consuming. So
I tried several world editor programs and even wrote a couple of my own. The
drive for obtaining or creating a world editor came from the fact that a
CircleMUD world had a fair degree of complexity and used numerous codes, which
I had to constantly look up in the building document.

The world editor programs I tried only assisted with the creation of rooms. The
architecture of a CircleMUD world is oriented around areas. An object, say a
candle, had to be defined in each area in which you wanted a candle. There was
no editor that I found that had a unified world view interface.

## zMUD

I was not particularly enamored with the method CircleMUD used to create
in-game mobiles and objects. This led to several attempts to create a means for
loading mobiles and objects using zMUD, a mud client program that can be
downloaded from [www.zuggsoft.com](http://www.zuggsoft.com/).

CircleMUD uses numbers to identify each room, mobile, and object. This made it
difficult, for me, to create the files that controlled where mobiles and
objects were created in the game. I was constantly having to look up the number
for a particular mobile or object.

## Licensing

CircleMUD is a derivative of DikuMUD and therefore comes with a license that
basically says you cannot make money running a mud using the CircleMUD server.

## Desire

For me, CircleMUD had limitations and I had a desire for something different.
In fact HolyQuest, using the CircleMUD server, was already different. In
HolyQuest a player can only be a warrior and human. This is a significant
departure from the mud norm of many classes like cleric, magician, wizard,
warrior, paladin, and thief, and many races like elf, human, orc, and others.

My desire for something different can be defined examining the things that, for
lack of a better phrase, I did not like about CircleMUD.

| CircleMUD | What I Wanted |
| --- | --- |
| Magic | No magic |
| Rooms, mobiles, and objects identified by a number | Rooms, mobiles, and objects identified by text |
| Zones or areas | Expand the world without regard to a numbering scheme |
| Bit vectors and codes, for example type flag 9 is armor | Use text, for example type: armor |
| License, cannot ever make any money | The opportunity to make a little money |

The only way for me to get what I wanted was to write my own mud server from
scratch. Now in the world of mudding this is definitely not a new idea. The
mudding world is strewn with the corpses of many mud servers that started out
with same goal as mine, to create something different. But nonetheless I
continued to pursue my desire for something different.

## OMugs

There was just one thing stopping me from writing my own mud server: I had no
idea how to code a telnet server application. A mud server must be able to open
and close internet connections as players log on and log off. So I was stuck
with an idea and no way to implement it.

One day as I was surfing the net in my quest for more knowledge about muds and
mud servers, I found a site that offered a subscription to a mudding magazine
and I promptly subscribed. In the first issue was an article on creating your
own mud from scratch accompanied by a code listing in C++. This was not by any
means a complete mud, just the part required to open and close internet
connections, allowing players to log on and off.

Eureka! This was exactly what I needed to start my own mud server codebase. I
studied the code and purchased a book, _Windows Sockets Network Programming_ by
Bob Quinn and Dave Shute, so I could decipher the WinSock commands. Having
obtained the knowledge necessary, I began work on my own mud server codebase.

The creators of most mud server codebases have given their creation a name.
Some examples are: CircleMUD, DikuMUD, Copper, Merc, and ROM. I needed a name.
One day on a trip to the Columbia, SC zoo my wife, Dawn, and I brainstormed
naming ideas for the new mud server. We made a list of words that might be used
to make an acronym. Some of the words on the list were: online, virtual, world,
adventure, game, server, text, text-based, internet, multi-user. We came up
with many acronyms. I decided on OtbMugs, Online Text-Based Multi-User Game
Server, and the codebase existed for several months as OtbMugs. But I came to
like the name less and less and decided to drop the `tb`, Text-Based, part
leaving OMugs, Online Multi-User Game Server, a name that Dawn had said she
liked on the day we were brainstorming names.

## Definition

OMugs is a combat oriented mud server and one of the goals of the game is to
gain levels. A player begins at level 1 and after some time playing the game,
the player will advance to level 2. This is called leveling and is rewarded by
giving the player a better chance of defeating an opponent that was previously
too difficult.

Socializing is an important aspect of any mud. If the game consisted of only
fighting, it could quickly become boring for many players. The ability to talk
to other players is very important to the success of a mud. OMugs supports
talking to people in-room where everyone in the room can hear what each other
is saying, as well as private messages, and messages that everyone playing the
game can hear.

Combat and socializing lead another very important factor, social standing or
status. A player's social standing on a mud is important. Just as a BMW can be
a status symbol, a rare and mighty sword can be a status symbol on a mud.
Levels can be viewed as ranks in the military. Although there is no chain of
command, a level 50 player knows more about the game than the new player
starting out at level 1.

## Combat Support

The following example of a play session serves to explain the OMugs
functionalities required to support combat.

A new player logs onto the game as Zeke. After looking around, he finds that he
is in a city with a sword in his hand and that he has no armor and no money. A
rat wanders by and Zeke decides to whack it. The rat is vanquished and Zeke
gets a rat ear and tail. After exploring the city for a while he discovers a
bake shop and sells the rat ear and tail to the baker for 3 silver pieces each.
Zeke heads for the Armor shop he discovered while exploring the city and buys a
leather tunic from armorer for 5 silver pieces. Zeke continues to reduce the rat
population and soon he sees a message saying that he has gained a level. Now
Zeke is level 2, has upgraded his armor, and decides to venture outside the city
walls. Upon exiting the city gates, he is jumped by a thief and fights for his
life. He is victorious and gets 10 gold pieces from the thief and a pair of
leather gloves. He puts the leather gloves on and continues his adventures.
This cycle of fighting, getting money, selling loot, upgrading his equipment,
and leveling will continue throughout Zeke's game life.

## Socializing

Players enjoy talking and playing together and OMugs provides several channels
of communication. Forms of communication include talking to other players in the
same room, in your group, to all players on the mud, or through a private
message to a single player. There are also non-verbal communications called
socials and emotes. Socials and emotes are only seen by players in the same
room. Some socials are: smile, wave, bow, and cheer. An emote allows a player
to type a message and have their name placed before the message. If Zeke typed
`emote scratches his head.`, the other players in the same room would see a
message saying `Zeke scratches his head.`

## Conclusion

OMugs, as of this writing, has 30 player commands, 7 administrator commands, and
9,534 lines of C++ code. There are many commands and features yet to be
implemented. This is just the beginning.

## Project Status

### Dates Server Components were Completed

| Component | Date |
| --- | --- |
| Advance | 02/25/2003 |
| Armor Class | 04/10/2003 |
| Assist | 11/27/2002 |
| Calendar | 01/08/2004 |
| Chat | 07/11/2003 |
| Color | 05/01/2002 |
| Consider | 10/09/2003 |
| Delete | 03/14/2003 |
| Destroy | 07/03/2002 |
| Drink | 07/09/2003 |
| Drop | 07/05/2002 |
| Edit Mobiles | 12/09/2003 |
| Edit Objects | 12/09/2003 |
| Emote | 06/26/2003 |
| Equipment | 08/28/2002 |
| Examine | 09/27/2002 |
| Flee | 11/15/2002 |
| Follow | 05/30/2002 |
| Give | 09/19/2002 |
| GoTo | 07/08/2002 |
| GoToArrive | 07/17/2002 |
| GoToDepart | 07/17/2002 |
| Group | 05/09/2002 |
| Gsay | 05/10/2002 |
| Hail | 06/02/2003 |
| Help | 04/08/2002 |
| Inventory | 07/03/2002 |
| Invisible | 10/14/2003 |
| Kill | 11/13/2002 |
| List | 03/11/2003 |
| Load Mobile | 10/11/2002 |
| Load Object | 07/03/2002 |
| Logon | 04/01/2002 |
| Look | 05/01/2002 |
| Money | 04/10/2002 |
| Motd | 03/27/2003 |
| Password | 03/03/2003 |
| Played | 12/06/2002 |
| Player file | 04/18/2002 |
| Quit | 04/01/2002 |
| Refresh | 06/25/2003 |
| Remove | 08/28/2002 |
| Restore | 12/10/2002 |
| RoomInfo | 07/10/2002 |
| Save | 07/17/2002 |
| Sell | 03/11/2003 |
| Show | 06/26/2003 |
| Skills | 05/08/2003 |
| Sleep | 07/12/2002 |
| Socials | 04/05/2002 |
| Spawn | 12/19/2002 |
| Stand | 05/03/2002 |
| Status | 04/04/2002 |
| Stop | 04/01/2002 |
| Tell | 04/02/2002 |
| Time | 12/06/2002 |
| Title | 04/30/2002 |
| Train | 04/15/2003 |
| Wake | 07/12/2002 |
| Wear | 08/28/2002 |
| Where Mobile | 12/03/2002 |
| Wield | 08/30/2002 |

### OMugs Done List

- 12/15/02 - Implement player levels
- 12/17/02 - Make mobiles fight back
- 12/18/02 - Assist command new functionality: allow player to control whether or not they can be assisted
- 12/18/02 - Look `<player>` command
- 12/18/02 - Look `<mobile>` command
- 12/19/02 - Enhance `where` command, add `where <object>` and `where <player>`
- 12/19/02 - Afk command, add afk player state, display on who listing
- 12/19/02 - Spawning mobiles now, needs work though
- 12/20/02 - Announce to room when mobile spawns
- 12/30/02 - Develop an event queue system, mob spawn event system
- 01/06/03 - Make mobiles move
- 01/08/03 - Make mobiles flagged as NoMove not move
- 01/15/03 - Create all spawn events, not just the first event and then skip out
- 01/30/03 - Disconnect players during logon if no response after a predefined number of ticks
- 01/31/03 - Advance a character to a given level, for example `advance kwam 18`
- 02/24/03 - Determine and log player's IP address
- 03/04/03 - If two players are fighting a mob and one player dies or flees, make the mob switch targets
- 03/05/03 - Allow player to change their password
- 03/07/03 - Allow player to delete themselves
- 03/13/03 - Shops
- 03/18/03 - Award loot to player(s) when mob dies
- 03/26/03 - Write help for kill command
- 03/27/03 - Implement MOTD
- 04/10/03 - Use armor in damage calculations
- 05/08/03 - Implement skills
- 05/12/03 - During a fight, show player and mobile health percentage
- 05/15/03 - When a player deletes themselves, delete PlayerObj and PlayerEqu
- 05/15/03 - Make mobs regen health; when full health remove dot notation and delete stats
- 05/22/03 - Make aggro mobs attack when player enters the room
- 05/22/03 - Add `all` or a count to sell command
- 05/28/03 - Get home directory instead of hard coding it
- 05/29/03 - Log when a player levels or is advanced a level or levels
- 05/29/03 - When creating a new player and the player is disconnected after entering their password, but they have not entered their sex, they can log back in, but sex is blank.
- 05/30/03 - Write Who Is Online program
- 06/03/03 - Added hail command
- 06/26/03 - Command to list all socials
