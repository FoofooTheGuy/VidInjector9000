#include "language.hpp"
#include "widgets.hpp"

std::vector<Languages::Language> Languages::LanguageFiles;

bool loadLanguage(std::string LangPath) {
	std::string outstr = "";
	std::error_code error;
	if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*LangPath.c_str()), error)) {
		return false;
	}
	if(error)
		return false;
	std::vector<std::string> filelines = fileRead(LangPath);
	if (filelines.size() == 0)
		return false;
	std::vector<std::string*> inLangVec;
	std::vector<std::string*> LangVec;
	fillLangVecs(&inLangVec, &LangVec);
	for (size_t i = 0; i < inLangVec.size(); i++) {
		if (parseLines(outstr, filelines, *inLangVec[i])) {
			*LangVec[i] = outstr;
		}
	}
	return true;
}

void theWidgets::applyLanguage(VI9Pparameters* parameters) {
	//consoleLog->SetLabel(wxString::FromUTF8(logFrameText)); // wxLogWindow is so weird idk how to change anything about it
	
	frame->SetLabel(wxString::FromUTF8(frameText));
	mainMenu->SetMenuLabel(0, wxString::FromUTF8(file));
	menuItemFileNew->SetItemLabel(wxString::FromUTF8(fileNew + "\tCtrl+N"));
	menuItemFileOpen->SetItemLabel(wxString::FromUTF8(fileOpen + "\tCtrl+O"));
	menuItemFileSave->SetItemLabel(wxString::FromUTF8(fileSave + "\tCtrl+S"));
	menuItemFileSaveAs->SetItemLabel(wxString::FromUTF8(fileSaveAs + "\tCtrl+Shift+S"));
	menuItemFileExport->SetItemLabel(wxString::FromUTF8(fileExport + "\tCtrl+E"));
	menuItemFileExtract->SetItemLabel(wxString::FromUTF8(fileExtract + "\tCtrl+Alt+E"));
	menuItemFileImportSeed->SetItemLabel(wxString::FromUTF8(fileImportSeed));
	menuItemFileQuit->SetItemLabel(wxString::FromUTF8(fileQuit  + "\tCtrl+Q"));
	mainMenu->SetMenuLabel(1, wxString::FromUTF8(options));
	menuItemOptionsLog->SetItemLabel(wxString::FromUTF8(optionsShowLog));
	menuItemOptionsSystem->SetItemLabel(wxString::FromUTF8(optionsSystemMode));
	menuItemOptionsLight->SetItemLabel(wxString::FromUTF8(optionsLightMode));
	menuItemOptionsDark->SetItemLabel(wxString::FromUTF8(optionsDarkMode));
	menuItemOptionsDeleteTemp->SetItemLabel(wxString::FromUTF8(optionsDeleteTemp));
	menuItemOptionsUpdateCheck->SetItemLabel(wxString::FromUTF8(optionsUpdateCheck));
	mainMenu->SetMenuLabel(2, wxString::FromUTF8(language));
	mainMenu->SetMenuLabel(3, wxString::FromUTF8(help));
	menuItemHelpAbout->SetItemLabel(wxString::FromUTF8(helpAbout + "\tF1"));
	modeText->SetLabel(wxString::FromUTF8(ModeText));
	modeChoiceBox->SetString(0, wxString::FromUTF8(SingleVideo));
	modeChoiceBox->SetString(1, wxString::FromUTF8(MultiVideo));
	modeChoiceBox->SetString(2, wxString::FromUTF8(ExtendedMulti));
	bannerText->SetLabel(wxString::FromUTF8(BannerText));
	bannerBrowse->SetLabel(wxString::FromUTF8(Browse));
	//bannerError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + ImageInfoError + " (0) " + SeeLog)); // wait no we dont want 0. see below for fix
	iconText->SetLabel(wxString::FromUTF8(IconText));
	iconBrowse->SetLabel(wxString::FromUTF8(Browse));
	//iconError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + ImageInfoError + " (0) " + SeeLog));
	shortnameText->SetLabel(wxString::FromUTF8(ShortNameText));
	//shortnameError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (0/64)"));
	longnameText->SetLabel(wxString::FromUTF8(LongNameText));
	//longnameError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (0/128)"));
	publisherText->SetLabel(wxString::FromUTF8(PublisherText));
	//publisherError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (0/64)"));
	copyCheck->SetLabel(wxString::FromUTF8(CopyrightCheckText));
	bannerCustomText->SetLabel(wxString::FromUTF8(BannerCustomText));
	bannerPreviewText->SetLabel(wxString::FromUTF8(BannerPreviewText));
	ffRewindCheck->SetLabel(wxString::FromUTF8(FFrewindCheckText));
	dimCheck->SetLabel(wxString::FromUTF8(DimCheckText));
	playerTitleText->SetLabel(wxString::FromUTF8(PlayerTitleText));
	moflexFileText->SetLabel(wxString::FromUTF8(MoflexFileText));
	menuBannerText->SetLabel(wxString::FromUTF8(MenuBannerText));
	moflexBrowse->SetLabel(wxString::FromUTF8(Browse));
	multiBannerBrowse->SetLabel(wxString::FromUTF8(Browse));
	splitPatchButton->SetLabel(wxString::FromUTF8(SplitIntoAPatch));
	
	buildframe->SetLabel(wxString::FromUTF8(buildFrameText));
	titleIDText->SetLabel(wxString::FromUTF8(TitleIDText));
	applicationTitleText->SetLabel(wxString::FromUTF8(ApplicationTitleText));
	productCodeText->SetLabel(wxString::FromUTF8(ProductCodeText));
	buildButton->SetLabel(wxString::FromUTF8(Build));
	cancelButton->SetLabel(wxString::FromUTF8(Cancel));
	
	aboutframe->SetLabel(wxString::FromUTF8(aboutFrameText));
	byMeText->SetLabel(wxString::FromUTF8(ByMeText));
	gitHubLinker->SetLabel(wxString::FromUTF8(GitHubLinker));
	
	if(!parameters->banner.empty()) { // so it doesn't overwrite the actual information
		bannerBox_wxEVT_TEXT(parameters); // it actually should do this because the other thing was removing the error code
	}
	if(!parameters->icon.empty()) {
		iconBox_wxEVT_TEXT(parameters);
	}
	if(!parameters->Sname.empty()) {
		shortnameBox_wxEVT_TEXT(parameters);
	}
	if(!parameters->Lname.empty()) {
		longnameBox_wxEVT_TEXT(parameters);
	}
	if(!parameters->publisher.empty()) {
		publisherBox_wxEVT_TEXT(parameters);
	}
}

