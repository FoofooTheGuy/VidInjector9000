#include "fs.hpp"

int make_movie_title(const VI9Pparameters& parameters, const std::string& romfsPath, const std::string& outCIA, const std::string& outTAR, const bool& dopatch) {
	std::error_code error;
	
	std::cout << CreatingFile << " romfs/movie_title.csv" << std::endl;
	
	std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/movie").c_str()), error);
	if (error) {
		std::cout << ErrorText << ' ' << romfsPath + "/movie" << '\n' << error.message() << std::endl;
		std::cout << ErrorText << ' ' << FailedToCreateFile << " \"" << (dopatch ? outTAR : outCIA) << '\"' << std::endl;
		return 8;
	}
	
	std::ofstream movie_title(std::string(romfsPath + "/movie/movie_title.csv").c_str(), std::ios_base::out | std::ios_base::binary);
	
	movie_title << "\xFF\xFE" + UTF8toUTF16("#JP,#EN,#FR,#GE,#IT,#SP,#CH,#KO,#DU,#PO,#RU,#TW\x0D\x0A");
	for (int i = 0; i < (parameters.mode ? ((parameters.splitPos && !dopatch) ? parameters.splitPos : parameters.rows) : 1); i++) { // this should be fine since we don't need this in extended anyway
		std::string outstr = parameters.PTitleVec.at(i);
		
		if (outstr[0] == '#') { // sneakily fix the string huhuhu
			outstr[0] = '\\';
			outstr.insert(1, "x23");
		}
		for (size_t j = 0; j < outstr.size(); j++) {
			if (outstr[j] == ',') {
				outstr[j] = '\\';
				outstr.insert(j + 1, "x2C");
			}
		}
		for (size_t j = 0; j < outstr.size(); j++) {
			if (outstr[j] == '\n') {
				outstr[j] = '\\';
				outstr.insert(j + 1, "n");
			}
		}
		for (int j = 0; j < 11; j++) { // do it 11 times because it needs to
			movie_title << UTF8toUTF16(outstr + ",");
		}
		movie_title << UTF8toUTF16(outstr + "\x0D\x0A"); // put the last stuff
	}
	movie_title.close();
	if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/movie/movie_title.csv").c_str()), error)) {
		std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << romfsPath << "/movie/movie_title.csv" << std::endl;
		return 9;
	}
	if (error) {
		return 10;
	}
	
	return 0;
}

