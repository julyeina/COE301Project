#pragma once

#include <string>

namespace utility {
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
    Clue();
    Clue(char symbol,
         const std::string& question,
         const std::string& correctAnswer,
         int maxAttempts,
         int points,
         const std::string& funFact);

    char getSymbol() const;
    const std::string& getQuestion() const;
    const std::string& getCorrectAnswer() const;
    int getMaxAttempts() const;
    int getPoints() const;
    const std::string& getFunFact() const;
    bool isCompleted() const;

    bool checkAnswer(const std::string& userAnswer) const;
    void setCompleted(bool value);
};
}
