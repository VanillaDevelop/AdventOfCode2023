#pragma once
#include "AoCSolution.h"
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <queue>

struct Node
{
	int x;
	int y;
	int Distance;
};

/// <summary>
/// Get the input from the file
/// </summary>
/// <param name="input_file">The input file</param>
/// <returns>A pair containing a vector (array of chars), and ints declaring the width and height of the map</returns>
std::pair<std::vector<char>, std::pair<int,int>> GetInput(const std::string & input_file)
{
	std::vector<char> input;
	std::ifstream file(input_file);
	std::string line;
	int width = 0;
	int height = 0;
	while (std::getline(file, line))
	{
		for (auto c : line)
		{
			input.push_back(c);
		}
		width = line.size();
		height++;
	}
	return std::make_pair(input, std::make_pair(width, height));
}

std::vector<char> MultiplyGrid(const std::vector<char> & input, int width, int height, int factor)
{
	std::vector<char> output;
	for (int l = 0; l < factor; l++)
	{
		for (int i = 0; i < width; i++)
		{
			for (int k = 0; k < factor; k++)
			{
				for (int j = 0; j < height; j++)
				{
					if(input[i+j] == 'S')
					{
						output.push_back('.');
					}
					else
					{
						output.push_back(input[i + j * width]);
					}
				}
			}
		}
	}
	return output;
}

int GetNumberOfSteps(const std::vector<char>& input, int width, int height, int x_start, int y_start, int x_end, int y_end)
{
	std::queue<Node> nodes;
	nodes.push({ x_start, y_start, 0 });
	std::vector<int> cost_map(width * height, INT_MAX);
	cost_map[x_start + y_start * width] = 0;

	//we can use BFS since all steps are equal cost
	while (!nodes.empty())
	{
		Node node = nodes.front();
		nodes.pop();

		//check up
		if (node.y > 0)
		{
			int index = node.x + (node.y - 1) * width;
			if (input[index] != '#')
			{
				if (cost_map[index] > node.Distance + 1)
				{
					nodes.push({ node.x, node.y - 1, node.Distance + 1 });
					cost_map[index] = node.Distance + 1;
				}
				if(node.x == x_end && node.y == y_end)
				{
					return node.Distance;
				}
			}
		}
		//check down
		if (node.y < height - 1)
		{
			int index = node.x + (node.y + 1) * width;
			if (input[index] != '#')
			{
				if (cost_map[index] > node.Distance + 1)
				{
					nodes.push({ node.x, node.y + 1, node.Distance + 1 });
					cost_map[index] = node.Distance + 1;
				}
				if (node.x == x_end && node.y == y_end)
				{
					return node.Distance;
				}
			}
		}
		//check left
		if (node.x > 0)
		{
			int index = node.x - 1 + node.y * width;
			if (input[index] != '#')
			{
				if (cost_map[index] > node.Distance + 1)
				{
					nodes.push({ node.x - 1, node.y, node.Distance + 1 });
					cost_map[index] = node.Distance + 1;
				}
				if (node.x == x_end && node.y == y_end)
				{
					return node.Distance;
				}
			}
		}
		//check right
		if (node.x < width - 1)
		{
			int index = node.x + 1 + node.y * width;
			if (input[index] != '#')
			{
				if (cost_map[index] > node.Distance + 1)
				{
					nodes.push({ node.x + 1, node.y, node.Distance + 1 });
					cost_map[index] = node.Distance + 1;
				}
				if (node.x == x_end && node.y == y_end)
				{
					return node.Distance;
				}
			}
		}
	}

	return 0;
}

