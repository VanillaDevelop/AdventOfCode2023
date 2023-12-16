#pragma once
#include "AoCSolution.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>

//forward declarations
std::pair<std::string, std::pair<int, int>> getInput(const std::string& file_name);
std::size_t calculateLoadWeight(std::string input, int width, int height);
void printGrid(const std::string& input, int width, int height);
void rotate(std::string& input, int width, int height);
void tiltLeft(std::string& input, int width, int height);
std::size_t calculateLoadWeightAfterTilt(std::string input, int width, int height);

std::unordered_map <std::string, std::size_t> map;

class Day14Solution : public AoCSolution
{
	public:
		void RunFirstPart(std::string input_file) override
		{
			//Get input
			auto inputPair = getInput(input_file);
			std::string input = inputPair.first;
			int width = inputPair.second.first;
			int height = inputPair.second.second;

			//Calculate the load weight
			std::size_t load_weight = calculateLoadWeightAfterTilt(input, width, height);

			std::cout << "Load Weight: " << load_weight << std::endl;
		}
	
		void RunSecondPart(std::string input_file) override
		{			
			//Get input
			auto inputPair = getInput(input_file);
			std::string input = inputPair.first;
			int width = inputPair.second.first;
			int height = inputPair.second.second;

			//rotate 3x so that north is pointing left
			rotate(input, width, height);
			rotate(input, width, height);
			rotate(input, width, height);

			bool found_loop = false;
			for (std::size_t i = 0; i < 1000000000; i++)
			{
				if (i % 10000000 == 0)
				{
					std::cout << (i / 10000000) << "% complete" << std::endl;
				}
				for (int j = 0; j < 4; j++)
				{
					tiltLeft(input, width, height);
					rotate(input, width, height);
				}
				if (map.find(input) != map.end() && !found_loop)
				{
					std::cout << "Found a loop at " << map[input] << " and " << i << std::endl;
					std::cout << "Skipping ahead...";
					int loop_size = i - map[input];
					std::cout << "Loop size: " << loop_size << std::endl;
					while (i + loop_size < 1000000000)
					{
						i += loop_size;
					}
					std::cout << "New i: " << i << std::endl;
					found_loop = true;
				}
				else
				{
					map[input] = i;
				}
			}

			//Rotate 1x so that north is pointing up
			rotate(input, width, height);

			//Print Grid
			printGrid(input, width, height);

			//Calculate the load weight
			std::size_t load_weight = calculateLoadWeight(input, width, height);

			std::cout << "Load Weight: " << load_weight << std::endl;

			std::cout << std::endl;
		}
};

/// <summary>
/// Rotates the input string 90 degrees clockwise
/// </summary>
/// <param name="input">The input string</param>
/// <param name="width">The width of the grid</param>
/// <param name="height">The height of the grid</param>
void rotate(std::string& input, int width, int height)
{
	std::string new_input = input;

	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			int i = row * width + col;
			int new_i = col * height + (height - row - 1);
			new_input[new_i] = input[i];
		}
	}

	input = new_input;
}

/// <summary>
/// Tilts all rocks to the left (west) side. This tilt works the easiest using string manipulation.
/// </summary>
/// <param name="input">The input string</param>
/// <param name="width">The width of the grid</param>
/// <param name="height">The height of the grid</param>
void tiltLeft(std::string& input, int width, int height)
{
	std::string new_input(input.size(), '.');
	int new_ptr = 0;

	for (int row = 0; row < height; row++)
	{
		new_ptr = row * width;
		for (int col = 0; col < width; col++)
		{
			int i = row * width + col;
			if (input[i] == 'O')
			{
				new_input[new_ptr] = 'O';
				new_ptr++;
			}
			else if (input[i] == '#')
			{
				new_input[i] = '#';
				new_ptr = i+1;
			}
		}
	}

	input = new_input;
}

void printGrid(const std::string & input, int width, int height)
{
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
			std::cout << input[row * width + col];
		std::cout << std::endl;
	}
}

std::size_t calculateLoadWeight(std::string input, int width, int height)
{
	std::size_t load_weight = 0;
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			int i = row * width + col;
			if (input[i] == 'O')
			{
				load_weight += height - row;
			}
		}
	}
	return load_weight;
}

std::size_t calculateLoadWeightAfterTilt(std::string input, int width, int height)
{
	std::size_t load_weight = 0;
	for (int col = 0; col < width; col++)
	{
		int last_weight = height;
		for (int row = 0; row < height; row++)
		{
			int i = row * width + col;
			if (input[i] == 'O')
			{
				load_weight += last_weight;
				last_weight--;
			}
			else if (input[i] == '#')
			{
				last_weight = height - row - 1;
			}
		}
	}
	return load_weight;
}

/// <summary>
/// Return a string containing the input file, and a pair containing the width and height of the grid
/// </summary>
/// <param name="file_name">The input file name</param>
/// <returns>A string containing the input file, and a pair containing the width and height of the grid</returns>
std::pair<std::string, std::pair<int, int>> getInput(const std::string& file_name)
{
	//copy the input file into a string
	std::ifstream file(file_name);
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string input = buffer.str();

	//get the height
	int height = std::count(input.begin(), input.end(), '\n') + 1;
	//get the width 
	int width = input.find_first_of('\n');
	//remove any \n characters
	input.erase(std::remove(input.begin(), input.end(), '\n'), input.end());
	return std::make_pair(input, std::make_pair(width, height));
}



