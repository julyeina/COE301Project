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

chrono::steady_clock::time_point startTime;  // TIMER VARIABLES
        chrono::steady_clock::time_point endTime;
        int timeLimitSeconds; // optional (0 = no limit)

        void placeCluesOnMap();
        bool allCluesCompleted() const;

    public:
        TreasureHunt();
        bool loadMap(const string& map);
        bool loadClues(const string& clues);
        void drawMap();
        void movePlayer(char direction, int steps);
        void triggerClue(char symbol);
        void startGame();
        void displayFinalResult();
        int findClueIndex(char symbol); 

        int getElapsedTime() const; // TIMER FUNCTIONS
        bool isTimeUp() const;
    };
}
