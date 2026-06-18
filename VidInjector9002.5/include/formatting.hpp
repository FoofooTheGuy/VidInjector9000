#pragma once

#include <filesystem>
#include <cstring>
#include <fstream>
#include <chrono>
#include <random>
#include <string>
#include <vector>

//return: number of code points in a utf8 string as opposed to number of bytes the string takes up
size_t chrcount(const std::string& str);

std::string tolowerstr(std::string str);
std::string toupperstr(std::string str);

/*
instr: string with or without the file extension
extension: file extension (eg. ".txt")
return: instr + extension
*/
std::string addMissingFileExtension(std::string instr, std::string extension);

// replace " with \"
std::string fixDoubleQuote(std::string str);

// find the index to half way through a utf8 string
size_t getHalfPos(const std::string &input);

// separate input into first and second with getHalfPos
void splitAtHalf(const std::string &input, std::string *first, std::string *second);

// remove the first character from a utf8 string
void removeFirstCharacter(std::string *str);

// remove the last character from a utf8 string
void removeLastCharacter(std::string *str);

/* remove characters from the middle of a utf8 string
input: inout string
loop: number of trimming iterations
*/
std::string shortenStr(const std::string &input, size_t loop);

std::error_code copyfile(std::string inpath, std::string outpath);

bool TIDisValid(uint32_t TID);
uint32_t RandomTID();

template<class T>
bool ASCII2number(T* outnum, const std::string& str, bool isHex = false) {
	if(isHex) {
		if (str.find_first_of("-abcdefABCDEF1234567890") == std::string::npos) {
			return false;
		}
	}

	if(!isHex) {
		if (str.find_first_of("-1234567890") == std::string::npos) {
			return false;
		}
	}

	std::size_t size = 0;
	int base = isHex ? 16 : 10;

	if constexpr (std::is_same<T, int>::value) {
		*(outnum) = std::stoi(str, &size, base);
	}
	else if constexpr (std::is_same<T, long>::value) {
		*(outnum) = std::stol(str, &size, base);
	}
	else if constexpr (std::is_same<T, unsigned long>::value) {
		*(outnum) = std::stoul(str, &size, base);
	}
	else if constexpr (std::is_same<T, long long>::value) {
		*(outnum) = std::stoll(str, &size, base);
	}
	else if constexpr (std::is_same<T, unsigned long long>::value) {
		*(outnum) = std::stoull(str, &size, base);
	}
	else {
		static_assert(false, "Invalid use of ASCII2number. This function only supports std::string or std::vector<std::string>");
	}

	return true;
}
