#include "vi9p.hpp"

int saveParameters(std::string parampath, int mode, std::string banner, std::string icon, int iconBorder, std::string Sname, std::string Lname, std::string publisher, int copycheck, std::string copyrightInfo, int FFrewind, int FadeOpt, uint8_t rows, std::vector<std::string> PTitleVec, std::vector<std::string> MoflexVec, std::vector<std::string> MBannerVec) {
	if((PTitleVec.size() & 0xFF) < rows)
		PTitleVec = std::vector<std::string>(rows, "");
	if((MoflexVec.size() & 0xFF) < rows)
		MoflexVec = std::vector<std::string>(rows, "");
	if((MBannerVec.size() & 0xFF) < rows)
		MBannerVec = std::vector<std::string>(rows, "");
	std::ofstream outparams(std::filesystem::path((const char8_t*)&*parampath.c_str()), std::ios_base::out | std::ios_base::binary);
	outparams <<
		StrVerParam << "=\"" << VI9PVER << "\"\n" <<
		IntMultiParam << "=\"" << std::to_string(mode) << "\"\n" <<
		StrBannerParam << "=\"" << banner << "\"\n" <<
		StrIconParam << "=\"" << icon << "\"\n" <<
		IntIconBorderParam << "=\"" << std::to_string(iconBorder) << "\"\n" <<
		StrSNameParam << "=\"" << Sname << "\"\n" <<
		StrLNameParam << "=\"" << Lname << "\"\n" <<
		StrPublisherParam << "=\"" << publisher << "\"\n" <<
		IntCopycheckParam << "=\"" << std::to_string(copycheck) << "\"\n" <<
		StrCopyrightParam << "=\"" << copyrightInfo << "\"\n" <<
		IntFFrewindParam << "=\"" << std::to_string(FFrewind) << "\"\n" <<
		IntFadeOptParam << "=\"" << std::to_string(FadeOpt) << "\"\n" <<
		IntRowsParam << "=\"" << std::to_string(rows) << "\"\n";
	for (uint8_t y = 0; y < rows; y++) {
		outparams <<
			StrPTitleParam << "(" << std::to_string(y) << ")=\"" << PTitleVec.at(y) << "\"\n" <<
			StrMoflexParam << "(" << std::to_string(y) << ")=\"" << MoflexVec.at(y) << "\"\n" <<
			StrMBannerParam << "(" << std::to_string(y) << ")=\"" << MBannerVec.at(y) << "\"\n";
	}
	outparams <<
		IntPreIndexParam << "=\"0\"\n";
	outparams.close();
	if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*parampath.c_str()))) {
		std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << parampath << std::endl;
		return 1;
	}
	return 0;
}

