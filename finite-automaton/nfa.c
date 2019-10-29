/*
	Basic Implementation of NFA in C.
*/

#include <stdio.h>
#include <stdlib.h>

#define				MAX_STATES			6
#define				MAX_INPUTS			2 //Considering 0 and 1 only now.

int ttable[MAX_STATES][MAX_INPUTS][MAX_INPUTS] = {

	{
		{1, 0},
		{2, 0}
	},
	{
		{3, -1},
		{-1, -1}
	},
	{
		{-1, -1},
		{3, -1}
	}
};

int validate_NFA(int current_state, int final_state, char *input) {

	if(*input == '\0')
		return current_state;

	int i_input, i, next_state;
	i_input = *input - '0';

	for(i = 0; i< MAX_INPUTS; ++i) {

		if((next_state = ttable[current_state][i_input][i]) == -1)
			return 0;

		if(validate_NFA(next_state, final_state, input + 1) == final_state)
			return final_state;
	}
	return 0;
}

int main() {

	printf("Testing\n");
	printf("%s\n", (validate_NFA(0, 4, g_test) == 4) ? ("Yes") : ("No"));
	return 0;
}
