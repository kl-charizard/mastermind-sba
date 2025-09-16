//
//  main.cpp
//  ICT-SBA-main
//
//  Created by Kenny Lam on 21/8/25.
//

// Standard libraries we need for this game
#include <iostream>  // for input/output (cin, cout)
#include <string>    // for string handling
#include <fstream>   // for file operations (save/load)
#include <cstdlib>   // for EXIT_SUCCESS
#include <ctime>     // for time functions

using namespace std;

// Game settings - these are like the rules of the game
int MAXATTEMPT = 10;                           // How many tries the player gets
const string SAVE_FILE = "mastermind_save.txt"; // Where we save the game

// This struct holds all the information about a game in progress
// Think of it like a save file that remembers everything about the current game
struct GameState {
    string secretCode;        // The code the player is trying to guess
    int codeLength = 4;       // How many digits in the code (3, 4, or 5)
    bool allowRepeat = false; // Can the code have repeated digits?
    bool cheat = false;       // Is cheat mode on? (shows the secret code)
    int maxAttempts = MAXATTEMPT; // How many tries the player gets
    int attemptsUsed = 0;     // How many tries the player has used so far
    bool userVsUser = false;  // Is this human vs human or human vs computer?
    int accumulatedSeconds = 0; // Time spent playing before current session
    bool inProgress = false;  // Is a game currently being played?
    time_t startTime;         // When did the current game session start?
};


// Shows the welcome screen when the game starts
void WelcomeMessage() {
    cout << "======================== \n";
    cout << "   MASTERMIND GAME \n";
    cout << "======================== \n";
    cout << "Welcome to the Mastermind Game!\n\n";
}

// Explains the rules to the player before they start playing
void PrintRules(int codeLength) {
    cout << "Game Rules:\n";
    cout << "- A secret code of " << codeLength << " digits (each 1-6).\n";
    cout << "- You have " << MAXATTEMPT << " tries to guess the code.\n";
    cout << "- Feedback: Red = correct digit & position; White = correct digit, wrong position.\n\n";
}

// Shows the main menu and asks the player what they want to do
void ShowMainMenu(bool cheatDefault) {
    cout << "Main Menu:\n";
    cout << "1) New Game\n";
    cout << "2) Load Game\n";
    cout << "3) Toggle Cheat Mode (Now: ";
    if (cheatDefault) {
        cout << "ON";
    } else {
        cout << "OFF";
    }
    cout << ")\n";
    cout << "4) Exit\n";
    cout << "Choose 1-4: ";
}

// Creates a random secret code for the computer to use
// If allowRepeat is true, digits can be repeated (like 1123)
// If allowRepeat is false, all digits must be different (like 1234)
string GenerateRandomCode(int length, bool allowRepeat) {
    char code[10]; // Array to hold the code (max 10 digits)
    int codeLength = 0; // How many digits we've added so far
    // Set up random number generator with current time as seed
    srand((unsigned int)time(nullptr));
    bool used[7] = {false, false, false, false, false, false, false}; // Track which digits 1-6 we've used
    for (int i = 0; i < length; i++) {
        int d = 0;
        if (allowRepeat) {
            // If repeats are allowed, just pick any digit 1-6
            d = 1 + rand() % 6;
        } else {
            // If no repeats, keep trying until we find an unused digit
            do {
                d = 1 + rand() % 6;
            } while (used[d]);
            used[d] = true; // Mark this digit as used
        }
        code[codeLength] = '0' + d; // Convert number to character and store in array
        codeLength++; // Move to next position
    }
    code[codeLength] = '\0'; // Add null terminator to make it a proper string
    return string(code); // Convert char array to string
}

