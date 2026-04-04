pragma once
#include <string>

using namespace std;

class Clue {
private:
	char symbol;
	string question;
	string correctAnswer;
	int maxAttempts;
	int points;
	bool completed;

public:

	Clue();
	Clue(char symbol, 
		const string& question, 
		const string& correctAnswer, 
		int maxAttempts, 
		int points);

	char getSymbol() const;
	string getQuestion() const;
	string getCorrectAnswer() const;
	int getMaxAttempts() const;
	int getPoints() const;
	bool isCompleted() const;
	void setCompleted(bool status);
	bool checkAnswer(const string& userAnswer) const;
};