int loadParameters(std::string parampath, int &mode, std::string &banner, std::string &icon, int &iconBorder, std::string &Sname, std::string &Lname, std::string &publisher, int &copycheck, std::string &copyrightInfo, int &FFrewind, int &FadeOpt, uint8_t &rows, std::vector<std::string> &PTitleVec, std::vector<std::string> &MoflexVec, std::vector<std::string> &MBannerVec, int &BannerPreviewIndex) {
	std::string outstr = "";
	uint32_t outrealint = 0;
	bool good = true;
	if (parampath == "") {
		return 2;
	}
	std::vector<std::string> filelines = fileRead(parampath);
	if (filelines.empty())
		return 3;
	if (parseLines(outstr, filelines, StrVerParam)) {
		if (outstr != VI9PVER) {
			std::cout << ErrorText << ' ' << BadValue << '\n' << BadVersion << ": (" << outstr << ")\n" << SupportedVersion << ' ' << VI9PVER << std::endl;
			return 4;
		}
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrVerParam << std::endl;
		return 5;
	}
	if (parseLines(outstr, filelines, IntMultiParam)) {
		if (!stoul_s(outrealint, outstr)) {
			std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << outstr << ")\n" << IntMultiParam << ' ' << BeANumber << std::endl;
			outrealint = 0;
			good = false;
		}
		mode = outrealint;
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << IntMultiParam << std::endl;
	}
	if (parseLines(outstr, filelines, StrBannerParam)) {
		banner = outstr;
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrBannerParam << std::endl;
	}
	if (parseLines(outstr, filelines, StrIconParam)) {
		icon = outstr;
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrIconParam << std::endl;
	}
	if (parseLines(outstr, filelines, IntIconBorderParam)) {
		if (!stoul_s(outrealint, outstr)) {
			std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << outstr << ")\n" << IntIconBorderParam << ' ' << BeANumber << std::endl;
			outrealint = 2;
		}
		iconBorder = ((outrealint & 0xFF) > 2 ? 2 : (outrealint & 0xFF));
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << IntIconBorderParam << std::endl;
	}
	if (parseLines(outstr, filelines, StrSNameParam)) {
		Sname = outstr;
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrSNameParam << std::endl;
	}
	if (parseLines(outstr, filelines, StrLNameParam)) {
		Lname = outstr;
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrLNameParam << std::endl;
	}
	if (parseLines(outstr, filelines, StrPublisherParam)) {
		publisher = outstr;
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrPublisherParam << std::endl;
	}
	if (parseLines(outstr, filelines, IntCopycheckParam)) {
		if (!stoul_s(outrealint, outstr)) {
			std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << outstr << ")\n" << IntCopycheckParam << ' ' << BeANumber << std::endl;
			outrealint = 0;
			good = false;
		}
		copycheck = outrealint ? 1 : 0;
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << IntCopycheckParam << std::endl;
	}
	if (parseLines(outstr, filelines, StrCopyrightParam)) {
		copyrightInfo = outstr;
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrCopyrightParam << std::endl;
	}
	if (parseLines(outstr, filelines, IntFFrewindParam)) {
		if (!stoul_s(outrealint, outstr)) {
			std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << outstr << ")\n" << IntFFrewindParam << ' ' << BeANumber << std::endl;
			outrealint = 1;
			good = false;
		}
		FFrewind = outrealint ? 1 : 0;
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << IntFFrewindParam << std::endl;
	}
	if (parseLines(outstr, filelines, IntFadeOptParam)) {
		if (!stoul_s(outrealint, outstr)) {
			std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << outstr << ")\n" << IntFadeOptParam << ' ' << BeANumber << std::endl;
			outrealint = 1;
			good = false;
		}
		FadeOpt = outrealint ? 1 : 0;
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << IntFadeOptParam << std::endl;
	}
	if (parseLines(outstr, filelines, IntRowsParam)) {
		if (!stoul_s(outrealint, outstr)) {
			std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << outstr << ")\n" << IntRowsParam << ' ' << BeANumber << std::endl;
			outrealint = 1;
			good = false;
		}
		if ((outrealint & 0xFF) > MAX_ROWS) {
			std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << outstr << ")\n" << noMoreThan27 << '.' << std::endl;
			outrealint = MAX_ROWS;
			good = false;
		}
		rows = ((outrealint & 0xFF) > MAX_ROWS ? MAX_ROWS : (outrealint & 0xFF));
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << IntRowsParam << std::endl;
	}
	PTitleVec = std::vector<std::string>(rows, "");
	MoflexVec = std::vector<std::string>(rows, "");
	MBannerVec = std::vector<std::string>(rows, "");
	for (uint8_t y = 0; y < rows; y++) {
		if (parseLines(outstr, filelines, StrPTitleParam + '(' + std::to_string(y) + ')')) {
			PTitleVec.at(y) = outstr;
		}
		else {
			good = false;
			std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrPTitleParam << '(' << std::to_string(y) << ")" << std::endl;
		}
		if (parseLines(outstr, filelines, StrMoflexParam + '(' + std::to_string(y) + ')')) {
			MoflexVec.at(y) = outstr;
		}
		else {
			good = false;
			std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrMoflexParam << '(' << std::to_string(y) << ")" << std::endl;
		}
		if (mode) {
			if (parseLines(outstr, filelines, StrMBannerParam + '(' + std::to_string(y) + ')')) {
				MBannerVec.at(y) = outstr;
			}
			else {
				good = false;
				std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrMBannerParam << '(' << std::to_string(y) << ")" << std::endl;
			}
		}
	}
	if (parseLines(outstr, filelines, IntPreIndexParam)) {
		if (!stoul_s(outrealint, outstr)) {
			std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << outstr << ")\n" << IntPreIndexParam << ' ' << BeANumber << std::endl;
			outrealint = 0;
			good = false;
		}
		BannerPreviewIndex = ((outrealint & 0xFF) > 26 ? 26 : (outrealint & 0xFF));
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << IntPreIndexParam << '\n' << ValueNoChange << '.' << std::endl;
	}
	if(good == true) std::cout << ParametersLoaded << '\n' << SuccessfullyLoaded << ' ' << parampath.substr(parampath.find_last_of("/\\") + 1) << '.' << std::endl;
	else std::cout << ParametersLoaded << '\n' << FailedToLoad << ' ' << parampath.substr(parampath.find_last_of("/\\") + 1) << ".\n" << ValidStillLoaded << std::endl;
	return !good;//because 0 is good for us
}

