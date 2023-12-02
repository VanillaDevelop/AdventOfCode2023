#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main()
{
	std::ifstream infile("Inputs/day1-1.txt");
	std::string line;

	int final_sum = 0;

	while (std::getline(infile, line))
	{
		int l_pointer = 0;
		int r_pointer = line.size() - 1;
		bool found_left = false;
		bool found_right = false;

		while (true)
		{
			if (isdigit(line[l_pointer]) && !found_left)
			{
				found_left = true;
				final_sum += (line[l_pointer] - '0') * 10;
			}
			if (isdigit(line[r_pointer]) && !found_right)
			{
				found_right = true;
				final_sum += (line[r_pointer] - '0');
			}
			if (found_left && found_right)
			{
				break;
			}
			l_pointer++;
			r_pointer--;
		}
	}

	std::cout << final_sum << std::endl;
}