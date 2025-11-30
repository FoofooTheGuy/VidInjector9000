#pragma once

// deal with the contents of romfs in cia and tar, and exefs in cia

#include <filesystem>
#include <iostream>
#include <fstream>

#include "stbiStuff.hpp"
#include "strings.hpp"
#include "arrays.hpp"
#include "files.hpp"
#include "vi9p.hpp"

int make_movie_title(const VI9Pparameters& parameters, const std::string& romfsPath, const std::string& outCIA, const std::string& outTAR, const bool& dopatch);
int make_settingsTL(const VI9Pparameters& parameters, const std::string& romfsPath, const std::string uniqueIDstr, const std::string& outCIA, const std::string& outTAR, const bool& dopatch);
int make_information_buttons(const VI9Pparameters& parameters, const std::string& romfsPath, const std::string& outCIA);
int make_Moflex(const VI9Pparameters& parameters, const std::string& romfsPath, const std::string& outCIA, const std::string& outTAR, const bool& dopatch);
int make_Bimgs(const VI9Pparameters& parameters, const std::string& romfsPath, const std::string& outCIA, const std::string& outTAR, const bool& dopatch);
int make_Icon(const VI9Pparameters& parameters, const std::string& romfsPath, const std::string& tempPath);
int make_Banner(const VI9Pparameters& parameters, const std::string& tempPath);

/*
path: input path to text file containing utf16 text
out: std::vector<std::string> containing each line per element (without notes or empty lines)
out: std::string containing everything except the byte order mark
return 0 if succeeded or other numbers for fail
*/
template<class T>
int UTF16fileToUTF8str(const std::string path, T* out) {
	// first check T is supported
	if constexpr (std::is_same<T, std::string>::value) {
		; // do nothing lol
	}
	else if constexpr (std::is_same<T, std::vector<std::string>>::value) {
		;
	}
	else {
		// https://stackoverflow.com/a/16504506
		static_assert(false, "Invalid use of UTF16fileToUTF8str(const std::string path, T* out). 'T*' only supports 'std::string*' or 'std::vector<std::string>*'");
	}
	
	std::ifstream input(std::filesystem::path((const char8_t*)&*path.c_str()), std::ios::binary);
	if (!input.is_open()) {
		std::cout << ErrorText << ' ' << FailedToReadFile << " \"" << path << "\"" << std::endl;
		return 1;
	}
	
	// read byte order mark
	uint16_t BOM;
	input.read(reinterpret_cast<char*>(&BOM), sizeof BOM);
	//std::cout << std::hex << BOM << std::endl;
	
	if ((BOM & 0xFF) != 0xFF && ((BOM & 0xFF00) >> 8) != 0xFE) { // if not little endian
		std::cout << ErrorText << ' ' << std::hex << BOM << std::endl;
		return 2;
	}
	
	std::string Lines = "";
	char byte;
	while(input.read(reinterpret_cast<char*>(&byte), sizeof byte)) {
		if constexpr (std::is_same<T, std::string>::value) {
			Lines += byte;
		}
		else if constexpr (std::is_same<T, std::vector<std::string>>::value) {
			if (byte == 0x0A && input.peek() == 0x00) {
				if (Lines[0] != '#' && !Lines.empty()) { // skip notes and empty lines
					out->push_back(UTF16toUTF8(Lines));
				}
				Lines.clear();
				input.get(); // 00
				continue;
			}
			else if (byte == 0x0D && input.peek() == 0x00) {
				if (Lines[0] != '#' && !Lines.empty()) { // skip notes and empty lines
					out->push_back(UTF16toUTF8(Lines));
					//std::cout << out->back() << std::endl; // dude
				}
				Lines.clear();
				input.get(); // 00
				if(input.peek() == 0x0A) {
					input.get(); // 0A
					if(input.peek() == 0x00) { // if this gets any longer, you should do a loop
						input.get(); // 00
					}
				}
				continue;
			}
			Lines += byte;
		}
	}
	if constexpr (std::is_same<T, std::string>::value) {
		*(out) = UTF16toUTF8(Lines);
	}
	return 0;
}

int get_information_buttons(VI9Pparameters* parameters, const std::string& romfsPath);
int get_copyright(VI9Pparameters* parameters, const std::string& romfsPath);
int get_settingsTL(VI9Pparameters* parameters, const std::string& romfsPath);
int get_movie_bnrname(VI9Pparameters* parameters, const std::string& romfsPath);
int get_movie_title(VI9Pparameters* parameters, const std::string& romfsPath);
