#include <iostream> 
#include <string> 
#include <fstream> 
#include <vector>
#include "TreasureHunt.h"

using namespace std; 

TreasureHunt:: TreasureHunt (){ 
    //sets all scores to 0 
}

bool TreasureHunt:: loadMap (const string & map){ 
    // load map.txt, stores in string and vector

    return true; 
}

bool TreasureHunt :: loadClues (cost string & clues){ 
    //load clue.txt, stores in vector and position
    return true; 
}

void TreasureHunt :: drawMap () const{ 
    //print map with updated player position
}

void TreasureHunt:: movePlayer(char direction){ 
    // WASD movement and calls new edited map 
}

void TreasureHunt :: triggerClue (char Symbol){ 
    // called when landmark symbol is landed on, asks question, changes points based on answer 
}

void TreasureHunt :: startGame () const{ 
    //game logic loop, wins and losses
}

void displayFinalResult () const{ 
    //shows completion percent and calls win/loss logic 
}