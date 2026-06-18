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
	for (char& i : str) {
		i = tolower(i) & 0xFF;
	}
	return str;
}

std::string toupperstr(std::string str) {
	for (char& i : str)
		i = toupper(i) & 0xFF;
	return str;
}

std::string addMissingFileExtension(std::string instr, std::string extension) {
	size_t find = tolowerstr(instr).rfind(extension);
	if(find == std::string::npos) {
		return (instr + extension);
	}
	return instr;
}

std::string fixDoubleQuote(std::string str) {
	for (size_t i = 0; i < str.size(); i++) {
		if (str[i] == '"') {
			str.insert(i, "\\");
			i++;
		}
	}
	return str;
}

size_t getHalfPos(const std::string &input) {
	size_t charstop = chrcount(input) / 2;
	size_t size = input.size();
	size_t pos = 0;
	
	for(size_t i = 0; i < charstop; i++) {
		(void)i;
		if(input[pos] == 0) {
			break;
		}
		
		if((pos + 1) > size) {
			break;
		}
		++pos;
		
		// get out from within a multi-byte utf8
		while((input[pos] & 0xC0) == 0x80) {
			if((pos + 1) > size) {
				break;
			}
			++pos;
		}
	}
	return pos;
}

void splitAtHalf(const std::string &input, std::string *first, std::string *second) {
	size_t pos = getHalfPos(input);
	
	*first = input.substr(0, pos);
	*second = input.substr(pos, input.size());
}

void removeFirstCharacter(std::string *str) {
	if(!str->empty()) {
		do {
			str->erase(0, 1);
		} while((str->front() & 0xC0) == 0x80);
	}
}

void removeLastCharacter(std::string *str) {
	if(!str->empty()) {
		do {
			str->pop_back();
		} while((str->back() & 0xC0) == 0x80);
		// remove the remaining byte of a multibyte character
		if((str->back() & 0xC0) > 0x80) {
			str->pop_back();
		}
	}
}

std::string shortenStr(const std::string &input, size_t loop) {
	const std::string ellipsis = "…";
	if(loop == 0) {
		return input;
	}
	
	if(loop >= chrcount(input) / 2) {
		return ellipsis;
	}
	
	std::string first;
	std::string second;
	
	splitAtHalf(input, &first, &second);
	if(chrcount(second) > chrcount(first)) {
		removeFirstCharacter(&second);
	}
	
	for(size_t i = 0; i < loop; i++) {
		(void)i;
		if(first.empty()) {
			break;
		}
		if(second.empty()) {
			break;
		}
		removeLastCharacter(&first);
		removeFirstCharacter(&second);
	}
	return first + ellipsis + second;
}

std::error_code copyfile(std::string inpath, std::string outpath) {//also works with directories
	std::error_code error(0, std::system_category());
	if(strcmp(inpath.c_str(), outpath.c_str()) == 0)
		return error;
	std::filesystem::remove_all(std::filesystem::path((const char8_t*)&*outpath.c_str()), error);
	if (error) return error;
	std::filesystem::copy(std::filesystem::path((const char8_t*)&*inpath.c_str()), std::filesystem::path((const char8_t*)&*outpath.c_str()), std::filesystem::copy_options::recursive, error);
	return error;
}

bool TIDisValid(uint32_t TID) { // https://github.com/ihaveamac/videoinject/blob/master/videoinject.py#L9
	uint32_t min = 0xC0000;
	uint32_t max = 0xEFFFF;
	switch (TID)
	{
	case 0xc0d00: // Side-Scrolling Example
	case 0xce1cc: // CHMM
	case 0xd921e: // homebrew launcher loader
	case 0xda001: // Smash Bros Dummy Application
	case 0xda002: // 3ds quick shutdown
	case 0xda003: // Wifi Toggle
	case 0xDEAD1: // MCU Bricker
	case 0xe7a5a: // NASA ALL
	case 0xec100: // PKSM
	case 0xEC600: // Video player for 3DS
	case 0xed990: // NotifyMii
	case 0xED000: // Google Translate
	case 0xEE3EE: // MurderLands
	case 0xeffec: // FileKong
	case 0xeffed: // TriaAl
	case 0:
	{
		return false;
	}
	break;
	default:
		if (TID > max || TID < min) return false;
	}
	return true;
}

uint32_t RandomTID() {
	uint32_t min = 0xC0000;
	uint32_t max = 0xEFFFF;
	uint32_t TID = 0;
	static std::mt19937 rng;

	while (!TIDisValid(TID)) {//loop until we get a good value
		rng.seed(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
		std::uniform_int_distribution<unsigned long> uniform(min, max);
		TID = uniform(rng);
	}
	return TID;
}
