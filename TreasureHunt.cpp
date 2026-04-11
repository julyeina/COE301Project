//This file implements the core logic for the Treasure Hunt game
//it Features : laoding map and clue data, player movements and interactions, scoring/timing/game state, and saving/loading progress 


#include "TreasureHunt.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <random> 

// ANSI Color Codes
#define RESET   "\033[0m"
#define ORANGE "\033[38;5;202m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"

using namespace std;

namespace utility {
namespace {
//Removeing leading and trailing whitespace from a string and is ued when reading input files to clean up formating 
string trim(const string& value) {
    const size_t first = value.find_first_not_of(" \t\r\n");
    if (first == string::npos) {
        return "";
    }

    const size_t last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}
}
//The constructor initializes deafault game state values: players starts at (0,0), score starts at 0, time limit is 5 min
TreasureHunt::TreasureHunt() {
    playerRow = 0;
    playerCol = 0;
    totalScore = 0;
    gameOver = false;
    timeLimitSeconds = 300;

}

// loads map from file into a 2D grid and finds the player's start position
bool utility::TreasureHunt::loadMap(const string& mapFile) {
    ifstream infile(mapFile.c_str());
    if (!infile.is_open()) {
        cout << "Longhorn Alert: Couldn't load the Forty Acres map!" << endl;
        return false;
    }

    mapGrid.clear();
    string line;

    // Read the file line by line into the grid
    while (getline(infile, line)) {
        // Handle Windows-style carriage returns
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1);
        }
        mapGrid.push_back(line);
    }

    if (mapGrid.empty()) {
        cout << "Longhorn Error: The campus map is empty... that ain't right." << endl;
        return false;
    }

    // Find the '@' symbol in the map file to use as the starting position
    bool foundStart = false;
    for (size_t row = 0; row < mapGrid.size(); ++row) {
        for (size_t col = 0; col < mapGrid[row].size(); ++col) {
            // Check for the @ you already drew in the map.txt
            if (mapGrid[row][col] == '@') { 
                playerRow = static_cast<int>(row);
                playerCol = static_cast<int>(col);
                
                // Replace the starting '@' with a '.' so no ghost is left behind
                mapGrid[row][col] = '.'; 
                
                foundStart = true;
                break;
            }
        }
        if (foundStart) break;
    }

    // Error if the map file is missing the starting player symbol
    if (!foundStart) {
        cout << "Longhorn Error: No @ symbol found on the map!" << endl;
        return false;
    }

    return true;
}

//loads clues from clue file
bool utility::TreasureHunt::loadClues(const string& cluesFile) {
    ifstream infile(cluesFile.c_str());
    if (!infile.is_open()) {
        cout << "Longhorn Alert: Clues couldn't be found across the Forty Acres!" << endl;
        return false;
    }

    clues.clear();
    string line;

    while (getline(infile, line)) {
        if (trim(line).empty()) {
            continue;
        }

        stringstream clueStream(line);
        string clueSymbol;
        string clueQuestion;
        string clueAnswer;
        string clueFunFact;

        // Parsing the pipe-delimited line
        getline(clueStream, clueSymbol, '|');
        getline(clueStream, clueQuestion, '|');
        getline(clueStream, clueAnswer, '|');
        getline(clueStream, clueFunFact);

        clueSymbol = trim(clueSymbol);
        clueQuestion = trim(clueQuestion);
        clueAnswer = trim(clueAnswer);
        clueFunFact = trim(clueFunFact);

        // Cleanup trailing pipes if they exist
        if (!clueFunFact.empty() && clueFunFact[clueFunFact.size() - 1] == '|') {
            clueFunFact.erase(clueFunFact.size() - 1);
            clueFunFact = trim(clueFunFact);
        }

        if (clueSymbol.empty() || clueQuestion.empty() || clueAnswer.empty()) {
            cout << "Longhorn Warning: Skipped a bad clue line." << endl;
            continue;
        }

        // Store the clue in the map under its specific symbol
        // This allows multiple questions to live under one letter (like 'T')
        char sym = clueSymbol[0];
        clues[sym].push_back(Clue(sym, clueQuestion, clueAnswer, 2, 10, clueFunFact));
    }

    if (clues.empty()) {
        cout << "Longhorn Error: No clues loaded. Bevo is disappointed." << endl;
        return false;
    }

    // NOTE: Shuffling the whole list is removed because we now pick 
    // a random clue from the symbol's bucket inside triggerClue.

    return true;
}


