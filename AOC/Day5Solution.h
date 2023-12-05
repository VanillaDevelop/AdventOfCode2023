#pragma once
#include "AoCSolution.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>

struct Mapping
{
	size_t dest;
	size_t source;
	size_t range;
};

class Day5Solution : public AoCSolution
{
public:
	void RunFirstPart(std::string input_file) override
	{
		std::ifstream infile(input_file);
		std::string line;

		//get the seeds from the first input
		std::getline(infile, line);
		std::vector<size_t> seeds = getSeeds(line);

		//get the maps from the remaining lines 
		std::vector<std::vector<Mapping>> maps = getMaps(infile);

		//get the destinations
		std::vector<size_t> destinations = getDestinations(seeds, maps);

		//print the minimum destination
		size_t min = *std::min_element(destinations.begin(), destinations.end());

		std::cout << "Minimum destination: " << min << std::endl;
	}
	void RunSecondPart(std::string input_file) override
	{
		std::ifstream infile(input_file);
		std::string line;

		//get the seed pairs (start, range) from the first input
		std::getline(infile, line);
		std::vector<size_t> seeds = getSeedRanges(line);

		//get the maps from the remaining lines 
		std::vector<std::vector<Mapping>> maps = getMaps(infile);

		//get the destinations
		std::vector<size_t> destinations = getRangeDestinationsRecursive(seeds, maps);

		//print the minimum destination
		size_t min = *std::min_element(destinations.begin(), destinations.end());

		std::cout << "Minimum destination: " << min << std::endl;
	}

private:
	/// <summary>
	/// Get all seed IDs from the first line of the input file
	/// </summary>
	/// <param name="header">A string containing the first line of the input file</param>
	/// <returns>A vector of all seed IDs in the header line</returns>
	std::vector<size_t> getSeeds(std::string header)
	{
		std::string seeds = header.substr(7, header.size() - 7) + " "; //add a space to the end so that the last seed is also parsed
		std::vector<size_t> seed_vector;
		size_t start_idx = 0;
		size_t next_idx = 0;

		while (start_idx < seeds.size() && (next_idx = seeds.find(" ", start_idx)) != std::string::npos)
		{
			size_t result;
			std::stringstream(seeds.substr(start_idx, next_idx - start_idx)) >> result;
			seed_vector.push_back(result);
			start_idx = next_idx + 1;
		}

		return seed_vector;
	}

	/// <summary>
	/// Get all seed ranges from the first line of the input file (Part 2)
	/// </summary>
	/// <param name="header">A string containing the first line of the input file</param>
	/// <returns>A vector of all seed ranged (begin, end) in the header file.</returns>
	std::vector<size_t> getSeedRanges(std::string header)
	{
		std::string seeds = header.substr(7, header.size() - 7) + " "; //add a space to the end so that the last seed is also parsed
		std::vector<size_t> seed_vector;
		size_t start_idx = 0;
		size_t next_idx = 0;

		while (start_idx < seeds.size() && (next_idx = seeds.find(" ", start_idx)) != std::string::npos)
		{
			size_t result;
			std::stringstream(seeds.substr(start_idx, next_idx - start_idx)) >> result;
			seed_vector.push_back(result);
			start_idx = next_idx + 1;
		}

		//transform from start,range to start,end
		for (size_t i = 1; i < seed_vector.size(); i += 2)
		{
			seed_vector[i] += seed_vector[i - 1] - 1;
		}

		return seed_vector;
	}

	/// <summary>
	/// Parse the remaining lines of the input file into a vector of mappings
	/// </summary>
	/// <param name="infile">The remaining lines of the input file</param>
	/// <returns>A list of list of mappings, each primary list item contains all mappings between a seed and the next target</returns>
	std::vector<std::vector<Mapping>> getMaps(std::ifstream& infile)
	{
		std::vector<std::vector<Mapping>> maps;
		std::string line;

		std::vector<Mapping> current_map;

		while (std::getline(infile, line))
		{
			if (line.size() == 0 && current_map.size() > 0)
			{
				//designate start of a new map
				maps.push_back(current_map);
				current_map.clear();
				continue;
			}

			if (!isdigit(line[0]))
			{
				//we don't care about the naming
				continue;
			}

			//otherwise, create a mapping based on the line
			int first_space = line.find(" ");
			int second_space = line.find(" ", first_space + 1);
			size_t dest, src, range;
			std::stringstream(line.substr(0, first_space)) >> dest;
			std::stringstream(line.substr(first_space + 1, second_space - first_space - 1)) >> src;
			std::stringstream(line.substr(second_space + 1, line.size() - second_space - 1)) >> range;
			Mapping mapping = {
				dest,
				src,
				range
			};
			current_map.push_back(mapping);
		}
		//at EOL, we need to push the last map
		maps.push_back(current_map);

		return maps;
	}

