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

std::error_code copyfile(std::string inpath, std::string outpath) {//also works with directories
	std::error_code error;
	std::filesystem::remove_all(std::filesystem::path((const char8_t*)&*outpath.c_str()), error);
	if (error) return error;
	std::filesystem::copy(std::filesystem::path((const char8_t*)&*inpath.c_str()), std::filesystem::path((const char8_t*)&*outpath.c_str()), std::filesystem::copy_options::recursive, error);
	return error;
}