//Displayes the current game map where "@" is the player  "#" are the walls "." are the tiles
void TreasureHunt::drawMap() {
    cout << "\n Longhorn Score: " << totalScore << endl;
    cout << "Move with W/A/S/D (Q to quit the Forty Acres): " << endl;

    for (size_t row = 0; row < mapGrid.size(); ++row) {
        for (size_t col = 0; col < mapGrid[row].size(); ++col) {
            if (static_cast<int>(row) == playerRow && static_cast<int>(col) == playerCol) {
                cout << '@';
            } else {
                cout << mapGrid[row][col];
            }
        }
        cout << endl;
    }
    cout << endl;
}
//Moves the player in a specifies direction for a specific number of steps
void utility::TreasureHunt::movePlayer(char direction, int steps) {
    const char move = static_cast<char>(tolower(static_cast<unsigned char>(direction)));

    if (move == 'q') {
        gameOver = true;
        return;
    }


    // Flag to track if we need to pause for a message
    bool shouldPause = false;

    if (direction == 'h' || direction == 'H') {
        cout << "Tick tock, tick tock, the Longhorn goes round the clock." << endl;
        shouldPause = true;
    }

    // Movement loop for sliding across multiple spaces
    else{
        for (int i = 0; i < steps; i++) {
        int newRow = playerRow;
        int newCol = playerCol;

        if (move == 'w') --newRow;
        else if (move == 's') ++newRow;
        else if (move == 'a') --newCol;
        else if (move == 'd') ++newCol;
        else {
            cout << "Invalid move, Longhorn. Use W, A, S, D, or Q." << endl;
            return;
        }

        // Check boundaries
        if (newRow < 0 || newRow >= static_cast<int>(mapGrid.size()) || 
            newCol < 0 || newCol >= static_cast<int>(mapGrid[newRow].size())) {
            cout << "You're leaving campus boundaries. Stay on the Forty Acres!" << endl;
            shouldPause = true;
            break; 
        }

        // Check for walls
        if (mapGrid[newRow][newCol] == '#') {
            cout << "You ran into a campus wall after " << i << " step(s). Watch where you're going!" << endl;
            shouldPause = true;
            break;
        }

        // Update player position
        playerRow = newRow;
        playerCol = newCol;
        }
    }

    // Final tile check after movement ends
    const char finalSymbol = mapGrid[playerRow][playerCol];

    // Trigger power-up if landed on exactly
    if (finalSymbol == '?') {
        handlePowerup();
        shouldPause = true;
    }
    // Trigger landmark if landed on exactly
    else if (finalSymbol >= 'A' && finalSymbol <= 'Z') {
        triggerClue(finalSymbol);
        shouldPause = true;
    }

    // Check for secret coordinates
    if (playerRow == 13 && playerCol == 36 && !squirrelFound) {
    cout << "Wait... Is that the legendary albino squirrel?!" << endl;
    cout << "The albino scored granted you mega aura. +50 points" << endl;
    totalScore += 50;
    squirrelFound = true;
    shouldPause = true;
    }


    // Pause so messages stay visible before map redraws
    if (shouldPause) {
        cout << "\n" << WHITE << "Press ENTER to continue..." << RESET;
        string pause;
        getline(cin, pause);
        
    }
}


