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
		lines.erase(0, lines.find("std::string ") + sizeof("std::string ") - 1);
		lines.replace(lines.find(" = "), sizeof(" = ") - 1, "=");
		lines.erase(lines.end() - 1);//remove ;
	}
	for(auto lines : filelines) {//unique junk
		lines.replace(lines.find("\""), sizeof("\"") - 1, "\x1F");
		lines.replace(lines.rfind("\""), sizeof("\"") - 1, "\x1F");
		while(lines.find("\\'") != std::string::npos) {
			lines.replace(lines.find("\\'"), sizeof("\\'") - 1, "\'");
		}
		while(lines.find("\\\"") != std::string::npos) {
			lines.replace(lines.find("\\\""), sizeof("\\\"") - 1, "\"");
		}
		while(lines.find("\\\\") != std::string::npos) {
			lines.replace(lines.find("\\\\"), sizeof("\\\\") - 1, "\\");
		}
		while(lines.find("\\n") != std::string::npos) {
			lines.replace(lines.find("\\n"), sizeof("\\n") - 1, "\n");
		}
		while(lines.find("\\r") != std::string::npos) {
			lines.replace(lines.find("\\r"), sizeof("\\r") - 1, "\r");
		}
		while(lines.find("\\t") != std::string::npos) {
			lines.replace(lines.find("\\t"), sizeof("\\r") - 1, "\r");
		}
		outLangtxt << lines << '\n';
	}
	std::ofstream outLangcode("InLang.cpp", std::ios_base::out | std::ios_base::binary);
	for(auto& lines : filelines) {
		lines.replace(lines.begin() + lines.find_first_of("\"") + 1, lines.end() - 1, lines.substr(0, lines.find_first_of("=")));
		lines.insert(0, "std::string inLang");
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
	loadLanguageCpp_ << "bool loadLanguage(std::string LangPath) {"
						"\n"
						"	std::string outstr = \"\";"
						"\n"
						"	std::error_code error;"
						"\n"
						"	if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*LangPath.c_str()), error)) {"
						"\n"
						"		//xtd::forms::message_box::show(*this, std::string::format(\"{} \\\"{}\\\"\", FailedToFindPath, std::string::format(\"{}/{}/language/{}/Language.txt\", ProgramDir, resourcesPath, Lang)), std::string::format(\"{} {}\", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);"
						"\n"
						"		return false;"
						"\n"
						"	}"
						"\n"
						"	if(error)"
						"\n"
						"		return false;"
						"\n"
						"	std::vector<std::string> filelines = fileRead(LangPath);"
						"\n"
						"	if (filelines.size() == 0)"
						"\n"
						"		return false;"
						"\n"
						"	std::vector<std::string*> inLangVec = { ";
	for(auto lines : filelines) {
		std::string inLangtxt = lines;
		inLangtxt.erase(0, inLangtxt.find("std::string ") + sizeof("std::string ") - 1);
		inLangtxt.erase(inLangtxt.begin() + inLangtxt.find(" = "), inLangtxt.end());
		loadLanguageCpp_ << '&' << inLangtxt << ", ";
	}
	loadLanguageCpp_ << "};"
						"\n"
						"	std::vector<std::string*> LangVec = { ";
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
						"			//xtd::forms::message_box::show(*this, std::string::format(\"{} {}\\n{}.\", FailedToFindVar, *inLangVec[i], ValueNoChange), std::string::format(\"{} {}\", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);"
						"\n"
						"		}"
						"\n"
						"	}"
						"\n"
						"	return true;"
						"\n"
						"}";
}
