#pragma once

#include <cstdint>
#include "strings.hpp"

#define MAX_ROWS 27

struct VI9Pparameters {
	int &mode;
	std::string &banner;
	std::string &icon;
	int &iconBorder;
	std::string &Sname;
	std::string &Lname;
	std::string &publisher;
	int &copycheck;
	std::string &copyrightInfo;
	int &FFrewind;
	int &FadeOpt;
	uint8_t &rows;
	std::vector<std::string> &PTitleVec;
	std::vector<std::string> &MoflexVec;
	std::vector<std::string> &MBannerVec;
	uint8_t &splitPos;
	int &BannerPreviewIndex;
};

class VI9P
{
	public:
	static std::string WorkingFile;
	//static VI9Pparameters Parameters;//im not quite sure if we want this
};

//todo: delete this and parse cli output for -pp in order to get parameters
//int loadParameters(std::string parampath, VI9Pparameters* parameters);
