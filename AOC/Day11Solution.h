#pragma once
#include "AoCSolution.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

//forward declarations
int* getUniverse(int & width, int & height, std::string input_file);
void visualizeUniverse(int* universe, int width, int height);
int* EXPAND_UNIVERSE(int& width, int& height, int* universe);
std::vector<std::pair<int, int>> getGalaxyPositions(int* universe, int width, int height);
int getGalaxyDistanceSum(std::vector<std::pair<int, int>> galaxy_positions);
std::pair<std::vector<int>, std::vector<int>> getEmptyPositions(int width, int height, int* universe, bool add_offset);
std::size_t getGalaxyDistanceButWithMathAndWithoutMakingTheArrayPhysicallyBiggerWhyDoIEvenDoThisToMyself(std::vector<std::pair<int, int>> galaxy_positions, std::vector<int> emptyCols, std::vector<int> emptyRows);

class Day11Solution : public AoCSolution
{
public:
	void RunFirstPart(std::string input_file) override
	{
		int width, height;
		int* universe = getUniverse(width, height, input_file);
		int* new_universe = EXPAND_UNIVERSE(width, height, universe);
		delete[] universe;
		visualizeUniverse(new_universe, width, height);
		

		//get positions of all galaxies
		std::vector<std::pair<int, int>> galaxy_positions = getGalaxyPositions(new_universe, width, height);

		//get sum of distances between all galaxies
		int sum = getGalaxyDistanceSum(galaxy_positions);

		std::cout << "Sum of distances between all galaxies: " << sum << std::endl;

		delete[] new_universe;
	}

	void RunSecondPart(std::string input_file) override
	{
		int width, height;
		int* universe = getUniverse(width, height, input_file);

		//get positions of all galaxies
		std::vector<std::pair<int, int>> galaxy_positions = getGalaxyPositions(universe, width, height);
		auto empty_positions = getEmptyPositions(width, height, universe, false);
		std::vector<int> empty_rows = empty_positions.first;
		std::vector<int> empty_columns = empty_positions.second;

		//get sum of distances between all galaxies
		std::size_t sum = getGalaxyDistanceButWithMathAndWithoutMakingTheArrayPhysicallyBiggerWhyDoIEvenDoThisToMyself(galaxy_positions, empty_columns, empty_rows);

		std::cout << "Sum of distances between all galaxies: " << sum << std::endl;

		delete[] universe;
	}
};

std::size_t getGalaxyDistanceButWithMathAndWithoutMakingTheArrayPhysicallyBiggerWhyDoIEvenDoThisToMyself(std::vector<std::pair<int, int>> galaxy_positions, std::vector<int> emptyCols, std::vector<int> emptyRows)
{
	std::size_t sum = 0;
	for (int i = 0; i < galaxy_positions.size(); i++)
	{
		for (int j = i + 1; j < galaxy_positions.size(); j++)
		{
			int min_row = std::min(galaxy_positions[i].first, galaxy_positions[j].first);
			int max_row = std::max(galaxy_positions[i].first, galaxy_positions[j].first);
			int min_col = std::min(galaxy_positions[i].second, galaxy_positions[j].second);
			int max_col = std::max(galaxy_positions[i].second, galaxy_positions[j].second);

			//check how many empty rows are between the two galaxies
			int empty_rows = 0;
			for (int row : emptyRows)
			{
				if (row > min_row && row < max_row)
				{
					empty_rows++;
				}
			}

			//check how many empty columns are between the two galaxies
			int empty_columns = 0;
			for (int column : emptyCols)
			{
				if (column > min_col && column < max_col)
				{
					empty_columns++;
				}
			}

			//each empty row and column adds 1000000 to the distance between the two galaxies, each non-empty one adds 1
			int row_dist = std::abs(galaxy_positions[i].first - galaxy_positions[j].first) + empty_rows * (1000000-1);
			int col_dist = std::abs(galaxy_positions[i].second - galaxy_positions[j].second) + empty_columns * (1000000-1);

			//add the distance between the two galaxies and the number of empty rows and columns between them
			sum += col_dist + row_dist;
		}
	}

	return sum;
}

