#pragma once

#include <string>

#include <wx/string.h>

// internal stuff that you don't translate

// stuff for use in version and tag. make sure it matches what you put on github
#define MAJ "3"
#define MIN "5"
#define PAT "2"

// internal
extern wxString ProgramDir;
extern std::string version;
extern std::string internalTag;

extern std::string tempPath;
extern std::string CLItempPath;
extern std::string githubRepo;
extern std::string githubRepoLink;

// resources
extern std::string resourcesPath;
extern std::string languagePath;
extern std::string imagePath;
extern std::string CLIFile;
extern std::string settingsFile;
extern std::string vi9pFile;
extern std::string seedFile;

// vi9p parameters
extern std::string StrVerParam;
extern std::string IntMultiParam;
extern std::string StrBannerParam;
extern std::string StrIconParam;
extern std::string IntIconBorderParam;
extern std::string StrSNameParam;
extern std::string StrLNameParam;
extern std::string StrPublisherParam;
extern std::string IntCopycheckParam;
extern std::string StrCopyrightParam;
extern std::string IntFFrewindParam;
extern std::string IntFadeOptParam;
extern std::string IntRowsParam;
extern std::string StrPTitleParam;
extern std::string StrMoflexParam;
extern std::string StrMBannerParam;
extern std::string IntSplitPatchParam;

// settings stuff
extern std::string IntDefaultLanguage;
extern std::string IntShowLog;
extern std::string IntColorMode;
extern std::string IntDeleteTemp;
extern std::string IntUpdateCheck;
extern std::string IntFrameWidth;
extern std::string IntFrameHeight;
extern std::string StrImagesPath;
extern std::string StrVideosPath;
