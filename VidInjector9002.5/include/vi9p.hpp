#pragma once

#include <cstdint>
#include <vector>
#include "programStrings.hpp"

#define MAX_ROWS 27

struct VI9Pparameters {
	int mode = 0;//0
	std::string banner = "";//1
	std::string icon = "";//2
	int iconBorder = 2;//3
	std::string Sname = "";//4
	std::string Lname = "";//5
	std::string publisher = "";//6
	int copycheck = 0;//7
	std::string copyrightInfo = "";//8
	int FFrewind = 1;//9
	int FadeOpt = 1;//10
	uint8_t rows = 1;
	std::vector<std::string> PTitleVec = std::vector<std::string>(1, "");//12
	std::vector<std::string> MoflexVec = std::vector<std::string>(1, "");//12 + rows
	std::vector<std::string> MBannerVec = std::vector<std::string>(1, "");//12 + rows * 2
	uint8_t splitPos = 0;//11
};

/*
Loading: bool to set while you are applying the parameters so it doesnt overwrite what we are loading, specifically when dealing with smdh
WorkingFile: the vi9p to actually write to
OutFile: the vi9p to copy the working file to when you choose to save it
MultiBannerIndex: the number for which row of multi banner to preview 
*/
class VI9P
{
	public:
	static bool Loading;
	static std::string WorkingFile;
	static std::string OutFile;
	static uint8_t MultiBannerIndex;
};

/*parse output of -pp from CLI
input: raw output of -pp
query: parameter to get the value from (like INT:MULTI)
value: value of the parameter
*/
void parsePP(const std::string input, const std::string query, std::string* value);