//handles interactions when players land on a clue : it prompts user with questions, upadates score based on results, removes clue from map if completed
void utility::TreasureHunt::triggerClue(char symbol) {
    // 1. Check if we have any clues for this symbol in our map
    if (clues.find(symbol) == clues.end() || clues[symbol].empty()) {
        return;
    }

    // 2. To randomize, we pick a random index from the list of clues for this symbol
    // We filter for one that isn't completed yet
    vector<int> availableIndices;
    for (int i = 0; i < clues[symbol].size(); i++) {
        if (!clues[symbol][i].isCompleted()) {
            availableIndices.push_back(i);
        }
    }

    // If all clues for this specific symbol are done, just exit
    if (availableIndices.empty()) {
        cout << "You've already mastered this landmark!" << endl;
        mapGrid[playerRow][playerCol] = '.'; // Just in case it wasn't cleared
        return;
    }

    // Pick a random available clue
    mt19937 gen(static_cast<unsigned int>(time(0)));
    uniform_int_distribution<int> dist(0, availableIndices.size() - 1);
    int randomIndex = availableIndices[dist(gen)];
    
    // Create a reference to the active clue so the code below still works easily
    Clue& activeClue = clues[symbol][randomIndex];

    // Checking for The Big Ticket skip before the brain workout starts
    if (skipCount > 0) {
        cout << ORANGE << "\n[!] THE BIG TICKET DETECTED! " << WHITE << "Use a skip? (y/n): " << RESET;
        string input;
        getline(cin, input);
        if (!input.empty() && (input[0] == 'y' || input[0] == 'Y')) {
            cout << "Skipped! No points, but you saved your GPA." << endl;
            activeClue.setCompleted(true);
            mapGrid[playerRow][playerCol] = '.'; // Wipe it off the map
            skipCount--; // One less ticket in the pocket
            return; 
        }
    }

    cout << "\n Longhorn Clue " << activeClue.getSymbol() << endl;
    cout << activeClue.getQuestion() << endl;

    bool solved = false;
    for (int attempt = 1; attempt <= activeClue.getMaxAttempts(); ++attempt) {
        cout << "Attempt " << attempt << " of " << activeClue.getMaxAttempts() << ": ";

        string userAnswer;
        getline(cin, userAnswer);

        // if player answered correctly
        if (activeClue.checkAnswer(userAnswer)) {
            solved = true;
            activeClue.setCompleted(true);
            
            // Calculating the payout (with a possible Bevo Bucks multiplier)
            int pointsToGive = activeClue.getPoints();
            if (doublePoints) {
                cout << ORANGE << "BEVO BUCKS ACTIVATED! Doubling your loot!" << RESET << endl;
                pointsToGive *= 2;
                doublePoints = false; // Power-up used up!
            }

            totalScore += pointsToGive;
            
            // LANDMARK DISAPPEARS ONLY ON SUCCESS
            mapGrid[playerRow][playerCol] = '.';

            cout << "Correct! You earned " << pointsToGive << " Longhorn points!" << endl;
            int remaining = timeLimitSeconds - getElapsedTime();
            if (remaining < 0) remaining = 0;
            cout << "Time left: " << remaining << " seconds" << endl;
            
            if (!activeClue.getFunFact().empty()) {
                cout << activeClue.getFunFact() << endl;
            }
            break;
        }
        // if player answered incorrectly
        cout << "Not quite, Longhorn." << endl;
    }

    if (!solved) {
        totalScore -= 5;
        cout << "Out of attempts. Lost 5 Longhorn points. The landmark remains..." << endl;
        
        int remaining = timeLimitSeconds - getElapsedTime(); 
        if (remaining < 0) remaining = 0;
        cout << "Time remaining: " << remaining << " seconds" << endl;
    }

    cout << "Current Longhorn Score: " << totalScore << endl;
    promptSave();
}


bool utility::TreasureHunt::allCluesCompleted() const {
    // iterate through map entries
    for (auto it = clues.begin(); it != clues.end(); ++it) {
        // check each vector of clues for current symbol
        for (const auto& clue : it->second) {
            // return false if any clue remains unfinished
            if (!clue.isCompleted()) {
                return false;
            }
        }
    }
    // all clues across all symbols are done
    return true;
}


