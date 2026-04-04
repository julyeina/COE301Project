#include "TreasureHunt.h"
#include <iostream> 
#include <string> 
#include <fstream> 
#include <vector>

using namespace std; 

TreasureHunt :: TreasureHunt ()
{ 
    //sets all scores to 0 
    playerRow = 0; 
    playerCol = 0; 
    totalScore = 0; 
    gameOver = false; 
}

bool TreasureHunt:: loadMap (const string & map)
{ 
    // load map.txt, stores in string and vector

    ifstream infile(map); 
    
    if (!infile.is_Open()){ 
        cout << "cant open map.txt" << endl; 
        return false; 
    }

    mapGrid.Clear(); 
    string line; 
    int startCount = 0; 

    for (int row = 0; getline(infile, line); row++) {
    mapGrid.push_back(line);

    for (int col = 0; col < line.length(); col++) \
    {
        if (line[col] == '#') 
        {
            playerRow = row;
            playerCol = col;
            startCount++;
        }
    }

    if (startCount != 1) 
    {
        cout << "map has more than one start position" << endl; 
        return false;
    }

    }

    return true; 
}

bool TreasureHunt :: loadClues (cost string & clues)
{ 
    //load clue.txt, stores in vector and position

    ifstream infile(clueFile);

    if (!infile.is_open()) 
    {
        cout << "cant open clue file" << endl;
        return false;
    }
    clues.clear();

    return true; 
}

void TreasureHunt :: drawMap () const
{ 
    //print map with updated player position

    cout << "Score: " << totalScore << endl;

    for (int i = 0; i < mapGrid.size(); i++) 
    {
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
    else if (direction == 'D' || direction == 'd') 
    {
        newCol++;
    }else {
        cout << "Invalid input" << endl; 
        return;
    }

    if (newRow < 0 || newRow >= mapGrid.size() ||
        newCol < 0 || newCol >= mapGrid[newRow].length()) 
    {
        cout << "moving outside map" << endl;
        return;
    }

    if (mapGrid[newRow][newCol] == '#') 
    {
        cout << "hit a wall" << endl;
        return;
    }
    char symbol = mapGrid[newRow][newCol];
//move player
    mapGrid[playerRow][newCol] = '.';
    playerRow = newRow;
    playCol = newCol;
    mapGrid[playerRow][PlayerCol] = '#';
//trigger clue if its not empty space
    if(symbol != '.' && symbol != '#')
    {
        triggerClue(symbol);
    }
}

int TreasureHunt :: findClueIndex(char symbol)
{
    for(int i = 0; i < clues.size(); i++)
    {
        if(clues[i].getSymbol() == symbol)
        {
            return i;
        }
    }
}

void TreasureHunt :: triggerClue (char Symbol)
{ 
    // called when landmark symbol is landed on, asks question, changes points based on answer 


    int clueIndex = findClueIndex(symbol); //need to add this class in

    if (clueIndex == -1) 
    {
        cout << "No clue found" << endl; 
        return;
    }

    if (clues[clueIndex].isCompleted()) 
    {
        cout << "already completed" << endl;
        return;
    }

    cout << "landmark reached" << endl;

    bool correct = clues[clueIndex].ask();

    if(correct)
    {
        totalScore += clues[clueIndex].getPoints();
    }else{ 
        totalScore -= 5;
    }
}

bool TreasureHunt:: allCluesCompleted()
{
    for(int i = 0; i < clues.size(), i++)
    {
        if(!clues[i].isCompleted())
        {
            return false;
        }
    }
    return true;
}
    //question/attempt logic

void TreasureHunt :: startGame ()  
{ 
    //game logic loop, wins and losses

    char direction;
    while (!gameOver) 
    {
        drawMap();

        cout << "Game Started use WASD to move";
        cin >> direction;
        

        movePlayer(direction);
//win conditions 
        if (allCluesCompleted()) 
        {
            cout << "You completed all the clues!" << endl;
            gameOver = true;
        }
//lose coditions 
        if (score < -10) 
        {
            cout << "Loser! Fake UT student" << endl;
            gameOver = true;
        }
    }

    displayFinalResult();
}

void TreasureHunt :: displayFinalResult () const
{ 
    //shows completion percent and calls win/loss logic 
    int completed = 0;

    for(int i = 0; i < clue.size(); i++)
    {
        if(clues[i].iscompleted() completed++;
    }

    double percent = 0;
    if(clues.size() > 0)
    {
        percent = (double)completed / clues.size() * 100;
    }

    cout << "\nFinal Score: " << totalScore << endl;
    cout << "You completed " << percent << "% of the map" << endl;

    if(percent == 100)
    {
        cout << "Winner Winner chicken dinner!" << endl;
    }else{
        cout << "LOSER!" << endl;
    }
}
