#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void makeLowerCase(char* str) {
	while (*str != '\0') {
		*str = (char) tolower(*str);
		str++;
	}
}

bool isStringAlpha(const char* str) {
	while (*str != '\0') {
		if (!isalpha(*str)) return false;
		str++;
	}
	return true;
}

#define WORD_LEN 5

enum CharResult { CORRECT, CONTAINED, INCORRECT };
struct GuessResult {
	enum CharResult charResults[WORD_LEN];
};

struct GuessResult checkGuess(const char* guess, const char* target) {
	struct GuessResult result;
	for (int i = 0; i < WORD_LEN; i++) {
		result.charResults[i] = INCORRECT;
	}

	for (int i = 0; i < WORD_LEN; i++) {
		// See if character exactly matches
		if (guess[i] == target[i]) {
			result.charResults[i] = CORRECT;
			continue;
		}
		// Otherwise, mark the first matching incorrect character as contained
		for (int j = 0; j < WORD_LEN; j++) {
			if (guess[j] == target[i] && result.charResults[j] == INCORRECT) {
				result.charResults[j] = CONTAINED;
				break;
			}
		}
	}

	return result;
}

void printResult(const char* guess, struct GuessResult result) {
	printf("\e[0m");

	for (int i = 0; i < WORD_LEN; i++) {
		switch(result.charResults[i]) {
			case INCORRECT:
				printf("\e[37m");
			break;
			case CONTAINED:
				printf("\e[33m");
			break;
			case CORRECT:
				printf("\e[32m");
			break;
		}
		printf("%c", guess[i]);
	}

	printf("\e[0m\n");
}

void moveTo(unsigned int row, unsigned int col) {
	printf("\e[%u;%uf", row, col);
}
void clearToLineEnd() {
	printf("\e[0K");
}
void clearToScreenEnd() {
	printf("\e[0J");
}

#define GUESSES 6
#define BUF_SIZE 512
int main() {
	// Enter alternate screen
	printf("\e[?1049h");
	moveTo(0, 0);
	clearToScreenEnd();

	char buffer[BUF_SIZE];
	char guessStr[WORD_LEN+1];
	char target[WORD_LEN+1] = "hello";

	char guessFormatString[16];
	sprintf(guessFormatString, "%%%ds", WORD_LEN);

	/* SCREEN LAYOUT
	 * Status line
	 * Guess 1
	 * Guess 2
	 * Guess ...
	 * Guess GUESSES
	 * Typing line
	 */

	unsigned int guess = 0;
	while (guess < GUESSES) {
		moveTo(GUESSES + 2, 1);
		clearToScreenEnd();
		fgets(buffer, BUF_SIZE, stdin);
		if (buffer[WORD_LEN] == '\n') buffer[WORD_LEN] = '\0';
		if (strlen(buffer) != WORD_LEN) {
			moveTo(1, 1);
			printf("Guess must be %d characters long", WORD_LEN);
			clearToLineEnd();
			continue;
		}

		if (!isStringAlpha(buffer)) {
			moveTo(1, 1);
			printf("Guess must be alphabetic characters");
			clearToLineEnd();
			continue;
		}

		sscanf(buffer, guessFormatString, guessStr);

		makeLowerCase(guessStr);

		moveTo(guess + 2, 1);
		printResult(guessStr, checkGuess(guessStr, target));
		clearToLineEnd();
		if (strcmp(guessStr, target) == 0) {
			printf("Correct!\n");
			goto cleanup;
		}
		guess++;
	}
	printf("Loser\n");

	// Leave alternate screen
cleanup:
	printf("\e[?1049h");
	return 0;
}
