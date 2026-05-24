#pragma once

#include <filesystem>
#include <cstring>
#include <fstream>
#include <vector>

/*do this to your file data
out: pointer to output data
filelines: vector of lines
query: variable to take from lines e.g. STRING:VER
return: true if it's success, false if it's not found*/
bool parseLines(std::string& out, std::vector<std::string> filelines, std::string query);
std::vector<std::string> fileRead(std::string inpath);
