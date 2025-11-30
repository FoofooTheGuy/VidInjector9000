#include "CTR_stuff.hpp"

int generate_preview(std::string inpath, uint8_t number, std::string outpath) {
	VI9Pparameters parameters;
	
	int res = loadParameters(inpath, &parameters);
	//uint32_t outrealint = 0;
	switch(number) {
		case 0: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << IntMultiParam << ")\n" << NothingToDo << std::endl;
			return 7;
		}
		case 1: {
			res = generateBannerPreview(parameters.banner, outpath);
			//std::cout << banner << std::endl;
			return res;
		}
		case 2: {
			res = generateIconPreview(parameters.icon, parameters.iconBorder, outpath);
			//std::cout << icon << std::endl;
			break;
		}
		case 3: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << IntIconBorderParam << ")\n" << NothingToDo << std::endl;
			//iconBorder = ((outrealint & 0xFF) > 2 ? 2 : (outrealint & 0xFF));
			return 8;
		}
		case 4: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << StrSNameParam << ")\n" << NothingToDo << std::endl;
			//Sname = newValue;
			return 9;
		}
		case 5: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << StrLNameParam << ")\n" << NothingToDo << std::endl;
			//Lname = newValue;
			return 10;
		}
		case 6: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << StrPublisherParam << ")\n" << NothingToDo << std::endl;
			//publisher = newValue;
			return 11;
		}
		case 7: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << IntCopycheckParam << ")\n" << NothingToDo << std::endl;
			//copycheck = outrealint ? 1 : 0;
			return 12;
		}
		case 8: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << StrCopyrightParam << ")\n" << NothingToDo << std::endl;
			//copyrightInfo = newValue;
			return 13;
		}
		case 9: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << IntFFrewindParam << ")\n" << NothingToDo << std::endl;
			//FFrewind = outrealint ? 1 : 0;
			return 14;
		}
		case 10: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << IntFadeOptParam << ")\n" << NothingToDo << std::endl;
			//FadeOpt = outrealint ? 1 : 0;
			return 15;
		}
		case 11: {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << IntSplitPatchParam << ")\n" << NothingToDo << std::endl;
			//splitPos = (rows > 1) ? (outrealint < rows ? outrealint : 1) : 0;
			return 16;
		}
	}
	//beware of incoming jank. tread lightly...
	uint8_t rowcase = 12;
	for((void)rowcase; rowcase < (parameters.PTitleVec.size() & 0xFF); rowcase++) {
		if(number == rowcase) {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << StrPTitleParam << ")\n" << NothingToDo << std::endl;
			return 17;
		}
	}
	rowcase = 12 + parameters.rows;
	for((void)rowcase; rowcase < (parameters.MoflexVec.size() & 0xFF); rowcase++) {
		if(number == rowcase) {
			std::cout << ErrorText << ' ' << BadValue << " (" << number << " -> " << StrMoflexParam << ")\n" << NothingToDo << std::endl;
			return 18;
		}
	}
	rowcase = 12 + (parameters.rows * 2);
	for(auto &row : parameters.MBannerVec) {
		if(number == rowcase) {
			res = generateBannerPreview(row, outpath, true);
			//std::cout << row << std::endl;
			return res;
		}
		rowcase++;
	}
	return res;
}


std::error_code Generate_Files(std::string dir, bool Multi) {
	std::error_code error;
	if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*dir.c_str()), error)) {
		if (error) {
			return error;
		}
		std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*dir.c_str()), error);
	}
	if (error) {
		return error;
	}
	miniz_cpp::zip_file file;
	file.load(Multi ? Multivid : Singlevid);
	std::vector<std::string> list = file.namelist();
	for (auto& member : list) {//plant seeds
		if (member.find_last_of("/") == member.size() - 1) {
			std::filesystem::create_directory(std::filesystem::path((const char8_t*)&*std::string(dir + "/" + member).c_str()), error);
			if (error) {
				return error;
			}
		}
	}
	file.extractall(dir, list);//grow fruit (don't you mean grow tree?)
	return error;
}

