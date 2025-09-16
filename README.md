# Mastermind Game - ICT SBA

A console-based Mastermind game implementation in C++ for ICT School-Based Assessment.

## Features

- **Multiple Difficulty Levels**: Easy (3 digits), Medium (4 digits), Hard (5 digits)
- **Two Game Modes**: User vs Computer or User vs User
- **Save/Load System**: Continue your game later
- **Cheat Mode**: Toggle to see the secret code
- **Time Tracking**: See how long you took to solve the puzzle
- **Beginner-Friendly Code**: Simple, well-commented C++ implementation

## How to Compile

### Using clang++ (macOS/Linux):
```bash
clang++ -std=c++11 -Wall -o mastermind main.cpp
```

### Using g++ (Linux/Windows):
```bash
g++ -std=c++11 -Wall -o mastermind main.cpp
```

### Using Xcode (macOS):
1. Open the project in Xcode
2. Press `Cmd+R` or click the "Run" button
3. Xcode will compile and run automatically

## How to Run

After compilation, run the executable:

```bash
./mastermind
```

## How to Push to GitHub

1. **Initialize Git repository** (if not already done):
   ```bash
   git init
   ```

2. **Add files to staging**:
   ```bash
   git add .
   ```

3. **Commit your changes**:
   ```bash
   git commit -m "Initial commit: Mastermind game implementation"
   ```

4. **Connect to GitHub repository**:
   ```bash
   git remote add origin https://github.com/kl-charizard/mastermind-sba.git
   ```

5. **Push to GitHub**:
   ```bash
   git push -u origin main
   ```

## Game Rules

- Guess the secret code of digits 1-6
- **Red pegs**: Correct digit in correct position
- **White pegs**: Correct digit in wrong position
- You have 10 attempts to crack the code
- Use 'save' to save your progress, 'cheat' to toggle cheat mode, 'quit' to return to menu

## File Structure

```
mastermind-sba/
├── main.cpp              # Main game source code
├── mastermind_pseudocode.txt  # Pseudocode documentation
├── README.md             # This file
└── mastermind_save.txt   # Save file (created when you save a game)
```

## Requirements

- C++11 compatible compiler (clang++, g++, or Visual Studio)
- No external dependencies - uses only standard C++ libraries

## Author

Created by Kenny Lam for ICT School-Based Assessment.

---

**Note**: The save file `mastermind_save.txt` will be created in the same directory as the executable when you save a game.