int make_settingsTL(const VI9Pparameters& parameters, const std::string& romfsPath, const std::string uniqueIDstr, const std::string& outCIA, const std::string& outTAR, const bool& dopatch) {
	std::error_code error;
	
	std::cout << CreatingFile << " romfs/settings/settingsTL.csv" << std::endl;
	
	std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/settings").c_str()), error);
	if (error) {
		std::cout << ErrorText << ' ' << romfsPath + "/settings" << '\n' << error.message() << std::endl;
		std::cout << ErrorText << ' ' << FailedToCreateFile << " \"" << (dopatch ? outTAR : outCIA) << '\"' << std::endl;
		return 11;
	}
	std::ofstream settingsTL(std::string(romfsPath + "/settings/settingsTL.csv").c_str(), std::ios_base::out | std::ios_base::binary);
	
	std::string outlongname = parameters.Lname;
	if (outlongname.empty()) { // it will read the file wrong if this is missing
		outlongname = " ";
	}
	if (outlongname[0] == '#') { // sneakily fix the string huhuhu
		outlongname[0] = '\\';
		outlongname.insert(1, "x23");
	}
	for (size_t j = 0; j < outlongname.size(); j++) {
		if (outlongname[j] == ',') {
			outlongname[j] = '\\';
			outlongname.insert(j + 1, "x2C");
		}
	}
	for (size_t j = 0; j < outlongname.size(); j++) {
		if (outlongname[j] == '\n') {
			outlongname[j] = '\\';
			outlongname.insert(j + 1, "n");
		}
	}
	
	std::string outpublisher = parameters.publisher;
	if (outpublisher.empty()) {
		outpublisher = " ";
	}
	if (outpublisher[0] == '#') { // sneakily fix the string huhuhu
		outpublisher[0] = '\\';
		outpublisher.insert(1, "x23");
	}
	for (size_t j = 0; j < outpublisher.size(); j++) {
		if (outpublisher[j] == ',') {
			outpublisher[j] = '\\';
			outpublisher.insert(j + 1, "x2C");
		}
	}
	for (size_t j = 0; j < outpublisher.size(); j++) {
		if (outpublisher[j] == '\n') {
			outpublisher[j] = '\\';
			outpublisher.insert(j + 1, "n");
		}
	}
	
	settingsTL << "\xFF\xFE" +
		UTF8toUTF16("# おしらせURL\x0D\x0A" // announcement URL?
			"# JP:\x0D\x0A"
			"none\x0D\x0A"
			"\x0D\x0A"
			"# EN:\x0D\x0A"
			"none\x0D\x0A"
			"\x0D\x0A"
			"# FR:\x0D\x0A"
			"none\x0D\x0A"
			"\x0D\x0A"
			"# GE:\x0D\x0A"
			"none\x0D\x0A"
			"\x0D\x0A"
			"# IT:\x0D\x0A"
			"none\x0D\x0A"
			"\x0D\x0A"
			"# SP:\x0D\x0A"
			"none\x0D\x0A"
			"\x0D\x0A"
			"# CN:\x0D\x0A"
			"none\x0D\x0A"
			"\x0D\x0A"
			"# KO:\x0D\x0A"
			"none\x0D\x0A"
			"\x0D\x0A"
			"# DU:\x0D\x0A"
			"none\x0D\x0A"
			"\x0D\x0A"
			"# PO:\x0D\x0A"
			"none\x0D\x0A"
			"\x0D\x0A"
			"# RU:\x0D\x0A"
			"none\x0D\x0A"
			"\x0D\x0A"
			"# TW:\x0D\x0A"
			"none\x0D\x0A"
			"\x0D\x0A"
			"# アプリ名（ロングネーム）\x0D\x0A" // app long name
			"# JP:\x0D\x0A"
			+ outlongname + "\x0D\x0A"
			"\x0D\x0A"
			"# EN:\x0D\x0A"
			+ outlongname + "\x0D\x0A"
			"\x0D\x0A"
			"# FR:\x0D\x0A"
			+ outlongname + "\x0D\x0A"
			"\x0D\x0A"
			"# GE:\x0D\x0A"
			+ outlongname + "\x0D\x0A"
			"\x0D\x0A"
			"# IT:\x0D\x0A"
			+ outlongname + "\x0D\x0A"
			"\x0D\x0A"
			"# SP:\x0D\x0A"
			+ outlongname + "\x0D\x0A"
			"\x0D\x0A"
			"# CN:\x0D\x0A"
			+ outlongname + "\x0D\x0A"
			"\x0D\x0A"
			"# KO:\x0D\x0A"
			+ outlongname + "\x0D\x0A"
			"\x0D\x0A"
			"# DU:\x0D\x0A"
			+ outlongname + "\x0D\x0A"
			"\x0D\x0A"
			"# PO:\x0D\x0A"
			+ outlongname + "\x0D\x0A"
			"\x0D\x0A"
			"# RU:\x0D\x0A"
			+ outlongname + "\x0D\x0A"
			"\x0D\x0A"
			"# TW:\x0D\x0A"
			+ outlongname + "\x0D\x0A"
			"\x0D\x0A"
			"# 拡張セーブデータのID（16進数）\x0D\x0A" // save data ID
			+ uniqueIDstr + "\x0D\x0A" // make it the save as unique ID because yes
			"\x0D\x0A"
			"# NADLタスクのID\x0D\x0A"
			"none\x0D\x0A"
			"\x0D\x0A"
			"# タスクの実行間隔（h）（10進数）\x0D\x0A"
			"0\x0D\x0A"
			"\x0D\x0A"
			"# タスクの実行回数（10進数）\x0D\x0A"
			"0\x0D\x0A"
			"\x0D\x0A"
			"# おしらせのあり、なし\x0D\x0A" // not sure what this is, but if you enable it in single vid it instantly crashes... maybe it's the thing telling you to take a break? nah because it's false and that still appears
			"false\x0D\x0A"
			"\x0D\x0A"
			"# 早送り、巻戻しボタンのあり、なし\x0D\x0A" // ff rewind
			+ (parameters.FFrewind ? "true" : "false") + "\x0D\x0A"
			"\x0D\x0A"
			"# 優しさ演出のあり、なし\x0D\x0A" // gentleness
			+ (parameters.FadeOpt ? "true" : "false") + "\x0D\x0A");
	
	if (parameters.mode) {
		settingsTL << UTF8toUTF16("\x0D\x0A"
			"# 動画の数\x0D\x0A" // amount of videos
			+ std::to_string((parameters.splitPos && !dopatch) ? parameters.splitPos : parameters.rows) + "\x0D\x0A"
			"\x0D\x0A"
			"# 動画パブリッシャー名\x0D\x0A" // publisher name
			"# JP:\x0D\x0A"
			+ outpublisher + "\x0D\x0A"
			"\x0D\x0A"
			"# EN:\x0D\x0A"
			+ outpublisher + "\x0D\x0A"
			"\x0D\x0A"
			"# FR:\x0D\x0A"
			+ outpublisher + "\x0D\x0A"
			"\x0D\x0A"
			"# GE:\x0D\x0A"
			+ outpublisher + "\x0D\x0A"
			"\x0D\x0A"
			"# IT:\x0D\x0A"
			+ outpublisher + "\x0D\x0A"
			"\x0D\x0A"
			"# SP:\x0D\x0A"
			+ outpublisher + "\x0D\x0A"
			"\x0D\x0A"
			"# CN:\x0D\x0A"
			+ outpublisher + "\x0D\x0A"
			"\x0D\x0A"
			"# KO:\x0D\x0A"
			+ outpublisher + "\x0D\x0A"
			"\x0D\x0A"
			"# DU:\x0D\x0A"
			+ outpublisher + "\x0D\x0A"
			"\x0D\x0A"
			"# PO:\x0D\x0A"
			+ outpublisher + "\x0D\x0A"
			"\x0D\x0A"
			"# RU:\x0D\x0A"
			+ outpublisher + "\x0D\x0A"
			"\x0D\x0A"
			"# TW:\x0D\x0A"
			+ outpublisher + "\x0D\x0A"
			"\x0D\x0A"
			"# WEBブラウザ用のURL\x0D\x0A" // web browser URL (?)
			"# JP:\x0D\x0A"
			"\x0D\x0A"
			"\x0D\x0A"
			"# EN:\x0D\x0A"
			"\x0D\x0A"
			"\x0D\x0A"
			"# FR:\x0D\x0A"
			"\x0D\x0A"
			"\x0D\x0A"
			"# GE:\x0D\x0A"
			"\x0D\x0A"
			"\x0D\x0A"
			"# IT:\x0D\x0A"
			"\x0D\x0A"
			"\x0D\x0A"
			"# SP:\x0D\x0A"
			"\x0D\x0A"
			"\x0D\x0A"
			"# CN:\x0D\x0A"
			"\x0D\x0A"
			"\x0D\x0A"
			"# KO:\x0D\x0A"
			"\x0D\x0A"
			"\x0D\x0A"
			"# DU:\x0D\x0A"
			"\x0D\x0A"
			"\x0D\x0A"
			"# PO:\x0D\x0A"
			"\x0D\x0A"
			"\x0D\x0A"
			"# RU:\x0D\x0A"
			"\x0D\x0A"
			"\x0D\x0A"
			"# TW:");
	}
	settingsTL.close();
	if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/settings/settingsTL.csv").c_str()), error)) {
		std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << romfsPath << "/settings/settingsTL.csv" << std::endl;
		return 12;
	}
	if(error) {
		return 13;
	}
	
	return 0;
}

