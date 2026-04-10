#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <chrono>
#include "Clue.h"
using namespace std;

namespace utility{
    class TreasureHunt{
    private:
        vector<string> mapGrid;
        vector<Clue> clues;
        int playerRow;
        int playerCol;
        int totalScore;
        bool gameOver;
        int skipCount = 0;          // Tracks "The Big Ticket" (Skips available)
        bool doublePoints = false;  // Tracks "x2 Bevo Bucks" (Next right answer is 2x)

        chrono::steady_clock::time_point startTime;  // TIMER VARIABLES
        chrono::steady_clock::time_point endTime;
        int timeLimitSeconds = 300; // Total time (Starting with 5 mins)

        void placeCluesOnMap();
        bool allCluesCompleted() const;

        bool squirrelFound = false; //for tracking the finding of the surprise waiting when the player finds the passage

    public:
        TreasureHunt();
        bool loadMap(const string& map);
        bool loadClues(const string& clues);
        void drawMap();
        void movePlayer(char direction, int steps);
        void triggerClue(char symbol);
        void startGame();
        void displayFinalResult();

        void promptSave();                        //Save Progress Function
        void saveGame(const string& filename);
        bool loadGame(const string& filename);    //Load Progress Function

        int findClueIndex(char symbol); 

        void handlePowerup(); //powerup function

        int getElapsedTime() const; // TIMER FUNCTIONS
        bool isTimeUp() const;
    };
}
