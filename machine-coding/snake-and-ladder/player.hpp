#ifndef SNL_PLAYER_INCLUDED
#define SNL_PLAYER_INCLUDED

#include <string>

class Player {

public:
	int
		position,
		n_times_bitten,
		n_times_rolled,
		n_times_climbed;
	std::string name;

public:
	Player(std::string name = "") {

		this->position = 
			this->n_times_bitten =
			this->n_times_rolled =
			this->n_times_climbed = 0;

		this->name = name;
	}
	int move(int n_steps) { //move increments to the dice roll value.

		this->position += n_steps;
		return n_steps;
	}
	int move_to(int position) { //set the position.

		this->position = position;
		return position;
	}
	int get_position() {

		return this->position;
	}
	std::string get_name() {

		return this->name;
	}
};

#endif
