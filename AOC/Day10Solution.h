#pragma once
#include "AoCSolution.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

enum class PIPE_CONNECTION
{
	EMPTY,
	TOP_BOTTOM,
	LEFT_RIGHT,
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_RIGHT,
	START_TILE,
	OUTSIDE,
	INSIDE
};

std::unordered_map <char, PIPE_CONNECTION> connection_map = {
	{'.', PIPE_CONNECTION::EMPTY},
	{'|', PIPE_CONNECTION::TOP_BOTTOM},
	{'-', PIPE_CONNECTION::LEFT_RIGHT},
	{'J', PIPE_CONNECTION::TOP_LEFT},
	{'L', PIPE_CONNECTION::TOP_RIGHT},
	{'7', PIPE_CONNECTION::BOTTOM_LEFT},
	{'F', PIPE_CONNECTION::BOTTOM_RIGHT},
	{'S', PIPE_CONNECTION::START_TILE}
};

//forward declarations
PIPE_CONNECTION* getMap(int & width, int & height, std::string input_file);
PIPE_CONNECTION* get_index(int x, int y, PIPE_CONNECTION* map, int width);
PIPE_CONNECTION getStartingTileDirection(PIPE_CONNECTION* map, int start_x, int start_y, int width, int height);
int get_max_distance(PIPE_CONNECTION* map, int start_x, int start_y, int width, int height);
PIPE_CONNECTION* next_pos(int& prev_direction, int & x, int & y, PIPE_CONNECTION* current, PIPE_CONNECTION* map, int width);
void visualizeMap(PIPE_CONNECTION* map, int width, int height);
PIPE_CONNECTION* createCleanMap(PIPE_CONNECTION* map, int width, int height, int start_x, int start_y);
void paintOutside(PIPE_CONNECTION* map, int width, int height);
void paintEdge(PIPE_CONNECTION* map, int width, int height);
void paintAdjacent(PIPE_CONNECTION* map, int width, int height, int x, int y, int last_direction);

class Day10Solution : public AoCSolution
{
	virtual void RunFirstPart(std::string input_file)
	{
		int width, height;
		PIPE_CONNECTION* map = getMap(width, height, input_file);

		//find position of start tile
		int start_x = 0;
		int start_y = 0;
		for (int i = 0; i < width * height; i++)
		{
			if (map[i] == PIPE_CONNECTION::START_TILE)
			{
				start_x = i % width;
				start_y = i / width;
				break;
			}
		}

		//find direction of start tile
		PIPE_CONNECTION * start_tile = get_index(start_x, start_y, map, width);

		//Set value of start_tile appropriately
		*start_tile = getStartingTileDirection(map, start_x, start_y, width, height);

		//find the max distance
		int max_distance = get_max_distance(map, start_x, start_y, width, height);

		delete map;
		std::cout << "Max Distance: " << max_distance << std::endl;
	}
	virtual void RunSecondPart(std::string input_file)
	{
		int width, height;
		PIPE_CONNECTION* map = getMap(width, height, input_file);

		//find position of start tile
		int start_x = 0;
		int start_y = 0;
		for (int i = 0; i < width * height; i++)
		{
			if (map[i] == PIPE_CONNECTION::START_TILE)
			{
				start_x = i % width;
				start_y = i / width;
				break;
			}
		}

		//find direction of start tile
		PIPE_CONNECTION* start_tile = get_index(start_x, start_y, map, width);

		//Set value of start_tile appropriately
		*start_tile = getStartingTileDirection(map, start_x, start_y, width, height);

		//clean up pipes that aren't part of the loop
		PIPE_CONNECTION* clean_map = createCleanMap(map, width, height, start_x, start_y);
		delete map;

		//paint the outside
		paintOutside(clean_map, width, height);

		//paint along the edge of the outside
		paintEdge(clean_map, width, height);

		//if I had more time to spend on this I would clean this up, but basically here we just ensure that all tiles adjacent to "outside" tiles within a loop
		//are also outside tiles.
		paintOutside(clean_map, width, height);

		//all the remaining empty tiles should be inside
		for (int i = 0; i < width * height; i++)
		{
			if (clean_map[i] == PIPE_CONNECTION::EMPTY)
			{
				clean_map[i] = PIPE_CONNECTION::INSIDE;
			}
		}

		visualizeMap(clean_map, width, height);

		//count the number of inside tiles
		int inside_count = 0;
		for (int i = 0; i < width * height; i++)
		{
			if (clean_map[i] == PIPE_CONNECTION::INSIDE)
			{
				inside_count++;
			}
		}

		//print the result
		std::cout << "Number of inside tiles: " << inside_count << std::endl;

		delete clean_map;
	}
};

