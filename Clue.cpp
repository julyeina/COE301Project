#include "Clue.h"

#include <algorithm>
#include <cctype>

namespace utility {
namespace {

//This is a helper function it : normailzes a string for comparison, removes all whitespace, converts all characters to lowercase so "New" and 'new" match
std::string normalize(const std::string& value) {
    std::string cleaned;
    cleaned.reserve(value.size());//reserves memory 

    for (char ch : value) {
        //ignores space and only keeps visiable characters
        if (!std::isspace(static_cast<unsigned char>(ch))) {
            //convert each character to lowercase for case insenitivity 
            cleaned.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
        }
    }

    return cleaned;
}
}

//default constructor it initializes a placeholder clue with default values
Clue::Clue()
    : symbol('?'), maxAttempts(1), points(0), completed(false) {}
//parameters for constructor and creates a fully defined clue wiht all properties set
Clue::Clue(char symbol,
           const std::string& question,
           const std::string& correctAnswer,
           int maxAttempts,
           int points,
           const std::string& funFact)
    : symbol(symbol),                    
      question(question),
      correctAnswer(correctAnswer),
      maxAttempts(maxAttempts),
      points(points),
      funFact(funFact),
      completed(false) {}
//returns the symbol represnting the clue on the map
char Clue::getSymbol() const {
    return symbol;
}
//returns the clue question text
const std::string& Clue::getQuestion() const {
    return question;
}
//returns the correct answer for checking user input
const std::string& Clue::getCorrectAnswer() const {
    return correctAnswer;
}
//returns how many attempts the user is allowed
int Clue::getMaxAttempts() const {
    return maxAttempts;
}
//returns how many points the clue is worth
int Clue::getPoints() const {
    return points;
}
//returns a fun fact associated with the clue if it has one
const std::string& Clue::getFunFact() const {
    return funFact;
}
//returns whether the clue has been completed
bool Clue::isCompleted() const {
    return completed;
}
//checks if the users answer matches the correct answer and uses normalization to not be case sensitive 
bool Clue::checkAnswer(const std::string& userAnswer) const {
    return normalize(userAnswer) == normalize(correctAnswer);
}
//marks the clue as completed or not also used when loading saved progress
void Clue::setCompleted(bool value) {
    completed = value;
}
}
