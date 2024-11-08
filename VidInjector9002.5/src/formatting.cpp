#include "formatting.hpp"

size_t chrcount(const std::string& str) {
	size_t length = 0;
	for (char c : str) {
		if ((c & 0xC0) != 0x80) {
			++length;
		}
	}
	return length;
}

std::string tolowerstr(std::string str) {
	for (char& i : str)
		i = tolower(i) & 0xFF;
	return str;
}

std::string toupperstr(std::string str) {
	for (char& i : str)
		i = toupper(i) & 0xFF;
	return str;
}