int make_information_buttons(const VI9Pparameters& parameters, const std::string& romfsPath, const std::string& outCIA) {
	std::error_code error;
	
	std::cout << CreatingFile << " romfs/settings/information_buttons.csv" << std::endl;
	
	std::filesystem::create_directories(std::string(romfsPath + "/settings").c_str(), error); // just in case Hehehhhah
	if (error) {
		std::cout << ErrorText << ' ' << romfsPath + "/settings" << '\n' << error.message() << std::endl;
		std::cout << ErrorText << ' ' << FailedToCreateFile << " \"" << outCIA << '\"' << std::endl;
		return 14;
	}
	std::ofstream information_buttons(std::string(romfsPath + "/settings/information_buttons.csv").c_str(), std::ios_base::out | std::ios_base::binary);
	information_buttons << (parameters.copycheck ? ("\xFF\xFE" + UTF8toUTF16("Copyright")) : "\xFF\xFE");
	information_buttons.close();
	if (!std::filesystem::exists(std::string(romfsPath + "/settings/information_buttons.csv").c_str(), error)) {
		std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << romfsPath << "/settings/information_buttons.csv" << std::endl;
		return 15;
	}
	if(error) {
		return 16;
	}
	
	if (parameters.copycheck) {
		std::cout << CreatingFile << " romfs/settings/copyright.txt" << std::endl;
		
		std::ofstream copyrighttxt(std::string(romfsPath + "/settings/copyright.txt").c_str(), std::ios_base::out | std::ios_base::binary);
		copyrighttxt << "\xFF\xFE" + UTF8toUTF16(parameters.copyrightInfo);
		copyrighttxt.close();
		if (!std::filesystem::exists(std::string(romfsPath + "/settings/copyright.txt").c_str(), error)) {
			std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << romfsPath << "/settings/copyright.txt" << std::endl;
			return 17;
		}
		if(error) {
			return 18;
		}
	}
	
	return 0;
}

