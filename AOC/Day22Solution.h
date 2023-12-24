#pragma once
#include "AoCSolution.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <set>

struct Cube {
	int x_1;
	int y_1;
	int z_1;
	int x_2;
	int y_2;
	int z_2;

	//< operator by z axis
	bool operator<(const Cube & other) const
	{
		return z_1 < other.z_1;
	}
};

std::vector<Cube> GetCubes(const std::string & input_file)
{
	std::vector<Cube> cubes;
	std::ifstream file(input_file);
	std::string line;
	while (std::getline(file, line))
	{
		std::size_t sep = line.find('~');
		std::stringstream ss_in(line.substr(0,sep));
		std::stringstream ss_out(line.substr(sep+1, line.size() - sep));
		std::string token;
		std::vector<std::string> tokens;
		while (std::getline(ss_in, token, ','))
		{
			tokens.push_back(token);
		}
		while (std::getline(ss_out, token, ','))
		{
			tokens.push_back(token);
		}
		Cube cube;
		cube.x_1 = std::stoi(tokens[0]);
		cube.y_1 = std::stoi(tokens[1]);
		cube.z_1 = std::stoi(tokens[2]);
		cube.x_2 = std::stoi(tokens[3]);
		cube.y_2 = std::stoi(tokens[4]);
		cube.z_2 = std::stoi(tokens[5]);
		cubes.push_back(cube);
	}
	return cubes;
}

bool IsSpaceFree(const Cube & cube, const std::vector<std::map<std::pair<int, int>, int>> & cube_layers)
{
	for (int z = cube.z_1; z <= cube.z_2; ++z)
	{
		for (int y = cube.y_1; y <= cube.y_2; ++y)
		{
			for (int x = cube.x_1; x <= cube.x_2; ++x)
			{
				if (cube_layers[z].find(std::make_pair(x, y)) != cube_layers[z].end())
				{
					return false;
				}
			}
		}
	}
	return true;
}

void PlaceCube(const Cube & cube, std::vector<std::map<std::pair<int, int>, int>>& cube_layers, int cube_id)
{
	for (int z = cube.z_1; z <= cube.z_2; ++z)
	{
		for (int y = cube.y_1; y <= cube.y_2; ++y)
		{
			for (int x = cube.x_1; x <= cube.x_2; ++x)
			{
				cube_layers[z][std::make_pair(x, y)] = cube_id;
			}
		}
	}
}

std::set<int> CubeSupportedBy(const Cube & cube, const std::vector<std::map<std::pair<int, int>, int>>& cube_layers, int this_cube)
{
	std::set<int> support_cubes;
	for (int z = cube.z_1; z <= cube.z_2; z++)
	{
		//if z = 1, skip (ground supports)
		if (z == 1)
		{
			continue;
		}
		for (int y = cube.y_1; y <= cube.y_2; y++)
		{
			for (int x = cube.x_1; x <= cube.x_2; x++)
			{
				//if we find a cube directly below
				if (cube_layers[z-1].find(std::make_pair(x, y)) != cube_layers[z-1].end())
				{
					//if it is not the same cube, add it to the list of cubes supporting this cube
					if (cube_layers[z - 1].at(std::make_pair(x, y)) != this_cube)
					{
						support_cubes.insert(cube_layers[z - 1].at(std::make_pair(x, y)));
					}
				}
			}
		}
	}
	return support_cubes;
}


