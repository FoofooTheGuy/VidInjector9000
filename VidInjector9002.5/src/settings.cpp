#include "settings.hpp"

unsigned int Settings::DefaultLanguage = 0;
bool Settings::ShowLog = 0;
unsigned int Settings::ColorMode = 2;
bool Settings::DeleteTemp = 0;
bool Settings::UpdateCheck = 1;
int Settings::FrameWidth = 1150;
int Settings::FrameHeight = 700;
std::string Settings::ImagesPath = "";//we cant use the wxwidgets Get() junk yet
std::string Settings::VideosPath = "";

void saveSettings() {
	std::ofstream settingsfile(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + settingsFile).c_str()), std::ios_base::out | std::ios_base::binary);
	settingsfile <<
		IntDefaultLanguage << "=" << "\x1F" << Settings::DefaultLanguage << "\x1F" << "\n" <<
		IntShowLog << "=" << "\x1F" << Settings::ShowLog << "\x1F" << "\n" <<
		IntColorMode << "=" << "\x1F" << Settings::ColorMode << "\x1F" << "\n" <<
		IntDeleteTemp << "=" << "\x1F" << Settings::DeleteTemp << "\x1F" << "\n" <<
		IntUpdateCheck << "=" << "\x1F" << Settings::UpdateCheck << "\x1F" << "\n" <<
		IntFrameWidth << "=" << "\x1F" << Settings::FrameWidth << "\x1F" << "\n" <<
		IntFrameHeight << "=" << "\x1F" << Settings::FrameHeight << "\x1F" << "\n" <<
		StrImagesPath << "=" << "\x1F" << Settings::ImagesPath << "\x1F" << "\n" <<
		StrVideosPath << "=" << "\x1F" << Settings::VideosPath << "\x1F" << std::endl;
	settingsfile.close();
}

std::vector<int> loadSettings() {
	std::string outstr = "";
	signed int outrealint = 0;
	std::vector<int> ret;
	int errorValue = 3;//first not so bad value we have is 3
	std::error_code error;
	if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + settingsFile).c_str()))) {
		std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath).c_str()), error);
		if (error)
			return std::vector<int>(1, 1);
		
		Settings::DefaultLanguage = 0;
		Settings::ShowLog = 0;
		Settings::ColorMode = 2;
		Settings::DeleteTemp = 0;
		Settings::UpdateCheck = 1;
		Settings::FrameWidth = 1150;
		Settings::FrameHeight = 700;
		Settings::ImagesPath = std::string(wxStandardPaths::Get().GetUserDir(wxStandardPaths::Dir_Pictures).ToUTF8());
		Settings::VideosPath = std::string(wxStandardPaths::Get().GetUserDir(wxStandardPaths::Dir_Videos).ToUTF8());
		saveSettings();
	}
	std::vector<std::string> filelines = fileRead(std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + settingsFile));
	if (filelines.empty())
		return std::vector<int>(1, 2);//maybe your disk is read only... at that point i think you have bigger problems than loading settings in FoofooTheGuy's VidInjector9002.5
	if (parseLines(outstr, filelines, IntDefaultLanguage)) {
		if (!ASCII2number<int>(&outrealint, outstr)) {
			outrealint = 0;
			ret.push_back(errorValue);
		}
		Settings::DefaultLanguage = outrealint;
		errorValue += 2;
	}
	else {
		++errorValue;
		ret.push_back(errorValue++);
	}
	
	if (parseLines(outstr, filelines, IntShowLog)) {
		if (!ASCII2number<int>(&outrealint, outstr)) {
			outrealint = 0;
			ret.push_back(errorValue);
		}
		Settings::ShowLog = outrealint ? 1 : 0;
		errorValue += 2;
	}
	else {
		++errorValue;
		ret.push_back(errorValue++);
	}
	
	if (parseLines(outstr, filelines, IntColorMode)) {
		if (!ASCII2number<int>(&outrealint, outstr)) {
			outrealint = 2;
			ret.push_back(errorValue);
		}
		Settings::ColorMode = outrealint;
		errorValue += 2;
	}
	else {
		++errorValue;
		ret.push_back(errorValue++);
	}
	
	
	if (parseLines(outstr, filelines, IntDeleteTemp)) {
		if (!ASCII2number<int>(&outrealint, outstr)) {
			outrealint = 0;
			ret.push_back(errorValue);
		}
		Settings::DeleteTemp = outrealint ? 1 : 0;
		errorValue += 2;
	}
	else {
		++errorValue;
		ret.push_back(errorValue++);
	}
	
	if (parseLines(outstr, filelines, IntUpdateCheck)) {
		if (!ASCII2number<int>(&outrealint, outstr)) {
			outrealint = 1;
			ret.push_back(errorValue);
		}
		Settings::UpdateCheck = outrealint ? 1 : 0;
		errorValue += 2;
	}
	else {
		++errorValue;
		ret.push_back(errorValue++);
	}
	
	if (parseLines(outstr, filelines, IntFrameWidth)) {
		if (!ASCII2number<int>(&outrealint, outstr)) {
			outrealint = 1150;
			ret.push_back(errorValue);
		}
		Settings::FrameWidth = outrealint;
		errorValue += 2;
	}
	else {
		++errorValue;
		ret.push_back(errorValue++);
	}
	
	if (parseLines(outstr, filelines, IntFrameHeight)) {
		if (!ASCII2number<int>(&outrealint, outstr)) {
			outrealint = 700;
			ret.push_back(errorValue);
		}
		Settings::FrameHeight = outrealint;
		errorValue += 2;
	}
	else {
		++errorValue;
		ret.push_back(errorValue++);
	}
	
	if (parseLines(outstr, filelines, StrImagesPath)) {
		Settings::ImagesPath = outstr;
	}
	else {
		ret.push_back(errorValue);
	}
	++errorValue;
	
	if (parseLines(outstr, filelines, StrVideosPath)) {
		Settings::VideosPath = outstr;
	}
	else {
		ret.push_back(errorValue);
	}
	++errorValue;
	
	ret.push_back(0);
	return ret;
}