int setParameter(std::string inpath, int number, std::string newValue, std::string outpath) {
	int mode = 0;
	std::string banner = "";
	std::string icon = "";
	int iconBorder = 2;
	std::string Sname = "";
	std::string Lname = "";
	std::string publisher = "";
	int copycheck = 0;
	std::string copyrightInfo = "";
	int FFrewind = 1;
	int FadeOpt = 1;
	uint8_t rows = 1;
	std::vector<std::string> PTitleVec = std::vector<std::string>(1, "");
	std::vector<std::string> MoflexVec = std::vector<std::string>(1, "");
	std::vector<std::string> MBannerVec = std::vector<std::string>(1, "");
	int BannerPreviewIndex = 0;
	
	int res = loadParameters(inpath, mode, banner, icon, iconBorder, Sname, Lname, publisher, copycheck, copyrightInfo, FFrewind, FadeOpt, rows, PTitleVec, MoflexVec, MBannerVec, BannerPreviewIndex);
	uint32_t outrealint = 0;
	switch(number) {
		case 0: {
			if (!stoul_s(outrealint, newValue)) {
				std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << newValue << ")\n" << IntMultiParam << ' ' << BeANumber << std::endl;
				return 4;
			}
			mode = outrealint;
			break;
		}
		case 1: {
			banner = newValue;
			break;
		}
		case 2: {
			icon = newValue;
			break;
		}
		case 3: {
			if (!stoul_s(outrealint, newValue)) {
				std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << newValue << ")\n" << IntIconBorderParam << ' ' << BeANumber << std::endl;
				return 5;
			}
			iconBorder = ((outrealint & 0xFF) > 2 ? 2 : (outrealint & 0xFF));
			break;
		}
		case 4: {
			Sname = newValue;
			break;
		}
		case 5: {
			Lname = newValue;
			break;
		}
		case 6: {
			publisher = newValue;
			break;
		}
		case 7: {
			if (!stoul_s(outrealint, newValue)) {
				std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << newValue << ")\n" << IntCopycheckParam << ' ' << BeANumber << std::endl;
				return 6;
			}
			copycheck = outrealint ? 1 : 0;
			break;
		}
		case 8: {
			copyrightInfo = newValue;
			break;
		}
		case 9: {
			if (!stoul_s(outrealint, newValue)) {
				std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << newValue << ")\n" << IntCopycheckParam << ' ' << BeANumber << std::endl;
				return 7;
			}
			FFrewind = outrealint ? 1 : 0;
			break;
		}
		case 10: {
			if (!stoul_s(outrealint, newValue)) {
				std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << newValue << ")\n" << IntCopycheckParam << ' ' << BeANumber << std::endl;
				return 8;
			}
			FadeOpt = outrealint ? 1 : 0;
			break;
		}
	}
	//beware of incoming jank
	int rowcase = 11;
	for(auto &row : PTitleVec) {
		if(number == rowcase) {
			row = newValue;
			break;
		}
		rowcase++;
	}
	rowcase = 11 + rows;
	for(auto &row : MoflexVec) {
		if(number == rowcase) {
			row = newValue;
			break;
		}
		rowcase++;
	}
	rowcase = 11 + (rows * 2);
	for(auto &row : MBannerVec) {
		if(number == rowcase) {
			row = newValue;
			break;
		}
		rowcase++;
	}
	saveParameters(outpath, mode, banner, icon, iconBorder, Sname, Lname, publisher, copycheck, copyrightInfo, FFrewind, FadeOpt, rows, PTitleVec, MoflexVec, MBannerVec);
	return res;
}