void TreasureHunt::handlePowerup() {
    // 1. Setup the Random Engine (MT19937) for the Bonus requirement
    mt19937 gen(static_cast<unsigned int>(time(0)));
    uniform_int_distribution<int> dist(1, 3); // Rolls a 1, 2, or 3

    int roll = dist(gen);

    cout << "\n" << ORANGE << "--- [ CAMPUS PERK ACTIVATED ] ---" << RESET << endl;

    // 2. Randomly assign one of the 3 power-ups
    if (roll == 1) {
        cout << WHITE << "You found " << ORANGE << "x2 BEVO BUCKS!" << RESET << endl;
        cout << "Your next correct answer is worth DOUBLE POINTS." << endl;
        doublePoints = true; 
    } 
    else if (roll == 2) {
        cout << WHITE << "You found " << ORANGE << "THE BIG TICKET!" << RESET << endl;
        cout << "You've earned a SKIP. Use it on any clue you don't know." << endl;
        skipCount++;
    } 
    else if (roll == 3) {
        // Name suggestion: "The Registrar's Grace Period"
        cout << WHITE << "You found " << ORANGE << "THE REGISTRAR'S GRACE PERIOD!" << RESET << endl;
        cout << "Your deadline was extended. +60 Seconds added immediately!" << endl;
        timeLimitSeconds += 60; // Adds the minute immediately
    }

    // 3. Important: Clear the '?' from the mapGrid so it's gone
    mapGrid[playerRow][playerCol] = '.'; 
}


