#include "settings.hpp"

std::string Settings::DefaultLanguage = "English";
bool Settings::ShowLog = 0;
int Settings::ColorMode = 2;

void saveSettings() {
	std::ofstream settingsfile(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + settingsFile).c_str()), std::ios_base::out | std::ios_base::binary);
	settingsfile <<
		StrDefaultLanguage << "=\"" << Settings::DefaultLanguage << "\"\n" <<
		IntShowLog << "=\"" << Settings::ShowLog << "\"\n" <<
		IntColorMode << "=\"" << Settings::ColorMode << "\"\n";
	settingsfile.close();
}

std::vector<int> loadSettings() {
	std::string outstr = "";
	uint32_t outrealint = 0;
	std::vector<int> ret;
	std::error_code error;
	if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + settingsFile).c_str()))) {
		std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath).c_str()), error);
		if (error)
			return std::vector<int>(1, 1);
		std::ofstream settingsfile(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + settingsFile).c_str()), std::ios_base::out | std::ios_base::binary);//save defaults
		settingsfile <<
			StrDefaultLanguage << "=\"" << "English" << "\"\n" <<
			IntShowLog << "=\"" << 0 << "\"\n" <<
			IntColorMode << "=\"" << 2 << "\"\n";
		settingsfile.close();
	}
	std::vector<std::string> filelines = fileRead(std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + settingsFile));
	if (filelines.empty())
		return std::vector<int>(1, 2);//what
	if (parseLines(outstr, filelines, StrDefaultLanguage)) {
		Settings::DefaultLanguage = outstr;
	}
	else {
		ret.push_back(3);
		//wxMessagebox(wxString::FromUTF8(FailedToFindVar + ' ' + StrDefaultLanguage + '\n' + ValueNoChange), wxString::FromUTF8(ErrorText + ' ' + MissingVariableError), wxICON_ERROR);
		//xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, StrDefaultLanguage, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
	}
	if (parseLines(outstr, filelines, IntShowLog)) {
		if (!stoul_s(outrealint, outstr)) {
			outrealint = 0;
			ret.push_back(4);
		}
		Settings::ShowLog = outrealint;
	}
	else {
		ret.push_back(5);
	}
	if (parseLines(outstr, filelines, IntColorMode)) {
		if (!stoul_s(outrealint, outstr)) {
			outrealint = 2;
			ret.push_back(6);
		}
		Settings::ColorMode = outrealint;
	}
	else {
		ret.push_back(7);
	}
	ret.push_back(0);
	return ret;
}