void paintEdge(PIPE_CONNECTION* map, int width, int height)
{
	//find the first tile that is north/south and that has an outside tile to the left
	int start_x = 0;
	int start_y = 0;

	for (int i = 0; i < width * height; i++)
	{
		if (map[i] == PIPE_CONNECTION::TOP_BOTTOM)
		{
			if (i % width != 0 && map[i - 1] == PIPE_CONNECTION::OUTSIDE)
			{
				start_x = i % width;
				start_y = i / width;
				break;
			}
		}
	}

	//from this tile, we can iterate along the edge of the loop, going up first
	int x = start_x;
	int y = start_y;
	int last_direction = 1;
	PIPE_CONNECTION* current = get_index(x, y, map, width);

	do
	{
		//based on the tile type and previous direction, we can determine more outside tiles
		paintAdjacent(map, width, height, x, y, last_direction);
		current = next_pos(last_direction, x, y, current, map, width);
	} while (start_x != x || start_y != y);
}

void paintAdjacent(PIPE_CONNECTION* map, int width, int height, int x, int y, int last_direction)
{
	PIPE_CONNECTION * current = get_index(x, y, map, width);
	bool paint_left = false;
	bool paint_right = false;
	bool paint_up = false;
	bool paint_down = false;

	switch (last_direction)
	{
		//if we went up, left is outside
		case 1:
			paint_left = true;
			break;

		case 2:
			//if we went right, up is outside
			paint_up = true;
			break;

		case 3:
			//if we went down, right is outside
			paint_right = true;
			break;

		case 4:
			//if we went left, down is outside
			paint_down = true;
			break;
	}

	//if we are in a corner tile, we also may need to paint additional tiles
	switch (*current)
	{
		case PIPE_CONNECTION::TOP_LEFT:
			//if we went down to get here, we also want to paint the bottom
			if (last_direction == 3)
			{
				paint_down = true;
			}
			break;

		case PIPE_CONNECTION::TOP_RIGHT:
			//if we went left to get here, we also want to paint the left
			if (last_direction == 4)
			{
				paint_left = true;
			}
			break;

		case PIPE_CONNECTION::BOTTOM_LEFT:
			//if we went right to get here, we also want to paint the right
			if (last_direction == 2)
			{
				paint_right = true;
			}
			break;

		case PIPE_CONNECTION::BOTTOM_RIGHT:
			//if we went up to get here, we also want to paint the top
			if (last_direction == 1)
			{
				paint_up = true;
			}
			break;
	}

	//paint the tiles
	if (paint_left && x - 1 >= 0 && *get_index(x - 1, y, map, width) == PIPE_CONNECTION::EMPTY)
	{
		*get_index(x - 1, y, map, width) = PIPE_CONNECTION::OUTSIDE;
	}
	if (paint_right && x + 1 < width && *get_index(x + 1, y, map, width) == PIPE_CONNECTION::EMPTY)
	{
		*get_index(x + 1, y, map, width) = PIPE_CONNECTION::OUTSIDE;
	}
	if (paint_up && y - 1 >= 0 && *get_index(x, y - 1, map, width) == PIPE_CONNECTION::EMPTY)
	{
		*get_index(x, y - 1, map, width) = PIPE_CONNECTION::OUTSIDE;
	}
	if (paint_down && y + 1 < height && *get_index(x, y + 1, map, width) == PIPE_CONNECTION::EMPTY)
	{
		*get_index(x, y + 1, map, width) = PIPE_CONNECTION::OUTSIDE;
	}
	//paint possible diagonals
	if (paint_left && paint_up && x - 1 >= 0 && y - 1 >= 0 && *get_index(x - 1, y - 1, map, width) == PIPE_CONNECTION::EMPTY)
	{
		*get_index(x - 1, y - 1, map, width) = PIPE_CONNECTION::OUTSIDE;
	}
	if (paint_left && paint_down && x - 1 >= 0 && y + 1 < height && *get_index(x - 1, y + 1, map, width) == PIPE_CONNECTION::EMPTY)
	{
		*get_index(x - 1, y + 1, map, width) = PIPE_CONNECTION::OUTSIDE;
	}
	if (paint_right && paint_up && x + 1 < width && y - 1 >= 0 && *get_index(x + 1, y - 1, map, width) == PIPE_CONNECTION::EMPTY)
	{
		*get_index(x + 1, y - 1, map, width) = PIPE_CONNECTION::OUTSIDE;
	}
	if (paint_right && paint_down && x + 1 < width && y + 1 < height && *get_index(x + 1, y + 1, map, width) == PIPE_CONNECTION::EMPTY)
	{
		*get_index(x + 1, y + 1, map, width) = PIPE_CONNECTION::OUTSIDE;
	}
}

