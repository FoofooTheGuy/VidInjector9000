#pragma once

#include <string>
#include <vector>
#include <cstring>
#include <fstream>
#include <filesystem>

std::string tolowerstr(std::string str);
std::string toupperstr(std::string str);

template<class T> bool ASCII2number(T* outnum, const std::string& str, bool isHex = false) {
	if (str.find_first_of("-abcdefABCDEF1234567890") == std::string::npos)
		return false;

	std::size_t size = 0;
	int base = isHex ? 16 : 10;

	if constexpr (std::is_same<T, int>::value)
		*(outnum) = std::stoi(str, &size, base);
	else if constexpr (std::is_same<T, long>::value)
		*(outnum) = std::stol(str, &size, base);
	else if constexpr (std::is_same<T, unsigned long>::value)
		*(outnum) = std::stoul(str, &size, base);
	else if constexpr (std::is_same<T, long long>::value)
		*(outnum) = std::stoll(str, &size, base);
	else if constexpr (std::is_same<T, unsigned long long>::value)
		*(outnum) = std::stoull(str, &size, base);
	else return false;

	return true;
}
