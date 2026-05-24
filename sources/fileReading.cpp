#include "fileReading.hpp"

bool parseLines(std::string& out, std::vector<std::string> filelines, std::string query) {
	std::string line = "";
	for(size_t i = 0; i < filelines.size(); i++) {
		if (filelines.at(i).substr(0, filelines.at(i).find_first_of("=")) == query) {
			//std::cout << str.substr(0, str.find_first_of("=")) << std::endl;
			size_t first = 0;
			size_t second = 0;
			first = filelines.at(i).find_first_of("\x1F") + 1;
			if(strcmp(filelines.at(i).substr(first).c_str(), "\x1F\n") == 0) {//if it's empty
			    out = "";
			    return true;
			}
			if (first > filelines.at(i).size())
				first = 0;
			while(second == 0) {
				if (first) {
					//std::cout << filelines.at(i).substr(first);
					second = filelines.at(i).substr(first).find_last_of("\x1F");
				}
				if(second > filelines.at(i).size()) {
					second = 0;
				}
				if(second) {
					out = filelines.at(i).substr(first).substr(0, second);
				}
				else {
					filelines.at(i) += filelines.at(i + 1);
					filelines.erase(filelines.begin() + i + 1);
				}
			}
			return true;
		}
	}
	return false;
}

std::vector<std::string> fileRead(std::string inpath) {
	std::vector<std::string> filelines;
	std::error_code error;
	if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*inpath.c_str()), error))
		return filelines;
	if(error)
		return filelines;
	std::ifstream infile(std::filesystem::path((const char8_t*)&*inpath.c_str()), std::ios_base::in | std::ios_base::binary);
	char Byte;
	std::string line = "";
	//split file into the vector
	infile.read(&Byte, 1);//grab first byte of file
	while (infile) {//continue until input stream fails
		line += Byte;//append byte to string
		/*size_t first = 0;
		size_t second = 0;
		first = line.find_first_of("\x1F");
		if (first > line.size())
			first = 0;//if find_first_of fails, this flings in the first numeral
		if (first)
			second = line.substr(first).find_last_of("\x1F");
		if(second > line.size())
			second = 0;
		if(second)*/
		if (Byte == '\n') {//put the line and clear the string for the next line
			filelines.push_back(line);
			line = "";
		}
		//printf("%1X\n", Byte);
		infile.read(&Byte, 1);//grab next byte of file
	}
	return filelines;
}

