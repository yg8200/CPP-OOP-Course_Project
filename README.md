# Adventure Game

A two-player cooperative puzzle-adventure that runs in the terminal, written in C++20 as a second-year
object-oriented programming project. Both players share one keyboard and must work together through
three rooms full of keys, doors, switches, springs, bombs, pushable obstacles, dark areas and riddles.

<!-- Add a screenshot or GIF of the game here: ![gameplay](docs/gameplay.gif) -->

## Features

- **Two players on one keyboard**, each with their own lives and score
- **Puzzle elements:** keys and doors, switches that open tiles, springs that launch you,
  pushable obstacles (chains of them need more force), bombs with a countdown, torches for dark rooms
  and riddles that are picked at random from a text file
- **Data-driven levels:** rooms are plain text files, including which key opens which door and
  which switch controls which tile (see [Level format](#level-format))
- **Record and replay:** record a session and play it back deterministically, either watched
  or as a fast, silent self-check
- **Cross-platform I/O layer:** the same code runs on macOS/Linux (ANSI escape codes + `termios`)
  and on Windows (Win32 console API)

## Build and run

You need a C++20 compiler (Xcode command line tools on macOS: `xcode-select --install`).

**With make:**

```bash
make run
```

**With CMake:**

```bash
mkdir build && cd build
cmake ..
make
./Adventure_Project
```

Use a real terminal window that is at least **80 columns x 25 rows**. The game finds its `assets/`
folder by itself, so it can be started from any directory.

## Controls

| Action        | Player 1 (`$`) | Player 2 (`&`) |
|---------------|:--------------:|:--------------:|
| Up            | `W`            | `I`            |
| Right         | `D`            | `L`            |
| Down          | `X`            | `M`            |
| Left          | `A`            | `J`            |
| Stay          | `S`            | `K`            |
| Drop the item | `E`            | `O`            |

Main menu: `1` start, `8` instructions, `3` toggle colors, `9` exit.
In game: `ESC` pauses (then `ESC` resumes, `H` returns to the menu). `6` skips to the next room (a
testing shortcut).

## How to play

Both players must reach the door out of each room, and the game is won when they leave the last one.

| Symbol | Element | Behavior |
|:------:|---------|----------|
| `W` | Wall | Blocks movement |
| `*` | Obstacle | Can be pushed if you have enough force; a row of obstacles needs more |
| `#` | Spring | Walk into it to compress it, then it launches you the other way, faster the more it was compressed |
| `/` `\` | Switch | Toggles when stepped on; when all switches of a tile are on, the tile disappears |
| `K` | Key | Pick it up (one item at a time) to open its door |
| `1`-`9` | Door | Opens when all of its keys have been used; leads to the next room |
| `?` | Riddle | Right answer: +100 points. Wrong answer: -50 points and -1 life |
| `B` | Bomb | Pick it up and drop it; it explodes after a short countdown and destroys nearby walls, tiles, switches, obstacles and riddles |
| `!` | Torch | Lights up the area around you in dark rooms |

An explosion near a player costs 3 lives and sends them back to the room's starting position.
The game is lost when a player runs out of lives.

## Record and replay

```bash
./adventure_game -save          # play normally and record the session
./adventure_game -load          # watch the recording play back
./adventure_game -load -silent  # replay without drawing anything; prints a pass/fail line
```

A recording is made of two files that are written next to the game assets:

- `adv-world.steps`: the random seed, the number of steps, then one `iteration key` pair per line
- `adv-world.result`: one `iteration event_type parameter` line per event (bomb hit, riddle answered,
  game won/lost/interrupted)

Replay feeds the recorded keys back into the game using the same seed and checks that every event
happens again at the same moment. If anything differs, the replay reports that the recording is
compromised. Random numbers come from `std::mt19937`, so recordings are portable between platforms.

## Level format

Each room is a text file (`assets/layout_N.txt`):

1. **19 lines of map**, 80 columns wide, using the symbols above plus `$` and `&` for the start
   positions of the two players and `' '` for empty space.
2. **Configuration lines** that connect the elements. Coordinates are written as two digits of row
   followed by two digits of column (0-based):

   | Line | Meaning |
   |------|---------|
   | `S0266 T0809` | Switch at row 02, col 66 controls the tile at row 08, col 09 |
   | `K1752 D1062` | Key at row 17, col 52 is needed to open the door at row 10, col 62 |
   | `D0041 D0101` | Door at row 00, col 41 leads to room 1 (the door's target is written as `D` + room number) |
   | `L2100 L2100` | Where the lives/score panel is drawn |
   | `R....` | The room is dark |

A text editor numbers lines and columns from 1, while the game counts from 0, so subtract 1 from both.

Riddles live in `assets/riddlers.txt`: a question, four answers, the number of the correct one and a
separator line, repeated.

## Project structure

```
include/        headers, grouped like the sources
src/core/       Game, Room, Player and the keyboard/replay game modes
src/entities/   GameObject and its subclasses (Wall, Key, Door, Bomb, Spring, Switch, Tile, ...)
src/recording/  Steps and Results (record/replay files)
src/utils/      terminal I/O, Menu, Point
assets/         room layouts, screens, instructions, riddles
```

`GameObject` is the base class of everything on the map. `Room` owns the object grid, `Player`
reacts to what it walks into, and `Game` runs the loop. `GameFromKeyboard` and `GameFromTextFile`
subclass `Game` to record or replay a session.

## Authors

Yuval George and Noa Granot, second-year Computer Science.
