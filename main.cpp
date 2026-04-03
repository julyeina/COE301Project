#include <iostream>
#include "TreasureHunt.h"

int main(){ 
    TreasureHunt game; 

    //ERROR CHECKING SCRIPTS    
    if (!game.loadmap("map.txt")){ 
        cout << "Error, cant load map" << endl; 
        return  1; 

    }

    if (!game.loadClues("clue.txt")){ 
        cout << "Error, cant load clues" << endl; 
        return 1; 
    }

    game.startgame(); 

    return 0; 
}