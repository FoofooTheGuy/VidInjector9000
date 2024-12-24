#pragma once

#include <string>

#include <wx/string.h>

//internal
extern wxString ProgramDir;
extern std::string version;

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
extern std::string language;
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
extern std::string ByMeText;
extern std::string GitHubLinker;

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
extern std::string githubRepoLink;

//resources
extern std::string resourcesPath;
extern std::string languagePath;
extern std::string imagePath;
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
extern std::string IntDefaultLanguage;
extern std::string IntShowLog;
extern std::string IntColorMode;
extern std::string IntFrameWidth;
extern std::string IntFrameHeight;
extern std::string StrImagesPath;
extern std::string StrVideosPath;

//use helper thing to make
extern std::string inLangLanguage;
extern std::string inLangErrorText;
extern std::string inLangMissingVariableError;
extern std::string inLangFailedToFindVar;
extern std::string inLangValueNoChange;
extern std::string inLangBadValue;
extern std::string inLangBadVersion;
extern std::string inLangSupportedVersion;
extern std::string inLangBeANumber;
extern std::string inLangframeText;
extern std::string inLanglogFrameText;
extern std::string inLangbuildFrameText;
extern std::string inLangaboutFrameText;
extern std::string inLangfile;
extern std::string inLangfileNew;
extern std::string inLangfileOpen;
extern std::string inLangfileSave;
extern std::string inLangfileSaveAs;
extern std::string inLangfileExport;
extern std::string inLangfileExtract;
extern std::string inLangfileImportSeed;
extern std::string inLangoptions;
extern std::string inLangoptionsShowLog;
extern std::string inLangoptionsSystemMode;
extern std::string inLangoptionsLightMode;
extern std::string inLangoptionsDarkMode;
extern std::string inLanglanguage;
extern std::string inLanghelp;
extern std::string inLanghelpAbout;
extern std::string inLangReturn;
extern std::string inLangModeText;
extern std::string inLangSingleVideo;
extern std::string inLangMultiVideo;
extern std::string inLangBrowse;
extern std::string inLangBannerText;
extern std::string inLangIconText;
extern std::string inLangShortNameText;
extern std::string inLangLongNameText;
extern std::string inLangPublisherText;
extern std::string inLangCopyrightCheckText;
extern std::string inLangBannerCustomText;
extern std::string inLangBannerPreviewText;
extern std::string inLangFFrewindCheckText;
extern std::string inLangDimCheckText;
extern std::string inLangTextTooLongError;
extern std::string inLangImageInfoError;
extern std::string inLangCopyFileError;
extern std::string inLangCLIError;
extern std::string inLangBuildError;
extern std::string inLangExtractError;
extern std::string inLangSeeLog;
extern std::string inLangFileSaved;
extern std::string inLangPlayerTitleText;
extern std::string inLangMoflexFileText;
extern std::string inLangMenuBannerText;
extern std::string inLangSplitIntoAPatch;
extern std::string inLangTitleIDText;
extern std::string inLangApplicationTitleText;
extern std::string inLangProductCodeText;
extern std::string inLangBuild;
extern std::string inLangCancel;
extern std::string inLangSuccessfullyBuilt;
extern std::string inLangSuccessfullyExtracted;
extern std::string inLangextractingArchive;
extern std::string inLangclickCancel;
extern std::string inLangByMeText;
extern std::string inLangGitHubLinker;
extern std::string inLangonlyMultiVideo;
extern std::string inLangmodeChoiceBoxTip;
extern std::string inLangbannerBoxTip;
extern std::string inLangbannerBrowseTip;
extern std::string inLangiconBoxTip;
extern std::string inLangiconBrowseTip;
extern std::string inLangshortnameBoxTip;
extern std::string inLanglongnameBoxTip;
extern std::string inLangpublisherBoxTip;
extern std::string inLangcopyBoxTip;
extern std::string inLangcopyCheckTip;
extern std::string inLangiconPreviewTip;
extern std::string inLangffRewindCheckTip;
extern std::string inLangdimCheckTip;
extern std::string inLangmultiBannerPreviewTip;
extern std::string inLangmultiBannerPreviewLeftTip;
extern std::string inLangmultiBannerPreviewRightTip;
extern std::string inLangmoflexFileTip;
extern std::string inLangplayerTitlesTip;
extern std::string inLangmoflexFilesTip;
extern std::string inLangmenuBannersTip;
extern std::string inLangmultiUpTip;
extern std::string inLangmultiDownTip;
extern std::string inLangmoflexBrowseTip;
extern std::string inLangmultiBannerBrowseTip;
extern std::string inLangremoveRowTip;
extern std::string inLangappendRowTip;
extern std::string inLangsplitPatchTip;
extern std::string inLangsplitPatchUpTip;
extern std::string inLangsplitPatchDownTip;
extern std::string inLangtitleIDBoxTip;
extern std::string inLangtitleIDButtonTip;
extern std::string inLangapplicationTitleBoxTip;
extern std::string inLangproductCodeBoxTip;
extern std::string inLangchooseDirSave;
extern std::string inLangmoflexFiles;
extern std::string inLangextractFiles;
extern std::string inLangvi9pFiles;
extern std::string inLangciaFiles;
extern std::string inLangtarFiles;
extern std::string inLangseeddbFiles;
extern std::string inLangallFiles;