void paintOutside(PIPE_CONNECTION* map, int width, int height)
{
	//paint everything along the top, bottom, left, right
	for (int i = 0; i < width*height; i++)
	{
		if (i < width || i >= width * (height - 1) || i % width == 0 || i % width == width - 1)
		{
			if (map[i] == PIPE_CONNECTION::EMPTY)
			{
				map[i] = PIPE_CONNECTION::OUTSIDE;
			}
		}
	}

	//paint everything that is adjacent to outside as outside
	for (int i = 0; i < width * height; i++)
	{
		if (map[i] == PIPE_CONNECTION::EMPTY)
		{
			if (i >= width && map[i - width] == PIPE_CONNECTION::OUTSIDE)
			{
				map[i] = PIPE_CONNECTION::OUTSIDE;
			}
			if (i < width * (height - 1) && map[i + width] == PIPE_CONNECTION::OUTSIDE)
			{
				map[i] = PIPE_CONNECTION::OUTSIDE;
			}
			if (i % width != 0 && map[i - 1] == PIPE_CONNECTION::OUTSIDE)
			{
				map[i] = PIPE_CONNECTION::OUTSIDE;
			}
			if (i % width != width - 1 && map[i + 1] == PIPE_CONNECTION::OUTSIDE)
			{
				map[i] = PIPE_CONNECTION::OUTSIDE;
			}
		}
	}

	//iterate backwards to get the bottom and right sides
	for (int i = width * height - 1; i >= 0; i--)
	{
		if (map[i] == PIPE_CONNECTION::EMPTY)
		{
			if (i >= width && map[i - width] == PIPE_CONNECTION::OUTSIDE)
			{
				map[i] = PIPE_CONNECTION::OUTSIDE;
			}
			if (i < width * (height - 1) && map[i + width] == PIPE_CONNECTION::OUTSIDE)
			{
				map[i] = PIPE_CONNECTION::OUTSIDE;
			}
			if (i % width != 0 && map[i - 1] == PIPE_CONNECTION::OUTSIDE)
			{
				map[i] = PIPE_CONNECTION::OUTSIDE;
			}
			if (i % width != width - 1 && map[i + 1] == PIPE_CONNECTION::OUTSIDE)
			{
				map[i] = PIPE_CONNECTION::OUTSIDE;
			}
		}
	}

	//iterate a third time to get the top and left sides
	for (int i = 0; i < width * height; i++)
	{
		if (map[i] == PIPE_CONNECTION::EMPTY)
		{
			if (i >= width && map[i - width] == PIPE_CONNECTION::OUTSIDE)
			{
				map[i] = PIPE_CONNECTION::OUTSIDE;
			}
			if (i < width * (height - 1) && map[i + width] == PIPE_CONNECTION::OUTSIDE)
			{
				map[i] = PIPE_CONNECTION::OUTSIDE;
			}
			if (i % width != 0 && map[i - 1] == PIPE_CONNECTION::OUTSIDE)
			{
				map[i] = PIPE_CONNECTION::OUTSIDE;
			}
			if (i % width != width - 1 && map[i + 1] == PIPE_CONNECTION::OUTSIDE)
			{
				map[i] = PIPE_CONNECTION::OUTSIDE;
			}
		}
	
	}
}

