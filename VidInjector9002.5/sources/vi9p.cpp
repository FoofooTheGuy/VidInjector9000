#include "vi9p.hpp"

bool VI9P::Loading = false;
std::string VI9P::WorkingFile = "";
std::string VI9P::OutFile = "";
uint8_t VI9P::MultiBannerIndex = 0;//0 to 26

void parsePP(const std::string input, const std::string query, std::string* value) {
	std::vector<std::string> lines;
	std::string line;
	//split into lines
	for(const auto &c : input) {
		line += c;
		if (c == '\n') {
			size_t found = line.find("] ");
			if(found < line.size()) {
				//std::cout << line.substr(found + std::string("] ").size());
				lines.push_back(line.substr(found + std::string("] ").size()));
			}
			else {
				lines.push_back(line);
			}
			line = "";
		}
	}
	for(size_t i = 0; i < lines.size(); i++) {
		if (lines.at(i).substr(0, lines.at(i).find_first_of("=")) == query) {
			size_t first = 0;
			size_t second = 0;
			first = lines.at(i).find_first_of("\x1F") + 1;
			if(strcmp(lines.at(i).substr(first).c_str(), "\x1F\n") == 0) {//if it's empty
				*(value) = "";
				return;
			}
			if (first > lines.at(i).size())
				first = 0;
			while(second == 0) {
				if (first) {
					second = lines.at(i).substr(first).find_last_of("\x1F");
				}
				if (second > lines.at(i).size()) {
					second = 0;
				}
				if (second) {
					*(value) = lines.at(i).substr(first).substr(0, second);
				}
				else {
					if(i < lines.size() - 1) {
						lines.at(i) += lines.at(i + 1);
						lines.erase(lines.begin() + i + 1);
					}
				}
			}
			break;
		}
	}
}
