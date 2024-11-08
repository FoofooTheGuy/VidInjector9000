#pragma once

#include <filesystem>
#include <fstream>
#include <vector>

#include "fileReading.hpp"
#include "formatting.hpp"
#include "strings.hpp"

class Settings
{
	public:
    static std::string DefaultLanguage;
    static bool ShowLog;
    static int ColorMode;
    static int FrameWidth;
    static int FrameHeight;
};

void saveSettings();

/*
returns a vector of int containing each fail (if any happened)
0 success (this will either be the last element in the vector if something fails or it will not be there if there is a problem with the settings file itself (see fail 1 and fail 2))
fails:
1 create resources dir
2 settings file
3 StrDefaultLanguage
4 bad value IntShowLog
5 IntShowLog
6 bad value IntDarkMode
7 IntDarkMode
*/
std::vector<int> loadSettings();