PIPE_CONNECTION* createCleanMap(PIPE_CONNECTION* map, int width, int height, int start_x, int start_y)
{
	PIPE_CONNECTION* clean_map = new PIPE_CONNECTION[width * height];
	for (int i = 0; i < width * height; i++)
	{
		clean_map[i] = PIPE_CONNECTION::EMPTY;
	}
	int x = start_x;
	int y = start_y;
	int last_direction = 0;
	PIPE_CONNECTION* current = get_index(x, y, map, width);

	//do the first step
	clean_map[y * width + x] = *current;
	current = next_pos(last_direction, x, y, current, map, width);

	while (start_x != x || start_y != y)
	{
		clean_map[y * width + x] = *current;
		current = next_pos(last_direction, x, y, current, map, width);
	}
	return clean_map;
}

void visualizeMap(PIPE_CONNECTION* map, int width, int height)
{
	for (int i = 0; i < width * height; i++)
	{
		switch (map[i])
		{
			case PIPE_CONNECTION::EMPTY:
				std::cout << '.';
				break;
			case PIPE_CONNECTION::TOP_BOTTOM:
				std::cout << '|';
				break;
			case PIPE_CONNECTION::LEFT_RIGHT:
				std::cout << '-';
				break;
			case PIPE_CONNECTION::TOP_LEFT:
				std::cout << 'Ù';
				break;
			case PIPE_CONNECTION::TOP_RIGHT:
				std::cout << 'À';
				break;
			case PIPE_CONNECTION::BOTTOM_LEFT:
				std::cout << '¿';
				break;
			case PIPE_CONNECTION::BOTTOM_RIGHT:
				std::cout << 'Ú';
				break;
			case PIPE_CONNECTION::START_TILE:
				std::cout << 'S';
				break;
			case PIPE_CONNECTION::OUTSIDE:
				std::cout << 'O';
				break;
			case PIPE_CONNECTION::INSIDE:
				std::cout << 'I';
				break;
		}
		if (i % width == width - 1)
		{
			std::cout << std::endl;
		}
	}
}

PIPE_CONNECTION* getMap(int & width, int & height, std::string input_file)
{
	//first, get the width and height of the map
	std::ifstream file(input_file);
	std::string line;
	height = 0;
	width = 0;
	while (std::getline(file, line))
	{
		height++;
		if (line.length() > width)
		{
			width = line.length();
		}
	}
	file.close();

	//then, create and fill the map
	PIPE_CONNECTION* map = new PIPE_CONNECTION[width * height];
	file.open(input_file);
	int i = 0;
	while (std::getline(file, line))
	{
		for (int j = 0; j < line.length(); j++)
		{
			map[i * width + j] = connection_map[line[j]];
		}
		i++;
	}
	file.close();
	return map;
}

int get_max_distance(PIPE_CONNECTION* map, int start_x, int start_y, int width, int height)
{
	int max_distance = 0;
	int x_1 = start_x;
	int y_1 = start_y;
	int x_2 = start_x;
	int y_2 = start_y;
	int last_direction_1 = 0;
	int last_direction_2 = 0;
	PIPE_CONNECTION* current_1 = get_index(x_1, y_1, map, width);
	PIPE_CONNECTION* current_2 = get_index(x_2, y_2, map, width);

	//do the first step (hardcoded directions)
	current_1 = next_pos(last_direction_1, x_1, y_1, current_1, map, width);
	last_direction_2 = (last_direction_1 + 2) % 4; //make sure the two paths start going in different directions
	current_2 = next_pos(last_direction_2, x_2, y_2, current_2, map, width);
	max_distance++;

	while (true)
	{
		current_1 = next_pos(last_direction_1, x_1, y_1, current_1, map, width);
		current_2 = next_pos(last_direction_2, x_2, y_2, current_2, map, width);
		max_distance++;

		//check if the paths have crossed
		if (x_1 == x_2 && y_1 == y_2)
		{
			return max_distance;
		}
	}
}