// Checks if a code or guess is valid according to the game rules
// Returns true if it's valid, false if it's not
bool ValidateCodeOrGuess(const string &s, int length, bool allowRepeat) {
    // Count how many characters are in the string
    int size = 0;
    while (s[size] != '\0') { // Count until we reach the end
        size++;
    }
    if (size != length) return false; // Wrong length
    bool seen[7] = {false, false, false, false, false, false, false}; // Track which digits we've seen
    for (int i = 0; i < size; i++) {
        char c = s[i];
        if (c < '1' || c > '6') return false; // Not a valid digit (1-6)
        int d = c - '0'; // Convert character to number
        if (!allowRepeat) {
            if (seen[d]) return false; // This digit was already used
            seen[d] = true; // Mark this digit as seen
        }
    }
    return true; // All checks passed
}

// Compares the player's guess with the secret code and gives feedback
// red = how many digits are correct AND in the right position
// white = how many digits are correct but in the wrong position
void GetFeedback(const string &secret, const string &guess, int &red, int &white) {
    // Count how many characters are in the secret
    int n = 0;
    while (secret[n] != '\0') { // Count until we reach the end
        n++;
    }
    red = 0;  // Start counting red pegs
    white = 0; // Start counting white pegs
    int countSecret[7] = {0,0,0,0,0,0,0}; // Count how many times each digit appears in secret
    int countGuess[7] = {0,0,0,0,0,0,0};  // Count how many times each digit appears in guess
    
    // First pass: count red pegs (exact matches) and count all digits
    for (int i = 0; i < n; i++) {
        if (secret[i] == guess[i]) {
            red++; // Perfect match - this is a red peg
        } else {
            // Not a perfect match, but count the digits for white peg calculation
            countSecret[secret[i] - '0']++;
            countGuess[guess[i] - '0']++;
        }
    }
    
    // Second pass: count white pegs (correct digit, wrong position)
    for (int d = 1; d <= 6; d++) {
        if (countSecret[d] < countGuess[d]) {
            white += countSecret[d]; // Limited by how many are in the secret
        } else {
            white += countGuess[d];  // Limited by how many are in the guess
        }
    }
}

// Saves the current game to a file so the player can continue later
void SaveGame(const GameState &gs) {
    ofstream out(SAVE_FILE);
    if (!out) {
        cout << "Failed to open save file." << endl;
        return;
    }
    out << 1 << "\n"; // Save file version number
    out << gs.codeLength << "\n";
    if (gs.allowRepeat) { out << 1 << "\n"; } else { out << 0 << "\n"; } // Save as 1 or 0
    if (gs.cheat) { out << 1 << "\n"; } else { out << 0 << "\n"; }
    out << gs.maxAttempts << "\n";
    out << gs.attemptsUsed << "\n";
    if (gs.userVsUser) { out << 1 << "\n"; } else { out << 0 << "\n"; }
    out << gs.accumulatedSeconds << "\n";
    out << gs.secretCode << "\n";
    out.close();
    cout << "Game saved to '" << SAVE_FILE << "'.\n";
}

// Loads a saved game from the file
// Returns true if successful, false if there was a problem
bool LoadGame(GameState &gs) {
    ifstream in(SAVE_FILE);
    if (!in) {
        cout << "No save file found." << endl;
        return false;
    }
    int version = 0;
    in >> version;
    if (!in || version != 1) {
        cout << "Unsupported save file." << endl;
        return false;
    }
    int allow = 0, cheat = 0, user = 0; // Temporary variables to read 1/0 values
    in >> gs.codeLength;
    in >> allow;
    in >> cheat;
    in >> gs.maxAttempts;
    in >> gs.attemptsUsed;
    in >> user;
    in >> gs.accumulatedSeconds;
    in >> gs.secretCode;
    if (!in) {
        cout << "Corrupted save file." << endl;
        return false;
    }
    // Convert the 1/0 values back to true/false
    if (allow == 1) {
        gs.allowRepeat = true;
    } else {
        gs.allowRepeat = false;
    }
    if (cheat == 1) {
        gs.cheat = true;
    } else {
        gs.cheat = false;
    }
    if (user == 1) {
        gs.userVsUser = true;
    } else {
        gs.userVsUser = false;
    }
    gs.inProgress = true;
    gs.startTime = time(nullptr); // Reset the timer for this session
    cout << "Game loaded. Resuming...\n";
    return true;
}

