#include "TreasureHunt.h"
#include <iostream> 
#include <string> 
#include <fstream> 
#include <vector>

using namespace std; 

TreasureHunt :: TreasureHunt (){ 
    //sets all scores to 0 
    playerRow = 0; 
    playerCol = 0; 
    totalScore = 0; 
    gameover = false; 
}

//error checkpoint for loadMap
bool TreasureHunt:: loadMap (const string & map){ 
    // load map.txt, stores in string and vector

    ifstream infile(map); 
    
    if (!infile.is_Open()){ 
        cout << "cant open map.txt" << endl; 
        return false; 
    }
//????
    mapgrid.Clear(); 
    string line; 
    int startingcount = 0; 

    for (int row = 0; getline(infile, line); row++) {
    mapGrid.push_back(line);

    for (int col = 0; col < line.length(); col++) {
        if (line[col] == '@') {
            playerRow = row;
            playerCol = col;
            startCount++;
        }
    }
//Error checkpoint for starting position
    if (startingCount != 1) {
        cout << "map has more than one start position" << endl; 
        return false;
    }

    }

    return true; 
}
//include clues from clue.txt
bool TreasureHunt :: loadClues (cost string & clues){ 
    //load clue.txt, stores in vector and position

    ifstream infile(clues);

    //error checkpoint for opening clue file
    if (!infile.is_open()) {
        cout << "cant open clue file" << endl;
        return false;
    }
    clues.clear();

    return true; 
}

void TreasureHunt :: drawMap () const{ 
    //print map with updated player position

    cout << "Score: " << score << endl;

    for (int i = 0; i < mapGrid.size(); i++) {
        cout << mapGrid[i] << endl;
    }

    cout << endl;
}

void TreasureHunt:: movePlayer(char direction){ 
    // WASD movement and calls new edited map 

    int newRow = playerRow;
    int newCol = playerCol;

    if (direction == 'W' || direction == 'w') {
        newRow--;
    }
    else if (direction == 'S' || direction == 's') {
        newRow++;
    }
    else if (direction == 'A' || direction == 'a') {
        newCol--;
    }
    else if (direction == 'D' || direction == 'd') {
        newCol++;
    }
    else {
        cout << "Invalid input" << endl; 
        return;
    }

    if (newRow < 0 || newRow >= mapGrid.size() ||
        newCol < 0 || newCol >= mapGrid[newRow].length()) {
        cout << "moving outside map" << endl;
        return;
    }

    if (mapGrid[newRow][newCol] == '#') {
        cout << "hit a wall" << endl;
        return;
    }

}

void TreasureHunt :: triggerClue (char Symbol){ 
    // called when landmark symbol is landed on, asks question, changes points based on answer 


    int clueIndex = findClueIndex(symbol); //need to add this class in

    if (clueIndex == -1) {
        cout << "No clue found" << endl; 
        return;
    }

    if (clues[clueIndex].isCompleted()) {
        cout << "already completed" << endl;
        return;
    }

    cout << "landmark reached" << endl;
    cout << clues[clueIndex].getQuestion() << endl;

    string userAnswer;
    bool solved = false;

    //question/attempt logic

}

void TreasureHunt :: startGame () const { 
    //game logic loop, wins and losses

    char direction;
    while (!gameOver) {
        drawMap();

        cout << "game started";
        cin >> direction;
        cin.ignore();

        movePlayer(direction);

        if (allCluesCompleted()) {
            gameOver = true;
        }

        if (score < -10) {
            gameOver = true;
        }
    }

    displayFinalResult();
}

void displayFinalResult () const{ 
    //shows completion percent and calls win/loss logic 

    cout << "Final Score: " << score << endl;
}