int make_Moflex(const VI9Pparameters& parameters, const std::string& romfsPath, const std::string& outCIA, const std::string& outTAR, const bool& dopatch) {
	std::error_code error;
	
	uint8_t Checker[4];
	for (int i = dopatch ? parameters.splitPos : 0; i < (parameters.mode ? ((parameters.splitPos && !dopatch) ? parameters.splitPos : parameters.rows) : 1); i++) {
		if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*parameters.MoflexVec.at(i).c_str()), error)) {
			std::cout << ErrorText << ' ' << FailedToFindPath << " \"" << parameters.MoflexVec.at(i) << "\"\n" << StrMoflexParam << '(' << i << ')' << std::endl;
			return 19;
		}
		if(error) {
			return 20;
		}
		std::string extension = parameters.MoflexVec.at(i).c_str();
		if (extension.find_last_of(".") != std::string::npos) {
			extension.erase(extension.begin(), extension.begin() + extension.find_last_of("."));
		}
		std::ifstream inmoflex(std::filesystem::path((const char8_t*)&*parameters.MoflexVec.at(i).c_str()), std::ios_base::in | std::ios::binary);
		for (int j = 0; j < 4; j++) {
			inmoflex >> Checker[j]; // https://stackoverflow.com/a/2974735
			if (extension != ".moflex" || Checker[j] != moflexMagic_bin_data[j]) {
				std::cout << ErrorText << ' ' << BadValue << "\n\"" << parameters.MoflexVec.at(i) << "\" " << MoflexError << std::endl;
				return 21;
			}
		}
		if(parameters.mode < 2) {
			std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/movie").c_str()), error);
			if (error) {
				std::cout << ErrorText << ' ' << romfsPath + "/movie" << '\n' << error.message() << std::endl;
				std::cout << ErrorText << ' ' << FailedToCreateFile << " \"" << (dopatch ? outTAR : outCIA) << '\"' << std::endl;
				return 22;
			}
		}
		switch(parameters.mode) {
			case 0: {
				std::cout << CopyingMoflex << " 1/1" << std::endl;
				//std::error_code error;
				error = copyfile(parameters.MoflexVec.at(i).c_str(), std::string(romfsPath + "/movie/movie.moflex").c_str());
				if (error) {
					std::cout << ErrorText << ' ' << FailedToCopyFile << "\n\"" << parameters.MoflexVec.at(i) << "\" -> \"" << romfsPath << "/movie/movie.moflex\"\n" << error.message() << std::endl;
					std::cout << ErrorText << ' ' << FailedToCreateFile << ' ' << romfsPath << "/movie/movie.moflex" << std::endl;
					return 26;
				}
				if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/movie/movie.moflex").c_str()), error)) { // this probably only happens if there's no disk space
					std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << romfsPath << "/movie/movie.moflex" << std::endl;
					return 27;
				}
				if(error) {
					return 28;
				}
			}
			break;
			case 1: {
				std::cout << CopyingMoflex << ' ' << std::to_string(i + 1) << '/' << std::to_string(parameters.rows) << std::endl;
				//std::error_code error;
				error = copyfile(parameters.MoflexVec.at(i).c_str(), std::string(romfsPath + "/movie/movie_" + std::to_string(i) + ".moflex").c_str());
				if (error) {
					std::cout << ErrorText << ' ' << FailedToCopyFile << "\n\"" << parameters.MoflexVec.at(i) << "\" -> \"" << romfsPath << "/movie/movie_" << std::to_string(i) << ".moflex\"\n" << error.message() << std::endl;
					std::cout << ErrorText << ' ' << FailedToCreateFile << ' ' << romfsPath << "/movie/movie_" << std::to_string(i) << ".moflex" << std::endl;
					return 23;
				}
				if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/movie/movie_" + std::to_string(i) + ".moflex").c_str()), error)) {
					std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << romfsPath << "/movie/movie_" << std::to_string(i) << ".moflex" << std::endl;
					return 24;
				}
				if(error) {
					return 25;
				}
			}
			break;
			case 2: {
				std::cout << CopyingMoflex << ' ' << std::to_string(i + 1) << '/' << std::to_string(parameters.rows) << std::endl;
				//std::error_code error;
				
				std::string moflexFile = "";
				if(parameters.PTitleVec.at(i).empty()) {
					size_t start = parameters.MoflexVec.at(i).find_last_of("\\/");
					if (start == std::string::npos) { // no path
						moflexFile = parameters.MoflexVec.at(i);
						std::cout << moflexFile << std::endl;
					}
					else {
						moflexFile = parameters.MoflexVec.at(i);
						moflexFile.erase(moflexFile.begin(), moflexFile.begin() + start);
						std::cout << moflexFile << std::endl;
					}
				}
				else { // rename to player title
					moflexFile = parameters.PTitleVec.at(i) + ".moflex";
				}
				
				error = copyfile(parameters.MoflexVec.at(i).c_str(), std::string(romfsPath + '/' + moflexFile).c_str());
				if (error) {
					std::cout << ErrorText << ' ' << FailedToCopyFile << "\n\"" << parameters.MoflexVec.at(i) << "\" -> \"" << romfsPath << '/' << moflexFile << "\"\n" << error.message() << std::endl;
					std::cout << ErrorText << ' ' << FailedToCreateFile << ' ' << romfsPath << '/' << moflexFile << std::endl;
					return 26;
				}
				if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfsPath + '/' + moflexFile).c_str()), error)) {
					std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << romfsPath << '/' << moflexFile << std::endl;
					return 27;
				}
				if(error) {
					return 28;
				}
			}
		}
	}
	
	return 0;
}

int make_Bimgs(const VI9Pparameters& parameters, const std::string& romfsPath, const std::string& outCIA, const std::string& outTAR, const bool& dopatch) {
	std::error_code error;
	
	for (int i = dopatch ? parameters.splitPos : 0; i < ((parameters.splitPos && !dopatch) ? parameters.splitPos : parameters.rows); i++) {
		std::vector<uint8_t> bimg(256 * 128 * sizeof(nnc_u16) + 0x20);
		
		std::cout << CreatingFile << " romfs/movie/movie_" << std::to_string(i) << ".bimg" << std::endl;
		
		uint8_t ret = convertToBimg(parameters.MBannerVec.at(i), bimg.data(), true);
		if (ret > 0) {
			std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << romfsPath << "/movie/movie_" << std::to_string(i) << ".bimg\n(" << std::to_string(ret) << ')' << std::endl;
			return 29;
		}
		std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/movie").c_str()), error);
		if (error) {
			std::cout << ErrorText << ' ' << romfsPath + "/movie" << '\n' << error.message() << std::endl;
			std::cout << ErrorText << ' ' << FailedToCreateFile << " \"" << (dopatch ? outTAR : outCIA) << '\"' << std::endl;
			return 30;
		}
		std::ofstream bimgfile(std::string(romfsPath + "/movie/movie_" + std::to_string(i) + ".bimg").c_str(), std::ios_base::out | std::ios_base::binary);
		bimgfile.write(reinterpret_cast<const char*>(bimg.data()), bimg.size());
		bimgfile.close();
	}
	// make movie_bnrname.csv
	std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/settings").c_str()), error);
	if (error) {
		std::cout << ErrorText << ' ' << romfsPath + "/settings" << '\n' << error.message() << std::endl;
		std::cout << ErrorText << ' ' << FailedToCreateFile << " \"" << (dopatch ? outTAR : outCIA) << '\"' << std::endl;
		return 31;
	}
	std::ofstream movie_bnrname(std::string(romfsPath + "/settings/movie_bnrname.csv").c_str(), std::ios_base::out | std::ios_base::binary);
	movie_bnrname << "\xFF\xFE" + UTF8toUTF16(std::to_string(parameters.rows) + "\x0D\x0A");
	for (int i = 0; i < parameters.rows; i++) {
		movie_bnrname << UTF8toUTF16("movie_" + std::to_string(i) + ".bimg\x0D\x0A");
	}
	movie_bnrname.close();
	if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/settings/movie_bnrname.csv").c_str()), error)) {
		std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << romfsPath << "/settings/movie_bnrname.csv" << std::endl;
		return 32;
	}
	if(error) {
		return 33;
	}
	
	return 0;
}