// Converts the game settings into a nice difficulty name for display
string DifficultyName(int len, bool allowRepeat) {
    if (len == 3) return "Easy";
    if (len == 4) return "Medium";
    if (len == 5) {
        if (allowRepeat) {
            return "Hard (repeats)";
        } else {
            return "Hard";
        }
    }
    return "Custom";
}

// This is the main game loop where the player tries to guess the code
void PlayGame(GameState &gs) {
    gs.inProgress = true;
    gs.startTime = time(nullptr); // Start timing this game session
    PrintRules(gs.codeLength);
    cout << "Difficulty: " << DifficultyName(gs.codeLength, gs.allowRepeat) << "\n";
    cout << "Mode: ";
    if (gs.userVsUser) {
        cout << "User vs User";
    } else {
        cout << "User vs Computer";
    }
    cout << "\n";
    cout << "Attempts Allowed: " << gs.maxAttempts << "\n\n";

    bool won = false;
    // Keep playing until the player runs out of attempts
    while (gs.attemptsUsed < gs.maxAttempts) {
        if (gs.cheat) {
            cout << "[CHEAT] Secret: " << gs.secretCode << "\n"; // Show the secret if cheat is on
        }
        cout << "Enter your guess (" << gs.codeLength << " digits 1-6)";
        cout << ", or type 'save', 'cheat', or 'quit': ";
        string guess;
        cin >> guess;

        if (guess == "save") {
            // Save the game and go back to menu
            time_t now = time(nullptr);
            if (now >= gs.startTime) {
                gs.accumulatedSeconds += (int)(now - gs.startTime); // Add current session time
            }
            SaveGame(gs);
            cout << "Returning to menu...\n\n";
            return;
        }
        if (guess == "cheat") {
            gs.cheat = !gs.cheat; // Toggle cheat mode
            cout << "Cheat is now ";
            if (gs.cheat) {
                cout << "ON";
            } else {
                cout << "OFF";
            }
            cout << "\n";
            continue; // Go back to asking for a guess
        }
        if (guess == "quit") {
            cout << "Quit to menu without saving.\n\n";
            return;
        }

        // Check if the guess is valid
        if (!ValidateCodeOrGuess(guess, gs.codeLength, gs.allowRepeat)) {
            cout << "Invalid guess. Please enter exactly " << gs.codeLength << " digits (1-6)";
            if (gs.allowRepeat) {
                cout << " (repeats allowed).";
            } else {
                cout << " (no repeats).";
            }
            cout << "\n";
            continue; // Ask for another guess
        }

        gs.attemptsUsed++; // Count this attempt
        int red = 0;
        int white = 0;
        GetFeedback(gs.secretCode, guess, red, white); // Get feedback on the guess
        cout << "Red: " << red << ", White: " << white << "\n";

        if (red == gs.codeLength) {
            // Player won! All digits are correct and in the right position
            time_t now = time(nullptr);
            int totalSeconds = gs.accumulatedSeconds;
            if (now >= gs.startTime) {
                totalSeconds += (int)(now - gs.startTime); // Add current session time
            }
            cout << "You cracked the code!\n";
            cout << "Attempts used: " << gs.attemptsUsed << "\n";
            cout << "Time taken: " << totalSeconds << " seconds\n\n";
            won = true;
            gs.inProgress = false;
            break; // Exit the game loop
        } else {
            cout << "Attempts left: " << (gs.maxAttempts - gs.attemptsUsed) << "\n\n";
        }
    }

    // If the player didn't win and ran out of attempts
    if (!won && gs.attemptsUsed >= gs.maxAttempts) {
        cout << "You ran out of attempts. The code was: " << gs.secretCode << "\n\n";
        gs.inProgress = false;
    }
}

