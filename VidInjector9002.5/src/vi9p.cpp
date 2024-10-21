#include "vi9p.hpp"

std::string VI9P::WorkingFile;
//VI9Pparameters VI9P::Parameters;

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
			//std::cout << ErrorText << ' ' << BadValue << '\n' << BadVersion << ": (" << outstr << ")\n" << SupportedVersion << ' ' << VI9PVER << std::endl;
			return 4;
		}
	}
	else {
		good = false;
		//std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrVerParam << std::endl;
		return 5;
	}
	if (parseLines(outstr, filelines, IntMultiParam)) {
		if (!stoul_s(outrealint, outstr)) {
			//std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << outstr << ")\n" << IntMultiParam << ' ' << BeANumber << std::endl;
			outrealint = 0;
			good = false;
		}
		parameters.mode = outrealint;
	}
	else {
		good = false;
		//std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << IntMultiParam << std::endl;
	}
	if (parseLines(outstr, filelines, StrBannerParam)) {
		parameters.banner = outstr;
	}
	else {
		good = false;
		//std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrBannerParam << std::endl;
	}
	if (parseLines(outstr, filelines, StrIconParam)) {
		parameters.icon = outstr;
	}
	else {
		good = false;
		//std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrIconParam << std::endl;
	}
	if (parseLines(outstr, filelines, IntIconBorderParam)) {
		if (!stoul_s(outrealint, outstr)) {
			//std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << outstr << ")\n" << IntIconBorderParam << ' ' << BeANumber << std::endl;
			outrealint = 2;
		}
		parameters.iconBorder = ((outrealint & 0xFF) > 2 ? 2 : (outrealint & 0xFF));
	}
	else {
		good = false;
		//std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << IntIconBorderParam << std::endl;
	}
	if (parseLines(outstr, filelines, StrSNameParam)) {
		parameters.Sname = outstr;
	}
	else {
		good = false;
		//std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrSNameParam << std::endl;
	}
	if (parseLines(outstr, filelines, StrLNameParam)) {
		parameters.Lname = outstr;
	}
	else {
		good = false;
		//std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrLNameParam << std::endl;
	}
	if (parseLines(outstr, filelines, StrPublisherParam)) {
		parameters.publisher = outstr;
	}
	else {
		good = false;
		//std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrPublisherParam << std::endl;
	}
	if (parseLines(outstr, filelines, IntCopycheckParam)) {
		if (!stoul_s(outrealint, outstr)) {
			//std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << outstr << ")\n" << IntCopycheckParam << ' ' << BeANumber << std::endl;
			outrealint = 0;
			good = false;
		}
		parameters.copycheck = outrealint ? 1 : 0;
	}
	else {
		good = false;
		//std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << IntCopycheckParam << std::endl;
	}
	if (parseLines(outstr, filelines, StrCopyrightParam)) {
		parameters.copyrightInfo = outstr;
	}
	else {
		good = false;
		//std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrCopyrightParam << std::endl;
	}
	if (parseLines(outstr, filelines, IntFFrewindParam)) {
		if (!stoul_s(outrealint, outstr)) {
			//std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << outstr << ")\n" << IntFFrewindParam << ' ' << BeANumber << std::endl;
			outrealint = 1;
			good = false;
		}
		parameters.FFrewind = outrealint ? 1 : 0;
	}
	else {
		good = false;
		//std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << IntFFrewindParam << std::endl;
	}
	if (parseLines(outstr, filelines, IntFadeOptParam)) {
		if (!stoul_s(outrealint, outstr)) {
			//std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << outstr << ")\n" << IntFadeOptParam << ' ' << BeANumber << std::endl;
			outrealint = 1;
			good = false;
		}
		parameters.FadeOpt = outrealint ? 1 : 0;
	}
	else {
		good = false;
		//std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << IntFadeOptParam << std::endl;
	}
	if (parseLines(outstr, filelines, IntRowsParam)) {
		if (!stoul_s(outrealint, outstr)) {
			//std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << outstr << ")\n" << IntRowsParam << ' ' << BeANumber << std::endl;
			outrealint = 1;
			good = false;
		}
		if ((outrealint & 0xFF) > MAX_ROWS) {
			//std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << outstr << ")\n" << noMoreThan27 << '.' << std::endl;
			outrealint = MAX_ROWS;
			good = false;
		}
		parameters.rows = ((outrealint & 0xFF) > MAX_ROWS ? MAX_ROWS : (outrealint & 0xFF));
	}
	else {
		good = false;
		//std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << IntRowsParam << std::endl;
	}
	parameters.PTitleVec = std::vector<std::string>(rows, "");
	parameters.MoflexVec = std::vector<std::string>(rows, "");
	parameters.MBannerVec = std::vector<std::string>(rows, "");
	for (uint8_t y = 0; y < rows; y++) {
		if (parseLines(outstr, filelines, StrPTitleParam + '(' + std::to_string(y) + ')')) {
			parameters.PTitleVec.at(y) = outstr;
		}
		else {
			good = false;
			//std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrPTitleParam << '(' << std::to_string(y) << ")" << std::endl;
		}
		if (parseLines(outstr, filelines, StrMoflexParam + '(' + std::to_string(y) + ')')) {
			parameters.MoflexVec.at(y) = outstr;
		}
		else {
			good = false;
			//std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrMoflexParam << '(' << std::to_string(y) << ")" << std::endl;
		}
		if (parseLines(outstr, filelines, StrMBannerParam + '(' + std::to_string(y) + ')')) {
			parameters.MBannerVec.at(y) = outstr;
		}
		else {
			good = false;
			//std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << StrMBannerParam << '(' << std::to_string(y) << ")" << std::endl;
		}
	}
	if (parseLines(outstr, filelines, IntSplitPatchParam)) {
		if (!stoul_s(outrealint, outstr)) {
			//std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << outstr << ")\n" << IntSplitPatchParam << ' ' << BeANumber << std::endl;
			outrealint = 0;
			good = false;
		}
		parameters.splitPos = (splitPos < rows) ? outrealint & 0xFF : rows - 1;
	}
	else {
		good = false;
		//std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << IntSplitPatchParam << '\n' << ValueNoChange << '.' << std::endl;
	}
	if (parseLines(outstr, filelines, IntPreIndexParam)) {
		if (!stoul_s(outrealint, outstr)) {
			//std::cout << ErrorText << ' ' << BadValue << '\n' << BadValue << ": (" << outstr << ")\n" << IntPreIndexParam << ' ' << BeANumber << std::endl;
			outrealint = 0;
			good = false;
		}
		parameters.BannerPreviewIndex = ((outrealint & 0xFF) > 26 ? 26 : (outrealint & 0xFF));
	}
	else {
		good = false;
		//std::cout << ErrorText << ' ' << MissingVariableError << '\n' << FailedToFindVar << ' ' << IntPreIndexParam << '\n' << ValueNoChange << '.' << std::endl;
	}
	//if(good == true) std::cout << ParametersLoaded << '\n' << SuccessfullyLoaded << ' ' << parampath.substr(parampath.find_last_of("/\\") + 1) << '.' << std::endl;
	//else std::cout << ParametersLoaded << '\n' << FailedToLoad << ' ' << parampath.substr(parampath.find_last_of("/\\") + 1) << ".\n" << ValidStillLoaded << std::endl;
	if(!good)
		return 6;
	return 0;
}