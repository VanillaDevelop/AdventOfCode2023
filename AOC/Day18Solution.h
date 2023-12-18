#pragma once
#include "AoCSolution.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <queue>

//forward declarations
std::pair<std::vector<std::string>, std::pair<int, int>> GetInput(const std::string & input_file);
void PrintMap(const std::vector<std::string> & map, int width, int height);
void FloodFill(std::vector<std::string> & map, int width, int height, int x, int y);
std::size_t GetCubeSize(std::vector<std::string> & map, int width, int height);
std::vector < std::pair<std::size_t, std::size_t>> GetPartTwoInput(const std::string & input_file);
std::size_t Shoelace(const std::vector<std::pair<std::size_t, std::size_t>> & points);

class Day18Solution : public AoCSolution
{
	void RunFirstPart(std::string input_file) override
	{
		auto input = GetInput(input_file);
		std::vector<std::string> map = input.first;
		int width = input.second.first;
		int height = input.second.second;

		FloodFill(map, width, height, 0, 0);
		FloodFill(map, width, height, width - 1, 0);
		FloodFill(map, width, height, 0, height - 1);
		FloodFill(map, width, height, width - 1, height - 1);
		PrintMap(map, width, height);

		std::size_t cubeSize = GetCubeSize(map, width, height);
		std::cout << "The lagoon size is: " << cubeSize << std::endl;
	}

	void RunSecondPart(std::string input_file) override
	{
		auto input = GetPartTwoInput(input_file);
		std::size_t cubeSize = Shoelace(input);
		std::cout << "The lagoon size is: " << cubeSize << std::endl;
	}
};

std::size_t Shoelace(const std::vector<std::pair<std::size_t, std::size_t>> & points)
{
	std::size_t area = 0;
	std::size_t perimeter = 0;
	for (std::size_t i = 0; i < points.size(); i++)
	{
		int j = (i + 1) % points.size();
		area += points[i].first * points[j].second;
		area -= points[i].second * points[j].first;
	}
	for (std::size_t i = 0; i < points.size(); i++)
	{
		int j = (i + 1) % points.size();
		perimeter += std::abs((long long)points[i].first - (long long)points[j].first) + std::abs((long long)points[i].second - (long long)points[j].second);
	}

	return std::abs(area / 2.0) + (perimeter / 2 + 1);
}

void FloodFill(std::vector<std::string> & map, int width, int height, int startX, int startY)
{
		
	std::queue<std::pair<int, int>> queue;
	queue.push({ startX, startY });

	while (!queue.empty())
	{
		int x = queue.front().first;
		int y = queue.front().second;
		queue.pop();

		if (x < 0 || x >= width || y < 0 || y >= height)
		{
			continue;
		}

		if (map[y * width + x] != ".")
		{
			continue;
		}

		map[y * width + x] = "O";

		queue.push({ x - 1, y });
		queue.push({ x + 1, y });
		queue.push({ x, y - 1 });
		queue.push({ x, y + 1 });
	}
}

std::size_t GetCubeSize(std::vector<std::string> & map, int width, int height)
{
	std::size_t cubeSize = 0;
	for(int i=0;i<height;i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (map[i * width + j] != "O")
			{
				cubeSize++;
			}
		}
	}
	return cubeSize;
}

/// <summary>
/// Print a binary version of the map to the console
/// </summary>
void PrintMap(const std::vector<std::string>& map, int width, int height)
{
	for (int i = 0; i < height; i++)
	{
		std::cout << std::endl;
		for (int j = 0; j < width; j++)
		{
			if (map[i * width + j].size() > 1)
			{
				std::cout << "#";
			}
			else
			{
				std::cout << map[i * width + j];
			}
		}
	}
	std::cout << std::endl;
}

std::vector <std::pair<std::size_t, std::size_t>> GetPartTwoInput(const std::string & input_file)
{
	std::vector < std::pair<std::size_t, std::size_t>> output;
	std::ifstream input(input_file);

	std::size_t x = 0, y = 0;
	output.push_back(std::make_pair(x, y));

	char direction;
	int distance;
	std::string fake_color;

	while (input >> direction >> distance >> fake_color)
	{
		std::stringstream ss;
		ss << std::hex << fake_color.substr(2, fake_color.size() - 4);
		std::size_t value = stoll(ss.str(), nullptr, 16);
		int direction = fake_color[fake_color.size() - 2] - '0';

		switch (direction)
		{
		case 0:
			x += value;
			break;

		case 1:
			y += value;
			break;

		case 2:
			x -= value;
			break;

		case 3:
			y -= value;
		}

		output.push_back(std::make_pair(x, y));
	}
	return output;
}


/// <summary>
/// Given an input file, returns a vector of strings representing the map, and a pair of ints representing the width and height of the map
/// </summary>
/// <param name="input_file">The input file</param>
/// <returns>A vector representing the dig map, and a pair of ints representing width and height.</returns>
std::pair<std::vector<std::string>, std::pair<int,int>> GetInput(const std::string & input_file)
{
	std::vector<std::string> output;
	int x = 0, y = 0;
	int min_x = 0, min_y = 0;
	int max_x = 0, max_y = 0;
	std::map<std::pair<int, int>, std::string> map;

	std::ifstream input(input_file);
	char direction;
	int distance;
	std::string color;

	while (input >> direction >> distance >> color)
	{
		for (int i = 0; i < distance; i++)
		{
			switch (direction)
			{
			case 'U':
				y--;
				break;
			case 'D':
				y++;
				break;
			case 'L':
				x--;
				break;
			case 'R':
				x++;
				break;
			}

			if (x < min_x)
				min_x = x;
			if (x > max_x)
				max_x = x;
			if (y < min_y)
				min_y = y;
			if (y > max_y)
				max_y = y;

			map[std::make_pair(x, y)] = color.substr(1, color.size() - 2);
		}
	}

	//convert map to vector
	for (int i = min_y; i <= max_y; i++)
	{
		for (int j = min_x; j <= max_x; j++)
		{
			if (map.find(std::make_pair(j, i)) == map.end())
			{
				output.push_back(".");
			}
			else
			{
				output.push_back(map.find(std::make_pair(j, i))->second);
			}
		}
	}

	return std::make_pair(output, std::make_pair<int, int>(max_x - min_x + 1, max_y - min_y + 1));
}

