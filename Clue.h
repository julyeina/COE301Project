#ifndef CLUE_H
#define	CLUE_H

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
	bool isCompleted() const;

	bool ask();
};

#endif