#include "vi9p.hpp"

int saveParameters(std::string parampath, VI9Pparameters parameters) {
	if((parameters.PTitleVec.size() & 0xFF) < parameters.rows)
		parameters.PTitleVec = std::vector<std::string>(parameters.rows, "");
	if((parameters.MoflexVec.size() & 0xFF) < parameters.rows)
		parameters.MoflexVec = std::vector<std::string>(parameters.rows, "");
	if((parameters.MBannerVec.size() & 0xFF) < parameters.rows)
		parameters.MBannerVec = std::vector<std::string>(parameters.rows, "");
	std::ofstream outparams(std::filesystem::path((const char8_t*)&*parampath.c_str()), std::ios_base::out | std::ios_base::binary);
	outparams <<
		StrVerParam << "=\"" << VI9PVER << "\"\n" <<
		IntMultiParam << "=\"" << std::to_string(parameters.mode) << "\"\n" <<
		StrBannerParam << "=\"" << parameters.banner << "\"\n" <<
		StrIconParam << "=\"" << parameters.icon << "\"\n" <<
		IntIconBorderParam << "=\"" << std::to_string(parameters.iconBorder) << "\"\n" <<
		StrSNameParam << "=\"" << parameters.Sname << "\"\n" <<
		StrLNameParam << "=\"" << parameters.Lname << "\"\n" <<
		StrPublisherParam << "=\"" << parameters.publisher << "\"\n" <<
		IntCopycheckParam << "=\"" << std::to_string(parameters.copycheck) << "\"\n" <<
		StrCopyrightParam << "=\"" << parameters.copyrightInfo << "\"\n" <<
		IntFFrewindParam << "=\"" << std::to_string(parameters.FFrewind) << "\"\n" <<
		IntFadeOptParam << "=\"" << std::to_string(parameters.FadeOpt) << "\"\n" <<
		IntRowsParam << "=\"" << std::to_string(parameters.rows) << "\"\n";
	for (uint8_t y = 0; y < parameters.rows; y++) {
		outparams <<
			StrPTitleParam << "(" << std::to_string(y) << ")=\"" << parameters.PTitleVec.at(y) << "\"\n" <<
			StrMoflexParam << "(" << std::to_string(y) << ")=\"" << parameters.MoflexVec.at(y) << "\"\n" <<
			StrMBannerParam << "(" << std::to_string(y) << ")=\"" << parameters.MBannerVec.at(y) << "\"\n";
	}
	outparams <<
		IntSplitPatchParam << "=\"" << std::to_string(parameters.splitPos) << "\"\n";
		//IntPreIndexParam << "=\"" << std::to_string(parameters.BannerPreviewIndex) << "\"\n";
	outparams.close();
	if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*parampath.c_str()))) {
		std::cout << ErrorText << ' ' << FailedToFindPath << '\n' << parampath << std::endl;
		return 1;
	}
	return 0;
}

