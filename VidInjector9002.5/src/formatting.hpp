#pragma once

#include <string>
#include <vector>
#include <cstring>
#include <fstream>
#include <filesystem>

std::string tolowerstr(std::string str);
std::string toupperstr(std::string str);

bool stol_s(long& output, std::string input, bool isHex = false);
bool stoul_s(uint8_t& output, std::string input, bool isHex = false);
bool stoul_s(uint32_t& output, std::string input, bool isHex = false);
bool stoul_s(unsigned long& output, std::string input, bool isHex = false);