PIPE_CONNECTION* next_pos(int& prev_direction, int & x, int & y, PIPE_CONNECTION* current, PIPE_CONNECTION* map, int width)
{
	int move_direction = 0;
	switch (*current)
	{
		case PIPE_CONNECTION::TOP_BOTTOM:
			move_direction = prev_direction == 3 ? 3 : 1;
			break;
		case PIPE_CONNECTION::LEFT_RIGHT:
			move_direction = prev_direction == 2 ? 2 : 4;
			break;
		case PIPE_CONNECTION::TOP_LEFT:
			move_direction = prev_direction == 3 ? 4 : 1;
			break;
		case PIPE_CONNECTION::TOP_RIGHT:
			move_direction = prev_direction == 3 ? 2 : 1;
			break;
		case PIPE_CONNECTION::BOTTOM_LEFT:
			move_direction = prev_direction == 1 ? 4 : 3;
			break;
		case PIPE_CONNECTION::BOTTOM_RIGHT:
			move_direction = prev_direction == 1 ? 2 : 3;
			break;
	}

	//get the opposite direction
	prev_direction = move_direction;

	switch (move_direction)
	{
		case 1:
			y--;
			break;

		case 2:
			x++;
			break;

		case 3:
			y++;
			break;

		case 4:
			x--;
			break;
	}


	return get_index(x, y, map, width);
}

PIPE_CONNECTION * get_index(int x, int y, PIPE_CONNECTION* map, int width)
{
	return &map[y * width + x];
}

PIPE_CONNECTION getStartingTileDirection(PIPE_CONNECTION* map, int start_x, int start_y, int width, int height)
{
	bool goes_left = false;
	bool goes_up = false;
	bool goes_right = false;
	bool goes_down = false;
	if (start_x != 0 &&
		(*get_index(start_x - 1, start_y, map, width) == PIPE_CONNECTION::LEFT_RIGHT ||
			*get_index(start_x - 1, start_y, map, width) == PIPE_CONNECTION::TOP_RIGHT ||
			*get_index(start_x - 1, start_y, map, width) == PIPE_CONNECTION::BOTTOM_RIGHT))
	{
		goes_left = true;
	}
	if (start_x != width - 1 &&
		(*get_index(start_x + 1, start_y, map, width) == PIPE_CONNECTION::LEFT_RIGHT ||
			*get_index(start_x + 1, start_y, map, width) == PIPE_CONNECTION::TOP_LEFT ||
			*get_index(start_x + 1, start_y, map, width) == PIPE_CONNECTION::BOTTOM_LEFT))
	{
		goes_right = true;
	}
	if (start_y != 0 &&
		(*get_index(start_x, start_y - 1, map, width) == PIPE_CONNECTION::TOP_BOTTOM ||
			*get_index(start_x, start_y - 1, map, width) == PIPE_CONNECTION::BOTTOM_RIGHT ||
			*get_index(start_x, start_y - 1, map, width) == PIPE_CONNECTION::BOTTOM_LEFT))
	{
		goes_up = true;
	}
	if (start_y != height - 1 &&
		(*get_index(start_x, start_y + 1, map, width) == PIPE_CONNECTION::TOP_BOTTOM ||
			*get_index(start_x, start_y + 1, map, width) == PIPE_CONNECTION::TOP_RIGHT ||
			*get_index(start_x, start_y + 1, map, width) == PIPE_CONNECTION::TOP_LEFT))
	{
		goes_down = true;
	}

	//set start tile to the correct direction
	if (goes_left && goes_down)
	{
		return PIPE_CONNECTION::BOTTOM_LEFT;
	}
	else if (goes_left && goes_up)
	{
		return PIPE_CONNECTION::TOP_LEFT;
	}
	else if (goes_right && goes_down)
	{
		return PIPE_CONNECTION::BOTTOM_RIGHT;
	}
	else if (goes_right && goes_up)
	{
		return PIPE_CONNECTION::TOP_RIGHT;
	}
	else if (goes_left && goes_right)
	{
		return PIPE_CONNECTION::LEFT_RIGHT;
	}
	else
	{
		return PIPE_CONNECTION::TOP_BOTTOM;
	}
}