void utility::TreasureHunt::startGame() {

    //Allows players to load a previously saved game when they start the game, giving them the option to continue from where they left off or start fresh. 
      char choice;
        while (true) {
            cout << "Pick up where you left off, Longhorn? (y/n): ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (choice == 'y' || choice == 'Y') {
                loadGame("save.txt");
                break;
            }
            else if (choice == 'n' || choice == 'N') {
                break;
            }
            else {
                cout << "Invalid input. Please enter 'y' or 'n'.\n";
            }
        }

        startTime = chrono::steady_clock::now();
   
    // Game Intro 
    cout << ORANGE << BOLD << "----------------------------------------------------" << RESET << endl;
    cout << WHITE << BOLD <<  "        QUIZ: SO YOU THINK YOU'RE A LONGHORN?       " << RESET << endl;
    cout << ORANGE << BOLD << "----------------------------------------------------" << RESET << endl;
    
    cout << WHITE << "Hello player! Welcome to: " << ORANGE << "So You Think You're a Longhorn!" << RESET << endl;
    cout << WHITE << "You are tasked with finding different " << ORANGE << "LANDMARKS" << WHITE << " on the map" << endl;
    cout << WHITE << "(represented by symbols and special characters). " << RESET << endl;
    cout << WHITE << "If you get the question right, the symbol will " << ORANGE << "DISAPPEAR," << RESET << endl;
    cout << WHITE << "indicating you mastered that location on the map." << endl;
    cout << WHITE << "If you get the question incorrect after 2 attempts," << endl;
    cout << WHITE << "don't worry navigate back to that location for another try with a " << endl;
    cout << ORANGE << "NEW" << WHITE << " question." << endl;
    cout << WHITE << "Power ups grant you " << ORANGE << "EXTRA TIME, DOUBLE POINTS," << WHITE << " or " << endl;
    cout << WHITE << "the option to " << ORANGE << "SKIP A CLUE." << RESET << endl;
    cout << WHITE << "You will only have " << ORANGE << "FIVE MINUTES" << WHITE << " to complete your quest...." << endl;
    cout << WHITE << "Unless you find a " << ORANGE << "POWER UP!" << RESET << endl;
    cout << WHITE << "By the end you'll find out how much of a " << ORANGE << "LONGHORN" << WHITE << " you are" << endl;
    cout << WHITE << "(we're going to " << ORANGE << "WEED YOU OUT" << WHITE << ")!" << RESET << endl << endl;

    cout << ORANGE << BOLD << "THE MISSION:" << RESET << endl;
    cout << WHITE << "Explore campus. Hit the landmarks. Answer the clues." << endl;
    cout << WHITE << "Be Aware: Each correct answer is 10 points, but an incorrect answer will cost you 5 points :< ." << endl;
    cout << WHITE << "P.S. there may or may not be a " << ORANGE << "HIDDEN PASSAGE" << WHITE << " somewhere..." << endl;
    cout << WHITE << "Enter h during any move for a " << ORANGE << "hint about the passage." << endl;
    cout << WHITE << "Your final percentage will reveal your " << ORANGE << BOLD << "TRUE STUDENT IDENTITY." << RESET << endl << endl;

    // GAME INSTRUCTION
    cout << ORANGE << BOLD << "[ HOW TO PLAY ]" << RESET << endl;
    cout << WHITE << "1. Move with " << ORANGE << "WASD" << WHITE << " and indicate the number of spaces" << endl;
    cout << WHITE << "   you would like to travel with a number. (ex: " << ORANGE << "w10" << WHITE << ")" << endl;
    cout << WHITE << "2. Step on " << ORANGE << "LANDMARK LETTERS" << WHITE << " to trigger questions." << endl;
    cout << WHITE << "3. Beat the " << ORANGE << "TIMER" << WHITE << " and don't let your " << ORANGE << "GPA" << WHITE << " drop too low!" << RESET << endl << endl;

    cout << WHITE << BOLD << "READY TO TEST YOUR SPIRIT? Press ENTER to begin..." << RESET;
    
    string dummy;
    getline(cin, dummy);


    while (!gameOver) {
        drawMap();

        if (isTimeUp()) {
            cout << "Time's up on the Forty Acres!" << endl;
            gameOver = true;
            break;
        }

        if (allCluesCompleted()) {
            cout << "\nALL CHALLENGES COMPLETE!" << endl;
            gameOver = true; // This breaks the while loop
        }

        cout << "Your move: ";
        char direction = '\0';
        int steps = 1;

        // input logic
        cin >> direction; // Grabs 'w', 'a', 's', or 'd'
        
        // If a number follows (like '10' in 'w10'), read it into steps
        if (cin.peek() != '\n' && isdigit(cin.peek())) {
            cin >> steps;
        }
        
        // Clear the buffer so the next loop is fresh
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        movePlayer(direction, steps);

        // Show timer after every move
        int remaining = timeLimitSeconds - getElapsedTime();
        if (remaining < 0) remaining = 0;

        cout << "Time remaining: " << remaining << " seconds" << endl;
        if (allCluesCompleted()) {
            cout << "You completed all the Longhorn challenges!" << endl;
            gameOver = true;
        }

    }

    endTime = chrono::steady_clock::now();
    displayFinalResult();

}

void utility::TreasureHunt::displayFinalResult() {
    int completed = 0;
    int totalCount = 0;

    // iterate through map categories to count clues
    for (auto it = clues.begin(); it != clues.end(); ++it) {
        // iterate through clues in each symbol's vector
        for (const auto& clue : it->second) {
            totalCount++; // count total questions in file
            if (clue.isCompleted()) {
                completed++; // count successful answers
            }
        }
    }

    double percent = 0.0;
    if (totalCount > 0) {

        const int TOTAL_LANDMARKS = 6;

        double percent = (static_cast<double>(completed) / static_cast<double>(TOTAL_LANDMARKS) * 100.0;
    }

    cout << "\nFinal Longhorn Score: " << totalScore << endl;
    cout << "Clues completed: " << completed << " / " << "6"  << endl;
    cout << "Completion: " << percent << "%" << endl;

    //buzzfeed-esque quiz answers
    if (percent == 100) {
        cout << "You know to much.... You're a TA." << endl;
    }
    else if (percent >= 80) {
        cout << "You got an 80, so you're a real Longhorn. B-." << endl;
    }
    else if (percent >= 60) {
        cout << "You must be a student athlete with that score :/" << endl;
    }
    else if (percent >= 40) {
        cout << "You got a 40 so you're from Texas State. Go study and party less." << endl;
    }
    else if (percent > 0) {
        cout << "AGGIE! You have one minute to get out of here." << endl;
    }
    else {
        cout << "You failed your quest... Try again!" << endl;
    }
    cout << "Time on the Forty Acres: " << getElapsedTime() << " seconds" << endl;
}


void TreasureHunt::promptSave()  // This ensures just saving the progress by saving the cordinates into a text file and then loading it back when the game is restarted.
{
    char choice;
    cout << "Save your Longhorn journey? (y/n): ";
    cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        saveGame("save.txt");
        cout << "Progress saved. Hook 'em!\n";
    }
}