void theWidgets::initLanguage() {
	{//menuLanguage
		std::vector<int> LANG_IDs {
			ID_LANG1,
			ID_LANG2,
			ID_LANG3,
			ID_LANG4,
			ID_LANG5,
			ID_LANG6,
			ID_LANG7,
			ID_LANG8,
			ID_LANG9,
			ID_LANG10,
		};
		size_t slot = 0;
		std::error_code error;
		// https://stackoverflow.com/a/612176
		for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + languagePath).c_str()), error)) {
			if (std::filesystem::exists(entry, error)) {
				std::string entrystr = entry.path().string(); // https://stackoverflow.com/q/45401822
				std::string outstr = "";
				if (std::filesystem::is_directory(entry, error)) {
					std::vector<std::string> filelines = fileRead(entrystr + "/Language.txt");
					if (parseLines(outstr, filelines, inLangLanguage)) {
						wxMenuItem* menuItemLanguage = menuLanguage->Append(LANG_IDs.at(slot), wxString::FromUTF8(outstr), "");
						(void)menuItemLanguage;
						Languages::LanguageFiles.push_back({outstr, std::filesystem::absolute(entry.path()).string() + "/Language.txt"});
						++slot;
					}
				}
			}
			if (error) {
				break;
			}
		}
	}
}