int make_U_Title(const VI9Pparameters& parameters, const std::string& romfsPath, const std::string& tempPath) {
	std::string U_Title_file = romfsPath + "/layout/U_Title.arc.l";
	std::error_code error;
	
	std::cout << CreatingFile << ' ' << U_Title_file << std::endl;
	
	if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(U_Title_file).c_str()), error)) {
		std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << U_Title_file << std::endl;
		return 35;
	}
	if(error) {
		std::cout << ErrorText << ' ' << romfsPath + U_Title_file << '\n' << error.message() << std::endl;
		return 36;
	}
	
	// load data
	{
		std::ifstream U_Title_stream(std::string(U_Title_file).c_str(), std::ios_base::binary | std::ios_base::ate);
		auto size = U_Title_stream.tellg();
		std::vector<uint8_t> lzFile(size);
		U_Title_stream.seekg(0);
		U_Title_stream.read(reinterpret_cast<char*>(lzFile.data()), size);
		U_Title_stream.close();
		
		// decompress .arc.l
		uint32_t decompressedsize = Get_Decompressed_size(lzFile.data());
		if (decompressedsize == 0xFFFFFFFF) {
			return 37;
		}
		std::cout << size << std::endl;
		std::cout << decompressedsize << std::endl;
		
		std::vector<uint8_t> U_Title_decomp(decompressedsize);
		
		if (DecompressLZ11(lzFile.data(), U_Title_decomp.data()) == 0xFFFFFFFF) {
			return 38;
		}
		
		std::ofstream darcfile(std::string(tempPath + "/U_Title.arc").c_str(), std::ios_base::out | std::ios_base::binary);
		darcfile.write(reinterpret_cast<const char*>(U_Title_decomp.data()), U_Title_decomp.size());
		darcfile.close();
	}
	// extract darc
	{
		int ret = extract_darc(std::string(tempPath + "/U_Title.arc").c_str(), std::string(tempPath + "/U_Title").c_str());
		if(ret) {
			std::cout << ErrorText << ' ' << FailedToCreateDirectory << ' ' << tempPath << "/U_Title\n(" << std::to_string(ret) << ')' << std::endl;
			return 39;
		}
	}
	// convert image to Title_rogo
	{
		int ret = convertToClim(parameters.MBannerVec.at(0), std::string(tempPath + "/U_Title/timg/Title_rogo.bclim").c_str());
		if(ret == 0) { // the custom image is real!! write custom blyt
			std::ofstream bclyt(std::string(tempPath + "/U_Title/blyt/U_Title.bclyt").c_str(), std::ios_base::out | std::ios_base::binary);
			bclyt.write(reinterpret_cast<const char*>(U_Title_bclyt_data), sizeof(U_Title_bclyt_data));
			bclyt.close();
		}
	}
	// build darc
	{
		// TODO: make darctool write to memory because we need to read it back anyway and it's small enough
		darctool::return_code ret = darctool::write_darc(std::string(tempPath + "/U_Title").c_str(), std::string(tempPath + "/new_U_Title.arc").c_str());
		if(ret != darctool::return_code::OK) {
			std::cout << ErrorText << ' ' << FailedToCreateFile << ' ' << tempPath << "/U_Title\n(" << darctool::return_str(ret) << ')' << std::endl;
			return 40;
		}
	}
	// compress new darc
	{
		std::ifstream infile(std::string(tempPath + "/new_U_Title.arc").c_str(), std::ios_base::binary | std::ios_base::ate);
		auto size = infile.tellg();
		std::vector<uint8_t> uncompressed(size);
		infile.seekg(0);
		infile.read(reinterpret_cast<char*>(uncompressed.data()), size);
		infile.close();
		
		uint32_t lzSize = 0;
		void* lzFile = lz11_compress(&lzSize, uncompressed.data(), uncompressed.size());

		std::ofstream outfile(U_Title_file, std::ios_base::binary); // finally...
		outfile.write(reinterpret_cast<const char*>(lzFile), lzSize);
		free(lzFile);
	}
	return 0;
}

