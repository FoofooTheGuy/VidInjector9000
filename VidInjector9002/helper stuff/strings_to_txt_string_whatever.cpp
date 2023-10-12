#include <iostream>
#include <vector>
#include <string>
#include <fstream>

int main(int argc, char* argv[]) {
	if(argc < 2) return -1;
	std::vector<std::string> filelines;
	std::ifstream infile(argv[1], std::ios_base::in | std::ios_base::binary);
	char Byte;
	std::string line = "";
	//split file into the vector
	infile.read(&Byte, 1);//grab first byte of file
	while (infile) {//continue until input stream fails
		if(Byte == '\n') {
			filelines.push_back(line);
			line = "";
		}
		line += Byte;//append byte to string
		infile.read(&Byte, 1);//grab next byte of file
	}
	std::ofstream outLangtxt("Language.txt", std::ios_base::out | std::ios_base::binary);
	for(auto& lines : filelines) {
		lines.erase(0, lines.find("xtd::ustring ") + sizeof("xtd::ustring ") - 1);
		lines.replace(lines.find(" = "), sizeof(" = ") - 1, "=");
		lines.erase(lines.end() - 1);
		outLangtxt << lines << '\n';
	}
	std::ofstream outLangcode("InLang.cpp", std::ios_base::out | std::ios_base::binary);
	for(auto& lines : filelines) {
		lines.replace(lines.begin() + lines.find_first_of("\"") + 1, lines.end() - 1, lines.substr(0, lines.find_first_of("=")));
		lines.insert(0, "xtd::ustring inLang");
		lines.replace(lines.find_first_of("="), sizeof("=") - 1, " = ");
		lines += ';';
		outLangcode << lines << '\n';
	}
	std::ofstream outLangheader("InLang.hpp", std::ios_base::out | std::ios_base::binary);
	for(auto lines : filelines) {
		lines.erase(lines.begin() + lines.find(" = "), lines.end() - 1);
		lines.insert(0, "extern ");
		outLangheader << lines << '\n';
	}
	std::ofstream loadLanguageCpp_("loadLanguage.cpp", std::ios_base::out | std::ios_base::binary);
	loadLanguageCpp_ << "bool loadLanguage(xtd::ustring Lang) {"
						"\n"
						"	xtd::ustring outstr = \"\";"
						"\n"
						"	if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*xtd::ustring::format(\"{}/{}/language/{}/Language.txt\", ProgramDir, resourcesPath, Lang).c_str()))) {"
						"\n"
						"		std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*xtd::ustring::format(\"{}/{}/language/English\", ProgramDir, resourcesPath).c_str()));"
						"\n"
						"		std::ofstream M_file(std::filesystem::path((const char8_t*)&*xtd::ustring::format(\"{}/{}/language/English/M.bmp\", ProgramDir, resourcesPath).c_str()), std::ios_base::out | std::ios_base::binary);"
						"\n"
						"		M_file.write(reinterpret_cast<const char*>(M_bmp), sizeof(M_bmp));"
						"\n"
						"		M_file.close();"
						"\n"
						"		xtd::forms::message_box::show(*this, xtd::ustring::format(\"{} \\\"{}\\\"\", FailedToFindPath, xtd::ustring::format(\"{}/{}/language/{}/Language.txt\", ProgramDir, resourcesPath, Lang)), xtd::ustring::format(\"{} {}\", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);"
						"\n"
						"		return false;"
						"\n"
						"	}"
						"\n"
						"	std::vector<xtd::ustring> filelines = fileRead(xtd::ustring::format(\"{}/{}/language/{}/Language.txt\", ProgramDir, resourcesPath, Lang));"
						"\n"
						"	if (filelines.size() == 0)"
						"\n"
						"		return false;"
						"\n"
						"	std::vector<xtd::ustring*> inLangVec = { ";
	for(auto lines : filelines) {
		std::string inLangtxt = lines;
		inLangtxt.erase(0, inLangtxt.find("xtd::ustring ") + sizeof("xtd::ustring ") - 1);
		inLangtxt.erase(inLangtxt.begin() + inLangtxt.find(" = "), inLangtxt.end());
		loadLanguageCpp_ << '&' << inLangtxt << ", ";
	}
	loadLanguageCpp_ << "};"
						"\n"
						"	std::vector<xtd::ustring*> LangVec = { ";
	for(auto lines : filelines) {
		std::string Langtxt = lines;
		Langtxt.erase(0, Langtxt.find("inLang") + sizeof("inLang") - 1);
		Langtxt.erase(Langtxt.begin() + Langtxt.find_first_of("=") - 1, Langtxt.end());
		loadLanguageCpp_ << '&' << Langtxt << ", ";
	}
	loadLanguageCpp_ << "};"
						"\n"
						"	for (size_t i = 0; i < inLangVec.size(); i++) {"
						"\n"
						"		if (parseLines(outstr, filelines, *inLangVec[i])) {"
						"\n"
						"			*LangVec[i] = outstr;"
						"\n"
						"		}"
						"\n"
						"		else {"
						"\n"
						"			xtd::forms::message_box::show(*this, xtd::ustring::format(\"{} {}\\n{}.\", FailedToFindVar, *inLangVec[i], ValueNoChange), xtd::ustring::format(\"{} {}\", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);"
						"\n"
						"		}"
						"\n"
						"	}"
						"\n"
						"	Languagedir = Lang;"
						"\n"
						"	return true;"
						"\n"
						"}";
}