Class required: TreasureHunt and Clue

Functions required for TreasureHunt class:
  loadMap()  
  loadClues()
  drawMap()
  movePlayer()
  triggerClue()
  startGame()
  displayFinalResult()

Functions suggested for Clue class:
  getSymbol()
  getQuestion()
  getPoints()
  checkAnswer()
  and more

main.cpp - run and start the program
map.txt - map input and needed for treasurehunt and laodmap() function
clues.txt - clue input
Clue.h - declare constructor, getter/bool functions
Clue.cpp 
TreasureHunt.h - declare functions 
TreasureHunt.cpp

Bonus implementations:
• Randomized clue order ** ask
• Timer system
• Save/load progress
• Larger Map with decorations
• Hidden secret location
• Display a “Fun Fact” message after a correct answer

Be Sure To:
- Make comments for every major part of your work
- Create "checkpoints" for if there is an error ie if(doesn't work) cout error message
- be sure to get rid of any error underlines in your work

- goal is to have a runnable code by sunday morning

1. scoring logic/attempts 
  triggerclue in treasurehunt.cpp 
2. clue calling logic (when you move have you hit a clue or not, calls clue function based on if you hit a clue)
  triggerclue and findclueindex
  Display a “Fun Fact” message after a correct answer
  Hidden secret location (random add another if statement to clue logic)
3-4. making sure everything compiles and runs
  all files
5. graphics/outputs/inputs
  clue and map, all files once compileable
  Larger Map with decorations
6. win/loss logic, percent completion, saving
  startgame and displayfinalresult, checkpoints for saving