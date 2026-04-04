#include "Clue.h"
#include <cctype>

using namespace std;

// make game case-insensitive for user answer
static string toLowerCopy(const std::string& s) {
	string out = s;
	for (char& ch : out){
		ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
	} 
	return out;
}

//default constructor
Clue::Clue(){
	symbol = ' ';
	question = "";
	correctAnswer = "";
	maxAttempts = 0;
	points = 0;
	completed = false;
}
//constructor
Clue::Clue(char sym, const string& q, const string& answer, int attempts, int pts){
	symbol = sym;
	question = q;
	correctAnswer = answer;
	maxAttempts = attempts;
	points = pts;
	completed = false;
}
	
char Clue::getSymbol() const {
	
	return symbol;
}

string Clue::getQuestion() const{
	return question;
}

string Clue:: getCorrectAnswer() const{
	return correctAnswer;
}
int Clue:: getMaxAttempts() const{
	return maxAttempts;
}
int Clue::getPoints() const{
	return points;
}

bool Clue::isCompleted() const {
	return completed;
}

void Clue:: setCompleted(bool status){
	completed = status;
}

bool Clue::checkAnswer(const string& userAnswer) const {
	return toLowerCopy(userAnswer) == toLowerCopy(correctAnswer);
}

