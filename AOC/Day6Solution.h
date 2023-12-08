#pragma once
#include "AoCSolution.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>


class Day6Solution : public AoCSolution
{
	public:
		void RunFirstPart(std::string input_file) override
		{
			//Parse input into array of times and distances for each race
			std::pair<std::vector<int>, std::vector<int>> input = readInput(input_file);
			std::vector<int> times = input.first;
			std::vector<int> distances = input.second;

			//Get the distance ranges for each race
			std::vector<std::pair<int, int>> ranges = getDistanceRanges(times, distances);

			//Multiply ranges together to get the answer
			int answer = 1;
			for (int i = 0; i < ranges.size(); i++)
			{
				answer *= (ranges[i].second - ranges[i].first) + 1;
			}

			std::cout << "Answer: " << answer << std::endl;
		}

		void RunSecondPart(std::string input_file) override
		{
			//Parse input into array of times and distances for each race
			std::pair<std::size_t, std::size_t> input = readInputSingle(input_file);

			std::size_t time = input.first;
			std::size_t distance = input.second;

			std::pair<std::size_t, std::size_t> charge_times = findSingleLargeChargeTime(time, distance);

			std::cout << "Answer: " << (charge_times.second - charge_times.first + 1) << std::endl;
		}

	private:
		/// <summary>
		/// Parse the input into a pair of vectors, one for times and one for distances in each race
		/// </summary>
		/// <param name="input_file">The input file</param>
		/// <returns>A pair of vectors, the first item representing the time for each race, the second representing the distance</returns>
		std::pair<std::vector<int>, std::vector<int>> readInput(std::string input_file)
		{
			std::vector<int> times;
			std::vector<int> distances;
			std::regex time_regex("[0-9]+");

			std::ifstream file(input_file);
			std::string line;
			
			//get times from first line
			std::getline(file, line);
			std::sregex_iterator time_it(line.begin(), line.end(), time_regex);

			while (time_it != std::sregex_iterator())
			{
				times.push_back(std::stoi((*time_it).str()));
				time_it++;
			}

			//get distances from second line
			std::getline(file, line);
			std::sregex_iterator distance_it(line.begin(), line.end(), time_regex);

			while (distance_it != std::sregex_iterator())
			{
				distances.push_back(std::stoi((*distance_it).str()));
				distance_it++;
			}

			return std::pair<std::vector<int>, std::vector<int>>(times, distances);
		}

		/// <summary>
		/// Parse the input into a pair of time and distance (2nd part)
		/// </summary>
		/// <param name="input_file">The input file</param>
		/// <returns>A pair, the first item representing the time, the second representing the distance</returns>
		std::pair<std::size_t, std::size_t> readInputSingle(std::string input_file)
		{
			std::vector<int> times;
			std::vector<int> distances;
			std::regex time_regex("[0-9]+");

			std::ifstream file(input_file);
			std::string line;

			std::string time;
			std::string distance;

			//get times from first line
			std::getline(file, line);
			std::sregex_iterator time_it(line.begin(), line.end(), time_regex);

			while (time_it != std::sregex_iterator())
			{
				time += time_it->str();
				time_it++;
			}

			//get distances from second line
			std::getline(file, line);
			std::sregex_iterator distance_it(line.begin(), line.end(), time_regex);

			while (distance_it != std::sregex_iterator())
			{
				distance += distance_it->str();
				distance_it++;
			}

			std::stringstream sstream_t(time);
			std::stringstream sstream_d(distance);

			std::size_t time_int;
			std::size_t distance_int;

			sstream_t >> time_int;
			sstream_d >> distance_int;

			return std::pair<std::size_t, std::size_t>(time_int, distance_int);
		}

		std::vector<std::pair<int, int>> getDistanceRanges(std::vector<int> times, std::vector<int> distances)
		{
			std::vector<std::pair<int, int>> ranges;
			for (int i = 0; i < times.size(); i++)
			{
				std::pair<int, int> charge_times = findChargeTimes(times[i], distances[i]);
				int min_charge_time = charge_times.first;
				int max_charge_time = charge_times.second;
				ranges.push_back(std::pair<int, int>(min_charge_time, max_charge_time));
			}

			return ranges;
		}

		/// <summary>
		/// Find the minimum/maximum time in which the boat can win the race
		/// </summary>
		/// <param name="race_time">The total race time</param>
		/// <param name="distance_to_beat">The distance that needs to be beat</param>
		/// <returns>A pair containing the minimum and maximum charge times that beat the target distance.</returns>
		std::pair<int,int> findChargeTimes(int race_time, int distance_to_beat)
		{
			// 0 and max_time can never be correct, because the boat will not move
			int min_charge_time = 1;
			int max_charge_time = race_time - 1;

			while(calculateDistance(min_charge_time, race_time) <= distance_to_beat)
			{
				min_charge_time++;
			}

			while (calculateDistance(max_charge_time, race_time) <= distance_to_beat)
			{
				max_charge_time--;
			}

			return std::pair<int,int>(min_charge_time, max_charge_time);
		}

		/// <summary>
		/// Basically the same function as findChargeTimes, but for a single large number...
		/// </summary>
		/// <param name="race_time"></param>
		/// <param name="distance_to_beat"></param>
		/// <returns></returns>
		std::pair<std::size_t, std::size_t> findSingleLargeChargeTime(std::size_t race_time, std::size_t distance_to_beat)
		{
			// 0 and max_time can never be correct, because the boat will not move
			std::size_t min_charge_time = 1;
			std::size_t max_charge_time = race_time - 1;

			while ((race_time - min_charge_time) * min_charge_time <= distance_to_beat)
			{
				min_charge_time++;
			}

			while ((race_time - max_charge_time) * max_charge_time <= distance_to_beat)
			{
				max_charge_time--;
			}

			return std::pair<std::size_t, std::size_t>(min_charge_time, max_charge_time);
		}

		/// <summary>
		/// For a given charge time and race time, calculate the boat distance travelled
		/// </summary>
		/// <param name="charge_time">The amount of time we have charged the boat</param>
		/// <param name="race_time">The amount of time for the race</param>
		/// <returns></returns>
		int calculateDistance(int charge_time, int race_time)
		{
			return (race_time - charge_time) * charge_time;
		}
};

