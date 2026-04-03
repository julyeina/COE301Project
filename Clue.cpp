#include "Clue.h"

Clue::Clue()
	: symbol('?'),
	maxAttempts(0),
	points(0),
	completed(false) {}

Clue::Clue(char symbol, 
		   const string& question, 
	       const string& correctAnswer, 
	       int maxAttempts, 
	       int points)
	: symbol(symbol),
	question(question),
	correctAnswer(correctAnswer),
	maxAttempts(maxAttempts),
	points(points),
	completed(false) {}
char Clue::getSymbol() const {
	return symbol;
}

bool Clue::isCompleted() const {
	return completed;
}

bool Clue::ask() {
	return false;
}