class Day22Solution : public AoCSolution
{
	void RunFirstPart(std::string input_file) override
	{
		//Get cubes from input file
		auto cubes = GetCubes(input_file);
		//sort cubes by z axis
		std::sort(cubes.begin(), cubes.end());

		//The vector represents the z axis, the pair represents the x,y axis, and the int represents the cube index
		//create n layers, up to the max z value
		std::vector<std::map<std::pair<int, int>, int>> cube_layers = std::vector<std::map<std::pair<int, int>, int>>(cubes[cubes.size() - 1].z_2 + 1);

		//iterate through cubes
		for (int i = 0; i < cubes.size(); i++)
		{
			Cube current_cube = cubes[i];
			//iterate through z axis
			while (IsSpaceFree(current_cube, cube_layers) && current_cube.z_1 > 0)
			{
				current_cube.z_1--;
				current_cube.z_2--;
			}
			//Place cube at lowest z value (+1 since the current one is not free)
			current_cube.z_1++;
			current_cube.z_2++;
			cubes[i] = current_cube;
			PlaceCube(current_cube, cube_layers, i);
		}

		//iterate over each placed cube and see what cubes support it - if only one cube supports it, we cannot remove it
		std::set<int> cant_remove;
		for (int i = 0; i < cubes.size(); i++)
		{
			auto supports = CubeSupportedBy(cubes[i], cube_layers, i);
			if (supports.size() == 1)
			{
				cant_remove.insert(*supports.begin());
			}
		}

		//Print the number of cubes that can be removed
		std::cout << "Num cubes that can be removed: " << cubes.size() - cant_remove.size() << std::endl;
	}

	void RunSecondPart(std::string input_file) override
	{
		//Get cubes from input file
		auto cubes = GetCubes(input_file);
		//sort cubes by z axis
		std::sort(cubes.begin(), cubes.end());

		//The vector represents the z axis, the pair represents the x,y axis, and the int represents the cube index
		//create n layers, up to the max z value
		std::vector<std::map<std::pair<int, int>, int>> cube_layers = std::vector<std::map<std::pair<int, int>, int>>(cubes[cubes.size() - 1].z_2 + 1);

		//iterate through cubes
		for (int i = 0; i < cubes.size(); i++)
		{
			Cube current_cube = cubes[i];
			//iterate through z axis
			while (IsSpaceFree(current_cube, cube_layers) && current_cube.z_1 > 0)
			{
				current_cube.z_1--;
				current_cube.z_2--;
			}
			//Place cube at lowest z value (+1 since the current one is not free)
			current_cube.z_1++;
			current_cube.z_2++;
			cubes[i] = current_cube;
			PlaceCube(current_cube, cube_layers, i);
		}

		//iterate over each placed cube to build a "dependency graph"
		std::map<int, std::set<int>> cube_dependencies;
		for (int i = 0; i < cubes.size(); i++)
		{
			auto supports = CubeSupportedBy(cubes[i], cube_layers, i);
			for (auto support : supports)
			{
				cube_dependencies[i].insert(support);
			}
		}

		long long falling_bricks = 0;
		for(int i = 0; i < cubes.size(); i++)
		{
			int removed_cubes = 0;

			//make a copy of the dependency graph
			auto dependencies = cube_dependencies;
			//determine how many cubes would fall if this cube was removed
			std::set<int> cubes_to_remove;
			cubes_to_remove.insert(i);

			//while we have cubes to remove
			while (cubes_to_remove.size() > 0)
			{
				//get the first cube to remove
				int cube_to_remove = *cubes_to_remove.begin();
				//remove it from the list
				cubes_to_remove.erase(cubes_to_remove.begin());
				//remove it from the dependency graph
				dependencies.erase(cube_to_remove);
				//increment the number of cubes removed
				removed_cubes++;
				//iterate over the dependency graph
				for (auto & dependency : dependencies)
				{
					//if the cube to remove is in the dependency list, remove it
					if (dependency.second.find(cube_to_remove) != dependency.second.end())
					{
						dependency.second.erase(cube_to_remove);
					}
					//if the dependency list is empty, add it to the list of cubes to remove
					if (dependency.second.size() == 0)
					{
						cubes_to_remove.insert(dependency.first);
					}
				}
			}

			//update the total number of falling bricks (not counting the original one)
			falling_bricks += removed_cubes - 1;
		}

		//Print the number of cubes that can be removed
		std::cout << "Total cubes removed: " << falling_bricks << std::endl;
	}
};

