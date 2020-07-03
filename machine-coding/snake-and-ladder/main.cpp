/*
______________________________________________________________________

	Machine Coding
	1. Snake and Ladder
	

	I/P format:
		Read the number of snakes. (n1)
		Next n1 lines will read head and tail position of the snake.
	
		Read the number of ladders. (n2)
		Next n2 lines will read start and end points of ladder.

		Read the number of players. (n3)
		Next n3 lines will read the names of players.

______________________________________________________________________*/
#include <iostream>
#include "game.hpp"

const int NUM_TILES = 100;

int main(int argc, char *argv[]) {

	int
		n_snakes,
		n_ladders,
		n_players;

	SNAKE_DATA		*s_data;
	LADDER_DATA		*l_data;
	PLAYER_DATA		*p_data;

	int i;

	std::cin >> n_snakes;
	s_data = new SNAKE_DATA[n_snakes];
	for (i = 0; i < n_snakes; ++i) {

		std::cin >> s_data[i].start_point;
		std::cin >> s_data[i].end_point;
	}
	std::cin >> n_ladders;
	l_data = new LADDER_DATA[n_ladders];
	for (i = 0; i < n_ladders; ++i) {

		std::cin >> l_data[i].start_point;
		std::cin >> l_data[i].end_point;
	}
	std::cin >> n_players;
	p_data = new PLAYER_DATA[n_players];
	for (i = 0; i < n_players; ++i) {

		std::cin >> p_data[i].name;
	}
	Game game(NUM_TILES,
		s_data, n_snakes,
		l_data, n_ladders,
		p_data, n_players
	);

	//game loop.
	int old_pos, new_pos, roll_val;
	bool game_over = false;
	while (!game_over) {

		for (i = 0; i < n_players; ++i) {

			old_pos = game.players[i]->get_position();
			new_pos = game.player_roll(i, roll_val);

			std::cout << game.players[i]->get_name() << " has rolled a " << roll_val << " and moved from " << old_pos << " to " << new_pos << 
				((new_pos < old_pos) ? (" [SnakeBite!]") : ((new_pos - old_pos > 6) ? (" [Ladder!]") : (""))) << std::endl;
			if (new_pos == game.get_game_size()) {

				std::cout << game.players[i]->get_name() << " has won the game!" << std::endl;
				game_over = true;
				break;
			}
		}
	}
	std::cout << std::endl << "Winner : " << game.players[i]->get_name() << std::endl <<
		"num_rolls = " << game.players[i]->n_times_rolled << std::endl <<
		"num_climbs = " << game.players[i]->n_times_climbed << std::endl <<
		"num_bitten = " << game.players[i]->n_times_bitten << std::endl;
	return 0;
}
