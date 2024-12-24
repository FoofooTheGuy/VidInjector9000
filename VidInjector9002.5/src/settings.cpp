#include "settings.hpp"

int Settings::DefaultLanguage = 0;
bool Settings::ShowLog = 0;
int Settings::ColorMode = 2;
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
		IntFrameWidth << "=" << "\x1F" << Settings::FrameWidth << "\x1F" << "\n" <<
		IntFrameHeight << "=" << "\x1F" << Settings::FrameHeight << "\x1F" << "\n" <<
		StrImagesPath << "=" << "\x1F" << Settings::ImagesPath << "\x1F" << "\n" <<
		StrVideosPath << "=" << "\x1F" << Settings::VideosPath << "\x1F" << std::endl;
	settingsfile.close();
}

std::vector<int> loadSettings() {
	std::string outstr = "";
	int outrealint = 0;
	std::vector<int> ret;
	std::error_code error;
	if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + settingsFile).c_str()))) {
		std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath).c_str()), error);
		if (error)
			return std::vector<int>(1, 1);
		
		Settings::DefaultLanguage = 0;
		Settings::ShowLog = 0;
		Settings::ColorMode = 2;
		Settings::FrameWidth = 1150;
		Settings::FrameHeight = 700;
		Settings::ImagesPath = std::string(wxStandardPaths::Get().GetUserDir(wxStandardPaths::Dir_Pictures).ToUTF8());
		Settings::VideosPath = std::string(wxStandardPaths::Get().GetUserDir(wxStandardPaths::Dir_Videos).ToUTF8());
		saveSettings();
	}
	std::vector<std::string> filelines = fileRead(std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + settingsFile));
	if (filelines.empty())
		return std::vector<int>(1, 2);//what
	if (parseLines(outstr, filelines, IntDefaultLanguage)) {
		if (!ASCII2number<int>(&outrealint, outstr)) {
			outrealint = 0;
			ret.push_back(3);
		}
		Settings::DefaultLanguage = outrealint;
	}
	else {
		ret.push_back(4);
	}
	if (parseLines(outstr, filelines, IntShowLog)) {
		if (!ASCII2number<int>(&outrealint, outstr)) {
			outrealint = 0;
			ret.push_back(5);
		}
		Settings::ShowLog = outrealint;
	}
	else {
		ret.push_back(6);
	}
	if (parseLines(outstr, filelines, IntColorMode)) {
		if (!ASCII2number<int>(&outrealint, outstr)) {
			outrealint = 2;
			ret.push_back(7);
		}
		Settings::ColorMode = outrealint;
	}
	else {
		ret.push_back(8);
	}
	if (parseLines(outstr, filelines, IntFrameWidth)) {
		if (!ASCII2number<int>(&outrealint, outstr)) {
			outrealint = 2;
			ret.push_back(9);
		}
		Settings::FrameWidth = outrealint;
	}
	else {
		ret.push_back(10);
	}
	if (parseLines(outstr, filelines, IntFrameHeight)) {
		if (!ASCII2number<int>(&outrealint, outstr)) {
			outrealint = 2;
			ret.push_back(11);
		}
		Settings::FrameHeight = outrealint;
	}
	else {
		ret.push_back(12);
	}
	if (parseLines(outstr, filelines, StrImagesPath)) {
		Settings::ImagesPath = outstr;
	}
	else {
		ret.push_back(13);
	}
	if (parseLines(outstr, filelines, StrVideosPath)) {
		Settings::VideosPath = outstr;
	}
	else {
		ret.push_back(14);
	}
	ret.push_back(0);
	return ret;
}