	/// <summary>
	/// Given seeds and mappings, find the final destination for each seed
	/// </summary>
	/// <param name="seeds">A list of seeds</param>
	/// <param name="maps">A list of list of mappings</param>
	/// <returns>A list of seed destinations</returns>
	std::vector<size_t> getDestinations(std::vector<size_t> seeds, std::vector<std::vector<Mapping>> maps)
	{
		std::vector<size_t> destinations;

		//for each seed, go through the maps and find the final destination
		for (size_t seed : seeds)
		{
			size_t current = seed;
			for (std::vector<Mapping> map : maps)
			{
				for (Mapping mapping : map)
				{
					if (mapping.source <= current && mapping.source + mapping.range > current)
					{
						current = mapping.dest + (current - mapping.source);
						break;
					}
				}
			}
			destinations.push_back(current);
		}

		return destinations;
	}

	/// <summary>
	/// Recursively transform a set of seed ranges into a new set of ranges based on the next mapping
	/// </summary>
	/// <param name="seed_pairs">The current set of seed ranges</param>
	/// <param name="remaining_maps">The remaining mappings</param>
	/// <returns>A set of [start, end] ranges for the final destinations</returns>
	std::vector<size_t> getRangeDestinationsRecursive(std::vector<size_t> seed_pairs, std::vector<std::vector<Mapping>> remaining_maps)
	{
		std::cout << "Current Iteration Ranges: " << seed_pairs.size() / 2 << std::endl;
		std::vector<size_t> remaining_seeds = seed_pairs;
		std::vector<size_t> next_seed_pairs;
		std::vector<Mapping> current_map = remaining_maps[0];

		while (remaining_seeds.size() > 0)
		{
			//take the first range
			size_t start = remaining_seeds[0];
			size_t end = remaining_seeds[1];
			remaining_seeds.erase(remaining_seeds.begin(), remaining_seeds.begin() + 2);

			//find the mapping that contains the start or end
			bool found = false;
			for (const Mapping mapping : current_map)
			{
				size_t mapping_start = mapping.source;
				size_t mapping_end = mapping.source + mapping.range - 1;
				size_t transform = mapping.dest - mapping.source;

				//range contains start
				if (mapping_start <= start && mapping_end >= end)
				{
					//if the whole range is contained in the mapping, add it to next_seed_pairs
					if (mapping_end >= end)
					{
						next_seed_pairs.push_back(start + transform);
						next_seed_pairs.push_back(end + transform);
					}
					//otherwise, add until mapping_end to next_seed pairs, rest to remaining_seeds
					else
					{
						next_seed_pairs.push_back(start + transform);
						next_seed_pairs.push_back(mapping_end + transform);
						remaining_seeds.push_back(mapping_end + 1);
						remaining_seeds.push_back(end);
					}
					found = true;
					break;
				}
				//range contains end
				else if (mapping_start <= end && mapping_end >= end)
				{
					//can't be the full range, otherwise start would be in it
					next_seed_pairs.push_back(mapping_start + transform);
					next_seed_pairs.push_back(end + transform);
					remaining_seeds.push_back(start);
					remaining_seeds.push_back(mapping_start - 1);
					found = true;
					break;
				}
			}

			//if neither start nor end were found, add the whole range to next seeds
			if (!found)
			{
				next_seed_pairs.push_back(start);
				next_seed_pairs.push_back(end);
			}
		}

		//if there are more maps, recurse
		if (remaining_maps.size() > 1)
		{
			return getRangeDestinationsRecursive(next_seed_pairs, std::vector<std::vector<Mapping>>(remaining_maps.begin() + 1, remaining_maps.end()));
		}
		//otherwise, return the destinations
		return next_seed_pairs;
	}
};

