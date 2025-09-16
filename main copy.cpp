//
//  main.cpp
//  ICT-SBA-main
//
//  Created by Kenny Lam on 21/8/25.
//

#include <iostream>

using namespace std;

int MAXATTEMPT = 10;

void WelcomeMessage() {
    cout << "======================== \n";
    cout << "   🎮 MASTERMIND GAME \n";
    cout << "======================== \n";
    cout << "Welcome to the Mastermind Game! \n";
}

void MainMenu() {
    cout << "Game Rules:\n";
    cout << "1. A secret code of 4 digits (1-6) is generated. \n";
    cout << "2. You have 10 tries to guess the code. \n";
    cout << "3. Feedback after each guess: \n - Red Peg: Correct digit in correct position \n - White Peg: Correct digit in wrong position \n";
    cout << "Start? (y/n)";
}

void EndMessage() {
    
}

void Game() {
    
}

int main() {
    string start;
    WelcomeMessage();
    MainMenu();
    cin >> start;
    while (start != "n") {
        Game();
        MainMenu();
        cin >> start;
        
        
    }
    return EXIT_SUCCESS;
}
