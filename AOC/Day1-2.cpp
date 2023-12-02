#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>


int find_occurrence_of_substring(std::string str, std::string sub, bool first=true)
{
	//position of substring occurrence start/end
	int pos = -1;
	//position of last occurrence
	std::string::size_type n = 0;
	//search for substring from n to end of string
	while ((n = str.find(sub, n)) != std::string::npos)
	{
		//if we are looking for the last occurrence, make sure we take the end index
		if (!first)
		{
			n += sub.size() - 1;
		}
		//set position of occurrence to n
		pos = n;

		//if first, we can stop looking, otherwise keep looking
		if(first)
		{
			break;
		}
	}
	return pos;
}

int main()
{
	std::ifstream infile("Inputs/day1-1.txt");
	std::string line;
	std::map<std::string, int> digit_map = {
		{"one", 1},
		{"two", 2},
		{"three", 3},
		{"four", 4},
		{"five", 5},
		{"six", 6},
		{"seven", 7},
		{"eight", 8},
		{"nine", 9}
	};

	int final_sum = 0;

	while (std::getline(infile, line))
	{
		int l_pointer = 0;
		int r_pointer = line.size() - 1;
		bool found_left = false;
		bool found_right = false;
		//store index of first/last fake digit, if exists
		int left_fakedigit = line.size();
		int right_fakedigit = -1;
		//store value of digits
		int left = 0;
		int right = 0;

		for (std::pair<std::string, int> digit : digit_map)
		{
			int pos = find_occurrence_of_substring(line, digit.first);
			if (pos != -1)
			{
				if (pos < left_fakedigit)
				{
					left_fakedigit = pos;
					left = digit.second;
				}
			}
			pos = find_occurrence_of_substring(line, digit.first, false);
			if (pos != -1)
			{
				if (pos > right_fakedigit)
				{
					right_fakedigit = pos;
					right = digit.second;
				}
			}
		}
		for (int i = 0; i < left_fakedigit; i++)
		{
			if (isdigit(line[i]))
			{
				left = line[i] - '0';
				break;
			}
		}
		for (int i = line.size() - 1; i > right_fakedigit; i--)
		{
			if (isdigit(line[i]))
			{
				right = line[i] - '0';
				break;
			}
		}

		final_sum += left * 10 + right;
	}

	std::cout << final_sum << std::endl;
}