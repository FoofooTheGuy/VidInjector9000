#pragma once

#include <filesystem>
#include <fstream>
#include <vector>

#include <wx/stdpaths.h>

#include "programStrings.hpp"
#include "fileReading.hpp"
#include "formatting.hpp"

class Settings
{
	public:
    static unsigned int DefaultLanguage;
    static bool ShowLog;
    static unsigned int ColorMode;
    static bool DeleteTemp;
    static bool UpdateCheck;
    static int FrameWidth;
    static int FrameHeight;
    static std::string ImagesPath;
    static std::string VideosPath;
};

void saveSettings();

/*
returns a vector of int containing each fail (if any happened)
0 success (this will either be the last element in the vector if something fails or it will not be there if there is a problem with the settings file itself (see fail 1 and fail 2))
fails:
1 create resources dir
2 settings file
3 bad value IntDefaultLanguage
4 failed to find IntDefaultLanguage
5 bad value IntShowLog
6 failed to find IntShowLog
7 bad value IntColorMode
8 failed to find IntColorMode
9 bad value IntDeleteTemp
10 failed to find IntDeleteTemp
11 bad value IntFrameWidth
12 failed to find IntFrameWidth
13 bad value IntFrameHeight
14 failed to find IntFrameHeight
15 failed to find StrImagesPath
16 failed to find StrVideosPath
*/
std::vector<int> loadSettings();