bool TIDisValid(uint32_t TID) {//https://github.com/ihaveamac/videoinject/blob/master/videoinject.py#L9
	uint32_t min = 0xC0000;
	uint32_t max = 0xEFFFF;
	switch (TID)
	{
	case 0xc0d00://Side-Scrolling Example
	case 0xce1cc://CHMM
	case 0xd921e://homebrew launcher loader
	case 0xda001://Smash Bros Dummy Application
	case 0xda002://3ds quick shutdown
	case 0xda003://Wifi Toggle
	case 0xDEAD1://MCU Bricker
	case 0xe7a5a://NASA ALL
	case 0xec100://PKSM
	case 0xEC600://Video player for 3DS
	case 0xed990://NotifyMii
	case 0xED000://Google Translate
	case 0xEE3EE://MurderLands
	case 0xeffec://FileKong
	case 0xeffed://TriaAl
	case 0:
	{
		return false;
	}
	break;
	default:
		if (TID > max || TID < min) return false;
	}
	return true;
}

uint32_t RandomTID() {
	uint32_t min = 0xC0000;
	uint32_t max = 0xEFFFF;
	uint32_t TID = 0;
	static std::mt19937 rng;

	while (!TIDisValid(TID)) {//loop until we get a good value
		rng.seed(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
		std::uniform_int_distribution<unsigned long> uniform(min, max);
		TID = uniform(rng);
	}
	return TID;
}

int setIcon(const std::string& inpath, std::string* Sname, std::string* Lname, std::string* Publisher) {
	bool smdhinput = true;
	
	while(smdhinput)
	{
		nnc_smdh smdh;
		nnc_file f;

		if (nnc_file_open(&f, inpath.c_str()) != NNC_R_OK) {
			smdhinput = false;
			return 7;
		}

		if (nnc_read_smdh(NNC_RSP(&f), &smdh) != NNC_R_OK) {
			smdhinput = false;
			NNC_RS_CALL0(f, close);
			return 8;
		}

		if (smdhinput) {
			for (int i = 0; i < NNC_SMDH_TITLES; i++) {
				*Sname = to_UTF8(smdh.titles[i].short_desc, sizeof(smdh.titles[i].short_desc) / 2);
				*Lname = to_UTF8(smdh.titles[i].long_desc, sizeof(smdh.titles[i].long_desc) / 2);
				*Publisher = to_UTF8(smdh.titles[i].publisher, sizeof(smdh.titles[i].publisher) / 2);
				if (!Sname->empty() && !Lname->empty() && !Publisher->empty()) {
					break;
				}
			}
		}
		NNC_RS_CALL0(f, close);
		return 0;
	}
	return 8;
}

int SetSMDH(std::string inpath, std::string Newicon, std::string outpath) {
	VI9Pparameters parameters;
	
	int res = loadParameters(inpath, &parameters);
	
	parameters.icon = Newicon;
	
	res = setIcon(parameters.icon, &parameters.Sname, &parameters.Lname, &parameters.publisher);
	
	saveParameters(outpath, parameters);
	
	return res;
}

int build_archive(std::string inVi9p, std::string outCIA, std::string outTAR, uint32_t uniqueID, std::string ApplicationName, std::string ProductCode) {
	//default finalization stuff
	/*uint32_t uniqueID = RandomTID();
	std::string ApplicationName = "video";
	std::string ProductCode = "VDIJ";*/
	char uniqueIDstr [6];
	sprintf(uniqueIDstr, "%05X", uniqueID);
	std::cout << UniqueIDText << ": " << uniqueIDstr << std::endl;

	VI9Pparameters parameters;
	
	int res = loadParameters(inVi9p, &parameters);
	
	bool dopatch = 0;
	std::error_code error;
	
	std::string tempPath = resourcesPath + "/CLItemp";
	std::string romfsPath;
	do {
		if (dopatch) {
			romfsPath = tempPath + "/luma/titles/000400000" + uniqueIDstr + "00/romfs";
			std::cout << romfsPath << std::endl;
		}
		else {
			romfsPath = tempPath + "/romfs";
		}
		// extract base
		{
			std::filesystem::remove_all(std::filesystem::path((const char8_t*)&*tempPath.c_str()), error);
			if (error) {
				std::cout << ErrorText << ' ' << tempPath << '\n' << error.message() << std::endl;
				std::cout << ErrorText << ' ' << FailedToCreateFile << " \"" << (dopatch ? outTAR : outCIA) << '\"' << std::endl;
				return 7;
			}
		}
		if(!dopatch) {
			Generate_Files(tempPath.c_str(), parameters.mode);
		}
		std::cout << CreatingFile << " romfs" << std::endl;
		// make movie_title.csv (player title)
		{	
			int ret = make_movie_title(parameters, romfsPath, outCIA, outTAR, dopatch);
			
			if (ret) {
				return ret;
			}
		}
		// make settingsTL.csv (menu title and stuff)
		{
			int ret = make_settingsTL(parameters, romfsPath, uniqueIDstr, outCIA, outTAR, dopatch);
			
			if (ret) {
				return ret;
			}
		}
		// make copyright stuff (multi vid only)
		if (parameters.mode == 1 && !dopatch) {
			int ret = make_information_buttons(parameters, romfsPath, outCIA);
			
			if (ret) {
				return ret;
			}
		}
		// copy moflex
		{
			int ret = make_Moflex(parameters, romfsPath, outCIA, outTAR, dopatch);
			
			if (ret) {
				return ret;
			}
		}
		// convert to bimg (multi vid only)
		if (parameters.mode == 1) {
			int ret = make_Bimgs(parameters, romfsPath, outCIA, outTAR, dopatch);
			
			if (ret) {
				return ret;
			}
		}
		// do exefs (icon and banner)
		if (!dopatch) {// dont need exefs for luma patch
			{
				int ret = make_Icon(parameters, romfsPath, tempPath);
				
				if (ret) {
					return ret;
				}
			}
			//make banner
			int ret = make_Banner(parameters, tempPath);
				
			if (ret) {
				return ret;
			}
		}
		// modify exheader
		if (!dopatch) { // don't need this in a patch either
			std::fstream exheader(std::string(tempPath + "/exheader.bin").c_str(), std::ios::in | std::ios::out | std::ios::binary);
			for (int i = 0; i < 8; i++) { // write application name only 8 bytes because that's the limit. i had to do this loop because it was being weird with .write ???
				exheader.seekp(i);
				exheader << char(ApplicationName.c_str()[i]);
			}
			exheader.seekp(0x1C9);
			exheader.write(reinterpret_cast<const char*>(&uniqueID), sizeof(uint32_t));
			exheader.seekp(0x201);
			exheader.write(reinterpret_cast<const char*>(&uniqueID), sizeof(uint32_t));
			exheader.seekp(0x601);
			exheader.write(reinterpret_cast<const char*>(&uniqueID), sizeof(uint32_t));
			exheader.close();
		}
		// CIA creation
		if (!dopatch) {
			int ret = writeCIA(romfsPath, tempPath, ProductCode, outCIA);
			
			if (ret) {
				return ret;
			}
		}
		else {//TAR creation
			writeTAR(tempPath, outTAR);
		}
		if (parameters.mode && !outTAR.empty() && parameters.splitPos && !dopatch) {
			dopatch = true;
		}
		else if (dopatch) {
			dopatch = false;
		}
	} while (dopatch);
	std::filesystem::remove_all(std::filesystem::path((const char8_t*)&*tempPath.c_str()), error);
	if (error) {
		std::cout << ErrorText << ' ' << tempPath << '\n' << error.message() << std::endl;
		//no return becuase nobody cares
	}
	return res;
}

int extract_archive(std::string inArc, std::string outDir, bool dopatch, std::string seedPath) {
	VI9Pparameters parameters;
	parameters.PTitleVec.clear();
	parameters.MoflexVec.clear();
	parameters.MBannerVec.clear();
	
	if (std::filesystem::path((const char8_t*)&*outDir.c_str()).is_relative())
		outDir = std::filesystem::absolute(std::filesystem::path((const char8_t*)&*outDir.c_str())).string();
	
	std::string tempPath = resourcesPath + "/CLItemp";
	std::string exportsPath = tempPath + "/exports";
	std::string romfspath = outDir + "/romfs";
	std::string exefspath = outDir + "/exefs";
	
	
	std::error_code error;
	/*std::filesystem::remove_all(std::filesystem::path((const char8_t*)&*outDir.c_str()), error); // probably dont delete outDir because people might have other stuff in here for some reason
	if (error) {
		std::cout << ErrorText << ' ' << outDir << '\n' << error.message() << std::endl;
		return 1;
	}*/
	std::filesystem::create_directory(std::filesystem::path((const char8_t*)&*outDir.c_str()), error);
	if (error) {
		std::cout << ErrorText << ' ' << outDir << '\n' << error.message() << std::endl;
		return 1;
	}
	// extract important files from the romfs and exefs of cia
	if(!dopatch) {
		// extract CIA
		int ret = extractCIA(inArc, outDir, seedPath);
		
		if (ret) {
			return ret;
		}
	}
	else {
		// extract TAR
		int ret = extractTAR(inArc, outDir, &parameters);
		
		if (ret) {
			return ret;
		}
	}
	// information_buttons.csv
	{
		int ret = get_information_buttons(&parameters, romfspath);
		
		if (ret) {
			return ret;
		}
	}
	bool good = true;
	// copyright.txt
	{
		int ret = get_copyright(&parameters, romfspath);
		
		if (ret) {
			good = false;
		}
	}
	// set banner and icon
	{
		parameters.banner = std::string(exefspath + "/banner.bin"); // do it like this because the vi9p is in outDir
		parameters.icon = std::string(exefspath + "/icon.bin");
		parameters.iconBorder = 0;
		
		(void)setIcon(std::string(exefspath + "/icon.bin").c_str(), &parameters.Sname, &parameters.Lname, &parameters.publisher);
	}
	// settingsTL.csv
	{
		int ret = get_settingsTL(&parameters, romfspath);
		
		if (ret) {
			good = false;
		}
	}
	// movie_bnrname.csv
	{
		int ret = get_movie_bnrname(&parameters, romfspath);
		
		if (ret) {
			good = false;
		}
	}
	// movie_title.csv
	{
		int ret = get_movie_bnrname(&parameters, romfspath);
		
		if (ret) {
			good = false;
		}
	}
	// set moflex files
	{
		if (std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfspath + "/movie/movie.moflex").c_str()))) { // single video only has this
			parameters.MoflexVec.push_back(std::string(romfspath + "/movie/movie.moflex"));
			if (parameters.rows > 1) {
				parameters.rows = 1;
			}
		}
		else {
			for (int i = 0; i < parameters.rows; i++) {
				parameters.MoflexVec.push_back(std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfspath + "/movie/movie_" + std::to_string(i) + ".moflex").c_str())) ? std::string(romfspath + "/movie/movie_" + std::to_string(i) + ".moflex") : "");
				std::cout << (std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(romfspath + "/movie/movie_" + std::to_string(i) + ".moflex").c_str())) ? std::string(romfspath + "/movie/movie_" + std::to_string(i) + ".moflex") : "") << std::endl;
			}
		}
	}
	
	if (good) {
		std::cout << ParametersLoaded << '\n' << SuccessfullyLoaded << ' ' << inArc.substr(inArc.find_last_of("/\\") + 1) << '.' << std::endl;
	}
	else {
		std::cout << ParametersLoaded << '\n' << FailedToLoad << ' ' << inArc.substr(inArc.find_last_of("/\\") + 1) << ".\n" << ValidStillLoaded << std::endl;
	}
	
	std::cout << CreatingFile << " \"" << outDir + "/parameters.vi9p\"" << std::endl;
	
	saveParameters(std::string(outDir + "/parameters.vi9p"), parameters);
	
	return 0;
}
