bool loadLanguage(xtd::ustring Lang) {
	xtd::ustring outstr = "";
	if (!std::filesystem::exists(xtd::ustring::format("{}/{}/language/{}/Language.txt", ProgramDir, resourcesPath, Lang).c_str())) {
		std::filesystem::create_directory(xtd::ustring::format("{}/{}/language/English", ProgramDir, resourcesPath).c_str());
		std::ofstream M_file(xtd::ustring::format("{}/{}/language/English/M.bmp", ProgramDir, resourcesPath).c_str(), std::ios_base::out | std::ios_base::binary);
		M_file.write(reinterpret_cast<const char*>(M_bmp), sizeof(M_bmp));
		M_file.close();
		xtd::forms::message_box::show(*this, xtd::ustring::format("{} \"{}\"", FailedToFindPath, xtd::ustring::format("{}/{}/language/{}/Language.txt", ProgramDir, resourcesPath, Lang)), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
		return false;
	}
	std::vector<xtd::ustring*> inLangVec = { &inLangLanguage, &inLangFormText, &inLangByMeText, &inLangParametersText, &inLangFinalizeText, &inLangOptionsText, &inLangModeText, &inLangSingleVideo, &inLangMultiVideo, &inLangBannerText, &inLangBrowse, &inLangSupportedImage48x48, &inLangSupportedImage200x120, &inLangSupportedImageList, &inLangCGFXList, &inLangAllFilesList, &inLangMoflexFilesList, &inLangParamFilesList, &inLangErrorText, &inLangImageInfoError, &inLangBannerPreviewText, &inLangCustomNotifText, &inLangIconText, &inLangShortNameText, &inLangLongNameText, &inLangPublisherText, &inLangTextTooLongError, &inLangMultiOnlyText, &inLangCopyrightCheckText, &inLangFFrewindText, &inLangFadeOptText, &inLangPlayerTitleText, &inLangMoflexFileText, &inLangMenuBannerText, &inLangSaveButtText, &inLangLoadButtText, &inLangAutoSaveText, &inLangAutoLoadText, &inLangLightModeText, &inLangDarkModeText, &inLangLanguageText, &inLangRestartText, &inLangConfirmClose, &inLangLosedata, &inLangConfirmSave, &inLangAlreadyExists, &inLangReplaceIt, &inLangBadValue, &inLangBadVersion, &inLangSupportedVersion, &inLangBeANumber, &inLangMissingVariableError, &inLangFailedToFindVar, &inLangFailedToFindPath, &inLangFailedToCreateFile, &inLangFailedToConvertImage, &inLangValueNoChange, &inLangnoMoreThan27, &inLangParametersLoaded, &inLangSuccessfullyLoaded, &inLangFailedToLoad, &inLangValidStillLoaded, &inLangTheFile, &inLangDoesntExist, &inLangLanguageChanged, &inLangRestartProgram, &inLangWasEnabled, &inLangTitleIDText, &inLangAppNameText, &inLangProductCodetext, &inLangBuildCIAText, &inLangCancel, &inLangMoflexError, &inLangrow, &inLangcolumn, };
	std::vector<xtd::ustring*> LangVec = { &Language, &FormText, &ByMeText, &ParametersText, &FinalizeText, &OptionsText, &ModeText, &SingleVideo, &MultiVideo, &BannerText, &Browse, &SupportedImage48x48, &SupportedImage200x120, &SupportedImageList, &CGFXList, &AllFilesList, &MoflexFilesList, &ParamFilesList, &ErrorText, &ImageInfoError, &BannerPreviewText, &CustomNotifText, &IconText, &ShortNameText, &LongNameText, &PublisherText, &TextTooLongError, &MultiOnlyText, &CopyrightCheckText, &FFrewindText, &FadeOptText, &PlayerTitleText, &MoflexFileText, &MenuBannerText, &SaveButtText, &LoadButtText, &AutoSaveText, &AutoLoadText, &LightModeText, &DarkModeText, &LanguageText, &RestartText, &ConfirmClose, &Losedata, &ConfirmSave, &AlreadyExists, &ReplaceIt, &BadValue, &BadVersion, &SupportedVersion, &BeANumber, &MissingVariableError, &FailedToFindVar, &FailedToFindPath, &FailedToCreateFile, &FailedToConvertImage, &ValueNoChange, &noMoreThan27, &ParametersLoaded, &SuccessfullyLoaded, &FailedToLoad, &ValidStillLoaded, &TheFile, &DoesntExist, &LanguageChanged, &RestartProgram, &WasEnabled, &TitleIDText, &AppNameText, &ProductCodetext, &BuildCIAText, &Cancel, &MoflexError, &row, &column, };
	for (size_t i = 0; i < inLangVec.size(); i++) {
		if (fileParse(outstr, xtd::ustring::format("{}/{}/language/{}/Language.txt", ProgramDir, resourcesPath, Lang), *inLangVec[i])) {
			*LangVec[i] = outstr;
		}
		else {
			xtd::forms::message_box::show(*this, xtd::ustring::format("{} {}\n{}.", FailedToFindVar, *inLangVec[i], ValueNoChange), xtd::ustring::format("{} {}", ErrorText, MissingVariableError), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
		}
	}
	Languagedir = Lang;
	return true;
}