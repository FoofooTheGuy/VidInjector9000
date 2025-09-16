#include "language.hpp"

std::vector<Languages::Language> Languages::LanguageFiles;

bool loadLanguage(std::string LangPath) {
	std::string outstr = "";
	std::error_code error;
	if (!std::filesystem::exists(std::filesystem::path((const char8_t*)&*LangPath.c_str()), error)) {
		//xtd::forms::message_box::show(*this, std::string::format("{} \"{}\"", FailedToFindPath, std::string::format("{}/{}/language/{}/Language.txt", ProgramDir, resourcesPath, Lang)), std::string::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
		return false;
	}
	if(error)
		return false;
	std::vector<std::string> filelines = fileRead(LangPath);
	if (filelines.size() == 0)
		return false;
	std::vector<std::string*> inLangVec = { &inLangLanguage, &inLangErrorText, &inLangMissingVariableError, &inLangFailedToFindVar, &inLangValueNoChange, &inLangBadValue, &inLangBadVersion, &inLangSupportedVersion, &inLangBeANumber, &inLangframeText, &inLanglogFrameText, &inLangbuildFrameText, &inLangaboutFrameText, &inLangupdateAvailable, &inLangdownloadVersion, &inLangfile, &inLangfileNew, &inLangfileOpen, &inLangfileSave, &inLangfileSaveAs, &inLangfileExport, &inLangfileExtract, &inLangfileImportSeed, &inLangoptions, &inLangoptionsShowLog, &inLangoptionsSystemMode, &inLangoptionsLightMode, &inLangoptionsDarkMode, &inLangoptionsDeleteTemp, &inLangoptionsUpdateCheck, &inLanglanguage, &inLanghelp, &inLanghelpAbout, &inLangReturn, &inLangModeText, &inLangSingleVideo, &inLangMultiVideo, &inLangBrowse, &inLangBannerText, &inLangIconText, &inLangShortNameText, &inLangLongNameText, &inLangPublisherText, &inLangCopyrightCheckText, &inLangBannerCustomText, &inLangBannerPreviewText, &inLangFFrewindCheckText, &inLangDimCheckText, &inLangTextTooLongError, &inLangImageInfoError, &inLangCopyFileError, &inLangCLIError, &inLangBuildError, &inLangExtractError, &inLangSeeLog, &inLangFileSaved, &inLangPlayerTitleText, &inLangMoflexFileText, &inLangMenuBannerText, &inLangSplitIntoAPatch, &inLangTitleIDText, &inLangApplicationTitleText, &inLangProductCodeText, &inLangBuild, &inLangCancel, &inLangSuccessfullyBuilt, &inLangSuccessfullyExtracted, &inLangextractingArchive, &inLangclickToCancel, &inLangByMeText, &inLangGitHubLinker, &inLangonlyMultiVideo, &inLangmodeChoiceBoxTip, &inLangbannerBoxTip, &inLangbannerBrowseTip, &inLangiconBoxTip, &inLangiconBrowseTip, &inLangshortnameBoxTip, &inLanglongnameBoxTip, &inLangpublisherBoxTip, &inLangcopyBoxTip, &inLangcopyCheckTip, &inLangiconPreviewTip, &inLangffRewindCheckTip, &inLangdimCheckTip, &inLangmultiBannerPreviewTip, &inLangmultiBannerPreviewLeftTip, &inLangmultiBannerPreviewRightTip, &inLangmoflexFileTip, &inLangplayerTitlesTip, &inLangmoflexFilesTip, &inLangmenuBannersTip, &inLangmultiUpTip, &inLangmultiDownTip, &inLangmoflexBrowseTip, &inLangmultiBannerBrowseTip, &inLangremoveRowTip, &inLangappendRowTip, &inLangsplitPatchTip, &inLangsplitPatchUpTip, &inLangsplitPatchDownTip, &inLangtitleIDBoxTip, &inLangtitleIDButtonTip, &inLangapplicationTitleBoxTip, &inLangproductCodeBoxTip, &inLangbuildButtonTip, &inLangcancelButtonTip, &inLangchooseDirSave, &inLangmoflexFiles, &inLangextractFiles, &inLangvi9pFiles, &inLangciaFiles, &inLangtarFiles, &inLangseeddbFiles, &inLangallFiles, };
	std::vector<std::string*> LangVec = { &Language, &ErrorText, &MissingVariableError, &FailedToFindVar, &ValueNoChange, &BadValue, &BadVersion, &SupportedVersion, &BeANumber, &frameText, &logFrameText, &buildFrameText, &aboutFrameText, &updateAvailable, &downloadVersion, &file, &fileNew, &fileOpen, &fileSave, &fileSaveAs, &fileExport, &fileExtract, &fileImportSeed, &options, &optionsShowLog, &optionsSystemMode, &optionsLightMode, &optionsDarkMode, &optionsDeleteTemp, &optionsUpdateCheck, &language, &help, &helpAbout, &Return, &ModeText, &SingleVideo, &MultiVideo, &Browse, &BannerText, &IconText, &ShortNameText, &LongNameText, &PublisherText, &CopyrightCheckText, &BannerCustomText, &BannerPreviewText, &FFrewindCheckText, &DimCheckText, &TextTooLongError, &ImageInfoError, &CopyFileError, &CLIError, &BuildError, &ExtractError, &SeeLog, &FileSaved, &PlayerTitleText, &MoflexFileText, &MenuBannerText, &SplitIntoAPatch, &TitleIDText, &ApplicationTitleText, &ProductCodeText, &Build, &Cancel, &SuccessfullyBuilt, &SuccessfullyExtracted, &extractingArchive, &clickToCancel, &ByMeText, &GitHubLinker, &onlyMultiVideo, &modeChoiceBoxTip, &bannerBoxTip, &bannerBrowseTip, &iconBoxTip, &iconBrowseTip, &shortnameBoxTip, &longnameBoxTip, &publisherBoxTip, &copyBoxTip, &copyCheckTip, &iconPreviewTip, &ffRewindCheckTip, &dimCheckTip, &multiBannerPreviewTip, &multiBannerPreviewLeftTip, &multiBannerPreviewRightTip, &moflexFileTip, &playerTitlesTip, &moflexFilesTip, &menuBannersTip, &multiUpTip, &multiDownTip, &moflexBrowseTip, &multiBannerBrowseTip, &removeRowTip, &appendRowTip, &splitPatchTip, &splitPatchUpTip, &splitPatchDownTip, &titleIDBoxTip, &titleIDButtonTip, &applicationTitleBoxTip, &productCodeBoxTip, &buildButtonTip, &cancelButtonTip, &chooseDirSave, &moflexFiles, &extractFiles, &vi9pFiles, &ciaFiles, &tarFiles, &seeddbFiles, &allFiles, };
	for (size_t i = 0; i < inLangVec.size(); i++) {
		if (parseLines(outstr, filelines, *inLangVec[i])) {
			*LangVec[i] = outstr;
		}
		else {
			//xtd::forms::message_box::show(*this, std::string::format("{} {}\n{}.", FailedToFindVar, *inLangVec[i], ValueNoChange), std::string::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
		}
	}
	return true;
}

