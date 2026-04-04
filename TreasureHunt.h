#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
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

    public:
        TreasureHunt();
        bool loadMap(const string& map);
        bool loadClues(const string& clues);
        void drawMap();
        void movePlayer(char direction);
        void triggerClue(char symbol);
        void startGame();
        void displayFinalResult();
    };
}