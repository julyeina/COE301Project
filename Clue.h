#pragma once
#include <iostream>
#include <string>

using namespace std;

namespace utility{
	class Clue{
	private:
		char symbol;
		string question;
		string correctAnswer;
		string funfact
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
			const string& funFact,

		char getSymbol() const;
		bool isCompleted() const;

		bool ask();
	};
}
