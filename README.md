# power4_evolved
An evolved C power four game

## Development

This game is developed in **C** using the following tools and libraries:

- **Compiler**: `clang` with standard C compilation flags (`-Wall -g -O0`)
- **Libraries**:
  - `pthread` - Used for multithreaded board operations (disarray feature with MATRIX board type)
  - `criterion` - Used for unit testing (optional, for test builds)
- **Build System**: `Make` - Standard Makefile for building executables

The project supports two board representation types:
- **MATRIX**: Traditional 2D array representation with multithreaded support
- **BITS**: Bit-packed representation for memory efficiency (limited to width < 16)

## Game Rules

### Standard Connect Four Rules

Players take turns dropping pieces into columns. Pieces fall to the lowest available position in the selected column. The first player to get **N pieces in a row** (horizontally, vertically, or diagonally) wins the game. The value of N (called "run") is configurable when starting the game.

### Evolved Features

This version includes two special moves that add strategic depth:

#### 1. **Disarray** (`^` command)
- **Effect**: Flips the board horizontally and applies gravity
- **How it works**:
  1. Each column is flipped vertically (top becomes bottom, bottom becomes top)
  2. All pieces then fall due to gravity to fill empty spaces below
- **Usage**: Type `^` during your turn instead of a column number
- **Technical note**: For MATRIX board type, this operation uses multithreading to process columns in parallel

#### 2. **Offset** (`!` command)
- **Effect**: Takes back the last move from both players
- **How it works**:
  1. Removes the most recently placed piece from both Black and White
  2. Pieces above the removed positions fall down due to gravity
  3. Turn alternates to the previous player
- **Usage**: Type `!` during your turn instead of a column number
- **Limitation**: Can only be used if both players have made at least one move

### Game Commands

During gameplay, you can:
- Enter a **column number** (0-9, A-Z, a-z) to drop a piece in that column
- Enter `^` to perform a **disarray** move
- Enter `!` to perform an **offset** move (undo last move from both players)

## Building and Running

### Make Commands

- **`make play`** - Builds the main game executable (`play`)
- **`make test`** - Builds the test executable (`test`) with Criterion testing framework
- **`make clean`** - Removes all build artifacts (executables, object files, debug symbols)

### Running the Game

After building with `make play`, run the game with the following command-line arguments:

```bash
./play -h <height> -w <width> -r <run> -b
```

or

```bash
./play -h <height> -w <width> -r <run> -m
```

**Parameters:**
- `-h <height>` - Board height (number of rows)
- `-w <width>` - Board width (number of columns)
- `-r <run>` - Number of pieces in a row required to win (must be ≤ max(width, height))
- `-b` - Use BITS board representation (memory-efficient, width must be < 16)
- `-m` - Use MATRIX board representation (supports multithreading for disarray)

**Example:**
```bash
./play -h 6 -w 7 -r 4 -m
```

This starts a 6x7 board game where players need 4 in a row to win, using the MATRIX representation.

### Gameplay

- Black player goes first
- Players alternate turns
- The game displays the board after each move
- The game ends when a player wins or the board is full (draw)