// Sets up a new game by asking the player for difficulty and mode settings
void NewGame(bool cheatDefault) {
    GameState gs;
    gs.cheat = cheatDefault;
    gs.maxAttempts = MAXATTEMPT;

    // Ask player to choose difficulty level
    cout << "Select Difficulty:\n";
    cout << "1) Easy (3 digits, no repeats)\n";
    cout << "2) Medium (4 digits, no repeats)\n";
    cout << "3) Hard (5 digits, repeats allowed)\n";
    cout << "Choose 1-3: ";
    string d;
    cin >> d;
    if (d == "1") {
        gs.codeLength = 3;
        gs.allowRepeat = false;
    } else if (d == "2") {
        gs.codeLength = 4;
        gs.allowRepeat = false;
    } else {
        gs.codeLength = 5;
        gs.allowRepeat = true;
    }

    // Ask player to choose game mode
    cout << "Mode:\n";
    cout << "1) User vs Computer (random code)\n";
    cout << "2) User vs User (manual code)\n";
    cout << "Choose 1-2: ";
    string m;
    cin >> m;
    if (m == "2") {
        gs.userVsUser = true;
    } else {
        gs.userVsUser = false;
    }

    // Set up the secret code based on the mode chosen
    if (gs.userVsUser) {
        // Human vs Human: ask Player 1 to enter a code for Player 2
        cout << "Enter the secret code for Player 2 (" << gs.codeLength << " digits 1-6)"
             << (gs.allowRepeat ? " (repeats allowed): " : " (no repeats): ");
        string code;
        cin >> code;
        while (!ValidateCodeOrGuess(code, gs.codeLength, gs.allowRepeat)) {
            cout << "Invalid code. Try again: ";
            cin >> code;
        }
        gs.secretCode = code;
        cout << string(50, '\n'); // Clear the screen so Player 2 can't see the code
    } else {
        // Human vs Computer: generate a random code
        gs.secretCode = GenerateRandomCode(gs.codeLength, gs.allowRepeat);
    }

    // Main game loop - keep playing until player wants to stop
    while (true) {
        gs.attemptsUsed = 0;        // Reset attempts for this round
        gs.accumulatedSeconds = 0;  // Reset time for this round
        PlayGame(gs);               // Play one game
        if (gs.inProgress) {
            // If we're here, the player saved or quit during the game
            break;
        }
        cout << "Play again with same settings? (y/n): ";
        string again; cin >> again;
        if (again != "y") {
            break; // Player wants to stop
        }
        // Set up a new secret code for the next round
        if (gs.userVsUser) {
            // Human vs Human: ask for a new code
            cout << "Enter a new secret code for Player 2: ";
            string code; cin >> code;
            while (!ValidateCodeOrGuess(code, gs.codeLength, gs.allowRepeat)) {
                cout << "Invalid code. Try again: ";
                cin >> code;
            }
            gs.secretCode = code;
            cout << string(50, '\n'); // Clear the screen
        } else {
            // Human vs Computer: generate a new random code
            gs.secretCode = GenerateRandomCode(gs.codeLength, gs.allowRepeat);
        }
    }
}

// This is where the program starts - the main function
int main() {
    bool cheatDefault = false; // Start with cheat mode off by default
    WelcomeMessage();          // Show the welcome screen

    // Main program loop - keep showing the menu until player chooses to exit
    while (true) {
        ShowMainMenu(cheatDefault);
        string choice; cin >> choice; cout << "\n";
        if (choice == "1") {
            // Player wants to start a new game
            NewGame(cheatDefault);
        } else if (choice == "2") {
            // Player wants to load a saved game
            GameState gs;
            if (LoadGame(gs)) {
                PlayGame(gs); // If load was successful, start playing
            }
        } else if (choice == "3") {
            // Player wants to toggle cheat mode for new games
            cheatDefault = !cheatDefault;
            cout << "Cheat default is now ";
            if (cheatDefault) {
                cout << "ON";
            } else {
                cout << "OFF";
            }
            cout << "\n\n";
        } else if (choice == "4") {
            // Player wants to exit the program
            cout << "Goodbye!\n";
            break; // Exit the main loop
        } else {
            // Player entered something invalid
            cout << "Invalid choice. Try again.\n\n";
        }
    }
    return EXIT_SUCCESS; // Tell the system the program ended successfully
}

