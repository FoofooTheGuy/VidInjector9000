#include "fileReading.hpp"

bool parseLines(std::string& out, std::vector<std::string> filelines, std::string query) {
	std::string line = "";
	for (auto& str : filelines) {
		if (str.substr(0, str.find_first_of("=")) == query) {
			size_t first = 0;
			size_t second = 0;
			first = str.find_first_of("\"") + 1;
			if (first > str.size()) first = 0;
			if (first) second = str.substr(first).find_last_of("\"");
			out = str.substr(first).substr(0, second);
			return true;
		}
	}
	return false;
}

std::vector<std::string> fileRead(std::string inpath) {
	std::vector<std::string> filelines;
	if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*inpath.c_str()))) return filelines;
	std::ifstream infile(std::filesystem::path((const char8_t*)&*inpath.c_str()), std::ios_base::in | std::ios_base::binary);
	char Byte;
	std::string line = "";
	//split file into the vector
	infile.read(&Byte, 1);//grab first byte of file
	while (infile) {//continue until input stream fails
		line += Byte;//append byte to string
		size_t first = 0;
		size_t second = 0;
		first = line.find_first_of("\"");
		if (first > line.size()) first = 0;//if find_first_of fails, this flings in the first numeral
		if (first) second = line.substr(first).find_last_of("\"");
		if (line.substr(first).substr(second) == "\"\n") {//put the line and clear the string for the next line
			filelines.push_back(line);
			line = "";
		}
		//printf("%1X\n", Byte);
		infile.read(&Byte, 1);//grab next byte of file
	}
	return filelines;
}

