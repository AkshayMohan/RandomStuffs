#ifndef SNL_GAME_INCLUDED
#define SNL_GAME_INCLUDED

#include "player.hpp"
#include "game_struct.h"

#include <vector>
#include <random>

typedef GAME_STRUCT			SNAKE_DATA;
typedef GAME_STRUCT			LADDER_DATA;
typedef PLAYER_INPUT_DATA	PLAYER_DATA;

class Game {

public:
	std::vector<Player*>	players;

	int						game_size;
	int						*game_tiles;

public:
	Game(int game_size,
		SNAKE_DATA *snake_data,		int n_snakes,
		LADDER_DATA *ladder_data,	int n_ladders,
		PLAYER_DATA *player_data,	int n_players) {

		int i;

		this->game_size		=	game_size;
		this->game_tiles	=	new int[game_size]();
		
		for (i = 0; i < n_snakes; ++i) {

			this->game_tiles[snake_data[i].start_point - 1] = snake_data[i].end_point;
		}
		for (i = 0; i < n_ladders; ++i) {

			this->game_tiles[ladder_data[i].start_point - 1] = ladder_data[i].end_point;
		}
		for (i = 0; i < n_players; ++i) {

			players.push_back(new Player(player_data[i].name));
		}
	}
	int dice_roll() {

		static const int
			min_val = 1,
			max_val = 6;

		static std::random_device rd;
		static std::mt19937 generator(rd());
		static std::uniform_int_distribution<int> distr(min_val, max_val);

		return distr(generator);
	}
	int player_roll(int player_idx, int &roll_val) {

		roll_val = this->dice_roll();
		++(this->players[player_idx]->n_times_rolled);
		int next_pos = roll_val + this->players[player_idx]->get_position();
		if (next_pos > this->game_size) {

			return 0; //Failed roll!
		}
		if (this->game_tiles[next_pos - 1]) {

			//ladder or snake:
			if (this->game_tiles[next_pos - 1] > this->players[player_idx]->get_position()) {

				++(this->players[player_idx]->n_times_climbed);
			}
			else {

				++(this->players[player_idx]->n_times_bitten);
			}
			this->players[player_idx]->move_to(this->game_tiles[next_pos - 1]);
		}
		else {

			//normal:
			this->players[player_idx]->move(roll_val);
		}
		return this->players[player_idx]->get_position();
	}

	int get_game_size() {

		return this->game_size;
	}
};

#endif
