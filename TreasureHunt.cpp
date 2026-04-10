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

//loads map from file into a 2D grid and finds the first walkable tile for the player
bool TreasureHunt::loadMap(const string& mapFile) {
    ifstream infile(mapFile.c_str());
    if (!infile.is_open()) {
        cout << "Longhorn Alert: Couldn't load the Forty Acres map!" << endl;
        return false;
    }

    mapGrid.clear();
    string line;

    while (getline(infile, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1);
        }
        mapGrid.push_back(line);
    }

    if (mapGrid.empty()) {
        cout << "Longhorn Error: The campus map is empty... that ain't right." << endl;
        return false;
    }
//Find the first '.' tile to use as the player's starting position
    bool foundStart = false;
    for (size_t row = 0; row < mapGrid.size(); ++row) {
        for (size_t col = 0; col < mapGrid[row].size(); ++col) {
            if (mapGrid[row][col] == '.') {
                playerRow = static_cast<int>(row);
                playerCol = static_cast<int>(col);
                foundStart = true;
                break;
            }
        }
        if (foundStart) {
            break;
        }
    }

    if (!foundStart) {
        cout << "Longhorn Error: No walkable paths on campus!" << endl;
        return false;
    }

    return true;
}
//loads clues from clue file
bool TreasureHunt::loadClues(const string& cluesFile) {
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

        getline(clueStream, clueSymbol, '|');
        getline(clueStream, clueQuestion, '|');
        getline(clueStream, clueAnswer, '|');
        getline(clueStream, clueFunFact);

        clueSymbol = trim(clueSymbol);
        clueQuestion = trim(clueQuestion);
        clueAnswer = trim(clueAnswer);
        clueFunFact = trim(clueFunFact);

        if (!clueFunFact.empty() && clueFunFact[clueFunFact.size() - 1] == '|') {
            clueFunFact.erase(clueFunFact.size() - 1);
            clueFunFact = trim(clueFunFact);
        }

        if (clueSymbol.empty() || clueQuestion.empty() || clueAnswer.empty()) {
            cout << "Longhorn Warning: Skipped a bad clue line." << endl;
            continue;
        }

        clues.push_back(Clue(clueSymbol[0], clueQuestion, clueAnswer, 2, 10, clueFunFact));
    }

    if (clues.empty()) {
        cout << "Longhorn Error: No clues loaded. Bevo is disappointed." << endl;
        return false;
    }

    //Bonus: Randomized Clue Order
    //created a "seed" using the current time so every game is different
    unsigned int seed = static_cast<unsigned int>(time(0));

    //initialized the random generator
    mt19937 generator(seed);

    //shuffled the entire vector of clue objects which reorders the landmarks in our list
    shuffle(clues.begin(), clues.end(), generator);

    placeCluesOnMap();
    return true;
}
//places clues randomly across walkable tiles and avoids placing a clue on the player's starting position
void TreasureHunt::placeCluesOnMap() {
    vector<pair<int, int> > openTiles;

    for (size_t row = 0; row < mapGrid.size(); ++row) {
        for (size_t col = 0; col < mapGrid[row].size(); ++col) {
            if (mapGrid[row][col] == '.' &&
                !(static_cast<int>(row) == playerRow && static_cast<int>(col) == playerCol)) {
                openTiles.push_back(make_pair(static_cast<int>(row), static_cast<int>(col)));
            }
        }
    }

    if (openTiles.size() < clues.size()) {
        cout << "Longhorn Warning: Not enough space on campus for all clues." << endl;
    }

    vector<bool> used(openTiles.size(), false);

    for (size_t i = 0; i < clues.size() && i < openTiles.size(); ++i) {
        size_t index = ((i + 1) * openTiles.size()) / (clues.size() + 1);
        if (index >= openTiles.size()) {
            index = openTiles.size() - 1;
        }

        while (index < used.size() && used[index]) {
            ++index;
        }
        if (index >= used.size()) {
            index = 0;
            while (index < used.size() && used[index]) {
                ++index;
            }
        }
        if (index >= used.size()) {
            break;
        }

        const int row = openTiles[index].first;
        const int col = openTiles[index].second;
        mapGrid[row][col] = clues[i].getSymbol();
        used[index] = true;
    }
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
    if (playerRow == 15 && playerCol == 38 && !squirrelFound) {
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


int TreasureHunt::findClueIndex(char symbol) {
    for (size_t i = 0; i < clues.size(); ++i) {
        if (clues[i].getSymbol() == symbol) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

//handles interactions when players land on a clue : it prompts user with questions, upadates score based on results, removes clue from map if completed
void utility::TreasureHunt::triggerClue(char symbol) {
    const int clueIndex = findClueIndex(symbol);
    if (clueIndex == -1) {
        return;
    }

    if (clues[clueIndex].isCompleted()) {
        return;
    }

    // Checking for The Big Ticket skip before the brain workout starts
    if (skipCount > 0) {
        cout << ORANGE << "\n[!] THE BIG TICKET DETECTED! " << WHITE << "Use a skip? (y/n): " << RESET;
        string input;
        getline(cin, input);
        if (!input.empty() && (input[0] == 'y' || input[0] == 'Y')) {
            cout << "Skipped! No points, but you saved your GPA." << endl;
            clues[clueIndex].setCompleted(true);
            mapGrid[playerRow][playerCol] = '.'; // Wipe it off the map
            skipCount--; // One less ticket in the pocket
            return; 
        }
    }

    cout << "\n Longhorn Clue " << clues[clueIndex].getSymbol() << endl;
    cout << clues[clueIndex].getQuestion() << endl;

    //when player answers a 'clue'
    bool solved = false;
    for (int attempt = 1; attempt <= clues[clueIndex].getMaxAttempts(); ++attempt) {
        cout << "Attempt " << attempt << " of " << clues[clueIndex].getMaxAttempts() << ": ";

        string userAnswer;
        getline(cin, userAnswer);

        //if player answered correctly
        if (clues[clueIndex].checkAnswer(userAnswer)) {
            solved = true;
            clues[clueIndex].setCompleted(true);
            
            // Calculating the payout (with a possible Bevo Bucks multiplier)
            int pointsToGive = clues[clueIndex].getPoints();
            if (doublePoints) {
                cout << ORANGE << "BEVO BUCKS ACTIVATED! Doubling your loot!" << RESET << endl;
                pointsToGive *= 2;
                doublePoints = false; // Power-up used up!
            }

            totalScore += pointsToGive;
            mapGrid[playerRow][playerCol] = '.';

            cout << "Correct! You earned " << pointsToGive << "  Longhorn points!" << endl;
            int remaining = timeLimitSeconds - getElapsedTime();
            if (remaining < 0) remaining = 0;
            cout << "Time left: " << remaining << " seconds" << endl;
            
            if (!clues[clueIndex].getFunFact().empty()) {
                cout << clues[clueIndex].getFunFact() << endl;
            }
            break;
        }
        //if player answered incorrectlly
        cout << "Not quite, Longhorn." << endl;
    }

    if (!solved) {
        totalScore -= 5;
        cout << "Out of attempts. Lost 5 Longhorn points." << endl;
        
        int remaining = timeLimitSeconds - getElapsedTime(); //Countdown after getting question right or wrong
        if (remaining < 0) remaining = 0;
        cout << "Time remaining: "
         << remaining
         << " seconds" << endl;
    }

    cout << "Current Longhorn Score: " << totalScore << endl;
    promptSave();
}


bool TreasureHunt::allCluesCompleted() const {
    for (size_t i = 0; i < clues.size(); ++i) {
        if (!clues[i].isCompleted()) {
            return false;
        }
    }

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

    cout << "Pick up where you left off, Longhorn? (y/n): "; //Allows players to load a previously saved game when they start the game, giving them the option to continue from where they left off or start fresh.
    char choice;
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (choice == 'y' || choice == 'Y') {
        loadGame("save.txt");
    }
    startTime = chrono::steady_clock::now();
    
    // Game Intro 
    cout << ORANGE << BOLD << "----------------------------------------------------" << RESET << endl;
    cout << WHITE << BOLD <<  "        QUIZ: SO YOU THINK YOU'RE A LONGHORN?       " << RESET << endl;
    cout << ORANGE << BOLD << "----------------------------------------------------" << RESET << endl;
    
    cout << WHITE << "Hello player! Welcome to: " << ORANGE << "So You Think You're a Longhorn!" << RESET << endl;
    cout << WHITE << "You are tasked with finding different " << ORANGE << "LANDMARKS" << WHITE << " on the map" << endl;
    cout << WHITE << "(represented by symbols and special characters). " << RESET << endl;
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

        if (allCluesCompleted()) {
            cout << "You completed all the Longhorn challenges!" << endl;
            gameOver = true;
        }

    }

    endTime = chrono::steady_clock::now();
    displayFinalResult();

}


void TreasureHunt::displayFinalResult() {
    int completed = 0;
    for (size_t i = 0; i < clues.size(); ++i) {
        if (clues[i].isCompleted()) {
            ++completed;
        }
    }

    double percent = 0.0;
    if (!clues.empty()) {
        percent = (static_cast<double>(completed) / static_cast<double>(clues.size())) * 100.0;
    }

    cout << "\nFinal Longhorn Score: " << totalScore << endl;
    cout << "Clues completed: " << completed << " / " << clues.size() << endl;
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
    cout << "You got a 40% so you're from Texas State. Go study and party less." << endl;
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

bool TreasureHunt::loadGame(const string& filename) // Ensures that the game can be loaded back by reading the cordinates and clues completed from the text file and then restoring the game state accordingly
{
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cout << "No saved Longhorn journey found.\n";
        return false;
    }

    // Load player position and score
    inFile >> playerRow >> playerCol;
    inFile >> totalScore;

    int clueCount;
    inFile >> clueCount;

    // Restore which clues were completed
    for (int i = 0; i < clueCount; ++i) {
        char symbol;
        bool completed;
        inFile >> symbol >> completed;

        int index = findClueIndex(symbol);
        if (index == -1) {
            continue;
        }

        clues[index].setCompleted(completed);

        // If a clue was already completed, remove it from the map
        if (completed) {
            for (size_t r = 0; r < mapGrid.size(); ++r) {
                for (size_t c = 0; c < mapGrid[r].size(); ++c) {
                    if (mapGrid[r][c] == symbol) {
                        mapGrid[r][c] = '.';
                    }
                }
            }
        }
    }

    cout << "Longhorn progress loaded successfully!\n";
    return true;
}

void TreasureHunt::saveGame(const string& filename)
{
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cout << "Error saving your Longhorn progress.\n";
        return;
    }

    // Save player state
    outFile << playerRow << " " << playerCol << endl;
    outFile << totalScore << endl;

    // Save clue completion state
    outFile << clues.size() << endl;
    for (int i = 0; i < clues.size(); i++) {
        outFile << clues[i].getSymbol() << " "
            << clues[i].isCompleted() << endl;
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