std::pair<std::vector<int>, std::vector<int>> getEmptyPositions(int width, int height, int * universe, bool add_offset = false)
{
	std::vector<int> empty_rows;
	std::vector<int> empty_columns;
	//get rows and columns that have no galaxies
	for (int i = 0; i < height; i++)
	{
		bool empty_row = true;
		for (int j = 0; j < width; j++)
		{
			if (universe[i * width + j] != 0)
			{
				empty_row = false;
				break;
			}
		}
		if (empty_row)
		{
			empty_rows.push_back(i + (add_offset ? empty_rows.size() : 0));
		}
	}

	for (int j = 0; j < width; j++)
	{
		bool empty_column = true;
		for (int i = 0; i < height; i++)
		{
			if (universe[i * width + j] != 0)
			{
				empty_column = false;
				break;
			}
		}
		if (empty_column)
		{
			empty_columns.push_back(j + (add_offset ? empty_columns.size() : 0));
		}
	}

	return std::make_pair(empty_rows, empty_columns);
}

int getGalaxyDistanceSum(std::vector<std::pair<int, int>> galaxy_positions)
{
	int sum = 0;
	for (int i= 0; i < galaxy_positions.size(); i++)
	{
		for (int j = i + 1; j < galaxy_positions.size(); j++)
		{
			sum += std::abs(galaxy_positions[i].first - galaxy_positions[j].first) + std::abs(galaxy_positions[i].second - galaxy_positions[j].second);
		}
	}

	return sum;
}

std::vector<std::pair<int, int>> getGalaxyPositions(int* universe, int width, int height)
{
	//iterate through universe and get positions of all galaxies
	std::vector<std::pair<int, int>> galaxy_positions;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (universe[i * width + j] != 0)
			{
				galaxy_positions.push_back(std::make_pair(i, j));
			}
		}
	}

	return galaxy_positions;
}

//capitalized function name because it sounds cool
int * EXPAND_UNIVERSE(int& width, int& height, int* universe)
{
	auto empty_positions = getEmptyPositions(width, height, universe, true);
	std::vector<int> empty_rows = empty_positions.first;
	std::vector<int> empty_columns = empty_positions.second;
	

	//expand universe so that each empty row and column is doubled
	int new_width = width + empty_columns.size();
	int new_height = height + empty_rows.size();

	int* new_universe = new int[new_width * new_height];

	//fill new universe with old universe
	int real_row = 0;
	for (int i = 0; i < new_height; i++)
	{
		int real_col = 0;
		//skip empty rows
		if (std::find(empty_rows.begin(), empty_rows.end(), i) != empty_rows.end())
		{
			continue;
		};
		for (int j = 0; j < new_width; j++) 
		{
			//skip empty columns
			if (std::find(empty_columns.begin(), empty_columns.end(), j) != empty_columns.end())
			{
				continue;
			};

			new_universe[i * new_width + j] = universe[real_row*width + real_col];
			real_col++;
		}
		real_row++;
	}

	//insert empty rows
	for(int row : empty_rows)
	{
		for (int j = 0; j < new_width; j++)
		{
			new_universe[row*new_width + j] = 0;
		}
	}

	//insert empty columns
	for (int column : empty_columns)
	{
		for (int i = 0; i < new_height; i++)
		{
			new_universe[i*new_width + column] = 0;
		}
	}

	//update width and height
	width = new_width;
	height = new_height;

	return new_universe;
}


int* getUniverse(int & width, int & height, std::string input_file)
{
	std::ifstream file{ input_file };
	std::string line;
	//get width and height
	width = 0;
	height = 0;
	while (std::getline(file, line))
	{
		height++;
		width = line.length();
	}
	file.close();

	//create universe
	int* universe = new int[width * height];

	//fill universe
	file.open(input_file);
	int i = 0;
	int galaxy_count = 1;

	while (std::getline(file, line))
	{
		for (int j = 0; j < width; j++)
		{
			if (line[j] == '.')
			{
				universe[i*width+j] = 0;
			}
			else if (line[j] == '#')
			{
				universe[i*width+j] = galaxy_count++;
			}
		}
		i++;
	}
	file.close();

	return universe;
}

void visualizeUniverse(int* universe, int width, int height)
{
	for (int i = 0; i < height; i++)
	{
		std::cout << std::endl;
		for (int j = 0; j < width; j++)
		{
			if (universe[i*width + j] == 0)
			{
				std::cout << ".";
			}
			else
			{
				std::cout << universe[i*width + j];
			}
		}
	}
	std::cout << std::endl;
}