#pragma once
#include "AoCSolution.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>

//forward declarations
std::vector<std::pair<std::string, std::vector<std::size_t>>> ParseInput(std::string & input_file);
std::size_t recursiveCountConfigurations(std::string& configuration, std::size_t min_str_index, const std::vector<std::size_t>& groups, std::size_t min_group_index);
std::vector<std::pair<std::string, std::vector<std::size_t>>> MultiplySprings(const std::vector<std::pair<std::string, std::vector<std::size_t>>> & input);

//funny memorization map
std::unordered_map<std::string, std::unordered_map<std::size_t, std::size_t>> memo;

class Day12Solution : public AoCSolution
{
public:
	void RunFirstPart(std::string input_file) override
	{
		std::vector<std::pair<std::string, std::vector<std::size_t>>> input = ParseInput(input_file);
		std::size_t full_count = 0;

		for(auto & input_pair : input)
		{
			std::string configuration = input_pair.first;
			std::vector<std::size_t> groups = input_pair.second;

			memo.clear();
			std::size_t count = recursiveCountConfigurations(configuration, 0, groups, 0);

			std::cout << "There are " << count << " possible configurations for the input " << configuration << " and groups ";
			for (auto & group : groups)
			{
				std::cout << group << " ";
			}
			std::cout << std::endl;

			full_count += count;
		}

		std::cout << "There are " << full_count << " possible configurations for all inputs" << std::endl;
	}

	void RunSecondPart(std::string input_file) override
	{
		std::vector<std::pair<std::string, std::vector<std::size_t>>> input = ParseInput(input_file);
		input = MultiplySprings(input);
		std::size_t full_count = 0;

		for (auto& input_pair : input)
		{
			std::string configuration = input_pair.first;
			std::vector<std::size_t> groups = input_pair.second;

			memo.clear();
			std::size_t count = recursiveCountConfigurations(configuration, 0, groups, 0);

			std::cout << "There are " << count << " possible configurations for the input " << configuration << " and groups ";
			for (auto& group : groups)
			{
				std::cout << group << " ";
			}
			std::cout << std::endl;

			full_count += count;
		}

		std::cout << "There are " << full_count << " possible configurations for all inputs" << std::endl;
	}
};

std::size_t get_from_map(const std::string& configuration, std::size_t min_str_index, std::size_t min_group_index)
{
	auto memo_it = memo.find(configuration.substr(min_str_index, configuration.size() - min_str_index));
	if (memo_it == memo.end())
	{
		return -1;
	}
	auto memo_it2 = memo_it->second.find(min_group_index);
	if (memo_it2 == memo_it->second.end())
	{
		return -1;
	}
	return memo_it2->second;
}

void write_to_map(const std::string& configuration, std::size_t min_str_index, std::size_t min_group_index, std::size_t count)
{
	auto memo_it = memo.find(configuration.substr(min_str_index, configuration.size() - min_str_index));
	if (memo_it == memo.end())
	{
		memo_it = memo.insert(std::make_pair(configuration.substr(min_str_index, configuration.size() - min_str_index), std::unordered_map<std::size_t, std::size_t>())).first;
	}
	memo_it->second.insert(std::make_pair(min_group_index, count));
};

