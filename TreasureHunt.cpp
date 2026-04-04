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

    timeLimitSeconds = 300; // Time limit of 5 minutes
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
        if (line[col] == '#') {
            playerRow = row;
            playerCol = col;
            startCount++;
        }
    }

    if (startCount != 1) {
        cout << "map has more than one start position" << endl; 
        return false;
    }
    }
    return true; 
}


bool TreasureHunt :: loadClues(const string& cluesFile){ 
    //load clue.txt, create obejct of Clue class for each landmarks and store objects in vector

    ifstream infile(cluesFile);

    if (!infile.is_open()) {
        cout << "cant open clue file" << endl;
        return false;
    }

    string line;
    while(getline(infile,line)){
        if(line.empty()){
            continue;
        }
        stringstream clueStream(line); 
        //seperate symbol, question, answer, attemp, point that was stored in one line from clue.txt
        //and store value in each variable
        string clueSymbol, clueQuestion, clueAns, clueAttempts, cluePoints;
        getline(clueStream, clueSymbol, '|');
        getline(clueStream, clueQuestion, '|');
        getline(clueStream, clueAns, '|');
        getline(clueStream, clueAttempts, '|');
        getline(clueStream, cluePoints, '|');

        char landmarkSymbol = clueSymbol[0]; //store first letter of word
        int maxAttempts = stoi(clueAttempts); //convert string into int type
        int points = stoi(cluePoints);

        Clue currentClue(landmarkSymbol,clueQuestion,clueAns,maxAttempts,points);
        clues.push_back(currentClue);
    }
    infile.close();
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

int TreasureHunt :: findClueIndex(char symbol){
    for (int i = 0; i< clues.size(); i++){
        if(clues[i].getSymbol() == symbol){
            return i;
        }
    }
    return -1;
}

void TreasureHunt :: triggerClue (char symbol){ 
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
    cout << clues[clueIndex].getQuestion() << endl;
    string userAnswer;
    int trials = 1;
    bool correct = false;
    Clue currentclue = clues[clueIndex];

    while(trials <= clues[clueIndex].getMaxAttempts() && correct == false){
        getline(cin, userAnswer);
        if(clues[clueIndex].checkAnswer(userAnswer)){
            correct = true;
            cout << "That's Correct!" << endl;
            totalScore += clues[clueIndex].getPoints();
            clues[clueIndex].setCompleted(true);
            cout << "Points earned: " << currentclue.getPoints() <<  " points." << endl;
        }else{ 
            trials++;
            cout << "Incorrect." << endl;
            //getCorrectAsnwer can be added here to display correct answer 
            if(trials <= currentclue.getMaxAttempts()){
                cout << "try again. " << endl;
            }
        }
    }
    if(correct == false){
        cout<< "Out of attempts" << endl;
        totalScore -= 5;
    }
    cout << "Your current total score is now" << totalScore << endl;
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
    startTime = chrono::steady_clock::now(); //START TIME
    char direction;
    while (!gameOver) 
    {
        drawMap();

    int elapsed = chrono::duration_cast<chrono::seconds>(
    chrono::steady_clock::now() - startTime).count();

    cout << "Time Elapsed: " << elapsed << " seconds" << endl;

    if (elapsed >= timeLimitSeconds) {
        cout << "Time is up!" << endl;
        gameOver = true;
        break;
            }

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

    endTime = chrono::steady_clock::now();  // STOP TIMER
    displayFinalResult();
    
        int totalTime = chrono::duration_cast<chrono::seconds>(
          endTime - startTime).count();

    cout << "Total Time Played: " << totalTime << " seconds" << endl;

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