int loadParameters(std::string parampath, VI9Pparameters* parameters) {
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
		parameters->mode = outrealint;
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << IntMultiParam << std::endl;
	}
	if (parseLines(outstr, filelines, StrBannerParam)) {
		parameters->banner = outstr;
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrBannerParam << std::endl;
	}
	if (parseLines(outstr, filelines, StrIconParam)) {
		parameters->icon = outstr;
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
		parameters->iconBorder = ((outrealint & 0xFF) > 2 ? 2 : (outrealint & 0xFF));
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << IntIconBorderParam << std::endl;
	}
	if (parseLines(outstr, filelines, StrSNameParam)) {
		parameters->Sname = outstr;
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrSNameParam << std::endl;
	}
	if (parseLines(outstr, filelines, StrLNameParam)) {
		parameters->Lname = outstr;
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrLNameParam << std::endl;
	}
	if (parseLines(outstr, filelines, StrPublisherParam)) {
		parameters->publisher = outstr;
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
		parameters->copycheck = outrealint ? 1 : 0;
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << IntCopycheckParam << std::endl;
	}
	if (parseLines(outstr, filelines, StrCopyrightParam)) {
		parameters->copyrightInfo = outstr;
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
		parameters->FFrewind = outrealint ? 1 : 0;
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
		parameters->FadeOpt = outrealint ? 1 : 0;
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
		parameters->rows = ((outrealint & 0xFF) > MAX_ROWS ? MAX_ROWS : (outrealint & 0xFF));
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << IntRowsParam << std::endl;
	}
	parameters->PTitleVec = std::vector<std::string>(parameters->rows, "");
	parameters->MoflexVec = std::vector<std::string>(parameters->rows, "");
	parameters->MBannerVec = std::vector<std::string>(parameters->rows, "");
	for (uint8_t y = 0; y < parameters->rows; y++) {
		if (parseLines(outstr, filelines, StrPTitleParam + '(' + std::to_string(y) + ')')) {
			parameters->PTitleVec.at(y) = outstr;
		}
		else {
			good = false;
			std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrPTitleParam << '(' << std::to_string(y) << ")" << std::endl;
		}
		if (parseLines(outstr, filelines, StrMoflexParam + '(' + std::to_string(y) + ')')) {
			parameters->MoflexVec.at(y) = outstr;
		}
		else {
			good = false;
			std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrMoflexParam << '(' << std::to_string(y) << ")" << std::endl;
		}
		if (parseLines(outstr, filelines, StrMBannerParam + '(' + std::to_string(y) + ')')) {
			parameters->MBannerVec.at(y) = outstr;
		}
		else {
			good = false;
			std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrMBannerParam << '(' << std::to_string(y) << ")" << std::endl;
		}
	}
	if (parseLines(outstr, filelines, IntSplitPatchParam)) {
		if (!stoul_s(outrealint, outstr)) {
			std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << outstr << ")\n" << IntSplitPatchParam << ' ' << BeANumber << std::endl;
			outrealint = 0;
			good = false;
		}
		parameters->splitPos = (parameters->splitPos < parameters->rows) ? outrealint & 0xFF : parameters->rows - 1;
	}
	else {
		good = false;
		std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << IntSplitPatchParam << '\n' << ValueNoChange << '.' << std::endl;
	}
	/*if (parseLines(outstr, filelines, IntPreIndexParam)) {
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
	}*/
	if(good == true) std::cout << ParametersLoaded << '\n' << SuccessfullyLoaded << ' ' << parampath.substr(parampath.find_last_of("/\\") + 1) << '.' << std::endl;
	else std::cout << ParametersLoaded << '\n' << FailedToLoad << ' ' << parampath.substr(parampath.find_last_of("/\\") + 1) << ".\n" << ValidStillLoaded << std::endl;
	if(!good)
		return 6;
	return 0;
}

int setParameter(std::string inpath, int number, std::string newValue, std::string outpath) {
	VI9Pparameters parameters;
	
	int res = loadParameters(inpath, &parameters);
	uint32_t outrealint = 0;
	switch(number) {
		case 0: {
			if (!stoul_s(outrealint, newValue)) {
				std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << newValue << ")\n" << IntMultiParam << ' ' << BeANumber << std::endl;
				return 7;
			}
			parameters.mode = outrealint;
			break;
		}
		case 1: {
			parameters.banner = newValue;
			break;
		}
		case 2: {
			parameters.icon = newValue;
			break;
		}
		case 3: {
			if (!stoul_s(outrealint, newValue)) {
				std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << newValue << ")\n" << IntIconBorderParam << ' ' << BeANumber << std::endl;
				return 8;
			}
			parameters.iconBorder = ((outrealint & 0xFF) > 2 ? 2 : (outrealint & 0xFF));
			break;
		}
		case 4: {
			parameters.Sname = newValue;
			break;
		}
		case 5: {
			parameters.Lname = newValue;
			break;
		}
		case 6: {
			parameters.publisher = newValue;
			break;
		}
		case 7: {
			if (!stoul_s(outrealint, newValue)) {
				std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << newValue << ")\n" << IntCopycheckParam << ' ' << BeANumber << std::endl;
				return 9;
			}
			parameters.copycheck = outrealint ? 1 : 0;
			break;
		}
		case 8: {
			parameters.copyrightInfo = newValue;
			break;
		}
		case 9: {
			if (!stoul_s(outrealint, newValue)) {
				std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << newValue << ")\n" << IntFFrewindParam << ' ' << BeANumber << std::endl;
				return 10;
			}
			parameters.FFrewind = outrealint ? 1 : 0;
			break;
		}
		case 10: {
			if (!stoul_s(outrealint, newValue)) {
				std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << newValue << ")\n" << IntFadeOptParam << ' ' << BeANumber << std::endl;
				return 11;
			}
			parameters.FadeOpt = outrealint ? 1 : 0;
			break;
		}
		case 11: {
			if (!stoul_s(outrealint, newValue)) {
				std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << newValue << ")\n" << IntSplitPatchParam << ' ' << BeANumber << std::endl;
				return 12;
			}
			parameters.splitPos = (parameters.rows > 1) ? (outrealint < parameters.rows ? outrealint : 1) : 0;
			break;
		}
	}
	//beware of incoming jank
	int rowcase = 12;
	for(auto &row : parameters.PTitleVec) {
		if(number == rowcase) {
			row = newValue;
			break;
		}
		rowcase++;
	}
	rowcase = 12 + parameters.rows;
	for(auto &row : parameters.MoflexVec) {
		if(number == rowcase) {
			row = newValue;
			break;
		}
		rowcase++;
	}
	rowcase = 12 + (parameters.rows * 2);
	for(auto &row : parameters.MBannerVec) {
		if(number == rowcase) {
			row = newValue;
			break;
		}
		rowcase++;
	}
	if(saveParameters(outpath, parameters) == 1)
		return 1;
	return res;
}