bool utility::TreasureHunt::loadGame(const string& filename) // Restore game state from file
{
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cout << "No saved Longhorn journey found.\n";
        return false;
    }

    // Load player position and score
    inFile >> playerRow >> playerCol;
    inFile >> totalScore;

    int totalSavedClues;
    inFile >> totalSavedClues;

    // Restore completion status for multiple clues per symbol
    for (int i = 0; i < totalSavedClues; ++i) {
        char symbol;
        bool completed;
        string questionText;
        
        inFile >> symbol >> completed;
        getline(inFile >> ws, questionText); // Identify specific clue by question text

        // Check if symbol exists in map
        if (clues.find(symbol) != clues.end()) {
            // Find specific clue in vector matching the saved question
            for (auto& clue : clues[symbol]) {
                if (clue.getQuestion() == questionText) {
                    clue.setCompleted(completed);
                    break;
                }
            }
        }

        // If a landmark symbol is fully cleared, remove from map grid
        if (completed) {
            bool anyLeft = false;
            if (clues.find(symbol) != clues.end()) {
                for (const auto& c : clues[symbol]) {
                    if (!c.isCompleted()) {
                        anyLeft = true;
                        break;
                    }
                }
            }
            
            // Only turn symbol into path if all questions for that letter are done
            if (!anyLeft) {
                for (size_t r = 0; r < mapGrid.size(); ++r) {
                    for (size_t c = 0; c < mapGrid[r].size(); ++c) {
                        if (mapGrid[r][c] == symbol) {
                            mapGrid[r][c] = '.';
                        }
                    }
                }
            }
        }
    }

    cout << "Longhorn progress loaded successfully!\n";
    return true;
}

void utility::TreasureHunt::saveGame(const string& filename) // save current state
{
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cout << "Error saving your Longhorn progress.\n";
        return;
    }

    // Save player state
    outFile << playerRow << " " << playerCol << endl;
    outFile << totalScore << endl;

    // Count every individual clue in the map
    int totalClues = 0;
    for (auto it = clues.begin(); it != clues.end(); ++it) {
        totalClues += it->second.size();
    }
    
    outFile << totalClues << endl;

    // Iterate through map categories
    for (auto it = clues.begin(); it != clues.end(); ++it) {
        // Iterate through clues in current vector
        for (const auto& clue : it->second) {
            // Save symbol, completion status, and question text to identify it later
            outFile << clue.getSymbol() << " "
                    << clue.isCompleted() << " "
                    << clue.getQuestion() << endl;
        }
    }

    outFile.close();
}

//track time/counts seconds
int TreasureHunt::getElapsedTime() const {
    chrono::steady_clock::time_point finish = endTime;
    if (!gameOver) {
        finish = chrono::steady_clock::now();
    }

    return static_cast<int>(
        chrono::duration_cast<chrono::seconds>(finish - startTime).count());
}

bool TreasureHunt::isTimeUp() const {
    return timeLimitSeconds > 0 && getElapsedTime() >= timeLimitSeconds;
}

}
