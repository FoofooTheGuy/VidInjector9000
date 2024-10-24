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

//main menu
extern std::string file;
extern std::string fileNew;
extern std::string fileOpen;
extern std::string fileSave;
extern std::string fileSaveAs;
extern std::string options;
extern std::string optionsShowLog;
extern std::string optionsSystemMode;
extern std::string optionsLightMode;
extern std::string optionsDarkMode;

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
extern std::string BannerPreviewText;
extern std::string FFrewindCheckText;
extern std::string DimCheckText;
extern std::string TextTooLongError;
extern std::string ImageInfoError;
extern std::string PlayerTitleText;
extern std::string MoflexFileText;
extern std::string MenuBannerText;
extern std::string SplitIntoAPatch;

//tool tips
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

//internal
extern std::string tempPath;

//resources
extern std::string resourcesPath;
extern std::string CLIFile;
extern std::string settingsFile;

//settings stuff
extern std::string StrDefaultLanguage;
extern std::string IntShowLog;
extern std::string IntColorMode;