/// <summary>
/// Get the cost map for a single grid, for a certain number of steps
/// </summary>
/// <param name="input">The input grid</param>
/// <param name="width">Width of the grid</param>
/// <param name="height">Height of the grid</param>
/// <param name="max_steps">The maximum number of steps we are trying to take</param>
/// <param name="start_points">The starting points</param>
/// <returns></returns>
std::vector<int> GetCostMap(const std::vector<char> & input, int width, int height, int max_steps, std::vector<std::pair<int, int>> start_points)
{
	std::vector<int> cost_map(width * height, INT_MAX);

	std::queue<Node> nodes;
	for(auto p : start_points)
	{
		cost_map[p.first + p.second * width] = 0;
		nodes.push({ p.first, p.second, 0 });
	}

	while (!nodes.empty())
	{
		Node node = nodes.front();
		nodes.pop();

		//check up
		if (node.y > 0)
		{
			int index = node.x + (node.y - 1) * width;
			if (input[index] != '#')
			{
				if (cost_map[index] > node.Distance + 1 && node.Distance + 1 <= max_steps)
				{
					cost_map[index] = node.Distance + 1;
					nodes.push({ node.x, node.y - 1, node.Distance + 1 });
				}
			}
		}
		//check down
		if (node.y < height - 1)
		{
			int index = node.x + (node.y + 1) * width;
			if (input[index] != '#')
			{
				if (cost_map[index] > node.Distance + 1 && node.Distance + 1 <= max_steps)
				{
					cost_map[index] = node.Distance + 1;
					nodes.push({ node.x, node.y + 1, node.Distance + 1 });
				}
			}
		}
		//check left
		if (node.x > 0)
		{
			int index = node.x - 1 + node.y * width;
			if (input[index] != '#')
			{
				if (cost_map[index] > node.Distance + 1 && node.Distance + 1 <= max_steps)
				{
					cost_map[index] = node.Distance + 1;
					nodes.push({ node.x - 1, node.y, node.Distance + 1 });
				}
			}
		}
		//check right
		if (node.x < width - 1)
		{
			int index = node.x + 1 + node.y * width;
			if (input[index] != '#')
			{
				if (cost_map[index] > node.Distance + 1 && node.Distance + 1 <= max_steps)
				{
					cost_map[index] = node.Distance + 1;
					nodes.push({ node.x + 1, node.y, node.Distance + 1 });
				}
			}
		}
	}

	return cost_map;
}

double lagrangeInterpolate(const std::vector<double>& x, const std::vector<double>& y, double x0) {
	double result = 0.0;
	int n = x.size();

	for (int i = 0; i < n; i++) {
		double term = y[i];
		for (int j = 0; j < n; j++) {
			if (j != i) {
				term = term * (x0 - x[j]) / (x[i] - x[j]);
			}
		}
		result += term;
	}

	return result;
}

std::size_t CountReachablePoints(const std::vector<int> & cost_map, int max_steps)
{
	std::size_t count = 0;
	for (auto c : cost_map)
	{
		if (c == max_steps || (c < max_steps && c % 2 == max_steps % 2))
		{
			count++;
		}
	}
	return count;
}

class Day21Solution : public AoCSolution
{
public:
	void RunFirstPart(std::string input_file) override
	{
		auto input = GetInput(input_file);
		auto grid = input.first;
		auto width = input.second.first;
		auto height = input.second.second;
		int target_value = 64;
		//find the "S" token
		int start_width = 0, start_height = 0;
		for (int i = 0; i < grid.size(); i++)
		{
			if (grid[i] == 'S')
			{
				start_width = i % width;
				start_height = i / width;
				break;
			}
		}

		auto cost_map = GetCostMap(grid, width, height, target_value, { {start_width, start_height} });
		std::cout << "There are " << CountReachablePoints(cost_map, target_value) << " locations that can be reached in " << target_value << " steps" << std::endl;

	}

	void RunSecondPart(std::string input_file) override
	{
		auto input = GetInput(input_file);
		auto grid = input.first;
		auto width = input.second.first;
		auto height = input.second.second;
		grid = MultiplyGrid(grid, width, height, 5);
		width *= 5;
		height *= 5;
		//find the "S" token
		int start_width = 0, start_height = 0;
		for (int i = 0; i < grid.size(); i++)
		{
			if (grid[i] == 'S')
			{
				start_width = width / 2;
				start_height = height / 2;
				break;
			}
		}

		//Get reachable points after 65, 65+131,65+131+131
		std::vector<double> x = { 65, 196, 327 };
			std::vector<double> y = { (double)CountReachablePoints(GetCostMap(grid, width, height, 65, { {start_width, start_height} }), 65),
			(double)CountReachablePoints(GetCostMap(grid, width, height, 196, { {start_width, start_height} }), 196),
			(double)CountReachablePoints(GetCostMap(grid, width, height, 327, { {start_width, start_height} }), 327) };

		//from here we can use lagrange interpolation to find the solution
		std::cout << "The solution is " << (long long)lagrangeInterpolate(x, y, 26501365) << std::endl;
	}
};

