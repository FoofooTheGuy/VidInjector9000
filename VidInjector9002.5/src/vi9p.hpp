#pragma once

#include <cstdint>
#include <vector>
#include "strings.hpp"

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
	uint8_t rows = 0;
	std::vector<std::string> PTitleVec = std::vector<std::string>(1, "");//12
	std::vector<std::string> MoflexVec = std::vector<std::string>(1, "");//12 + rows
	std::vector<std::string> MBannerVec = std::vector<std::string>(1, "");//12 + rows * 2
	uint8_t splitPos = 0;//11
};

class VI9P
{
	public:
	static std::string WorkingFile;
	//static VI9Pparameters Parameters;//im not quite sure if we want this
};

//todo: delete this and parse cli output for -pp in order to get parameters
//int loadParameters(std::string parampath, VI9Pparameters* parameters);

/*parse output of -pp from CLI
input: raw output of -pp
query: parameter to get the value from (like INT:MULTI)
value: value of the parameter
*/
void parsePP(const std::string input, const std::string query, std::string* value);