// TODO: redo these return values when you finish the U_Title stuff
int make_Icon(const VI9Pparameters& parameters, const std::string& romfsPath, const std::string& tempPath) {
	std::error_code error;
	
	std::cout << CreatingFile << " exefs/icon" << std::endl;
	uint8_t ret = 0;
	ret = convertToIcon(parameters.icon, std::string(tempPath + "/exefs/icon"), UTF8toUTF16(parameters.Sname), UTF8toUTF16(parameters.Lname), UTF8toUTF16(parameters.publisher), parameters.iconBorder);
	if (ret > 0) {
		std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << tempPath << "/exefs/icon\n(" << std::to_string(ret) << ')' << std::endl;
		return 34;
	}
	if (parameters.mode == 1) { // multi vid needs an icon here so that it can make ext data or something (the game crashes if it isn't here)
		std::cout << CreatingFile << " romfs/icon.icn" << std::endl;
		//std::error_code error;
		error = copyfile(std::string(tempPath + "/exefs/icon").c_str(), std::string(romfsPath + "/icon.icn").c_str());
		if (error) {
			std::cout << ErrorText << ' ' << FailedToCopyFile << "\n\"" << tempPath << "/exefs/icon\" -> \"" << romfsPath << "/icon.icn\"\n" << error.message() << std::endl;
			std::cout << ErrorText << ' ' << FailedToCreateFile << ' ' << romfsPath << "/icon.icn" << std::endl;
			return 35;
		}
		if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/icon.icn").c_str()), error)) {
			std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << romfsPath << "/icon.icn" << std::endl;
			return 36;
		}
		if(error) {
			return 37;
		}
	}
	
	return 0;
}

int make_Banner(const VI9Pparameters& parameters, const std::string& tempPath) {
	std::error_code error;
	
	uint8_t ret = 0;
	std::cout << CreatingFile << " exefs/banner" << std::endl;
	uint8_t Checker[4];
	bool bannerbool = false;
	std::ifstream inbanner(std::filesystem::path((const char8_t*)&*parameters.banner.c_str()), std::ios::binary);
	if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*parameters.banner.c_str()), error)) {
		for (int i = 0; i < 4; i++) {
			inbanner >> Checker[i]; // https://stackoverflow.com/a/2974735
			if (Checker[i] == bannerMagic_bin_data[i]) {
				bannerbool = true;
			}
			else {
				bannerbool = false;
				break;
			}
		}
	}
	if(error) {
		return 38;
	}
	if (bannerbool) { // if CBMD
		//std::error_code error;
		error = copyfile(parameters.banner, std::string(tempPath + "/exefs/banner").c_str());
		if (error) {
			std::cout << ErrorText << ' ' << FailedToCopyFile << "\n\"" << parameters.banner << "\" -> \"" << tempPath << "/exefs/banner\"\n" << error.message() << std::endl;
			std::cout << ErrorText << ' ' << FailedToCreateFile << ' ' << tempPath << "/exefs/banner" << std::endl;
			return 39;
		}
	}
	else if (!bannerbool) {
		uint8_t buffer[256 * 128 * sizeof(nnc_u16)];
		ret = convertToBimg(parameters.banner, buffer, false);
		if (ret) {
			std::cout << ErrorText << ' ' << BadValue << '\n' << FailedToConvertImage << " \"" << parameters.banner << "\"\n(" << std::to_string(ret) << ')' << std::endl;
			return 40;
		}
		
		// create bcmdl
		std::vector<uint8_t> bcmdl;
		bcmdl = std::vector<uint8_t>(sizeof(bannerheader_bin_data) + sizeof(buffer) + sizeof(bannerfooter_bin_data));
		memcpy(bcmdl.data(), bannerheader_bin_data, sizeof(bannerheader_bin_data));
		memcpy(bcmdl.data() + sizeof(bannerheader_bin_data), buffer, sizeof(buffer));
		memcpy(bcmdl.data() + sizeof(bannerheader_bin_data) + sizeof(buffer), bannerfooter_bin_data, sizeof(bannerfooter_bin_data));
		
		// build banner (stolen from bannertool)
		CBMD cbmd;
		memset(&cbmd, 0, sizeof(cbmd));
		
		cbmd.cgfxSizes[0] = sizeof(bannerheader_bin_data) + sizeof(buffer) + sizeof(bannerfooter_bin_data);
		cbmd.cgfxs[0] = bcmdl.data();
		
		cbmd.cwavSize = sizeof(banner_bcwav_data);
		cbmd.cwav = (void*)banner_bcwav_data;
		
		uint32_t bnrSize = 0;
		
		void* bnr = cbmd_build_data(&bnrSize, cbmd); // calloc
		
		std::ofstream bnrfile(std::string(tempPath + "/exefs/banner").c_str(), std::ios_base::out | std::ios_base::binary);
		bnrfile.write(reinterpret_cast<const char*>(bnr), bnrSize);
		bnrfile.close();
		free(bnr);
	}
	if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(tempPath + "/exefs/banner").c_str()), error)) {
		std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << FailedToCreateFile << ' ' << tempPath << "/exefs/banner" << std::endl;
		return 41;
	}
	if(error) {
		return 42;
	}
	
	return ret;
}