/// <summary>
/// Recursively count the number of valid configurations that satisfy the adjacent groups of damaged springs.
/// </summary>
/// <param name="configuration">A string representing the configuration of springs</param>
/// <param name="min_str_index">The current minimum string index to consider</param>
/// <param name="groups">The number of groups of adjacent damaged springs</param>
/// <param name="min_group_index">The current minimum group index to consider</param>
/// <returns>A count of valid configurations given the constrastd::size_ts.</returns>
std::size_t recursiveCountConfigurations(std::string& configuration, std::size_t min_str_index, const std::vector<std::size_t> & groups, std::size_t min_group_index)
{
	//first check if we have already computed this configuration
	auto memo_it = memo.find(configuration.substr(min_str_index, configuration.size() - min_str_index));
	std::size_t map_count = get_from_map(configuration, min_str_index, min_group_index);
	if (map_count != -1)
	{
		return map_count;
	}

	//get the maximum indices for the string and the groups
	std::size_t max_str_index = configuration.size() - 1;
	std::size_t max_group_index = groups.size() - 1;

	//If we have iterated through the entire string, the configuration is valid iff no more groups are left to determine
	if (min_str_index > max_str_index && min_group_index > max_group_index)
	{
		return 1;
	}

	//We can handle everything in one call until we reach a "?" character
	while (min_str_index <= max_str_index && configuration[min_str_index] != '?')
	{
		//Strip leading ". characters
		while (min_str_index <= max_str_index && configuration[min_str_index] == '.')
		{
			min_str_index++;
		}

		//For leading "#" characters
		while (min_str_index <= max_str_index && configuration[min_str_index] == '#')
		{
			//there must be at least min_group_index remaining characters in the string
			if (min_group_index > max_group_index || min_str_index + groups[min_group_index]-1 > max_str_index)
			{
				return 0;
			}
			//These must all be "#" or "?" characters
			for (std::size_t i = 0; i < groups[min_group_index]; i++)
			{
				if (configuration[min_str_index + i] == '.')
				{
					return 0;
				}
			}
			//the n+1th character, if it exists, must not be a "#" character
			if (min_str_index + groups[min_group_index] <= max_str_index && configuration[min_str_index + groups[min_group_index]] == '#')
			{
				return 0;
			}

			//This is a valid configuration so far, so we increase the indices accordingly
			min_str_index += groups[min_group_index] + 1;
			min_group_index++;
		}
	}

	//If we iterated through the whole string and there are no more groups to determine, this is a valid configuration
	if (min_str_index > max_str_index && min_group_index > max_group_index)
	{
		return 1;
	}
	else if (min_str_index > max_str_index)
	{
		return 0;
	}

	std::size_t count = 0;
	//Otherwise, we now have a leading "?" that we must replace with a "#" and "." respectively
	if (configuration[min_str_index] == '?')
	{
		//to save recursive steps, we only insert a "#" character if the conditions satisfy the next group
		if (min_group_index <= max_group_index && min_str_index + groups[min_group_index] - 1 <= max_str_index)
		{
			//Check if there is no dot character in this set of characters
			bool no_dot = true;
			for (std::size_t i = 0; i < groups[min_group_index]; i++)
			{
				if (configuration[min_str_index + i] == '.')
				{
					no_dot = false;
					break;
				}
			}
			//If there is no dot character, we can insert a "#" character
			if (no_dot)
			{
				configuration[min_str_index] = '#';
				count += recursiveCountConfigurations(configuration, min_str_index, groups, min_group_index);
				configuration[min_str_index] = '?';
			}
		}
		//When parsing ".", we know that this whitespace character will get skipped directly, so we can recurse on the next character
		count += recursiveCountConfigurations(configuration, min_str_index+1, groups, min_group_index);
	}
	write_to_map(configuration, min_str_index, min_group_index, count);
	return count;
}


/// <summary>
/// Multiply the input string and vector 5x each
/// </summary>
/// <param name="input">The input data</param>
/// <returns>An increased value of the input data</returns>
std::vector<std::pair<std::string, std::vector<std::size_t>>> MultiplySprings(const std::vector<std::pair<std::string, std::vector<std::size_t>>> & input)
{
	std::vector<std::pair<std::string, std::vector<std::size_t>>> output;
	for (auto & input_pair : input)
	{
		std::string configuration = input_pair.first;
		std::vector<std::size_t> groups = input_pair.second;

		std::string new_configuration = "";
		std::vector<std::size_t> new_groups;
		//multiply the configuration by 5
		for (std::size_t i = 0; i < 5; i++)
		{
			new_configuration += configuration;
			if(i != 4)
			{
				new_configuration += "?";
			}
			for(auto & group : groups)
			{
				new_groups.push_back(group);
			}
		}

		output.push_back(std::make_pair(new_configuration, new_groups));
	}
	return output;

}

/// <summary>
/// Given the input, convert std::size_to a list of spring configuration states and group counts of damaged springs
/// </summary>
/// <param name="input_file">The input file</param>
/// <returns>A list of pairs containing a spring configuration state (string, first) and group counts of damaged springs (std::size_t vector, second)</returns>
std::vector<std::pair<std::string, std::vector<std::size_t>>> ParseInput(std::string & input_file)
{
	std::vector<std::pair<std::string, std::vector<std::size_t>>> input;
	std::ifstream file(input_file);
	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string springs;
		iss >> springs;

		std::vector<std::size_t> groups;
		std::size_t value;

		for(std::size_t i; iss >> i;) 
		{
			groups.push_back(i);
			if (iss.peek() == ',')
				iss.ignore();
		}
		input.push_back(std::make_pair(springs, groups));
	}
	return input;
}