int printParameter(std::string inpath) {
	int mode = 0;
	std::string banner = "";
	std::string icon = "";
	int iconBorder = 2;
	std::string Sname = "";
	std::string Lname = "";
	std::string publisher = "";
	int copycheck = 0;
	std::string copyrightInfo = "";
	int FFrewind = 1;
	int FadeOpt = 1;
	uint8_t rows = 1;
	std::vector<std::string> PTitleVec = std::vector<std::string>(1, "");
	std::vector<std::string> MoflexVec = std::vector<std::string>(1, "");
	std::vector<std::string> MBannerVec = std::vector<std::string>(1, "");
	int BannerPreviewIndex = 0;
	
	int res = loadParameters(inpath, mode, banner, icon, iconBorder, Sname, Lname, publisher, copycheck, copyrightInfo, FFrewind, FadeOpt, rows, PTitleVec, MoflexVec, MBannerVec, BannerPreviewIndex);
	std::cout << "[ 0] " << IntMultiParam << "=\"" << std::to_string(mode) << "\"\n" <<
				 "[ 1] " << StrBannerParam << "=\"" << banner << "\"\n" <<
				 "[ 2] " << StrIconParam << "=\"" << icon << "\"\n" <<
				 "[ 3] " << IntIconBorderParam << "=\"" << std::to_string(iconBorder) << "\"\n" <<
				 "[ 4] " << StrSNameParam << "=\"" << Sname << "\"\n" <<
				 "[ 5] " << StrLNameParam << "=\"" << Lname << "\"\n" <<
				 "[ 6] " << StrPublisherParam << "=\"" << publisher << "\"\n" <<
				 "[ 7] " << IntCopycheckParam << "=\"" << std::to_string(copycheck) << "\"\n" <<
				 "[ 8] " << StrCopyrightParam << "=\"" << copyrightInfo << "\"\n" <<
				 "[ 9] " << IntFFrewindParam << "=\"" << std::to_string(FFrewind) << "\"\n" <<
				 "[10] " << IntFadeOptParam << "=\"" << std::to_string(FadeOpt) << "\"" << std::endl;
	//beware of incoming jank
	size_t rowcase = 11;
	uint8_t rowcount = 0;
	for(auto &row : PTitleVec) {
		std::cout << '[' << rowcase << "] " << StrPTitleParam << '(' << std::to_string(rowcount) << ')' << "=\"" << row << "\"" << std::endl;
		rowcount++;
		rowcase++;
	}
	rowcase = 11 + rows;
	rowcount = 0;
	for(auto &row : MoflexVec) {
		std::cout << '[' << rowcase << "] " << StrMoflexParam << '(' << std::to_string(rowcount) << ')' << "=\"" << row << "\"" << std::endl;
		rowcount++;
		rowcase++;
	}
	rowcase = 11 + (rows * 2);
	rowcount = 0;
	for(auto &row : MBannerVec) {
		std::cout << '[' << rowcase << "] " << StrMBannerParam << '(' << std::to_string(rowcount) << ')' << "=\"" << row << "\"" << std::endl;
		rowcount++;
		rowcase++;
	}
	return res;
}

int add_row(std::string inpath, std::string outpath) {
	int mode = 0;
	std::string banner = "";
	std::string icon = "";
	int iconBorder = 2;
	std::string Sname = "";
	std::string Lname = "";
	std::string publisher = "";
	int copycheck = 0;
	std::string copyrightInfo = "";
	int FFrewind = 1;
	int FadeOpt = 1;
	uint8_t rows = 1;
	std::vector<std::string> PTitleVec = std::vector<std::string>(1, "");
	std::vector<std::string> MoflexVec = std::vector<std::string>(1, "");
	std::vector<std::string> MBannerVec = std::vector<std::string>(1, "");
	int BannerPreviewIndex = 0;
	
	int res = loadParameters(inpath, mode, banner, icon, iconBorder, Sname, Lname, publisher, copycheck, copyrightInfo, FFrewind, FadeOpt, rows, PTitleVec, MoflexVec, MBannerVec, BannerPreviewIndex);
	
	//mode = 1;//this shouldn't really be done here
	if(rows == MAX_ROWS) {
		std::cout << ErrorText << ' ' << noMoreThan27 << std::endl;
		return 6;//6 because we used loadParameters
	}
	rows++;
	PTitleVec.push_back("");
	MoflexVec.push_back("");
	MBannerVec.push_back("");
	
	saveParameters(outpath, mode, banner, icon, iconBorder, Sname, Lname, publisher, copycheck, copyrightInfo, FFrewind, FadeOpt, rows, PTitleVec, MoflexVec, MBannerVec);
	return res;
}

int sub_row(std::string inpath, std::string outpath) {
	int mode = 0;
	std::string banner = "";
	std::string icon = "";
	int iconBorder = 2;
	std::string Sname = "";
	std::string Lname = "";
	std::string publisher = "";
	int copycheck = 0;
	std::string copyrightInfo = "";
	int FFrewind = 1;
	int FadeOpt = 1;
	uint8_t rows = 1;
	std::vector<std::string> PTitleVec = std::vector<std::string>(1, "");
	std::vector<std::string> MoflexVec = std::vector<std::string>(1, "");
	std::vector<std::string> MBannerVec = std::vector<std::string>(1, "");
	int BannerPreviewIndex = 0;
	
	int res = loadParameters(inpath, mode, banner, icon, iconBorder, Sname, Lname, publisher, copycheck, copyrightInfo, FFrewind, FadeOpt, rows, PTitleVec, MoflexVec, MBannerVec, BannerPreviewIndex);
	
	if(rows == 1) {
		return 6;//6 because we used loadParameters
	}
	rows--;
	PTitleVec.pop_back();
	MoflexVec.pop_back();
	MBannerVec.pop_back();
	
	saveParameters(outpath, mode, banner, icon, iconBorder, Sname, Lname, publisher, copycheck, copyrightInfo, FFrewind, FadeOpt, rows, PTitleVec, MoflexVec, MBannerVec);
	return res;
}