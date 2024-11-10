#include "vi9p.hpp"

std::string VI9P::WorkingFile = "";
uint8_t VI9P::MultiBannerIndex = 0;

void parsePP(const std::string input, const std::string query, std::string* value) {
	std::vector<std::string> lines;
	std::string line;
	//split into lines
	for(const auto &c : input) {
		line += c;
		if (c == '\n') {
			lines.push_back(line.substr(line.find("] ") + std::string("] ").size()));
			line = "";
		}
	}
	for(const auto &s : lines) {
		if (s.substr(0, s.find_first_of("=")) == query) {
			size_t first = 0;
			size_t second = 0;
			first = s.find_first_of("\"") + 1;
			if (first > s.size()) first = 0;
			if (first) second = s.substr(first).find_last_of("\"");
			*(value) = s.substr(first).substr(0, second);
			break;
		}
	}
}