int get_U_Title(VI9Pparameters* parameters, const std::string& outDir) {
	std::string U_Title_file = outDir + "/romfs/layout/U_Title.arc.l";
	std::string extract_path = outDir + "/U_Title";
	
	if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(U_Title_file).c_str()))) {
		std::cout << "U_Title.arc.l" << std::endl;
		
		std::error_code error;
		
		// load data
		{
			std::ifstream U_Title_stream(std::string(U_Title_file).c_str(), std::ios_base::binary | std::ios_base::ate);
			auto size = U_Title_stream.tellg();
			std::vector<uint8_t> lzFile(size);
			U_Title_stream.seekg(0);
			U_Title_stream.read(reinterpret_cast<char*>(lzFile.data()), size);
			U_Title_stream.close();
			
			// decompress .arc.l
			uint32_t decompressedsize = Get_Decompressed_size(lzFile.data());
			if (decompressedsize == 0xFFFFFFFF) {
				return 0;
			}
			std::cout << size << std::endl;
			std::cout << decompressedsize << std::endl;
			
			std::vector<uint8_t> U_Title_decomp(decompressedsize);
			
			if (DecompressLZ11(lzFile.data(), U_Title_decomp.data()) == 0xFFFFFFFF) {
				return 0;
			}
			
			std::ofstream darcfile(std::string(extract_path + ".arc").c_str(), std::ios_base::out | std::ios_base::binary);
			darcfile.write(reinterpret_cast<const char*>(U_Title_decomp.data()), U_Title_decomp.size());
			darcfile.close();
		}
		// extract darc
		{
			int ret = extract_darc(std::string(extract_path + ".arc").c_str(), std::string(extract_path).c_str());
			if(ret) {
				std::cout << ErrorText << ' ' << FailedToCreateDirectory << ' ' << extract_path << "\n(" << std::to_string(ret) << ')' << std::endl;
				return 0;
			}
		}
		parameters->MBannerVec.push_back(extract_path + "/timg/Title_rogo.bclim");
		parameters->mode = 2;
	}
	
	return 0;
}

int get_information_buttons(VI9Pparameters* parameters, const std::string& romfsPath) { // should this return uint8_t to save 3 bytes of memory? :>
	std::vector<std::string> trimmed;
	
	if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/settings/information_buttons.csv").c_str()))) {
		
		std::cout << "information_buttons.csv" << std::endl;
		
		int ret = UTF16fileToUTF8str(std::string(romfsPath + "/settings/information_buttons.csv"), &trimmed);
		if (ret > 0) {
			std::cout << ErrorText << " (" << std::to_string(ret) << ")\n" << FailedToReadFile << ": \"information_buttons.csv\"" << std::endl;
		}
		else {
			parameters->mode = 1;
			parameters->copycheck = (trimmed.size() > 0 && strcmp(trimmed.at(0).c_str(), "Copyright") == 0) ? 1 : 0;
		}
	}
	
	return 0;
}

int get_copyright(VI9Pparameters* parameters, const std::string& romfsPath) {
	std::string trimmed = "";
	
	if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/settings/copyright.txt").c_str()))) {
		
		std::cout << "copyright.txt" << std::endl;
		
		int ret = UTF16fileToUTF8str(std::string(romfsPath + "/settings/copyright.txt"), &trimmed);
		if (ret > 0) {
			std::cout << ErrorText << " (" << std::to_string(ret) << ")\n" << FailedToReadFile << ": \"copyright.txt\"" << std::endl;
			return ret;
		}
		
		parameters->mode = 1;
		parameters->copyrightInfo = trimmed;
	}
	
	return 0;
}

int get_settingsTL(VI9Pparameters* parameters, const std::string& romfsPath) {
	std::vector<std::string> trimmed;
	
	std::cout << "settingsTL.csv" << std::endl;
	
	int ret = UTF16fileToUTF8str(std::string(romfsPath + "/settings/settingsTL.csv"), &trimmed);
	if (ret > 0) {
		std::cout << ErrorText << " (" << std::to_string(ret) << ")\n" << FailedToReadFile << ": \"settingsTL.csv\"" << std::endl;
		return ret;
	}
	
	if (trimmed.size() > 29) {
		parameters->rows = 1;
		parameters->FFrewind = (strstr(trimmed.at(29).c_str(), "true") != NULL) ? 1 : 0;
		parameters->FadeOpt = (strstr(trimmed.at(30).c_str(), "true") != NULL) ? 1 : 0;
	}
	
	return 0;
}

