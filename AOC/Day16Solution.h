#pragma once
#include "AoCSolution.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>

//forward declarations
std::pair<std::string, std::pair<int, int>> GetInput(std::string file_name);
void NavigateBeam(std::string input_string, bool* energized_grid, int width, int height, int x, int y, int direction, std::unordered_set<int>& navigated);
std::size_t CountEnergized(bool* energized_grid, int width, int height);
void PrintEnergizedGrid(bool* energized_grid, int width, int height);
std::size_t CountEnergizedGridTiles(std::string input_string, int width, int height, int x, int y, int direction);

class Day16Solution : public AoCSolution
{
public:
	void RunFirstPart(std::string input_file) override
	{
		auto input = GetInput(input_file);
		std::string input_string = input.first;
		int width = input.second.first;
		int height = input.second.second;

		//get the number of energized tiles
		std::size_t count = CountEnergizedGridTiles(input_string, width, height, 0, 0, 1);

		std::cout << "Energized tiles: " << count << std::endl;
	}

	void RunSecondPart(std::string input_file) override
	{
		auto input = GetInput(input_file);
		std::string input_string = input.first;
		int width = input.second.first;
		int height = input.second.second;


		//get the number of energized tiles for each corner tile
		std::size_t max = 0;
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				if (x == 0 || x == width - 1 || y == 0 || y == height - 1)
				{
					if (x == 0)
					{
						std::size_t count = CountEnergizedGridTiles(input_string, width, height, x, y, 1);
						if(count > max) max = count;
					}
					if (x == width - 1)
					{
						std::size_t count = CountEnergizedGridTiles(input_string, width, height, x, y, 3);
						if (count > max) max = count;
					}
					if (y == 0)
					{
						std::size_t count = CountEnergizedGridTiles(input_string, width, height, x, y, 2);
						if (count > max) max = count;
					}
					if (y == height - 1)
					{
						std::size_t count = CountEnergizedGridTiles(input_string, width, height, x, y, 0);
						if (count > max) max = count;
					}
				}
			}
		}

		std::cout << "Max Energized Tiles: " << max << std::endl;
	}
};

std::size_t CountEnergizedGridTiles(std::string input_string, int width, int height, int x, int y, int direction)
{
	bool* energized_grid = new bool[width * height];
	std::fill_n(energized_grid, width * height, false);

	std::unordered_set<int> navigated;

	//navigate the beam
	NavigateBeam(input_string, energized_grid, width, height, x, y, direction, navigated);

	//count the energized tiles
	std::size_t count = CountEnergized(energized_grid, width, height);

	delete[] energized_grid;

	return count;

}

void PrintEnergizedGrid(bool* energized_grid, int width, int height)
{
	for (int y = 0; y < height; y++)
	{
		std::cout << std::endl;
		for (int x = 0; x < width; x++)
		{
			if (energized_grid[y * width + x]) std::cout << '#';
			else std::cout << '.';
		}
	}
	std::cout << std::endl;
}

std::size_t CountEnergized(bool* energized_grid, int width, int height)
{
	std::size_t count = 0;
	for (int i = 0; i < width * height; i++)
	{
		if (energized_grid[i]) count++;
	}
	return count;
}

void NavigateBeam(std::string input_string, bool* energized_grid, int width, int height, int x, int y, int direction, std::unordered_set<int>& navigated)
{
	//bounds check
	while (x >= 0 && x < width && y >= 0 && y < height)
	{
		int index = y * width + x;
		//check if we already navigated this beam
		if (navigated.find(direction * 100000 + index) != navigated.end())
			return;

		//add this beam to the navigated set
		navigated.insert(direction * 100000 + index);

		//energize the tile
		energized_grid[index] = true;

		//check the current symbol
		if (input_string[index] == '.' || (input_string[index] == '-' && direction % 2 == 1) || (input_string[index] == '|' && direction % 2 == 0))
		{
			//pass through
			switch (direction)
			{
				case 0:
					y--;
					break;
				case 1:
					x++;
					break;
				case 2:
					y++;
					break;
				case 3:
					x--;
					break;
			}
		}
		else if(input_string[index] == '\\')
		{
			//right => down, down => right, left => up, up => left (= 1 becomes 2, 2 becomes 1, 3 becomes 0, 0 becomes 3)
			if (direction % 2 == 1) direction = (direction + 1) % 4;
			else direction = (direction + 3) % 4;

			x += (direction == 1) - (direction == 3);
			y += (direction == 2) - (direction == 0);
		}
		else if (input_string[index] == '/')
		{
			//right => up, down => left, left => down, up => right (= 1 becomes 0, 2 becomes 3, 3 becomes 2, 0 becomes 1)
			if (direction % 2 == 1) direction = (direction + 3) % 4;
			else direction = (direction + 1) % 4;

			x += (direction == 1) - (direction == 3);
			y += (direction == 2) - (direction == 0);
		}
		else if (input_string[index] == '-' && direction % 2 == 0)
		{
			//Recursive split
			NavigateBeam(input_string, energized_grid, width, height, x-1, y, 3, navigated);
			NavigateBeam(input_string, energized_grid, width, height, x+1, y, 1, navigated);
		}
		else if (input_string[index] == '|' && direction % 2 == 1)
		{
			//Recursive split
			NavigateBeam(input_string, energized_grid, width, height, x, y-1, 0, navigated);
			NavigateBeam(input_string, energized_grid, width, height, x, y+1, 2, navigated);
		}
	}
}


std::pair<std::string, std::pair<int,int>> GetInput(std::string file_name)
{
	std::ifstream file(file_name);
	std::string line;
	std::string input;
	int width = 0;
	int height = 0;
	while (std::getline(file, line))
	{
		input += line;
		width = line.size();
		height++;
	}
	return std::make_pair(input, std::make_pair(width, height));
}
