#pragma once

#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include "fileReading.hpp"
#include "formatting.hpp"
#include "strings.hpp"

#define MAX_ROWS 27


void saveParameters(std::string parampath, int mode = 0, std::string banner = "", std::string icon = "", int iconBorder = 2, std::string Sname = "", std::string Lname = "", std::string publisher = "", int copycheck = 0, std::string copyrightInfo = "", int FFrewind = 1, int FadeOpt = 1, uint8_t rows = 1, std::vector<std::string> PTitleVec = std::vector<std::string>(1, ""), std::vector<std::string> MoflexVec = std::vector<std::string>(1, ""), std::vector<std::string> MBannerVec = std::vector<std::string>(1, ""));
int loadParameters(std::string parampath, int &mode, std::string &banner, std::string &icon, int &iconBorder, std::string &Sname, std::string &Lname, std::string &publisher, int &copycheck, std::string &copyrightInfo, int &FFrewind, int &FadeOpt, uint8_t &rows, std::vector<std::string> &PTitleVec, std::vector<std::string> &MoflexVec, std::vector<std::string> &MBannerVec, int &BannerPreviewIndex);
int setParameter(std::string inpath, int number, std::string newValue, std::string outpath);
int printParameter(std::string inpath);
int add_row(std::string inpath, std::string outpath);
int sub_row(std::string inpath, std::string outpath);