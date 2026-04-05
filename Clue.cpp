#include "Clue.h"

#include <algorithm>
#include <cctype>

namespace utility {
namespace {
std::string normalize(const std::string& value) {
    std::string cleaned;
    cleaned.reserve(value.size());

    for (char ch : value) {
        if (!std::isspace(static_cast<unsigned char>(ch))) {
            cleaned.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
        }
    }

    return cleaned;
}
}

Clue::Clue()
    : symbol('?'), maxAttempts(1), points(0), completed(false) {}

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

char Clue::getSymbol() const {
    return symbol;
}

const std::string& Clue::getQuestion() const {
    return question;
}

const std::string& Clue::getCorrectAnswer() const {
    return correctAnswer;
}

int Clue::getMaxAttempts() const {
    return maxAttempts;
}

int Clue::getPoints() const {
    return points;
}

const std::string& Clue::getFunFact() const {
    return funFact;
}

bool Clue::isCompleted() const {
    return completed;
}

bool Clue::checkAnswer(const std::string& userAnswer) const {
    return normalize(userAnswer) == normalize(correctAnswer);
}

void Clue::setCompleted(bool value) {
    completed = value;
}
}
