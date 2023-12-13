#pragma once
#include "AoCSolution.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>

//forward declarations
std::vector<std::pair<std::vector<char>, std::pair<int, int>>> getInput(const std::string& file_name);
int tryFindHorizontalSymmetry(const std::pair<std::vector<char>, std::pair<int, int>> & grid);
int tryFindVerticalSymmetry(const std::pair<std::vector<char>, std::pair<int, int>> & grid);
int tryFindHorizontalSymmetryWithSmudge(const std::pair<std::vector<char>, std::pair<int, int>> & grid);
int tryFindVerticalSymmetryWithSmudge(const std::pair<std::vector<char>, std::pair<int, int>> & grid);

class Day13Solution : public AoCSolution
{
public:
	void RunFirstPart(std::string input_file) override
	{
		auto grids = getInput(input_file);

		std::size_t sum = 0;

		//find the symmetry line for each grid
		for (auto grid : grids)
		{
			int symmetry_line = tryFindHorizontalSymmetry(grid);
			if(symmetry_line >= 0)
			{
				sum += (symmetry_line+1) * 100;
				std::cout << "Horizontal symmetry line found at row " << symmetry_line << std::endl;
			}

			symmetry_line = tryFindVerticalSymmetry(grid);
			if (symmetry_line >= 0)
			{
				sum += (symmetry_line + 1);
				std::cout << "Vertical symmetry line found at column " << symmetry_line << std::endl;
			}
		}

		std::cout << "Sum: " << sum << std::endl;
	}

	void RunSecondPart(std::string input_file) override
	{
		auto grids = getInput(input_file);

		std::size_t sum = 0;

		//find the symmetry line for each grid
		for (auto grid : grids)
		{
			int symmetry_line = tryFindHorizontalSymmetryWithSmudge(grid);
			if (symmetry_line >= 0)
			{
				sum += (symmetry_line + 1) * 100;
				std::cout << "Horizontal symmetry line found at row " << symmetry_line << std::endl;
			}

			symmetry_line = tryFindVerticalSymmetryWithSmudge(grid);
			if (symmetry_line >= 0)
			{
				sum += (symmetry_line + 1);
				std::cout << "Vertical symmetry line found at column " << symmetry_line << std::endl;
			}
		}

		std::cout << "Sum: " << sum << std::endl;
	}
};

int tryFindVerticalSymmetryWithSmudge(const std::pair<std::vector<char>, std::pair<int, int>> & grid)
{
	std::vector<char> grid_data = grid.first;
	int width = grid.second.first;
	int height = grid.second.second;

	for (int column = 0; column < width; column++)
	{
		int min_distance = std::min(column + 1, width - column - 1);
		int errors = 0;
		for (int i = 0; i < min_distance; i++)
		{
			for (int j = 0; j < height; j++)
			{
				if (grid_data[j * width + column - i] != grid_data[j * width + column + i + 1])
				{
					errors++;
					if (errors > 1)
					{
						break;
					}
				}
			}
			if (errors > 1)
			{
				break;
			}
		}
		if (errors == 1)
		{
			return column;
		}
	}

	return -1;
}

int tryFindHorizontalSymmetryWithSmudge(const std::pair<std::vector<char>, std::pair<int, int>> & grid)
{
	std::vector<char> grid_data = grid.first;
	int width = grid.second.first;
	int height = grid.second.second;

	for (int row = 0; row < height; row++)
	{
		int min_distance = std::min(row + 1, height - row - 1);
		int errors = 0;
		for (int i = 0; i < min_distance; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (grid_data[(row - i) * width + j] != grid_data[(row + i + 1) * width + j])
				{
					errors++;
					if (errors > 1)
					{
						break;
					}
				}
			}
			if (errors > 1)
			{
				break;
			}
		}
		if (errors == 1)
		{
			return row;
		}
	}

	return -1;
}

