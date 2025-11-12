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

int movie_title(VI9Pparameters& parameters, std::string& romfsPath, std::string& outCIA, std::string& outTAR, bool& dopatch);

int settingsTL(VI9Pparameters& parameters, std::string& romfsPath, std::string uniqueIDstr, std::string& outCIA, std::string& outTAR, bool& dopatch);

int information_buttons(VI9Pparameters& parameters, std::string& romfsPath, std::string& outCIA);

int copyMoflex(VI9Pparameters& parameters, std::string& romfsPath, std::string& outCIA, std::string& outTAR, bool& dopatch);

int makeBimgs(VI9Pparameters& parameters, std::string& romfsPath, std::string& outCIA, std::string& outTAR, bool& dopatch);

int makeIcon(VI9Pparameters& parameters, std::string& romfsPath, std::string& tempPath);

int makeBanner(VI9Pparameters& parameters, std::string& tempPath);

void editExheader(std::string& tempPath, uint32_t& uniqueID, std::string& ApplicationName);
