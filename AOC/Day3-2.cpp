#include <iostream>
#include <string>
#include <fstream>
#include <vector>

void set_adjacent_id(int* truth_map, int index, int line_count, int char_count, int gear_index)
{
	bool is_left = index % char_count == 0;
	bool is_right = index % char_count == char_count - 1;
	bool is_top = index < char_count;
	bool is_bottom = index >= (line_count - 1) * char_count;
	truth_map[index] = gear_index;
	if (!is_left) truth_map[index - 1] = gear_index;
	if (!is_right) truth_map[index + 1] = gear_index;
	if (!is_top) truth_map[index - char_count] = gear_index;
	if (!is_bottom) truth_map[index + char_count] = gear_index;
	if (!is_left && !is_top) truth_map[index - char_count - 1] = gear_index;
	if (!is_left && !is_bottom) truth_map[index + char_count - 1] = gear_index;
	if (!is_right && !is_top) truth_map[index - char_count + 1] = gear_index;
	if (!is_right && !is_bottom) truth_map[index + char_count + 1] = gear_index;
}

int main()
{
	std::ifstream infile("Inputs/day3-1.txt");
	std::string line;

	//Get line count by counting number of \n symbols until EOF.
	//Last line does not contain a newline character (lol)
	int line_count = std::count(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>(), '\n') + 1;
	//reset pointer back to start of file
	infile.clear();
	infile.seekg(0, std::ios::beg);
	//read one line to see how many characters are in the line
	std::getline(infile, line);
	int char_count = line.size();
	//reset pointer back to start of file
	infile.clear();
	infile.seekg(0, std::ios::beg);

	//create a 2-dimensional array of size line_count x char_count for storing the values
	int* value_map = new int[line_count * char_count];
	//and for storing the truth values
	int* truth_map = new int[line_count * char_count];
	//default values
	for (int i = 0; i < line_count * char_count; i++)
	{
		value_map[i] = -1;
		truth_map[i] = 0;
	}


	//read each line and store the values in the value_map and truth_maps
	int line_index = 0;
	int gear_index = 1;
	while (std::getline(infile, line))
	{
		for (int i = 0; i < line.size(); i++)
		{
			if (isdigit(line[i]))
			{
				value_map[line_index * char_count + i] = line[i] - '0';
			}
			else if (line[i] == '*')
			{
				set_adjacent_id(truth_map, line_index * char_count + i, line_count, char_count, gear_index);
				gear_index++;
			}
		}
		line_index++;
	}

	//debug: print truth map
	if (true)
	{
		for (int i = 0; i < line_count; i++)
		{
			for (int j = 0; j < char_count; j++)
			{
				std::cout << truth_map[i * char_count + j];
			}
			std::cout << std::endl;
		}
	}

	//debug: print value map
	if (true)
	{
		for (int i = 0; i < line_count; i++)
		{
			for (int j = 0; j < char_count; j++)
			{
				std::cout << (value_map[i * char_count + j] == -1 ? "." : std::to_string(value_map[i * char_count + j]));
			}
			std::cout << std::endl;
		}
	}

	//create array for the number of gears
	int* gear_sums = new int[gear_index];
	int* gear_counts = new int[gear_index];
	for (int i = 0; i < gear_index; i++)
	{
		gear_sums[i] = 1;
		gear_counts[i] = 0;
	}

	int final_sum = 0;
	int add_current = 0;
	int current_sum = 0;
	for (int i = 0; i < line_count; i++)
	{
		if (add_current != 0)
		{
			gear_sums[add_current] *= current_sum;
			gear_counts[add_current]++;
		}
		current_sum = 0;
		add_current = false;

		for (int j = 0; j < char_count; j++)
		{
			if (value_map[i * char_count + j] != -1)
			{
				if (truth_map[i * char_count + j] != 0)
				{
					add_current = truth_map[i * char_count + j];
				}
				current_sum *= 10;
				current_sum += value_map[i * char_count + j];
			}
			else
			{
				//if some digit was true, add to the final sum and reset
				if (add_current != 0)
				{
					gear_sums[add_current] *= current_sum;
					gear_counts[add_current]++;
				}
				current_sum = 0;
				add_current = false;
			}
		}
	}
	if (add_current != 0)
	{
		gear_sums[add_current] *= current_sum;
		gear_counts[add_current]++;
	}

	for (int i = 1; i < gear_index; i++)
	{
		if (gear_counts[i] == 2)
		{
			final_sum += gear_sums[i];
		}
	}

	std::cout << "Final sum: " << final_sum << std::endl;

	delete[] value_map;
	delete[] truth_map;
	delete[] gear_sums;
	delete[] gear_counts;
}