int tryFindVerticalSymmetry(const std::pair<std::vector<char>, std::pair<int, int>> & grid)
{
	std::vector<char> grid_data = grid.first;
	int width = grid.second.first;
	int height = grid.second.second;

	//Each integer represents the column that the vertical symmetry line would pass through (e.g., 0 is between columns 0 and 1)
	std::set<int> candidates;
	for (int i = 0; i < width - 1; i++)
	{
		candidates.insert(i);
	}

	//do a single pass through the grid, checking if there is symmetry between the current column and the next column
	for (int i = 0; i < candidates.size(); i++)
	{
		for (int j = 0; j < height; j++)
		{
			if (grid_data[j * width + i] != grid_data[j * width + i + 1])
			{
				candidates.erase(i);
				break;
			}
		}
	}

	//if there are no candidates left, then there is no vertical symmetry line
	if (candidates.size() == 0)
	{
		return -1;
	}

	//if there are candidates left, then we need to check these columns for full symmetry
	for (int candidate_column : candidates)
	{
		int min_distance = std::min(candidate_column + 1, width - candidate_column - 1);
		bool is_symmetric = true;
		for (int i = 0; i < min_distance; i++)
		{
			for (int j = 0; j < height; j++)
			{
				if (grid_data[j * width + candidate_column - i] != grid_data[j * width + candidate_column + i + 1])
				{
					is_symmetric = false;
					break;
				}
			}
			if (!is_symmetric)
			{
				break;
			}
		}
		if (is_symmetric)
		{
			return candidate_column;
		}
	}

	//if we iterated through all the candidates and didn't find a vertical symmetry line, then there is no vertical symmetry line
	return -1;
}

int tryFindHorizontalSymmetry(const std::pair<std::vector<char>, std::pair<int,int>> & grid)
{
	std::vector<char> grid_data = grid.first;
	int width = grid.second.first;
	int height = grid.second.second;

	//Each integer represents the row that the horizontal symmetry line would pass through (e.g., 0 is between rows 0 and 1)
	std::set<int> candidates;
	for (int i = 0; i < height-1; i++)
	{
		candidates.insert(i);
	}

	//do a single pass through the grid, checking if there is symmetry between the current row and the next row
	for (int i = 0; i < candidates.size(); i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (grid_data[i*width + j] != grid_data[(i + 1)*width + j])
			{
				candidates.erase(i);
				break;
			}
		}
	}

	//if there are no candidates left, then there is no horizontal symmetry line
	if (candidates.size() == 0)
	{
		return -1;
	}

	//if there are candidates left, then we need to check these rows for full symmetry
	for (int candidate_row : candidates)
	{
		int min_distance = std::min(candidate_row + 1, height - candidate_row - 1);
		bool is_symmetric = true;
		for (int i = 0; i < min_distance; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (grid_data[(candidate_row - i)*width + j] != grid_data[(candidate_row + i + 1)*width + j])
				{
					is_symmetric = false;
					break;
				}
			}
			if (!is_symmetric)
			{
				break;
			}
		}
		if (is_symmetric)
		{
			return candidate_row;
		}
	}
	
	//if we iterated through all the candidates and didn't find a horizontal symmetry line, then there is no horizontal symmetry line
	return -1;
}

std::vector<std::pair<std::vector<char>, std::pair<int, int>>> getInput(const std::string& file_name)
{
	//copy the input file into a string
	std::ifstream file(file_name);
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string input = buffer.str() + "\n\n";

	//create a vector of grids
	std::vector<std::pair<std::vector<char>, std::pair<int, int>>> result;

	//split the input into grids based on the double newline
	size_t start = 0;
	size_t end = input.find("\n\n", start);
	while (end != std::string::npos)
	{
		//get the substring for this grid
		std::string grid = input.substr(start, end - start);
		//get the height of this grid
		int height = std::count(grid.begin(), grid.end(), '\n') + 1;
		//get the width of this grid
		int width = grid.find_first_of('\n');
		//remove any \n characters
		grid.erase(std::remove(grid.begin(), grid.end(), '\n'), grid.end());
		std::vector<char> grid_vec;
		std::copy(grid.begin(), grid.end(), std::back_inserter(grid_vec));
		result.push_back(std::make_pair(grid_vec, std::make_pair(width, height)));
		start = end + 2;
		end = input.find("\n\n", start);
	}

	return result;
}