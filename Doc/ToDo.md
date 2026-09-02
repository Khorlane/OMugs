# OMugs To Do List

## Commands

- Light command
- Shout command
- Transfer command (admin)

## World

- Doors

## Connections

- Enable banning of IP addresses
- Add command to disconnect players
- Sometimes player is not there but the game thinks they are; connection is
  still there, no player on the other end. Kill the connection, and be sure the
  player file is set to `Online:No`.

## Mobiles

- Make `Mobile::CountMob` count wounded mobs
- Make wimpy mobs flee when almost dead

## Players

- When a player dies, reduce move points to zero
- Allow players to create a description of themselves
- Player CRUD dialog

## Objects

- Containers
- Clean up `pObject = new Object` code. What if `<object>.txt` does not exist
  for some reason? A pointer to object is still returned?

## Shops

- Add shop specific messages for buy / sell success / failure like
  `BuyNotExist`, `BuySuccess`, `BuyNotAfford`, `SellSuccess`, and
  `SellNotExist`

## Misc

- Externalize starting hit points, move points, starting room, greeting, etc.
  In other words, most if not all `Config.h` stuff
- Complete move points implementation
- Design and write weather system
- Design and write quest system
- Send message to all players when the watch changes
