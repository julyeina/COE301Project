#include <iostream>
#include "TreasureHunt.h"
using namespace std;
using namespace utility;

int main(){ 
    TreasureHunt game; 

    //ERROR CHECKING SCRIPTS    
    if (!game.loadMap("map.txt")){ 
        cout << "Longhorn Alert: Couldn’t load the campus map! Check your file and try again." << endl; 
        return  1; 

    }

    if (!game.loadClues("clue.txt")){ 
        cout << "Longhorn Alert: Clues are missing from the Forty Acres! Fix the file and reload." << endl; 
        return 1; 
    }

    game.startGame(); 

    return 0; 
}