int printParameter(std::string inpath) {
	VI9Pparameters parameters;
	
	int res = loadParameters(inpath, &parameters);
	std::cout << "[ 0] " << IntMultiParam << "=\"" << std::to_string(parameters.mode) << "\"\n" <<
				 "[ 1] " << StrBannerParam << "=\"" << parameters.banner << "\"\n" <<
				 "[ 2] " << StrIconParam << "=\"" << parameters.icon << "\"\n" <<
				 "[ 3] " << IntIconBorderParam << "=\"" << std::to_string(parameters.iconBorder) << "\"\n" <<
				 "[ 4] " << StrSNameParam << "=\"" << parameters.Sname << "\"\n" <<
				 "[ 5] " << StrLNameParam << "=\"" << parameters.Lname << "\"\n" <<
				 "[ 6] " << StrPublisherParam << "=\"" << parameters.publisher << "\"\n" <<
				 "[ 7] " << IntCopycheckParam << "=\"" << std::to_string(parameters.copycheck) << "\"\n" <<
				 "[ 8] " << StrCopyrightParam << "=\"" << parameters.copyrightInfo << "\"\n" <<
				 "[ 9] " << IntFFrewindParam << "=\"" << std::to_string(parameters.FFrewind) << "\"\n" <<
				 "[10] " << IntFadeOptParam << "=\"" << std::to_string(parameters.FadeOpt) << "\"\n" <<
				 "[11] " << IntSplitPatchParam << "=\"" << std::to_string(parameters.splitPos) << "\"" << std::endl;
	//beware of incoming jank
	size_t rowcase = 12;
	uint8_t rowcount = 0;
	for(auto &row : parameters.PTitleVec) {
		std::cout << '[' << rowcase << "] " << StrPTitleParam << '(' << std::to_string(rowcount) << ')' << "=\"" << row << "\"" << std::endl;
		rowcount++;
		rowcase++;
	}
	rowcase = 12 + parameters.rows;
	rowcount = 0;
	for(auto &row : parameters.MoflexVec) {
		std::cout << '[' << rowcase << "] " << StrMoflexParam << '(' << std::to_string(rowcount) << ')' << "=\"" << row << "\"" << std::endl;
		rowcount++;
		rowcase++;
	}
	rowcase = 12 + (parameters.rows * 2);
	rowcount = 0;
	for(auto &row : parameters.MBannerVec) {
		std::cout << '[' << rowcase << "] " << StrMBannerParam << '(' << std::to_string(rowcount) << ')' << "=\"" << row << "\"" << std::endl;
		rowcount++;
		rowcase++;
	}
	return res;
}

int add_row(std::string inpath, std::string outpath) {
	VI9Pparameters parameters;
	
	int res = loadParameters(inpath, &parameters);
	
	//mode = 1;//this shouldn't really be done here
	if(parameters.rows == MAX_ROWS) {
		std::cout << ErrorText << ' ' << noMoreThan27 << std::endl;
		return 7;//7 because we used loadParameters
	}
	parameters.rows++;
	parameters.PTitleVec.push_back("");
	parameters.MoflexVec.push_back("");
	parameters.MBannerVec.push_back("");
	
	saveParameters(outpath, parameters);
	return res;
}

int sub_row(std::string inpath, std::string outpath) {
	VI9Pparameters parameters;
	
	int res = loadParameters(inpath, &parameters);
	
	if(parameters.rows == 1) {
		return 7;//7 because we used loadParameters
	}
	parameters.rows--;
	parameters.PTitleVec.pop_back();
	parameters.MoflexVec.pop_back();
	parameters.MBannerVec.pop_back();
	
	saveParameters(outpath, parameters);
	return res;
}

int return_rows(std::string inpath) {
	VI9Pparameters parameters;
	
	int res = loadParameters(inpath, &parameters);
	
	if(res) {
		std::cout << std::to_string(res*(-1)) << std::endl;
		return res*(-1);
	}
	
	std::cout << std::to_string(parameters.rows) << std::endl;
	return parameters.rows;
}
