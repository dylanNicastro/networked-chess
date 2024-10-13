# Networked Chess
This project is a very basic implementation of chess, intended to work between two separate program instances on Linux.

> [!CAUTION]
> The code includes headers only on Linux. It will not work correctly if you attempt to compile it on Windows!

## Features:
- Players take turns moving pieces

- Valid move detection

- Capturing pieces

## How to Play
1) Download the source code and compile it on a Linux distribution

2) One player launches the Server, and the other player launches the Client which will connect to the server.

3) Take turns making moves!

---

### Move Command:
Makes a move depending on the positions given.

```
/move <oldPosition><newPosition>[promotionPiece]
```
- `oldPosition`: The position of the piece to be moved, given as a letter and a number (e.g. "a4")
- `newPosition`: The position to move the piece to, given as a letter and a number (e.g. "b5")
- `promotionPiece`: Optional argument specifying what piece to promote a pawn to in the case it reaches the opposite end of the board. If this argument is supplied when it shouldn't be, the command is not executed.
    - Queen: `'q'`
    - Rook: `'r'`
    - Bishop: `'b'`
    - Knight: `'n'`

**Example Move**:
`/move a4b5`

**Example Promotion Move**:
`/move e7e8q`

---

### Forfeit Command:
Forfeits the game, effectively ending it. Takes no arguments.

```
/forfeit
```

---

### Chessboard Command:
Prints out the current state of the chessboard. Takes no arguments.

```
/chessboard
```

#### Example of Starting Board:
```
rnbqkbnr
pppppppp
........
........
........
........
PPPPPPPP
RNBQKBNR
```
Uppercase letters represent white pieces, and lowercase letters represent black pieces.

---

### Import Command:
Imports a game from a FEN string.

```
/import <fenString>
```
- `fenString`: [What is a FEN string?](https://www.chess.com/terms/fen-chess)

**Example Import**:
`/import rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b`

---

### Save Command:
Saves the current game state in `game_database.txt`.

```
/save <username>
```
- `username`: Cannot contains spaces. Used when loading the specific game later on.

**Example Save**:
`/save DylanNicastro`

---

### Load Command:
Loads a game state from `game_database.txt` based on the username given.

```
/load <username>
```
- `username`: Cannot contains spaces. Used to find the desired game.

**Example Load**:
`/load DylanNicastro`

(Will only work if `/save DylanNicastro` was used in the past)