int get_movie_bnrname(VI9Pparameters* parameters, const std::string& romfsPath) {
	std::vector<std::string> trimmed;
	
	if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/settings/movie_bnrname.csv").c_str()))) {
		
		std::cout << "movie_bnrname.csv" << std::endl;
		
		int ret = UTF16fileToUTF8str(std::string(romfsPath + "/settings/movie_bnrname.csv"), &trimmed);
		if (ret > 0) {
			std::cout << ErrorText << " (" << std::to_string(ret) << ")\n" << FailedToReadFile << ": \"movie_bnrname.csv\"" << std::endl;
			return ret;
		}
		
		parameters->mode = 1;
		parameters->rows = 0;
		std::vector<std::string> output;
		for (auto& LN : trimmed) {
			std::string extension = LN;
			if (extension.find_last_of(".") != std::string::npos) {
				extension.erase(extension.begin(), extension.begin() + extension.find_last_of("."));
			}
			while (!extension.empty() && extension.back() != 'g') { // remove any extra stuff like a line break (make sure last char is 'g')
				extension.pop_back();
			}
			if (strcmp(extension.c_str(), ".bimg") == 0) {
				while (!LN.empty() && LN.back() != 'g') {
					LN.pop_back();
				}
				output.push_back(std::string(romfsPath + "/movie/" + LN));
				++parameters->rows;
			}
		}
		if (output.size() > MAX_ROWS) {
			std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << std::to_string(output.size()) << ")\n" << noMoreThan27 << '.' << std::endl;
			while (output.size() > MAX_ROWS) {
				output.pop_back();
				--parameters->rows;
			}
		}
		for (size_t i = 0; i < output.size(); i++) {
			parameters->MBannerVec.push_back(output.at(i));
		}
	}
	else { // this pretty much means it's a single video
		parameters->rows = 1;
		parameters->mode = 0;
		parameters->MBannerVec.push_back("");
	}
	
	return 0;
}

int get_movie_title(VI9Pparameters* parameters, const std::string& romfsPath) {
	std::vector<std::string> trimmed;
	
	std::cout << "movie_title.csv" << std::endl;
	
	int ret = UTF16fileToUTF8str(std::string(romfsPath + "/movie/movie_title.csv"), &trimmed);
	if (ret > 0) {
		std::cout << ErrorText << " (" << std::to_string(ret) << ")\n" << FailedToReadFile << ": \"movie_title.csv\"" << std::endl;
		parameters->rows = 1;
		for (int i = 0; i < parameters->rows; i++) {
			parameters->PTitleVec.push_back("");
		}
		return ret;
	}
	
	parameters->rows = 0;
	std::vector<std::string> output;
	for (auto& LN : trimmed) {
		while (LN[0] == ',') {
			LN.erase(0, 1);
		}
		if (LN.find(',') < LN.size()) {
			LN.erase(LN.find(','), LN.size() - 1);
		}
		output.push_back(LN);
		++parameters->rows;
	}
	if (output.size() > MAX_ROWS) {
		std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << std::to_string(output.size()) << ")\n" << noMoreThan27 << '.' << std::endl;
		while (output.size() > MAX_ROWS) {
			output.pop_back();
			--parameters->rows;
		}
	}
	for (size_t i = 0; i < output.size(); i++) {
		//std::cout << "\"" << output.at(i) << "\"" << std::endl; // dude
		if (strcmp(output.at(i).c_str(), "\r") == 0) { // this will happen if it was empty so don't push the carriage return
			parameters->PTitleVec.push_back("");
		}
		else {
			parameters->PTitleVec.push_back(output.at(i));
		}
	}
	
	return 0;
}

// https://stackoverflow.com/a/61067330
template <typename TP>
std::time_t to_time_t(TP tp)
{
    std::chrono::time_point sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(tp - TP::clock::now() + std::chrono::system_clock::now());
    return std::chrono::system_clock::to_time_t(sctp);
}

int get_moflex(VI9Pparameters* parameters, const std::string& romfsPath) {
	if (parameters->mode == 2) {
		std::error_code error;
		std::filesystem::path dir = romfsPath;
		for (const auto& entry : std::filesystem::directory_iterator(dir)) {
			if (std::filesystem::is_regular_file(entry, error)) {
				std::filesystem::file_time_type fileTime = std::filesystem::last_write_time(entry.path());
				time_t time = to_time_t<std::filesystem::file_time_type>(fileTime);
				
				std::cout << time << " : " << entry.path().filename() << std::endl;
				
				uint8_t Checker[4];
				std::string extension = entry.path().string();
				if (extension.find_last_of(".") != std::string::npos) {
					extension.erase(extension.begin(), extension.begin() + extension.find_last_of("."));
				}
				std::ifstream inmoflex(std::filesystem::path((const char8_t*)&*entry.path().string().c_str()), std::ios_base::in | std::ios::binary);
				for (int j = 0; j < 4; j++) {
					inmoflex >> Checker[j]; // https://stackoverflow.com/a/2974735
					if (extension != ".moflex" || Checker[j] != moflexMagic_bin_data[j]) {
						continue;
					}
				}
				parameters->MoflexVec.push_back(entry.path().string());
				parameters->rows++;
			}
			if (error) {
				std::cout << ErrorText << ' ' << entry.path().filename() << '\n' << error.message() << std::endl;
			}
		}
	}
	else if (parameters->mode < 2) {
		if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/movie/movie.moflex").c_str()))) { // single video only has this
			parameters->MoflexVec.push_back(std::string(romfsPath + "/movie/movie.moflex"));
			if (parameters->rows > 1) {
				parameters->rows = 1;
			}
		}
		else {
			for (int i = 0; i < parameters->rows; i++) {
				parameters->MoflexVec.push_back(std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/movie/movie_" + std::to_string(i) + ".moflex").c_str())) ? std::string(romfsPath + "/movie/movie_" + std::to_string(i) + ".moflex") : "");
				std::cout << (std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfsPath + "/movie/movie_" + std::to_string(i) + ".moflex").c_str())) ? std::string(romfsPath + "/movie/movie_" + std::to_string(i) + ".moflex") : "") << std::endl;
			}
		}
	}
	
	return 0;
}