void applyLanguage(InitWidgets* wid, VI9Pparameters* parameters) {
	//wid->consoleLog->SetLabel(wxString::FromUTF8(logFrameText));//wxLogWindow is so weird idk how to change anything about it
	
	wid->frame->SetLabel(wxString::FromUTF8(frameText));
	wid->mainMenu->SetMenuLabel(0, wxString::FromUTF8(file));
	wid->menuItemFileNew->SetItemLabel(wxString::FromUTF8(fileNew + "\tCtrl+N"));
	wid->menuItemFileOpen->SetItemLabel(wxString::FromUTF8(fileOpen + "\tCtrl+O"));
	wid->menuItemFileSave->SetItemLabel(wxString::FromUTF8(fileSave + "\tCtrl+S"));
	wid->menuItemFileSaveAs->SetItemLabel(wxString::FromUTF8(fileSaveAs + "\tCtrl+Shift+S"));
	wid->menuItemFileExport->SetItemLabel(wxString::FromUTF8(fileExport + "\tCtrl+E"));
	wid->menuItemFileExtract->SetItemLabel(wxString::FromUTF8(fileExtract + "\tCtrl+Alt+E"));
	wid->menuItemFileImportSeed->SetItemLabel(wxString::FromUTF8(fileImportSeed));
	wid->mainMenu->SetMenuLabel(1, wxString::FromUTF8(options));
	wid->menuItemOptionsLog->SetItemLabel(wxString::FromUTF8(optionsShowLog));
	wid->menuItemOptionsSystem->SetItemLabel(wxString::FromUTF8(optionsSystemMode));
	wid->menuItemOptionsLight->SetItemLabel(wxString::FromUTF8(optionsLightMode));
	wid->menuItemOptionsDark->SetItemLabel(wxString::FromUTF8(optionsDarkMode));
	wid->menuItemOptionsDeleteTemp->SetItemLabel(wxString::FromUTF8(optionsDeleteTemp));
	wid->menuItemOptionsUpdateCheck->SetItemLabel(wxString::FromUTF8(optionsUpdateCheck));
	wid->mainMenu->SetMenuLabel(2, wxString::FromUTF8(language));
	wid->mainMenu->SetMenuLabel(3, wxString::FromUTF8(help));
	wid->menuItemHelpAbout->SetItemLabel(wxString::FromUTF8(helpAbout + "\tF1"));
	wid->modeText->SetLabel(wxString::FromUTF8(ModeText));
	wid->modeChoiceBox->SetString(0, wxString::FromUTF8(SingleVideo));
	wid->modeChoiceBox->SetString(1, wxString::FromUTF8(MultiVideo));
	wid->bannerText->SetLabel(wxString::FromUTF8(BannerText));
	wid->bannerBrowse->SetLabel(wxString::FromUTF8(Browse));
	//wid->bannerError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + ImageInfoError + " (0) " + SeeLog));//wait no we dont want 0. see below for fix
	wid->iconText->SetLabel(wxString::FromUTF8(IconText));
	wid->iconBrowse->SetLabel(wxString::FromUTF8(Browse));
	//wid->iconError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + ImageInfoError + " (0) " + SeeLog));
	wid->shortnameText->SetLabel(wxString::FromUTF8(ShortNameText));
	//wid->shortnameError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (0/64)"));
	wid->longnameText->SetLabel(wxString::FromUTF8(LongNameText));
	//wid->longnameError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (0/128)"));
	wid->publisherText->SetLabel(wxString::FromUTF8(PublisherText));
	//wid->publisherError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (0/64)"));
	wid->copyCheck->SetLabel(wxString::FromUTF8(CopyrightCheckText));
	wid->bannerCustomText->SetLabel(wxString::FromUTF8(BannerCustomText));
	wid->bannerPreviewText->SetLabel(wxString::FromUTF8(BannerPreviewText));
	wid->ffRewindCheck->SetLabel(wxString::FromUTF8(FFrewindCheckText));
	wid->dimCheck->SetLabel(wxString::FromUTF8(DimCheckText));
	wid->playerTitleText->SetLabel(wxString::FromUTF8(PlayerTitleText));
	wid->moflexFileText->SetLabel(wxString::FromUTF8(MoflexFileText));
	wid->menuBannerText->SetLabel(wxString::FromUTF8(MenuBannerText));
	wid->moflexBrowse->SetLabel(wxString::FromUTF8(Browse));
	wid->multiBannerBrowse->SetLabel(wxString::FromUTF8(Browse));
	wid->splitPatchButton->SetLabel(wxString::FromUTF8(SplitIntoAPatch));
	
	wid->buildframe->SetLabel(wxString::FromUTF8(buildFrameText));
	wid->titleIDText->SetLabel(wxString::FromUTF8(TitleIDText));
	wid->applicationTitleText->SetLabel(wxString::FromUTF8(ApplicationTitleText));
	wid->productCodeText->SetLabel(wxString::FromUTF8(ProductCodeText));
	wid->buildButton->SetLabel(wxString::FromUTF8(Build));
	wid->cancelButton->SetLabel(wxString::FromUTF8(Cancel));
	
	wid->aboutframe->SetLabel(wxString::FromUTF8(aboutFrameText));
	wid->byMeText->SetLabel(wxString::FromUTF8(ByMeText));
	wid->gitHubLinker->SetLabel(wxString::FromUTF8(GitHubLinker));
	
	if(!parameters->banner.empty())//so it doesn't overwrite the actual information
		bannerBox_wxEVT_TEXT(wid, parameters);//it actually should do this because the other thing was removing the error code
	if(!parameters->icon.empty())
		iconBox_wxEVT_TEXT(wid, parameters);
	if(!parameters->Sname.empty())
		shortnameBox_wxEVT_TEXT(wid, parameters);
	if(!parameters->Lname.empty())
		longnameBox_wxEVT_TEXT(wid, parameters);
	if(!parameters->publisher.empty())
	publisherBox_wxEVT_TEXT(wid, parameters);
}

void initLanguage(InitWidgets* wid) {
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
		//https://stackoverflow.com/a/612176
		for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + languagePath).c_str()), error)) {
			if (std::filesystem::exists(entry, error)) {
				std::string entrystr = entry.path().string();//https://stackoverflow.com/q/45401822
				std::string outstr = "";
				if (std::filesystem::is_directory(entry, error)) {
					std::vector<std::string> filelines = fileRead(entrystr + "/Language.txt");
					if (parseLines(outstr, filelines, inLangLanguage)) {
						wxMenuItem* menuItemLanguage = wid->menuLanguage->Append(LANG_IDs.at(slot), wxString::FromUTF8(outstr), "");
						(void)menuItemLanguage;
						Languages::LanguageFiles.push_back({outstr, std::filesystem::absolute(entry.path()).string() + "/Language.txt"});
						++slot;
					}
					//else xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, inLangLanguage, ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
				}
			}
			//else xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}", FailedToFindPath, entry), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
			if (error) {
				//xtd::forms::message_box::show(*this, xtd::ustring::format("{}/{}/language\n{}", ProgramDir, resourcesPath, error.message()), xtd::ustring::format("{}", ErrorText), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
				break;
			}
		}
	}
}
