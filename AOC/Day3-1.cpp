#include <iostream>
#include <string>
#include <fstream>
#include <vector>

void set_adjacent_true(bool* truth_map, int index, int line_count, int char_count)
{
	bool is_left = index % char_count == 0;
	bool is_right = index % char_count == char_count - 1;
	bool is_top = index < char_count;
	bool is_bottom = index >= (line_count - 1) * char_count;
	truth_map[index] = true;
	if (!is_left) truth_map[index - 1] = true;
	if (!is_right) truth_map[index + 1] = true;
	if (!is_top) truth_map[index - char_count] = true;
	if (!is_bottom) truth_map[index + char_count] = true;
	if (!is_left && !is_top) truth_map[index - char_count - 1] = true;
	if (!is_left && !is_bottom) truth_map[index + char_count - 1] = true;
	if (!is_right && !is_top) truth_map[index - char_count + 1] = true;
	if (!is_right && !is_bottom) truth_map[index + char_count + 1] = true;
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
	bool* truth_map = new bool[line_count * char_count];
	//default values
	for (int i = 0; i < line_count * char_count; i++)
	{
		value_map[i] = -1;
		truth_map[i] = false;
	}


	//read each line and store the values in the value_map and truth_maps
	int line_index = 0;
	while (std::getline(infile, line))
	{
		for (int i = 0; i < line.size(); i++)
		{
			if (isdigit(line[i]))
			{
				value_map[line_index * char_count + i] = line[i] - '0';
			}
			else if (line[i] != '.')
			{
				set_adjacent_true(truth_map, line_index * char_count + i, line_count, char_count);
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
				std::cout << (truth_map[i * char_count + j] ? "T" : "F");
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

	int final_sum = 0;
	bool add_current = false;
	int current_sum = 0;
	for (int i = 0; i < line_count; i++)
	{
		if(add_current) final_sum += current_sum;
		current_sum = 0;
		add_current = false;

		for (int j = 0; j < char_count; j++)
		{
			if (value_map[i * char_count + j] != -1)
			{
				if (truth_map[i * char_count + j])
				{
					add_current = true;
				}

				current_sum *= 10;
				current_sum += value_map[i * char_count + j];
			}
			else
			{
				//if some digit was true, add to the final sum and reset
				if (add_current)
				{
					final_sum += current_sum;
				}
				current_sum = 0;
				add_current = false;
			}
		}
	}
	if (add_current)
	{
		final_sum += current_sum;
	}

	std::cout << "Final sum: " << final_sum << std::endl;

	delete[] value_map;
	delete[] truth_map;
}