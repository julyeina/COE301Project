#include "TreasureHunt.h"

#include <cctype>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

namespace utility {
namespace {
string trim(const string& value) {
    const size_t first = value.find_first_not_of(" \t\r\n");
    if (first == string::npos) {
        return "";
    }

    const size_t last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}
}

TreasureHunt::TreasureHunt() {
    playerRow = 0;
    playerCol = 0;
    totalScore = 0;
    gameOver = false;
    timeLimitSeconds = 300;

}

bool TreasureHunt::loadMap(const string& mapFile) {
    ifstream infile(mapFile.c_str());
    if (!infile.is_open()) {
        cout << "Error: could not open map file." << endl;
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
        cout << "Error: map file is empty." << endl;
        return false;
    }

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
        cout << "Error: map does not contain any walkable tiles." << endl;
        return false;
    }

    return true;
}

bool TreasureHunt::loadClues(const string& cluesFile) {
    ifstream infile(cluesFile.c_str());
    if (!infile.is_open()) {
        cout << "Error: could not open clue file." << endl;
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
            cout << "Warning: skipped an invalid clue line." << endl;
            continue;
        }

        clues.push_back(Clue(clueSymbol[0], clueQuestion, clueAnswer, 2, 10, clueFunFact));
    }

    if (clues.empty()) {
        cout << "Error: no clues were loaded." << endl;
        return false;
    }

    placeCluesOnMap();
    return true;
}

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
        cout << "Warning: not enough open spaces for every clue." << endl;
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

void TreasureHunt::drawMap() {
    cout << "\nScore: " << totalScore << endl;
    cout << "Use W A S D to move. Enter Q to quit." << endl;

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

void TreasureHunt::movePlayer(char direction) {
    const char move = static_cast<char>(tolower(static_cast<unsigned char>(direction)));

    if (move == 'q') {
        gameOver = true;
        return;
    }

    int newRow = playerRow;
    int newCol = playerCol;

    if (move == 'w') {
        --newRow;
    } else if (move == 's') {
        ++newRow;
    } else if (move == 'a') {
        --newCol;
    } else if (move == 'd') {
        ++newCol;
    } else {
        cout << "Invalid input. Use W, A, S, D, or Q." << endl;
        return;
    }

    if (newRow < 0 || newRow >= static_cast<int>(mapGrid.size())) {
        cout << "You cannot move outside the map." << endl;
        return;
    }

    if (newCol < 0 || newCol >= static_cast<int>(mapGrid[newRow].size())) {
        cout << "You cannot move outside the map." << endl;
        return;
    }

    if (mapGrid[newRow][newCol] == '#') {
        cout << "You hit a wall." << endl;
        return;
    }

    playerRow = newRow;
    playerCol = newCol;

    const char symbol = mapGrid[playerRow][playerCol];
    if (symbol != '.') {
        triggerClue(symbol);
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

void TreasureHunt::triggerClue(char symbol) {
    const int clueIndex = findClueIndex(symbol);
    if (clueIndex == -1) {
        return;
    }

    if (clues[clueIndex].isCompleted()) {
        return;
    }

    cout << "\nClue " << clues[clueIndex].getSymbol() << endl;
    cout << clues[clueIndex].getQuestion() << endl;

    bool solved = false;
    for (int attempt = 1; attempt <= clues[clueIndex].getMaxAttempts(); ++attempt) {
        cout << "Attempt " << attempt << " of " << clues[clueIndex].getMaxAttempts() << ": ";

        string userAnswer;
        getline(cin, userAnswer);

        if (clues[clueIndex].checkAnswer(userAnswer)) {
            solved = true;
            clues[clueIndex].setCompleted(true);
            totalScore += clues[clueIndex].getPoints();
            mapGrid[playerRow][playerCol] = '.';

            cout << "Correct! You earned " << clues[clueIndex].getPoints() << " points." << endl;
            int remaining = timeLimitSeconds - getElapsedTime();
            if (remaining < 0) remaining = 0;
            cout << "Time left: " << remaining << " seconds" << endl;
            
            if (!clues[clueIndex].getFunFact().empty()) {
                cout << clues[clueIndex].getFunFact() << endl;
            }
            break;
        }

        cout << "Incorrect." << endl;
    }

    if (!solved) {
        totalScore -= 5;
        cout << "Out of attempts. 5 points were deducted." << endl;
        
        int remaining = timeLimitSeconds - getElapsedTime(); //Countdown after getting question right or wrong
        if (remaining < 0) remaining = 0;
        cout << "Time remaining: "
         << remaining
         << " seconds" << endl;

    }

    cout << "Current score: " << totalScore << endl;
}

bool TreasureHunt::allCluesCompleted() const {
    for (size_t i = 0; i < clues.size(); ++i) {
        if (!clues[i].isCompleted()) {
            return false;
        }
    }

    return true;
}

void TreasureHunt::startGame() {
    startTime = chrono::steady_clock::now();

    while (!gameOver) {
        drawMap();

        if (isTimeUp()) {
            cout << "Time is up!" << endl;
            gameOver = true;
            break;
        }

        cout << "Move: ";
        char direction = '\0';
        cin >> direction;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        movePlayer(direction);

        if (allCluesCompleted()) {
            cout << "You completed all of the clues." << endl;
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

    cout << "\nFinal Score: " << totalScore << endl;
    cout << "Clues completed: " << completed << " / " << clues.size() << endl;
    cout << "Completion: " << percent << "%" << endl;
    cout << "Time played: " << getElapsedTime() << " seconds" << endl;
}

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
