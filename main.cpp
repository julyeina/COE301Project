#include <iostream>
#include "TreasureHunt.h"
using namespace std;
using namespace utility;

int main(){ 
    TreasureHunt game; 

    //ERROR CHECKING SCRIPTS    
    if (!game.loadMap("map.txt")){ 
        cout << "Error, cant load map" << endl; 
        return  1; 

    }

    if (!game.loadClues("clue.txt")){ 
        cout << "Error, cant load clues" << endl; 
        return 1; 
    }

    game.startGame(); 

    return 0; 
}