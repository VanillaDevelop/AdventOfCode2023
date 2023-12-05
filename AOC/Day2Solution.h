#pragma once
#include "AoCSolution.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

class Day2Solution : public AoCSolution
{
	public:
		void RunFirstPart(std::string input_file) override
		{
			std::ifstream infile(input_file);
			std::string line;

			int possible_game_sum = 0;

			std::map<std::string, int> max_counts = {
				{"red", 12},
				{"green", 13},
				{"blue", 14}
			};

			while (std::getline(infile, line))
			{
				//for each line, strip the "Game x: " from the front and get the id
				std::string::size_type col = line.find(":");
				int game_id = std::stoi(line.substr(5, col - 5));
				std::string game = line.substr(col + 2, line.size() - col - 2);
				if (is_game_possible(game, max_counts))
				{
					possible_game_sum += game_id;
				}
			}

			std::cout << possible_game_sum << std::endl;
		}

		void RunSecondPart(std::string input_file) override
		{
			std::ifstream infile(input_file);
			std::string line;

			int count_sum = 0;

			std::map<std::string, int> max_counts = {
				{"red", 12},
				{"green", 13},
				{"blue", 14}
			};

			while (std::getline(infile, line))
			{
				//for each line, strip the "Game x: " from the front and get the id
				std::string::size_type col = line.find(":");
				std::string game = line.substr(col + 2, line.size() - col - 2);
				std::map<std::string, int> cube_counts = {
					{"red", 0},
					{"green", 0},
					{"blue", 0}
				};
				min_cube_counts(game, cube_counts);
				int product = 1;
				for (auto& kv : cube_counts)
				{
					product *= kv.second;
				}
				std::cout << product << std::endl;
				count_sum += product;
			}

			std::cout << count_sum << std::endl;
		}

private:
	bool is_item_fine(std::string item, std::map<std::string, int> max_counts)
	{
		//a single item consists of a number, space, and a color
		std::string::size_type space = item.find(" ");
		int val = std::stoi(item.substr(0, space));
		std::string color = item.substr(space + 1, item.size() - space - 1);
		//if the max value for color exceeds the value presented, return false
		return max_counts[color] >= val;
	}

	bool is_game_fine(std::string game, std::map<std::string, int> max_counts)
	{
		//a game consists of comma and space separated numbers and items
		//split by , -> item delimiter
		std::string::size_type last_item = 0;
		std::string::size_type item = 0;
		while ((item = game.find(", ", last_item)) != std::string::npos)
		{
			std::string single_item = game.substr(last_item, item - last_item);
			//if the max value for color exceeds the value presented, return false
			if (!is_item_fine(single_item, max_counts))
			{
				return false;
			}
			//prepare for next item
			last_item = item + 2;
		}
		//do one more item for the remainder of the game string
		std::string single_item = game.substr(last_item, game.size() - last_item);
		if (!is_item_fine(single_item, max_counts))
		{
			return false;
		}
		return true;
	}

	bool is_game_possible(std::string line, std::map<std::string, int> max_counts)
	{
		//Assume "Game x: " is stripped from the line before being passed to this function
		//Split by ; -> round delimiter
		std::string::size_type last = 0;
		std::string::size_type n = 0;
		//search for substring from n to end of string

		while ((n = line.find(";", last)) != std::string::npos)
		{
			std::string game = line.substr(last, n - last);
			if (!is_game_fine(game, max_counts))
			{
				return false;
			}

			//prepare for next game
			last = n + 2;
		}
		//do one more round for the remainder of the game string
		std::string game = line.substr(last, line.size() - last);
		if (!is_game_fine(game, max_counts))
		{
			return false;
		}

		return true;
	}

	void add_to_cube_counts(std::string item, std::map<std::string, int>& cube_counts)
	{
		//a single item consists of a number, space, and a color
		std::string::size_type space = item.find(" ");
		int val = std::stoi(item.substr(0, space));
		std::string color = item.substr(space + 1, item.size() - space - 1);
		//if the max value for color exceeds the value presented, update the count
		if (cube_counts[color] < val)
		{
			cube_counts[color] = val;
		}
	}

	void set_round_counts(std::string round, std::map<std::string, int>& cube_counts)
	{
		//a game consists of comma and space separated numbers and items
		//split by , -> item delimiter
		std::string::size_type last_item = 0;
		std::string::size_type item = 0;
		while ((item = round.find(", ", last_item)) != std::string::npos)
		{
			std::string single_item = round.substr(last_item, item - last_item);
			add_to_cube_counts(single_item, cube_counts);
			//prepare for next item
			last_item = item + 2;
		}
		//do one more item for the remainder of the game string
		std::string single_item = round.substr(last_item, round.size() - last_item);
		add_to_cube_counts(single_item, cube_counts);
	}

	bool min_cube_counts(std::string line, std::map<std::string, int>& cube_counts)
	{
		//Assume "Game x: " is stripped from the line before being passed to this function
		//Split by ; -> round delimiter
		std::string::size_type last = 0;
		std::string::size_type n = 0;
		//search for substring from n to end of string

		while ((n = line.find(";", last)) != std::string::npos)
		{
			std::string game = line.substr(last, n - last);
			set_round_counts(game, cube_counts);

			//prepare for next game
			last = n + 2;
		}
		//do one more round for the remainder of the game string
		std::string game = line.substr(last, line.size() - last);
		set_round_counts(game, cube_counts);

		return true;
	}
};

