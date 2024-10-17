#pragma once

#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include "fileReading.hpp"
#include "formatting.hpp"
#include "strings.hpp"

#define MAX_ROWS 27

struct VI9Pparameters {
	std::string ver = VI9PVER;
	int mode = 0;
	std::string banner = "";
	std::string icon = "";
	int iconBorder = 2;
	std::string Sname = "";
	std::string Lname = "";
	std::string publisher = "";
	int copycheck = 0;
	std::string copyrightInfo = "";
	int FFrewind = 1;
	int FadeOpt = 1;
	uint8_t rows = 1;
	std::vector<std::string> PTitleVec = std::vector<std::string>(1, "");
	std::vector<std::string> MoflexVec = std::vector<std::string>(1, "");
	std::vector<std::string> MBannerVec = std::vector<std::string>(1, "");
	uint8_t splitPos = 0;
};

int saveParameters(std::string parampath, VI9Pparameters parameters);
int loadParameters(std::string parampath, VI9Pparameters* parameters);
/*
return:
0 good
1 failed to save
2 failed to load (loadParameters)
3 failed to read param (loadParameters)
4 Bad version (loadParameters)
5 failed to find StrVerParam (loadParameters)
6 failed to load something from parameters (loadParameters)
7 IntMultiParam bad value
8 IntIconBorderParam bad value
9 IntCopycheckParam bad value
10 IntFFrewindParam bad value
11 IntFadeOptParam bad value
12 IntSplitPatchParam bad value
*/
int setParameter(std::string inpath, int number, std::string newValue, std::string outpath);
/*
return:
0 good
2 failed to load (loadParameters)
3 failed to read param (loadParameters)
4 Bad version (loadParameters)
5 failed to find StrVerParam (loadParameters)
6 failed to load something from parameters (loadParameters)
*/
int printParameter(std::string inpath);
/*
return:
0 good
2 failed to load (loadParameters)
3 failed to read param (loadParameters)
4 Bad version (loadParameters)
5 failed to find StrVerParam (loadParameters)
6 failed to load something from parameters (loadParameters)
7 no more rows to add
*/
int add_row(std::string inpath, std::string outpath);
/*
return:
0 good
2 failed to load (loadParameters)
3 failed to read param (loadParameters)
4 Bad version (loadParameters)
5 failed to find StrVerParam (loadParameters)
6 failed to load something from parameters (loadParameters)
7 no more rows to remove
*/
int sub_row(std::string inpath, std::string outpath);