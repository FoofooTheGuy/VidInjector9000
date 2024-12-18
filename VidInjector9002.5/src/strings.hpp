#pragma once

#include <string>

#include <wx/string.h>

//internal
extern wxString ProgramDir;

//important
extern std::string ErrorText;
extern std::string MissingVariableError;
extern std::string FailedToFindVar;
extern std::string ValueNoChange;
//other
extern std::string Language;
extern std::string BadValue;
extern std::string BadVersion;
extern std::string SupportedVersion;
extern std::string BeANumber;

//frame text
extern std::string frameText;
extern std::string logFrameText;
extern std::string buildFrameText;
extern std::string aboutFrameText;

//main menu
extern std::string file;
extern std::string fileNew;
extern std::string fileOpen;
extern std::string fileSave;
extern std::string fileSaveAs;
extern std::string fileExport;
extern std::string fileExtract;
extern std::string fileImportSeed;
extern std::string options;
extern std::string optionsShowLog;
extern std::string optionsSystemMode;
extern std::string optionsLightMode;
extern std::string optionsDarkMode;
extern std::string help;
extern std::string helpAbout;

//widgets
extern std::string Return;
extern std::string ModeText;
extern std::string SingleVideo;
extern std::string MultiVideo;
extern std::string Browse;
extern std::string BannerText;
extern std::string IconText;
extern std::string ShortNameText;
extern std::string LongNameText;
extern std::string PublisherText;
extern std::string CopyrightCheckText;
extern std::string BannerCustomText;
extern std::string BannerPreviewText;
extern std::string FFrewindCheckText;
extern std::string DimCheckText;
extern std::string TextTooLongError;
extern std::string ImageInfoError;
extern std::string CopyFileError;
extern std::string CLIError;
extern std::string BuildError;
extern std::string ExtractError;
extern std::string SeeLog;
extern std::string FileSaved;
extern std::string PlayerTitleText;
extern std::string MoflexFileText;
extern std::string MenuBannerText;
extern std::string SplitIntoAPatch;
extern std::string TitleIDText;
extern std::string ApplicationTitleText;
extern std::string ProductCodeText;
extern std::string Build;
extern std::string Cancel;
extern std::string SuccessfullyBuilt;
extern std::string SuccessfullyExtracted;
extern std::string extractingArchive;
extern std::string clickCancel;

//tool tips
extern std::string onlyMultiVideo;
extern std::string modeChoiceBoxTip;
extern std::string bannerBoxTip;
extern std::string bannerBrowseTip;
extern std::string iconBoxTip;
extern std::string iconBrowseTip;
extern std::string shortnameBoxTip;
extern std::string longnameBoxTip;
extern std::string publisherBoxTip;
extern std::string copyBoxTip;
extern std::string copyCheckTip;
extern std::string iconPreviewTip;
extern std::string ffRewindCheckTip;
extern std::string dimCheckTip;
extern std::string multiBannerPreviewTip;
extern std::string multiBannerPreviewLeftTip;
extern std::string multiBannerPreviewRightTip;
extern std::string moflexFileTip;
extern std::string playerTitlesTip;
extern std::string moflexFilesTip;
extern std::string menuBannersTip;
extern std::string multiUpTip;
extern std::string multiDownTip;
extern std::string moflexBrowseTip;
extern std::string multiBannerBrowseTip;
extern std::string removeRowTip;
extern std::string appendRowTip;
extern std::string splitPatchTip;
extern std::string splitPatchUpTip;
extern std::string splitPatchDownTip;
extern std::string titleIDBoxTip;
extern std::string titleIDButtonTip;
extern std::string applicationTitleBoxTip;
extern std::string productCodeBoxTip;

//file stuffs (todo: image files? i dont know what all the image file extensions can be so maybe i'll just leave it at all files)
extern std::string chooseDirSave;
extern std::string moflexFiles;
extern std::string extractFiles;
extern std::string vi9pFiles;
extern std::string ciaFiles;
extern std::string tarFiles;
extern std::string seeddbFiles;
extern std::string allFiles;

//internal
extern std::string tempPath;
extern std::string CLItempPath;

//resources
extern std::string resourcesPath;
extern std::string CLIFile;
extern std::string settingsFile;
extern std::string vi9pFile;
extern std::string seedFile;

//vi9p parameters
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

//settings stuff
extern std::string StrDefaultLanguage;
extern std::string IntShowLog;
extern std::string IntColorMode;
extern std::string IntFrameWidth;
extern std::string IntFrameHeight;
extern std::string StrImagesPath;
extern std::string StrVideosPath;
