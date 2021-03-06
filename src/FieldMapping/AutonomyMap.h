#ifndef AUTONOMY_MAP_H
#define AUTONOMY_MAP_H

#include<iostream> // allows input and output
#include<String> // includes the String class to be used
#include<fstream> // allows filestream to be used
#include<vector> // this will allow us to use a vector
#include<math.h>

using std::endl;
using std::cout;
using std::cin;
using std::vector;

const int COLS = 4; // number of columns the field is divided into
const int ROWS = 10; // number of rows the field is divided into
const int MINING_ROW = 5; // row where mining field starts
// right means to the right if the robot is facing mining area
enum Direction {RIGHT, LEFT, TOWARDS_MINE, TOWARDS_BIN, NOT_DETERMINED};

#endif
