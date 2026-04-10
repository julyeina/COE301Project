#pragma once //this ensures the header file is only included once during compilation

#include <string>

namespace utility {

//clue class represents a single question/interaction in the treasure hunt game 
//each has : a symbol, a question and correct answer, limited number of attemps, a point value, optional fun fact. and completions status
class Clue {
private:
    char symbol;
    std::string question;
    std::string correctAnswer;
    int maxAttempts;
    int points;
    std::string funFact;
    bool completed;

public:

//default constructor it creasts a placeholder clue with default values
    Clue();

//parametizes the constructer and intializes a fully defined clue with all properties
    Clue(char symbol,
         const std::string& question,
         const std::string& correctAnswer,
         int maxAttempts,
         int points,
         const std::string& funFact);

//these are the getter methods they allow for read onlhy access to private member variables
    char getSymbol() const;
    const std::string& getQuestion() const;
    const std::string& getCorrectAnswer() const;
    int getMaxAttempts() const;
    int getPoints() const;
    const std::string& getFunFact() const;
    bool isCompleted() const;

//checks if user's answer matches the correct answer
    bool checkAnswer(const std::string& userAnswer) const;

//updates whether the clue has been completed it is used when the player solves clue and in loading a saved game state
    void setCompleted(bool value);
};
}
