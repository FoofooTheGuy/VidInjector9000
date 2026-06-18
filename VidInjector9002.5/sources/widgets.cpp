#include "widgets.hpp"

std::string Exports::CIA = "";
std::string Exports::TAR = "";
std::string Extracted::Archive = "";
int Borders::width = 0;
int Borders::height = 0;

int Execution::flags = wxEXEC_SYNC | wxEXEC_NODISABLE;

theWidgets::theWidgets() {
	frame = new wxFrame(nullptr, wxID_ANY, wxString::FromUTF8(frameText), wxDefaultPosition, {Settings::FrameWidth, Settings::FrameHeight});
	frame->SetIcon(wxString::FromUTF8(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + imagePath + "/icon.png"));
		
	panel = new wxPanel(frame);
	getBorders();
	
	mainMenu = new wxMenuBar();
	
	menuFile = new wxMenu();
	menuItemFileNew = menuFile->Append(wxID_NEW, wxString::FromUTF8(fileNew + "\tCtrl+N"));
	menuItemFileOpen = menuFile->Append(wxID_OPEN, wxString::FromUTF8(fileOpen + "\tCtrl+O"));
	menuItemFileSave = menuFile->Append(wxID_SAVE, wxString::FromUTF8(fileSave + "\tCtrl+S"));
	menuItemFileSaveAs = menuFile->Append(wxID_SAVEAS, wxString::FromUTF8(fileSaveAs + "\tCtrl+Shift+S"));
	menuItemFileExport = menuFile->Append(ID_EXPORT, wxString::FromUTF8(fileExport + "\tCtrl+E"));
	menuItemFileExtract = menuFile->Append(ID_EXTRACT, wxString::FromUTF8(fileExtract + "\tCtrl+Alt+E"));
	menuItemFileImportSeed = menuFile->Append(ID_IMPORTSEED, wxString::FromUTF8(fileImportSeed));
	menuItemFileQuit = menuFile->Append(ID_QUIT, wxString::FromUTF8(fileQuit));
	
	menuItemFileNew->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW, wxART_MENU));
	menuItemFileOpen->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_MENU));
	menuItemFileSave->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_MENU));
	menuItemFileSaveAs->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS, wxART_MENU));
	menuItemFileExport->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_MENU));
	menuItemFileExtract->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_MENU));
	
	menuOptions = new wxMenu();
	menuItemOptionsSystem = menuOptions->Append(ID_SYSTEM, wxString::FromUTF8(optionsSystemMode), "", wxITEM_RADIO);
	menuItemOptionsLight = menuOptions->Append(ID_LIGHT, wxString::FromUTF8(optionsLightMode), "", wxITEM_RADIO);
	menuItemOptionsDark = menuOptions->Append(ID_DARK, wxString::FromUTF8(optionsDarkMode), "", wxITEM_RADIO);
	menuItemOptionsLog = menuOptions->Append(ID_LOGBOOL, wxString::FromUTF8(optionsShowLog), "", wxITEM_CHECK);
	menuItemOptionsDeleteTemp = menuOptions->Append(ID_DELETETEMP, wxString::FromUTF8(optionsDeleteTemp), "", wxITEM_CHECK);
	menuItemOptionsUpdateCheck = menuOptions->Append(ID_UPDATECHECK, wxString::FromUTF8(optionsUpdateCheck), "", wxITEM_CHECK);
	
	menuLanguage = new wxMenu();
	
	menuHelp = new wxMenu();
	menuItemHelpAbout = menuHelp->Append(ID_ABOUT, wxString::FromUTF8(helpAbout + "\tF1"));
	
	menuItemHelpAbout->SetBitmap(wxArtProvider::GetBitmap(wxART_HELP_BOOK, wxART_MENU));
	
	mainMenu->Append(menuFile, wxString::FromUTF8(file));
	mainMenu->Append(menuOptions, wxString::FromUTF8(options));
	mainMenu->Append(menuLanguage, wxString::FromUTF8(language));
	mainMenu->Append(menuHelp, wxString::FromUTF8(help));
	
	frame->SetMenuBar(mainMenu);
	
	consoleLog = new wxLogWindow(frame, wxString::FromUTF8(logFrameText), false);
	
	modeText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(ModeText), {10, 10});
	modeChoiceBox = new wxChoice(panel, wxID_ANY);
	for (const auto &s : {wxString::FromUTF8(SingleVideo), wxString::FromUTF8(MultiVideo), wxString::FromUTF8(ExtendedMulti)}) { // https://github.com/gammasoft71/Examples_wxWidgets/blob/adbd395081bf25c9034f2b64eee62608a943441f/src/CommonControls/Choice/Choice.cpp#L10
        modeChoiceBox->Append(s);
	}
	
	bannerText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(BannerText));
	bannerBox = new wxPathCtrl(panel, wxID_ANY, wxEmptyString);
	bannerBrowse = new wxButton(panel, wxID_ANY, wxString::FromUTF8(Browse));
	bannerError = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(ErrorText + ' ' + ImageInfoError + " (0) " + SeeLog));
	
	iconText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(IconText));
	iconBox = new wxPathCtrl(panel, wxID_ANY, wxEmptyString);
	iconBrowse = new wxButton(panel, wxID_ANY, wxString::FromUTF8(Browse));
	iconError = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(ErrorText + ' ' + ImageInfoError + " (0) " + SeeLog));
	
	shortnameText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(ShortNameText));
	shortnameBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	shortnameError = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (0/64)"));
	
	longnameText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(LongNameText));
	longnameBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	longnameError = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (0/128)"));
	
	publisherText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(PublisherText));
	publisherBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	publisherError = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (0/64)"));
	
	copyBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, {300, 200}, wxTE_MULTILINE | wxTE_CENTRE);
	copyCheck = new wxCheckBox(panel, wxID_ANY, wxString::FromUTF8(CopyrightCheckText));
	
	bannerPreview = new wxStaticBitmap(panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, {264, 154}, wxBORDER_NONE);
	bannerCustomText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(BannerCustomText));
	bannerPreviewText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(BannerPreviewText));
	
	iconPreview = new wxBitmapButton(panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, {48, 48});
	
	ffRewindCheck = new wxCheckBox(panel, wxID_ANY, wxString::FromUTF8(FFrewindCheckText));
	dimCheck = new wxCheckBox(panel, wxID_ANY, wxString::FromUTF8(DimCheckText));
	
	multiBannerPreview = new wxBitmapButton(panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, {400, 240});
	
	multiBannerPreviewIndex = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8("/"));
	multiBannerPreviewLeft = new wxButton(panel, wxID_ANY, wxString::FromUTF8("←"));
	multiBannerPreviewRight = new wxButton(panel, wxID_ANY, wxString::FromUTF8("→"));
	
	playerTitleText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(PlayerTitleText));
	moflexFileText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(MoflexFileText));
	menuBannerText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(MenuBannerText));
	
	mediaPanel = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxBORDER_THEME);
	scrolledPanel = new wxScrolled<wxPanel>(mediaPanel);
	scrolledPanel->SetScrollRate(10, 10);
	
	//std::vector<wxTextCtrl*> PlayerTitles;
	//std::vector<wxPathCtrl*> MoflexFiles;
	//std::vector<wxPathCtrl*> MenuBanners;
	
	//std::vector<wxButton*> MultiUp;
	//std::vector<wxButton*> MultiDown;
	
	{ // row stuff
		doAddRows(1);
		ShowMultiUpDown();
	}
	
	moflexBrowse = new wxButton(scrolledPanel, wxID_ANY, wxString::FromUTF8(Browse));
	multiBannerBrowse = new wxButton(scrolledPanel, wxID_ANY, wxString::FromUTF8(Browse));
	
	removeRow = new wxButton(scrolledPanel, wxID_ANY, wxString::FromUTF8("-"));
	appendRow = new wxButton(scrolledPanel, wxID_ANY, wxString::FromUTF8("+"));
	
	splitPatchButton = new wxToggleButton(scrolledPanel, wxID_ANY, wxString::FromUTF8(SplitIntoAPatch));
	splitPatchLine = new wxColouredLine(scrolledPanel, {0, 0, 0});
	
	splitPatchUp = new wxButton(scrolledPanel, wxID_ANY, wxString::FromUTF8("↑"), wxDefaultPosition, wxDefaultSize);
	splitPatchDown = new wxButton(scrolledPanel, wxID_ANY, wxString::FromUTF8("↓"), wxDefaultPosition, wxDefaultSize);
	{ // splitPatchUp
		int width, height;
		wxFont f = splitPatchUp->GetFont();
		
		splitPatchUp->GetTextExtent(splitPatchUp->GetLabel(), &width, &height, nullptr, nullptr, &f);
		splitPatchUp->SetSize(width + Borders::width + 5, height + Borders::height);
	}
	{ // splitPatchDown
		int width, height;
		wxFont f = splitPatchDown->GetFont();
		
		splitPatchDown->GetTextExtent(splitPatchDown->GetLabel(), &width, &height, nullptr, nullptr, &f);
		splitPatchDown->SetSize(width + Borders::width + 5, height + Borders::height);
	}
	
	rowText = new wxStaticText(scrolledPanel, wxID_ANY, wxString::FromUTF8("1/" + std::to_string(MAX_ROWS)));
	
	buildframe = new wxFrame(frame, wxID_ANY, wxString::FromUTF8(buildFrameText), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxCLOSE_BOX|wxRESIZE_BORDER|wxFRAME_FLOAT_ON_PARENT|wxCLIP_CHILDREN);
	buildpanel = new wxPanel(buildframe);
	
	titleIDText = new wxStaticText(buildpanel, wxID_ANY, wxString::FromUTF8(TitleIDText));
	titleIDBox = new wxTextCtrl(buildpanel, wxID_ANY, wxEmptyString);
	zerozero = new wxStaticText(buildpanel, wxID_ANY, wxString::FromUTF8("00"));
	titleIDButton = new wxButton(buildpanel, wxID_ANY, wxString::FromUTF8("⚄"));
	
	applicationTitleText = new wxStaticText(buildpanel, wxID_ANY, wxString::FromUTF8(ApplicationTitleText));
	applicationTitleBox = new wxTextCtrl(buildpanel, wxID_ANY, wxString::FromUTF8("video"));
	
	productCodeText = new wxStaticText(buildpanel, wxID_ANY, wxString::FromUTF8(ProductCodeText));
	productCodeBox = new wxTextCtrl(buildpanel, wxID_ANY, wxString::FromUTF8("VDIJ"));
	
	statusText = new wxStaticText(buildpanel, wxID_ANY, "   ");
	
	buildBar = new wxGauge(buildpanel, wxID_ANY, 1, wxDefaultPosition, {-1, 25});
	exportArchive = new wxProcess(frame);
	barPulser = new wxTimer();
	exportLogger = new wxTimer();
	
	buildButton = new wxButton(buildpanel, wxID_ANY, wxString::FromUTF8(Build));
	{ // buildButton
		int w, buttwidth, h, buttheight;
		wxFont f;
		
		wxButton button(panel, wxID_ANY, buildButton->GetLabel());
		button.Show(false);
		button.GetSize(&buttwidth, &buttheight);
		f = buildButton->GetFont();
		buildButton->GetTextExtent(buildButton->GetLabel(), &w, &h, nullptr, nullptr, &f);
		buttwidth = buttwidth - w;
		buttheight = buttheight - h;

		buildButton->SetSize(w + (buttwidth * 2), h + (buttheight * 2));
	}
	
	cancelButton = new wxButton(buildpanel, wxID_ANY, wxString::FromUTF8(Cancel));
	
	//extractDialog = new wxProgressDialog(wxEmptyString, wxEmptyString);
	//extractDialog->Destroy();
	extractArchive = new wxProcess(frame);
	extractPulser = new wxTimer();
	extractLogger = new wxTimer();
	
	aboutframe = new wxFrame(frame, wxID_ANY, wxString::FromUTF8(aboutFrameText), wxDefaultPosition, wxDefaultSize, wxRESIZE_BORDER|wxCAPTION|wxCLOSE_BOX|wxFRAME_FLOAT_ON_PARENT|wxCLIP_CHILDREN);
	aboutpanel = new wxPanel(aboutframe);
	titleLogo = new wxStaticBitmap(aboutpanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	byMeText = new wxStaticText(aboutpanel, wxID_ANY, wxString::FromUTF8(ByMeText));
	gitHubLinker = new wxHyperlinkCtrl(aboutpanel, wxID_ANY, wxString::FromUTF8(GitHubLinker), wxString::FromUTF8(githubRepoLink));
	versionText = new wxStaticText(aboutpanel, wxID_ANY, wxString::FromUTF8(version));
	
	updateCheck = new wxWebRequest(wxWebSession::GetDefault().CreateRequest(frame, "https://api.github.com/repos/" + githubRepo + "/tags"));
}

int theWidgets::executeCommand(const wxString &command, wxArrayString* output, wxArrayString* errors) {
	wxArrayString my_output;
	wxArrayString my_errors;
	int ret = 0;
	
	ret = wxExecute(command, my_output, my_errors, Execution::flags);
	
	consoleLog->LogTextAtLevel(0, command + "\n==========\n");
	for (auto &s : my_output) {
		consoleLog->LogTextAtLevel(0, s);
	}
	consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
	
	if(output != nullptr) {
		*output = wxArrayString(my_output);
	}
	if(errors != nullptr) {
		*errors = wxArrayString(my_errors);
	}
	
	return ret;
}

void theWidgets::doAddRows(int rows) {
	for(int i = 0; i < rows; i++) {
		wxTextCtrl* box = new wxTextCtrl(scrolledPanel, wxID_ANY, wxEmptyString);
		PlayerTitles.push_back(box);
	}
	for(int i = 0; i < rows; i++) {
		wxPathCtrl* box = new wxPathCtrl(scrolledPanel, wxID_ANY, wxEmptyString);
		MoflexFiles.push_back(box);
	}
	for(int i = 0; i < rows; i++) {
		wxPathCtrl* box = new wxPathCtrl(scrolledPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
		MenuBanners.push_back(box);
	}
	
	for(int i = 0; i < rows; i++) {
		wxButton* button = new wxButton(scrolledPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
		int width, height;
		wxFont f = button->GetFont();
		
		button->SetLabel(wxString::FromUTF8("↑"));
		
		button->GetTextExtent(button->GetLabel(), &width, &height, nullptr, nullptr, &f);
		button->SetSize(width + Borders::width + 5, height + Borders::height);
		
		MultiUp.push_back(button);
	}
	for(int i = 0; i < rows; i++) {
		wxButton* button = new wxButton(scrolledPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
		int width, height;
		wxFont f = button->GetFont();
		
		button->SetLabel(wxString::FromUTF8("↓"));
		
		button->GetTextExtent(button->GetLabel(), &width, &height, nullptr, nullptr, &f);
		button->SetSize(width + Borders::width + 5, height + Borders::height);
		
		MultiDown.push_back(button);
	}
}

void theWidgets::setFonts() {
	{ // modeText
		int w, h;
		wxFont f(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		
		modeText->SetFont(f);
		modeText->GetTextExtent(modeText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		modeText->SetSize(w, h);
	}
	
	{ // bannerText
		int w, h;
		wxFont f;
		
		f = modeText->GetFont();
		
		bannerText->SetFont(f);
		bannerText->GetTextExtent(bannerText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		bannerText->SetSize(w, h);
	}
	{ // bannerBrowse
		int w, h;
		wxFont f;
		
		f = bannerBrowse->GetFont();
		bannerBrowse->GetTextExtent(bannerBrowse->GetLabel(), &w, &h, nullptr, nullptr, &f);

		bannerBrowse->SetSize((w / 2) + (w * 2), (h / 2) + (h * 2));
	}
	{ // bannerError
		int w, h;
		wxFont f;
		
		f = modeText->GetFont().Scale(0.666F).Bold();
		
		bannerError->SetFont(f);
		bannerError->GetTextExtent(bannerError->GetLabel(), &w, &h, nullptr, nullptr, &f);
		bannerError->SetSize(w, h);
		
		//bannerError->Show(false);
	}
	
	{ // iconText
		int w, h;
		wxFont f;
		
		f = modeText->GetFont();
		
		iconText->SetFont(f);
		iconText->GetTextExtent(iconText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		iconText->SetSize(w, h);
	}
	{ // iconBrowse
		int w, h;
		wxFont f;

		f = iconBrowse->GetFont();
		iconBrowse->GetTextExtent(iconBrowse->GetLabel(), &w, &h, nullptr, nullptr, &f);

		iconBrowse->SetSize((w / 2) + (w * 2), (h / 2) + (h * 2));
	}
	{ // iconError
		int w, h;
		wxFont f;
		
		f = bannerError->GetFont().Bold();
		
		iconError->SetFont(f);
		iconError->GetTextExtent(iconError->GetLabel(), &w, &h, nullptr, nullptr, &f);
		iconError->SetSize(w, h);
		
		//iconError->Show(false);
	}

	{ // shortnameText
		int w, h;
		wxFont f;
		
		f = modeText->GetFont();
		
		shortnameText->SetFont(f);
		shortnameText->GetTextExtent(shortnameText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		shortnameText->SetSize(w, h);
	}
	{ // shortnameBox
		int w, h;
		wxFont f;
		
		f = shortnameBox->GetFont();
		
		shortnameBox->SetFont(f);
		shortnameBox->GetTextExtent("A", &w, &h, nullptr, nullptr, &f);
		shortnameBox->GetSize(&w, NULL);
		shortnameBox->SetSize(w, (h * 2) + 10);//uhhhhhh
	}
	{ // shortnameError
		int w, h;
		wxFont f;
		
		f = bannerError->GetFont();
		f = f.Bold();
		
		shortnameError->SetFont(f);
		shortnameError->GetTextExtent(shortnameError->GetLabel(), &w, &h, nullptr, nullptr, &f);
		shortnameError->SetSize(w, h);
	}
	
	{ // longnameText
		int w, h;
		wxFont f;
		
		f = modeText->GetFont();
		
		longnameText->SetFont(f);
		longnameText->GetTextExtent(longnameText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		longnameText->SetSize(w, h);
	}
	{ // longnameBox
		int w, h;
		wxFont f;
		
		f = longnameBox->GetFont();
		
		longnameBox->SetFont(f);
		longnameBox->GetTextExtent("A", &w, &h, nullptr, nullptr, &f);
		longnameBox->GetSize(&w, NULL);
		longnameBox->SetSize(w, (h * 2) + 10); // uhhhhhh
	}
	{ // longnameError
		int w, h;
		wxFont f;
		
		f = bannerError->GetFont().Bold();
		
		longnameError->SetFont(f);
		longnameError->GetTextExtent(longnameError->GetLabel(), &w, &h, nullptr, nullptr, &f);
		longnameError->SetSize(w, h);
	}
	
	{ // publisherText
		int w, h;
		wxFont f;
		
		f = modeText->GetFont();
		
		publisherText->SetFont(f);
		publisherText->GetTextExtent(publisherText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		publisherText->SetSize(w, h);
	}
	{ // publisherBox
		int w, h;
		wxFont f;
		
		f = publisherBox->GetFont();
		
		publisherBox->SetFont(f);
		publisherBox->GetTextExtent("A", &w, &h, nullptr, nullptr, &f);
		publisherBox->GetSize(&w, NULL);
		publisherBox->SetSize(w, (h * 2) + 10);//uhhhhhh
	}
	{ // publisherError
		int w, h;
		wxFont f;
		
		f = bannerError->GetFont().Bold();
		
		publisherError->SetFont(f);
		publisherError->GetTextExtent(publisherError->GetLabel(), &w, &h, nullptr, nullptr, &f);
		publisherError->SetSize(w, h);
	}
	
	{ // bannerCustomText
		int w, h;
		wxFont f;
		
		f = bannerError->GetFont().GetBaseFont();
		
		bannerCustomText->SetFont(f);
		bannerCustomText->GetTextExtent(bannerCustomText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		bannerCustomText->SetSize(w, h);
	}
	{ // bannerPreviewText
		int w, h;
		wxFont f;
		
		f = bannerError->GetFont().GetBaseFont();
		
		bannerPreviewText->SetFont(f);
		bannerPreviewText->GetTextExtent(bannerPreviewText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		bannerPreviewText->SetSize(w, h);
	}

	{ // copyCheck
		int w, width, mywidth, boxwidth, h;
		wxFont f;
		
		wxCheckBox Check(panel, wxID_ANY, copyCheck->GetLabel());
		f = Check.GetFont();
		Check.GetTextExtent(copyCheck->GetLabel(), &width, NULL, nullptr, nullptr, &f);
		Check.GetSize(&mywidth, NULL);
		Check.Show(false);
		
		boxwidth = mywidth - width;
		
		f = modeText->GetFont();
		
		copyCheck->SetFont(f);
		copyCheck->GetTextExtent(copyCheck->GetLabel(), &w, &h, nullptr, nullptr, &f);
		copyCheck->SetSize(w + boxwidth, h);
	}
	
	{ // ffRewindCheck
		int w, width, mywidth, boxwidth, h;
		wxFont f;
		
		wxCheckBox Check(panel, wxID_ANY, ffRewindCheck->GetLabel());
		f = Check.GetFont();
		Check.GetTextExtent(ffRewindCheck->GetLabel(), &width, NULL, nullptr, nullptr, &f);
		Check.GetSize(&mywidth, NULL);
		Check.Show(false);
		
		boxwidth = mywidth - width;
		
		f = modeText->GetFont().Scale(0.9F);
		
		ffRewindCheck->SetFont(f);
		ffRewindCheck->GetTextExtent(ffRewindCheck->GetLabel(), &w, &h, nullptr, nullptr, &f);
		ffRewindCheck->SetSize(w + boxwidth, h);
	}
	{ // dimCheck
		int w, width, mywidth, boxwidth, h;
		wxFont f;
		
		wxCheckBox Check(panel, wxID_ANY, ffRewindCheck->GetLabel());
		f = Check.GetFont();
		Check.GetTextExtent(dimCheck->GetLabel(), &width, NULL, nullptr, nullptr, &f);
		Check.GetSize(&mywidth, NULL);
		Check.Show(false);
		
		boxwidth = mywidth - width;
		
		f = ffRewindCheck->GetFont();
		
		dimCheck->SetFont(f);
		dimCheck->GetTextExtent(dimCheck->GetLabel(), &w, &h, nullptr, nullptr, &f);
		dimCheck->SetSize(w + boxwidth, h);
	}

	{ // multiBannerPreviewIndex
		int w, h;
		wxFont f;
		
		f = bannerPreviewText->GetFont();
		
		multiBannerPreviewIndex->SetFont(f);
		multiBannerPreviewIndex->GetTextExtent(multiBannerPreviewIndex->GetLabel(), &w, &h, nullptr, nullptr, &f);
		multiBannerPreviewIndex->SetSize(w, h);
	}
	
	{ // playerTitleText
		int w, h;
		wxFont f;
		
		f = modeText->GetFont();
		
		playerTitleText->SetFont(f);
		playerTitleText->GetTextExtent(playerTitleText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		playerTitleText->SetSize(w, h);
	}
	{ // moflexFileText
		int w, h;
		wxFont f;
		
		f = modeText->GetFont();
		
		moflexFileText->SetFont(f);
		moflexFileText->GetTextExtent(moflexFileText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		moflexFileText->SetSize(w, h);
	}
	{ // menuBannerText
		int w, h;
		wxFont f;
		
		f = modeText->GetFont();
		
		menuBannerText->SetFont(f);
		menuBannerText->GetTextExtent(menuBannerText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		menuBannerText->SetSize(w, h);
	}
	
	{ // moflexBrowse
		int w, h;
		wxFont f;
		
		f = moflexBrowse->GetFont();
		moflexBrowse->GetTextExtent(moflexBrowse->GetLabel(), &w, &h, nullptr, nullptr, &f);

		moflexBrowse->SetSize((w / 2) + (w * 2), (h / 2) + (h * 2));
	}
	{ // multiBannerBrowse
		int w, h;
		wxFont f;
		
		f = multiBannerBrowse->GetFont();
		multiBannerBrowse->GetTextExtent(multiBannerBrowse->GetLabel(), &w, &h, nullptr, nullptr, &f);

		multiBannerBrowse->SetSize((w / 2) + (w * 2), (h / 2) + (h * 2));
	}
	
	{ // removeRow
		int w, h;
		wxFont f;
		
		f = modeText->GetFont().Scale(1.1F);
		
		removeRow->SetFont(f);
		removeRow->GetTextExtent(removeRow->GetLabel(), NULL, &h, nullptr, nullptr, &f);
		removeRow->GetSize(&w, NULL);
		removeRow->SetSize(w, h + (h / 2));
	}
	{ // appendRow
		int w, h;
		wxFont f;
		
		f = removeRow->GetFont();
		
		appendRow->SetFont(f);
		appendRow->GetTextExtent(appendRow->GetLabel(), NULL, &h, nullptr, nullptr, &f);
		appendRow->GetSize(&w, NULL);
		appendRow->SetSize(w, h + (h / 2));
	}

	{ // splitPatchButton
		int w, h;
		wxFont f;
		
		f = ffRewindCheck->GetFont();
		
		splitPatchButton->SetFont(f);
		splitPatchButton->GetTextExtent(splitPatchButton->GetLabel(), &w, &h, nullptr, nullptr, &f);
		splitPatchButton->SetSize(w + (w / 4), h + (h / 2));
	}
	
	{ // rowText
		int w, h;
		wxFont f;
		
		f = bannerPreviewText->GetFont();
		
		rowText->SetFont(f);
		rowText->GetTextExtent(rowText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		rowText->SetSize(w, h);
	}
	// buildpanel
	{ // titleIDText
		int w, h;
		wxFont f;
		
		f = modeText->GetFont();
		
		titleIDText->SetFont(f);
		titleIDText->GetTextExtent(titleIDText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		titleIDText->SetSize(w, h);
	}
	{ // titleIDBox
		int w, h;
		wxFont f;
		
		f = titleIDBox->GetFont();
		f.SetFamily(wxFONTFAMILY_TELETYPE);
		
		titleIDBox->SetFont(f);
		titleIDBox->GetTextExtent(wxString::FromUTF8("12345"), &w, &h, nullptr, nullptr, &f);
		titleIDBox->SetSize(w + 20, h + 10);
	}
	{ // zerozero
		int w, h;
		wxFont f;
		
		f = modeText->GetFont();
		
		zerozero->SetFont(f);
		zerozero->GetTextExtent(zerozero->GetLabel(), &w, &h, nullptr, nullptr, &f);
		zerozero->SetSize(w, h);
	}
	{ // titleIDButton
		int w, h;
		wxFont f;
		
		f = modeText->GetFont().Scale(2);
		
		titleIDButton->SetFont(f);
		titleIDButton->GetTextExtent(titleIDButton->GetLabel(), &w, &h, nullptr, nullptr, &f);
		titleIDButton->SetSize(w > h ? w : h, w > h ? w : h);
	}
	
	{ // applicationTitleText
		int w, h;
		wxFont f;
		
		f = modeText->GetFont();
		
		applicationTitleText->SetFont(f);
		applicationTitleText->GetTextExtent(applicationTitleText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		applicationTitleText->SetSize(w, h);
	}
	{ // applicationTitleBox
		wxFont f;
		
		f = productCodeBox->GetFont();
		f.SetFamily(wxFONTFAMILY_TELETYPE);
		
		applicationTitleBox->SetFont(f);
	}
	
	{ // productCodeText
		int w, h;
		wxFont f;
		
		f = modeText->GetFont();
		
		productCodeText->SetFont(f);
		productCodeText->GetTextExtent(productCodeText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		productCodeText->SetSize(w, h);
	}
	{ // productCodeBox
		int w, h;
		wxFont f;
		
		f = productCodeBox->GetFont();
		f.SetFamily(wxFONTFAMILY_TELETYPE);
		
		productCodeBox->SetFont(f);
		productCodeBox->GetTextExtent(wxString::FromUTF8("1234"), &w, &h, nullptr, nullptr, &f);
		productCodeBox->SetSize(w + 20, h + 10);
	}
	
	{ // statusText
		int w, h;
		wxFont f;
		
		f = modeText->GetFont();
		
		statusText->SetFont(f);
		statusText->GetTextExtent(statusText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		statusText->SetSize(w, h);
		
		statusText->Show(false);
	}
	
	{ // gitHubLinker
		int w, h;
		wxFont f;
		
		f = modeText->GetFont();
		
		gitHubLinker->SetFont(f);
		gitHubLinker->GetTextExtent(gitHubLinker->GetLabel(), &w, &h, nullptr, nullptr, &f);
		gitHubLinker->SetSize(w, h);
	}
}

void theWidgets::EnableBannerLeftRight() {
	if(MenuBanners.size() != 1) {
		if(VI9P::MultiBannerIndex > 0 && VI9P::MultiBannerIndex < MenuBanners.size() - 1) {
			multiBannerPreviewLeft->Enable(true);
			multiBannerPreviewRight->Enable(true);
		}
		if(VI9P::MultiBannerIndex <= 0) {
			multiBannerPreviewLeft->Enable(false);
			multiBannerPreviewRight->Enable(true);
		}
		size_t disabled_row;
		for(disabled_row = 0; disabled_row < MenuBanners.size(); disabled_row++) {
			if(!MenuBanners.at(disabled_row)->IsEnabled()) {
				break;
			}
		}
		// stop at last enabled row or last row
		if(VI9P::MultiBannerIndex >= disabled_row - 1 || VI9P::MultiBannerIndex >= MenuBanners.size() - 1) {
			multiBannerPreviewRight->Enable(false);
			multiBannerPreviewLeft->Enable(true);
		}
	}
	else {
		multiBannerPreviewLeft->Enable(false);
		multiBannerPreviewRight->Enable(false);
	}
	setCursors();
}

void theWidgets::ShowMultiUpDown() {
	for(const auto &row : MultiUp) {
		row->Enable(true);
		row->Show(true);
	}
	MultiUp.front()->Enable(false);
	MultiUp.front()->Show(false);
	for(const auto &row : MultiDown) {
		row->Enable(true);
		row->Show(true);
	}
	MultiDown.back()->Enable(false);
	MultiDown.back()->Show(false);
}

void theWidgets::ShowPatchUpDown() {
	splitPatchUp->Enable(parameters.splitPos > 1);
	splitPatchUp->Show(parameters.splitPos > 1);
	splitPatchDown->Enable(parameters.splitPos < parameters.rows - 1);
	splitPatchDown->Show(parameters.splitPos < parameters.rows - 1);
	if(parameters.rows > MAX_ROWS_MULTI) {
		splitPatchDown->Enable(parameters.splitPos < MAX_ROWS_MULTI - 1);
		splitPatchDown->Show(parameters.splitPos < MAX_ROWS_MULTI - 1);
	}
}

void theWidgets::setToolTips() {
	modeChoiceBox->SetToolTip(wxString::FromUTF8(modeChoiceBoxTip));
	bannerBox->SetToolTip(wxString::FromUTF8(bannerBoxTip));
	bannerBrowse->SetToolTip(wxString::FromUTF8(bannerBrowseTip));
	iconBox->SetToolTip(wxString::FromUTF8(iconBoxTip));
	iconBrowse->SetToolTip(wxString::FromUTF8(iconBrowseTip));
	shortnameBox->SetToolTip(wxString::FromUTF8(shortnameBoxTip));
	longnameBox->SetToolTip(wxString::FromUTF8(longnameBoxTip));
	publisherBox->SetToolTip(wxString::FromUTF8(publisherBoxTip));
	
	if(parameters.mode == 1) {
		copyBox->SetToolTip(wxString::FromUTF8(copyBoxTip));
	}
	else {
		copyBox->SetToolTip(wxString::FromUTF8(onlyMultiVideo));
	}
	
	if(parameters.mode == 1) {
		copyCheck->SetToolTip(wxString::FromUTF8(copyCheckTip));
	}
	else {
		copyCheck->SetToolTip(wxString::FromUTF8(onlyMultiVideo));
	}
	
	iconPreview->SetToolTip(wxString::FromUTF8(iconPreviewTip));
	ffRewindCheck->SetToolTip(wxString::FromUTF8(ffRewindCheckTip));
	dimCheck->SetToolTip(wxString::FromUTF8(dimCheckTip));
	
	if(parameters.mode == 0) {
		multiBannerPreview->SetToolTip(wxString::FromUTF8(onlyMultiVideo));
	}
	else {
		multiBannerPreview->SetToolTip(wxString::FromUTF8(multiBannerPreviewTip));
	}
	
	multiBannerPreviewLeft->SetToolTip(wxString::FromUTF8(multiBannerPreviewLeftTip));
	multiBannerPreviewRight->SetToolTip(wxString::FromUTF8(multiBannerPreviewRightTip));
	moflexFileText->SetToolTip(wxString::FromUTF8(moflexFileTip));
	for(size_t row = 0; row < PlayerTitles.size(); row++) {
		PlayerTitles.at(row)->SetToolTip(wxString::FromUTF8(playerTitlesTip + " (" + std::to_string(row + 1) + ')'));
	}
	for(size_t row = 0; row < MoflexFiles.size(); row++) {
		MoflexFiles.at(row)->SetToolTip(wxString::FromUTF8(moflexFilesTip + " (" + std::to_string(row + 1) + ')'));
	}
	for(size_t row = 0; row < MenuBanners.size(); row++) {
		if(parameters.mode == 0) {
			MenuBanners.at(row)->SetToolTip(wxString::FromUTF8(onlyMultiVideo));
		}
		else if(parameters.mode == 1) {
			MenuBanners.at(row)->SetToolTip(wxString::FromUTF8(menuBannersTip + " (" + std::to_string(row + 1) + ')'));
		}
		else if(parameters.mode == 2) {
			if(row == 0) {
				MenuBanners.at(row)->SetToolTip(wxString::FromUTF8(topImageTip));
			}
			else {
				MenuBanners.at(row)->SetToolTip(wxString::FromUTF8(onlyMultiVideo));
			}
		}
	}
	for(size_t row = 0; row < MultiUp.size(); row++) {
		MultiUp.at(row)->SetToolTip(wxString::FromUTF8(multiUpTip));
	}
	for(size_t row = 0; row < MultiDown.size(); row++) {
		MultiDown.at(row)->SetToolTip(wxString::FromUTF8(multiDownTip));
	}
	
	moflexBrowse->SetToolTip(wxString::FromUTF8(moflexBrowseTip));
	
	if(parameters.mode == 0) {
		multiBannerBrowse->SetToolTip(wxString::FromUTF8(onlyMultiVideo));
	}
	else if(parameters.mode == 1) {
		multiBannerBrowse->SetToolTip(wxString::FromUTF8(multiBannerBrowseTip));
	}
	else if(parameters.mode == 2) {
		multiBannerBrowse->SetToolTip(wxString::FromUTF8(extendedTopImageBrowseTip));
	}
	
	removeRow->SetToolTip(wxString::FromUTF8(removeRowTip));
	
	if(parameters.mode == 0) {
		appendRow->SetToolTip(wxString::FromUTF8(onlyMultiVideo));
	}
	else {
		appendRow->SetToolTip(wxString::FromUTF8(appendRowTip));
	}
	
	if(parameters.mode == 0) {
		splitPatchButton->SetToolTip(wxString::FromUTF8(splitPatchTip + '\n' + onlyMultiVideo));
	}
	else if(parameters.mode == 1) {
		splitPatchButton->SetToolTip(wxString::FromUTF8(splitPatchTip));
	}
	else if(parameters.mode == 2) {
		splitPatchButton->SetToolTip(wxString::FromUTF8(onlyMultiVideo));
	}
	
	splitPatchUp->SetToolTip(wxString::FromUTF8(splitPatchUpTip));
	splitPatchDown->SetToolTip(wxString::FromUTF8(splitPatchDownTip));
	
	titleIDBox->SetToolTip(wxString::FromUTF8(titleIDBoxTip));
	titleIDButton->SetToolTip(wxString::FromUTF8(titleIDButtonTip));
	applicationTitleBox->SetToolTip(wxString::FromUTF8(applicationTitleBoxTip));
	productCodeBox->SetToolTip(wxString::FromUTF8(productCodeBoxTip));
	buildButton->SetToolTip(wxString::FromUTF8(buildButtonTip));
	cancelButton->SetToolTip(wxString::FromUTF8(cancelButtonTip));
	
	titleLogo->SetToolTip(wxString::FromUTF8(frameText));
	gitHubLinker->SetToolTip(wxString::FromUTF8(githubRepoLink));
}

void theWidgets::setCursors() {
	modeChoiceBox->SetCursor(modeChoiceBox->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	bannerBox->SetCursor(bannerBox->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_NO_ENTRY);
	bannerBrowse->SetCursor(bannerBrowse->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	iconBox->SetCursor(iconBox->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_NO_ENTRY);
	iconBrowse->SetCursor(iconBrowse->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	iconPreview->SetCursor(iconPreview->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_NO_ENTRY);
	shortnameBox->SetCursor(shortnameBox->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_NO_ENTRY);
	longnameBox->SetCursor(longnameBox->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_NO_ENTRY);
	publisherBox->SetCursor(publisherBox->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_NO_ENTRY);
	copyBox->SetCursor(copyBox->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_NO_ENTRY);
	copyCheck->SetCursor(copyCheck->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	iconPreview->SetCursor(iconPreview->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	ffRewindCheck->SetCursor(ffRewindCheck->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	dimCheck->SetCursor(dimCheck->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	multiBannerPreview->SetCursor(multiBannerPreview->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	multiBannerPreviewLeft->SetCursor(multiBannerPreviewLeft->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	multiBannerPreviewRight->SetCursor(multiBannerPreviewRight->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	for(const auto& row : PlayerTitles) {
		row->SetCursor(row->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_NO_ENTRY);
	}
	for(const auto& row : MoflexFiles) {
		row->SetCursor(row->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_NO_ENTRY);
	}
	for(const auto& row : MenuBanners) {
		row->SetCursor(row->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_NO_ENTRY);
	}
	for(const auto& row : MultiUp) {
		row->SetCursor(row->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	}
	for(const auto& row : MultiDown) {
		row->SetCursor(row->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	}
	moflexBrowse->SetCursor(moflexBrowse->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	multiBannerBrowse->SetCursor(multiBannerBrowse->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	removeRow->SetCursor(removeRow->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	appendRow->SetCursor(appendRow->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	splitPatchButton->SetCursor(splitPatchButton->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	splitPatchUp->SetCursor(splitPatchButton->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	splitPatchDown->SetCursor(splitPatchButton->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	frame->SetCursor(wxCURSOR_ARROW);
	panel->SetCursor(wxCURSOR_ARROW);
	mediaPanel->SetCursor(wxCURSOR_ARROW);
	
	titleIDBox->SetCursor(titleIDButton->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_NO_ENTRY);
	titleIDButton->SetCursor(titleIDButton->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	applicationTitleBox->SetCursor(titleIDButton->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_NO_ENTRY);
	buildButton->SetCursor(titleIDButton->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	cancelButton->SetCursor(titleIDButton->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
}

void theWidgets::positionWidgets() {
	{ // modeChoiceBox
		int x, y, height;
		modeText->GetPosition(&x, &y);
		modeText->GetSize(NULL, &height);
		
		modeChoiceBox->Move(x, y + height);
	}
	
	{ // bannerText
		int x, y, height;
		modeChoiceBox->GetPosition(&x, &y);
		modeChoiceBox->GetSize(NULL, &height);
		
		bannerText->Move(x, y + (height * 2));
	}
	{ // bannerBox
		int x, y, width, height, myheight;
		bannerText->GetPosition(&x, &y);
		bannerText->GetSize(&width, &height);
		bannerBox->GetSize(NULL, &myheight);
		
		bannerBox->Move(x + width + 5, y + ((height - myheight) / 2));
	}
	{ // bannerBrowse y
		int myx, y, height, myheight;
		bannerBox->GetPosition(NULL, &y);
		bannerBox->GetSize(NULL, &height);
		bannerBrowse->GetPosition(&myx, NULL);
		bannerBrowse->GetSize(NULL, &myheight);
		
		bannerBrowse->Move(myx, y + ((height - myheight) / 2));
	}
	{ // bannerError
		int x, y, height;
		bannerBox->GetPosition(&x, NULL);
		bannerBrowse->GetPosition(NULL, &y);
		bannerBrowse->GetSize(NULL, &height);
		
		bannerError->Move(x, y + height);
	}
	
	{ // iconText
		int x, y, height, myheight, browseheight;
		modeText->GetPosition(&x, NULL);
		bannerError->GetPosition(NULL, &y);
		bannerError->GetSize(NULL, &height);
		iconText->GetSize(NULL, &myheight);
		iconBrowse->GetSize(NULL, &browseheight);
		
		iconText->Move(x, y + height + ((browseheight - myheight) / 2));
	}
	{ // iconBox
		int x, y, width, height, myheight;
		iconText->GetPosition(&x, &y);
		iconText->GetSize(&width, &height);
		iconBox->GetSize(NULL, &myheight);
		
		iconBox->Move(x + width + 5, y + ((height - myheight) / 2));
		//iconBox->SetSize(, &myheight);
	}
	{ // iconBrowse y
		int myx, y, height;
		iconBrowse->GetPosition(&myx, NULL);
		bannerError->GetPosition(NULL, &y);
		bannerError->GetSize(NULL, &height);
		
		iconBrowse->Move(myx, y + height);
	}
	{ // iconError
		int x, y, height;
		iconBox->GetPosition(&x, NULL);
		iconBrowse->GetPosition(NULL, &y);
		iconBrowse->GetSize(NULL, &height);
		
		iconError->Move(x, y + height);
	}
	
	{ // bannerPreview y
		int x, y;
		modeText->GetPosition(NULL, &y);
		bannerPreview->GetPosition(&x, NULL);
		
		bannerPreview->Move(x, y);
	}
	
	{ // shortnameText
		int x, y, bannerPreviewY, iconBrowseY, height, bannerPreviewHeight, bannerPreviewTextHeight, iconPreviewHeight;
		modeText->GetPosition(&x, NULL);
		iconError->GetPosition(NULL, &y);
		iconError->GetSize(NULL, &height);
		
		bannerPreview->GetPosition(NULL, &bannerPreviewY);
		bannerPreview->GetSize(NULL, &bannerPreviewHeight);
		bannerPreviewText->GetSize(NULL, &bannerPreviewTextHeight);
		iconBrowse->GetPosition(NULL, &iconBrowseY);
		iconPreview->GetSize(NULL, &iconPreviewHeight);
		
		std::vector<int> sizes(3, 0);
		sizes.at(0) = (bannerPreviewY + bannerPreviewHeight + bannerPreviewTextHeight); // banner preview
		sizes.at(1) = (y + height); // icon error
		sizes.at(2) = (iconBrowseY + iconPreviewHeight); // icon preview
		
		std::vector<int>::iterator Heightest = std::max_element(sizes.begin(), sizes.end()); // dont even worry
		
		shortnameText->Move(x, (*Heightest) + 6);
	}
	{ // shortnameBox
		int x, y, width;
		shortnameText->GetPosition(&x, &y);
		shortnameText->GetSize(&width, NULL);
		
		shortnameBox->Move(x + width + 5, y);
	}
	{ // shortnameError
		int x, y, height;
		shortnameBox->GetPosition(&x, &y);
		shortnameBox->GetSize(NULL, &height);
		
		shortnameError->Move(x, y + height);
	}
	
	{ // longnameText
		int x, y, height;
		modeText->GetPosition(&x, NULL);
		shortnameError->GetPosition(NULL, &y);
		shortnameError->GetSize(NULL, &height);
		
		longnameText->Move(x, y + height);
	}
	{ // longnameBox
		int x, y, width;
		longnameText->GetPosition(&x, &y);
		longnameText->GetSize(&width, NULL);
		
		longnameBox->Move(x + width + 5, y);
	}
	{ // longnameError
		int x, y, height;
		longnameBox->GetPosition(&x, &y);
		longnameBox->GetSize(NULL, &height);
		
		longnameError->Move(x, y + height);
	}
	
	{ // publisherText
		int x, y, height;
		modeText->GetPosition(&x, NULL);
		longnameError->GetPosition(NULL, &y);
		longnameError->GetSize(NULL, &height);
		
		publisherText->Move(x, y + height);
	}
	{ // publisherBox
		int x, y, width;
		publisherText->GetPosition(&x, &y);
		publisherText->GetSize(&width, NULL);
		
		publisherBox->Move(x + width + 5, y);
	}
	{ // publisherError
		int x, y, height;
		publisherBox->GetPosition(&x, &y);
		publisherBox->GetSize(NULL, &height);
		
		publisherError->Move(x, y + height);
	}
	
	{ // ffRewindCheck
		int x, y, height;
		publisherError->GetPosition(&x, &y);
		publisherError->GetSize(NULL, &height);
		
		ffRewindCheck->Move(x, y + height);
	}
	{ // dimCheck
		int x, y, height;
		ffRewindCheck->GetPosition(&x, &y);
		ffRewindCheck->GetSize(NULL, &height);
		
		dimCheck->Move(x, y + height);
	}
	
	// stuff that moves or changes size
	{ // copyBox
		int x, y, width, mywidth, checkwidth, checkheight;
		modeText->GetPosition(&x, &y);
		panel->GetSize(&width, NULL);
		copyBox->GetSize(&mywidth, NULL);
		copyCheck->GetSize(&checkwidth, NULL);
		copyCheck->GetSize(NULL, &checkheight);
		
		int iconErrorX, iconErrorW, iconPreviewW, bannerPreviewW, bannerPreviewTextW;
		iconError->GetPosition(&iconErrorX, NULL);
		iconError->GetSize(&iconErrorW, NULL);
		iconPreview->GetSize(&iconPreviewW, NULL);
		bannerPreview->GetSize(&bannerPreviewW, NULL);
		bannerPreviewText->GetSize(&bannerPreviewTextW, NULL);
		
		if(width >= iconErrorX + iconErrorW + iconPreviewW + 5 + (bannerPreviewW > bannerPreviewTextW ? bannerPreviewW : bannerPreviewTextW) + 15 + (mywidth > checkwidth ? mywidth : checkwidth) + x) {
			copyBox->Move(width - (mywidth > checkwidth ? mywidth : checkwidth) - x, y + checkheight);
		}
		else {
			copyBox->Move(iconErrorX + iconErrorW + iconPreviewW + 5 + (bannerPreviewW > bannerPreviewTextW ? bannerPreviewW : bannerPreviewTextW) + 15, y + checkheight);
		}
	}
	{ // copyCheck
		int x, y, myheight;
		copyBox->GetPosition(&x, &y);
		copyCheck->GetSize(NULL, &myheight);
		
		copyCheck->Move(x, y - myheight); // why not just put this in before the box?
	}
	
	{ // bannerPreview x
		int x, y, mywidth, bannerPreviewTextW;
		copyCheck->GetPosition(&x, NULL);
		bannerPreview->GetPosition(NULL, &y);
		bannerPreview->GetSize(&mywidth, NULL);
		bannerPreviewText->GetSize(&bannerPreviewTextW, NULL);
		
		bannerPreview->Move(x - (mywidth > bannerPreviewTextW ? mywidth : ((bannerPreviewTextW - mywidth) * 2)) - (mywidth > bannerPreviewTextW ? 15 : 15 * 2), y);
	}
	{ // bannerCustomText
		int x, y, width, mywidth, height, myheight;
		bannerPreview->GetPosition(&x, &y);
		bannerPreview->GetSize(&width, &height);
		bannerCustomText->GetSize(&mywidth, &myheight);
		
		bannerCustomText->Move(x + ((width - mywidth) / 2), y + ((height - myheight) / 2));
	}
	{ // bannerPreviewText
		int x, y, width, mywidth, height;
		bannerPreview->GetPosition(&x, &y);
		bannerPreview->GetSize(&width, &height);
		bannerPreviewText->GetSize(&mywidth, NULL);
		
		bannerPreviewText->Move(x + ((width - mywidth) / 2), y + height);
	}
	
	{ // iconPreview
		int x, bannerPreviewTextx, y, mywidth;
		iconBrowse->GetPosition(NULL, &y);
		bannerPreview->GetPosition(&x, NULL);
		iconPreview->GetSize(&mywidth, NULL);
		bannerPreviewText->GetPosition(&bannerPreviewTextx, NULL);
		
		iconPreview->Move((x < bannerPreviewTextx ? x : bannerPreviewTextx) - mywidth - 5, y);
	}
	{ // bannerBrowse x
		int x, bannerPreviewTextx, myy, mywidth;
		bannerPreview->GetPosition(&x, NULL);
		bannerBrowse->GetPosition(NULL, &myy);
		bannerBrowse->GetSize(&mywidth, NULL);
		bannerPreviewText->GetPosition(&bannerPreviewTextx, NULL);
		
		bannerBrowse->Move((x < bannerPreviewTextx ? x : bannerPreviewTextx) - mywidth - 5, myy);
	}
	{ // iconBrowse x
		int x, myy, mywidth;
		iconPreview->GetPosition(&x, NULL);
		iconBrowse->GetPosition(NULL, &myy);
		iconBrowse->GetSize(&mywidth, NULL);
		
		iconBrowse->Move(x - mywidth - 2, myy);
	}
	{ // bannerBox width
		int myx, browsex, myheight;
		bannerBrowse->GetPosition(&browsex, NULL);
		bannerBox->GetPosition(&myx, NULL);
		bannerBox->GetSize(NULL, &myheight);
		
		bannerBox->SetSize(browsex - myx - 2, myheight);
	}
	{ // iconBox width
		int myx, browsex, myheight;
		iconBrowse->GetPosition(&browsex, NULL);
		iconBox->GetPosition(&myx, NULL);
		iconBox->GetSize(NULL, &myheight);
		
		iconBox->SetSize(browsex - myx - 2, myheight);
	}
	{ // multiBannerPreview
		int x, y, width, mywidth, height;
		copyBox->GetPosition(&x, &y);
		copyBox->GetSize(&width, &height);
		multiBannerPreview->GetSize(&mywidth, NULL);
		
		multiBannerPreview->Move(x - (mywidth - width), y + height + 5);
	}
	{ // multiBannerPreviewIndex
		int x, y, width, mywidth, height;
		multiBannerPreview->GetPosition(&x, &y);
		multiBannerPreview->GetSize(&width, &height);
		multiBannerPreviewIndex->GetSize(&mywidth, NULL);
		
		multiBannerPreviewIndex->Move(x + ((width - mywidth) / 2), y + height);
	}
	{ // multiBannerPreviewLeft
		int x, y, height;
		multiBannerPreview->GetPosition(&x, &y);
		multiBannerPreview->GetSize(NULL, &height);
		//multiBannerIndex->GetSize(&mywidth, NULL);
		
		multiBannerPreviewLeft->Move(x, y + height);
	}
	{ // multiBannerPreviewRight
		int x, y, width, mywidth, height;
		multiBannerPreview->GetPosition(&x, &y);
		multiBannerPreview->GetSize(&width, &height);
		multiBannerPreviewRight->GetSize(&mywidth, NULL);
		
		multiBannerPreviewRight->Move((x + width) - mywidth, y + height);
	}
	{ // shortnameBox width
		int myx, myheight, x;
		multiBannerPreview->GetPosition(&x, NULL);
		shortnameBox->GetPosition(&myx, NULL);
		shortnameBox->GetSize(NULL, &myheight);
		
		shortnameBox->SetSize(x - myx - 15, myheight);
	}
	{ // longnameBox width
		int myx, myheight, x;
		multiBannerPreview->GetPosition(&x, NULL);
		longnameBox->GetPosition(&myx, NULL);
		longnameBox->GetSize(NULL, &myheight);
		
		longnameBox->SetSize(x - myx - 15, myheight);
	}
	{ // publisherBox width
		int myx, myheight, x;
		multiBannerPreview->GetPosition(&x, NULL);
		publisherBox->GetPosition(&myx, NULL);
		publisherBox->GetSize(NULL, &myheight);
		
		publisherBox->SetSize(x - myx - 15, myheight);
	}
	{ // playerTitleText y
		int x, y, checky, height, checkheight;
		multiBannerPreviewLeft->GetPosition(NULL, &y);
		dimCheck->GetPosition(NULL, &checky);
		dimCheck->GetSize(NULL, &checkheight);
		multiBannerPreviewLeft->GetSize(NULL, &height);
		playerTitleText->GetPosition(&x, NULL);
		
		playerTitleText->Move(x, (y > checky ? (y + height + 5) : (checky + checkheight + 5)));
	}
	{ // moflexFileText y
		int x, y, checky, height, checkheight;
		multiBannerPreviewLeft->GetPosition(NULL, &y);
		dimCheck->GetPosition(NULL, &checky);
		dimCheck->GetSize(NULL, &checkheight);
		multiBannerPreviewLeft->GetSize(NULL, &height);
		moflexFileText->GetPosition(&x, NULL);
		
		moflexFileText->Move(x, (y > checky ? (y + height + 5) : (checky + checkheight + 5)));
	}
	{ // menuBannerText y
		int x, y, checky, height, checkheight;
		multiBannerPreviewLeft->GetPosition(NULL, &y);
		multiBannerPreviewLeft->GetSize(NULL, &height);
		dimCheck->GetPosition(NULL, &checky);
		dimCheck->GetSize(NULL, &checkheight);
		menuBannerText->GetPosition(&x, NULL);
		
		menuBannerText->Move(x, (y > checky ? (y + height + 5) : (checky + checkheight + 5)));
	}
	{ // mediaPanel
		int x, myx, y, myy, width, height;
		modeText->GetPosition(&x, NULL);
		playerTitleText->GetPosition(NULL, &y);
		playerTitleText->GetSize(NULL, &height);
		
		mediaPanel->Move(x, y + height + 1);
		
		panel->GetSize(&width, &height);
		mediaPanel->GetPosition(&myx, &myy);
		
		mediaPanel->SetSize(width - (myx * 2), height - myy - myx);
	}
	{
		std::vector<int> sizes(5, 0);
		playerTitleText->GetSize(&sizes[0], NULL);
		moflexFileText->GetSize(&sizes[1], NULL);
		menuBannerText->GetSize(&sizes[2], NULL);
		moflexBrowse->GetSize(&sizes[3], NULL);
		multiBannerBrowse->GetSize(&sizes[4], NULL);
		std::vector<int>::iterator widestText = std::max_element(sizes.begin(), sizes.end());
		
		{ // PlayerTitles
			for(size_t row = 0; row < PlayerTitles.size(); row++) {
				int scrolledx, scrolledy, ppux, ppuy, previousy, width, upwidth, downwidth, upheight, height;
				mediaPanel->GetSize(&width, NULL);
				scrolledPanel->GetViewStart(&scrolledx, &scrolledy);
				scrolledPanel->GetScrollPixelsPerUnit(&ppux, &ppuy);
				MultiUp.at(0)->GetSize(&upwidth, NULL);
				MultiDown.at(0)->GetSize(&downwidth, NULL);
				PlayerTitles.at(row)->GetSize(NULL, &height);
				
				if(PlayerTitles.size() == 1) {
					if((width - (ppux * 2)) / 3 >= *widestText) {
						PlayerTitles.at(row)->SetSize((width - (ppux * 2)) / 3, height);
					}
					else {
						PlayerTitles.at(row)->SetSize(*widestText, height);
					}
				}
				else {
					if((width - (upwidth + 3 + downwidth + (ppux * 2))) / 3 >= *widestText) {
						PlayerTitles.at(row)->SetSize((width - (upwidth + 3 + downwidth + (ppux * 2))) / 3, height);
					}
					else {
						PlayerTitles.at(row)->SetSize(*widestText, height);
					}
				}
				
				if(row > 0) {
					PlayerTitles.at(row - 1)->GetPosition(NULL, &previousy);
					if(row == parameters.splitPos) {
						splitPatchUp->GetSize(NULL, &upheight);
						PlayerTitles.at(row)->Move(0 - (scrolledx * ppux), (previousy + height + upheight));
					}
					else {
						PlayerTitles.at(row)->Move(0 - (scrolledx * ppux), (previousy + height));
					}
				}
				else {
					PlayerTitles.at(row)->Move(0 - (scrolledx * ppux), (height * row) - (scrolledy * ppuy));
				}
			}
		}
		{ // MoflexFiles
			for(size_t row = 0; row < MoflexFiles.size(); row++) {
				int scrolledx, scrolledy, ppux, ppuy, previousy, width, upwidth, downwidth, upheight, height;
				mediaPanel->GetSize(&width, NULL);
				scrolledPanel->GetViewStart(&scrolledx, &scrolledy);
				scrolledPanel->GetScrollPixelsPerUnit(&ppux, &ppuy);
				MultiUp.at(0)->GetSize(&upwidth, NULL);
				MultiDown.at(0)->GetSize(&downwidth, NULL);
				MoflexFiles.at(row)->GetSize(NULL, &height);
				
				if(MoflexFiles.size() == 1) {
					if((width - (ppux * 2)) / 3 >= *widestText) {
						MoflexFiles.at(row)->SetSize((width - (ppux * 2)) / 3, height);
					}
					else {
						MoflexFiles.at(row)->SetSize(*widestText, height);
					}
				}
				else {
					if((width - (upwidth + 3 + downwidth + (ppux * 2))) / 3 >= *widestText) {
						MoflexFiles.at(row)->SetSize((width - (upwidth + 3 + downwidth + (ppux * 2))) / 3, height);
					}
					else {
						MoflexFiles.at(row)->SetSize(*widestText, height);
					}
				}
				
				MoflexFiles.at(row)->GetSize(&width, NULL);
				
				//row->Move((width) - (scrolledx * ppux), (height * currentrow) - (scrolledy * ppuy));
				if(row > 0) {
					MoflexFiles.at(row - 1)->GetPosition(NULL, &previousy);
					if(row == parameters.splitPos) {
						splitPatchUp->GetSize(NULL, &upheight);
						MoflexFiles.at(row)->Move((width) - (scrolledx * ppux), (previousy + height + upheight));
					}
					else {
						MoflexFiles.at(row)->Move((width) - (scrolledx * ppux), (previousy + height));
					}
				}
				else {
					MoflexFiles.at(row)->Move((width) - (scrolledx * ppux), (height * row) - (scrolledy * ppuy));
				}
			}
		}
		{ // MenuBanners
			for(size_t row = 0; row < MenuBanners.size(); row++) {
				int scrolledx, scrolledy, ppux, ppuy, previousy, width, upwidth, downwidth, upheight, height;
				mediaPanel->GetSize(&width, NULL);
				scrolledPanel->GetViewStart(&scrolledx, &scrolledy);
				scrolledPanel->GetScrollPixelsPerUnit(&ppux, &ppuy);
				MultiUp.at(0)->GetSize(&upwidth, NULL);
				MultiDown.at(0)->GetSize(&downwidth, NULL);
				MenuBanners.at(row)->GetSize(NULL, &height);
				
				if(MenuBanners.size() == 1) {
					if((width - (ppux * 2)) / 3 >= *widestText) {
						MenuBanners.at(row)->SetSize((width - (ppux * 2)) / 3, height);
					}
					else {
						MenuBanners.at(row)->SetSize(*widestText, height);
					}
				}
				else {
					if((width - (upwidth + 3 + downwidth + (ppux * 2))) / 3 >= *widestText) {
						MenuBanners.at(row)->SetSize((width - (upwidth + 3 + downwidth + (ppux * 2))) / 3, height);
					}
					else {
						MenuBanners.at(row)->SetSize(*widestText, height);
					}
				}
				
				MenuBanners.at(row)->GetSize(&width, NULL);
				
				if(row > 0) {
					MenuBanners.at(row - 1)->GetPosition(NULL, &previousy);
					if(row == parameters.splitPos) {
						splitPatchUp->GetSize(NULL, &upheight);
						MenuBanners.at(row)->Move((width * 2) - (scrolledx * ppux), (previousy + height + upheight));
					}
					else {
						MenuBanners.at(row)->Move((width * 2) - (scrolledx * ppux), (previousy + height));
					}
				}
				else {
					MenuBanners.at(row)->Move((width * 2) - (scrolledx * ppux), (height * row) - (scrolledy * ppuy));
				}
			}
		}
	}
	{ // playerTitleText x
		int x, boxx, y, width, mywidth;
		mediaPanel->GetPosition(&x, NULL);
		PlayerTitles.at(0)->GetSize(&width, NULL);
		PlayerTitles.at(0)->GetPosition(&boxx, NULL);
		playerTitleText->GetSize(&mywidth, NULL);
		playerTitleText->GetPosition(NULL, &y);
		
		playerTitleText->Move(x + boxx + ((width - mywidth) / 2), y);
	}
	{ // moflexFileText x
		int x, boxx, y, width, mywidth;
		mediaPanel->GetPosition(&x, NULL);
		MoflexFiles.at(0)->GetSize(&width, NULL);
		MoflexFiles.at(0)->GetPosition(&boxx, NULL);
		moflexFileText->GetSize(&mywidth, NULL);
		moflexFileText->GetPosition(NULL, &y);
		
		moflexFileText->Move(x + boxx + ((width - mywidth) / 2), y);
	}
	{ // menuBannerText x
		int x, boxx, y, width, mywidth;
		mediaPanel->GetPosition(&x, NULL);
		MenuBanners.at(0)->GetSize(&width, NULL);
		MenuBanners.at(0)->GetPosition(&boxx, NULL);
		menuBannerText->GetSize(&mywidth, NULL);
		menuBannerText->GetPosition(NULL, &y);
		
		menuBannerText->Move(x + boxx + ((width - mywidth) / 2), y);
	}
	{ // MultiUp
		int x, y, width, height, myheight;
		for(size_t i = 0; i < MenuBanners.size(); i++) {
			MenuBanners.at(i)->GetSize(&width, &height);
			MenuBanners.at(i)->GetPosition(&x, &y);
			MultiUp.at(i)->GetSize(NULL, &myheight);
			
			MultiUp.at(i)->Move(x + width, y + ((height - myheight) / 2));
		}
	}
	{ // MultiDown
		int x, y, width;
		for(size_t i = 0; i < MultiUp.size(); i++) {
			MultiUp.at(i)->GetSize(&width, NULL);
			MultiUp.at(i)->GetPosition(&x, &y);
			
			MultiDown.at(i)->Move(x + width + 3, y);
		}
	}
	{ // moflexBrowse
		int x, y, width, mywidth, height;
		MoflexFiles.back()->GetSize(&width, &height);
		MoflexFiles.back()->GetPosition(&x, &y);
		moflexBrowse->GetSize(&mywidth, NULL);
		
		moflexBrowse->Move(x + ((width - mywidth) / 2), y + height);
	}
	{ // multiBannerBrowse
		int x, y, width, mywidth, height;
		MenuBanners.back()->GetSize(&width, &height);
		MenuBanners.back()->GetPosition(&x, &y);
		multiBannerBrowse->GetSize(&mywidth, NULL);
		
		multiBannerBrowse->Move(x + ((width - mywidth) / 2), y + height);
	}
	{ // removeRow
		int y, width, mywidth, appendrowwidth, height;
		multiBannerBrowse->GetSize(NULL, &height);
		multiBannerBrowse->GetPosition(NULL, &y);
		mediaPanel->GetSize(&width, NULL);
		removeRow->GetSize(&mywidth, NULL);
		appendRow->GetSize(&appendrowwidth, NULL);
		
		removeRow->Move(((width - (mywidth + 2 + appendrowwidth)) / 2), y + height + 2);
	}
	{ // appendRow
		int x, y, width;
		removeRow->GetSize(&width, NULL);
		removeRow->GetPosition(&x, &y);
		
		appendRow->Move(x + width + 2, y);
	}
	{ // splitPatchButton
		int x, y, width, mywidth, appendrowwidth, height;
		removeRow->GetSize(&width, &height);
		removeRow->GetPosition(&x, &y);
		appendRow->GetSize(&appendrowwidth, NULL);
		splitPatchButton->GetSize(&mywidth, NULL);
		
		splitPatchButton->Move(x + (((width + 2 + appendrowwidth) - mywidth) / 2), y + height + 2);
	}
	{ // splitPatchDown
		int x, y, width, mywidth, myheight;
		MenuBanners.at(parameters.splitPos)->GetSize(&width, NULL);
		MenuBanners.at(parameters.splitPos)->GetPosition(&x, &y);
		splitPatchDown->GetSize(&mywidth, &myheight);
		
		splitPatchDown->Move((x + width) - mywidth, y - myheight);
	}
	{ // splitPatchUp
		int x, y, mywidth;
		splitPatchDown->GetPosition(&x, &y);
		splitPatchUp->GetSize(&mywidth, NULL);
		
		splitPatchUp->Move(x - 3 - mywidth, y);
	}
	{ // splitPatchLine
		int x, upx, upy, downwidth, upwidth, upheight;
		PlayerTitles.at(parameters.splitPos)->GetPosition(&x, NULL);
		splitPatchUp->GetPosition(&upx, &upy);
		splitPatchUp->GetSize(&upwidth, &upheight);
		splitPatchDown->GetSize(&downwidth, NULL);
		
		splitPatchLine->Move(x + upwidth + 3 + downwidth + 3, upy + ((upheight - 3) / 2));
		splitPatchLine->SetSize(upx - 3 - x - upwidth - 3 - downwidth - 3, 3);
	}
	{ // rowText
		int x, y, width, mywidth, height;
		splitPatchButton->GetSize(&width, &height);
		splitPatchButton->GetPosition(&x, &y);
		rowText->GetSize(&mywidth, NULL);
		
		rowText->Move(x + ((width - mywidth) / 2), y + height + 2);
	}
	{ // scrolledPanel
		int width, boxwidth, upwidth, height, boxheight, browseheight, removeheight, splitpatchheight, splitupheight, rowtextheight; // todo: splitpatch line
		mediaPanel->GetSize(&width, &height);
		PlayerTitles.at(0)->GetSize(&boxwidth, &boxheight);
		MultiUp.at(0)->GetSize(&upwidth, NULL); // up and down are the same size so just use this (or are they...?)
		moflexBrowse->GetSize(NULL, &browseheight);
		removeRow->GetSize(NULL, &removeheight); // - and + are the same size
		splitPatchButton->GetSize(NULL, &splitpatchheight);
		splitPatchUp->GetSize(NULL, &splitupheight);
		rowText->GetSize(NULL, &rowtextheight);
		//splitPatchButton->GetSize(NULL, &virtheight);

		scrolledPanel->SetSize(width, height);
		if(PlayerTitles.size() == 1) {
			// size to contain all widgets (dont use coordinates)
			scrolledPanel->SetVirtualSize(boxwidth * 3, boxheight * PlayerTitles.size() + splitupheight + browseheight + 2 + removeheight + 2 + splitpatchheight + 2 + rowtextheight);
		}
		else {
			// size to contain all widgets (dont use coordinates)
			scrolledPanel->SetVirtualSize((boxwidth * 3) + upwidth + 3 + upwidth, boxheight * PlayerTitles.size() + splitupheight + browseheight + 2 + removeheight + 2 + splitpatchheight + 2 + rowtextheight);
		}
	}
	// buildpanel stuff
	{ // titleIDText
		int panelwidth, mywidth, boxwidth, zzwidth, buttwidth;
		buildpanel->GetSize(&panelwidth, NULL);
		titleIDText->GetSize(&mywidth, NULL);
		titleIDBox->GetSize(&boxwidth, NULL);
		zerozero->GetSize(&zzwidth, NULL);
		titleIDButton->GetSize(&buttwidth, NULL);
		
		titleIDText->Move((panelwidth - (mywidth + boxwidth + zzwidth + 3 + buttwidth)) / 2, 15);
	}
	{ // titleIDBox
		int x, y, width, height, myheight;
		titleIDText->GetPosition(&x, &y);
		titleIDText->GetSize(&width, &height);
		titleIDBox->GetSize(NULL, &myheight);
		
		titleIDBox->Move(x + width, ((myheight > height) ? y - ((myheight - height) / 2) : y + ((height - myheight) / 2)));
	}
	{ // zerozero
		int x, y, width;
		titleIDText->GetPosition(NULL, &y);
		titleIDBox->GetPosition(&x, NULL);
		titleIDBox->GetSize(&width, NULL);
		
		zerozero->Move(x + width, y);
	}
	{ // titleIDButton
		int x, y, width, height, myheight;
		zerozero->GetPosition(&x, &y);
		zerozero->GetSize(&width, &height);
		titleIDButton->GetSize(NULL, &myheight);
		
		titleIDButton->Move(x + width + 3, ((myheight > height) ? y - ((myheight - height) / 2) : y + ((height - myheight) / 2)));
	}
	
	{ // applicationTitleText
		int x, y, height;
		titleIDButton->GetSize(NULL, &height); // this is probably the largest thing
		titleIDText->GetPosition(&x, NULL);
		titleIDButton->GetPosition(NULL, &y);
		
		applicationTitleText->Move(x, y + height + 10);
	}
	{ // applicationTitleBox
		int x, y, width, height, myheight;
		applicationTitleText->GetPosition(&x, &y);
		applicationTitleText->GetSize(&width, &height);
		applicationTitleBox->GetSize(NULL, &myheight);
		
		applicationTitleBox->Move(x + width + 5, ((myheight > height) ? y - ((myheight - height) / 2) : y + ((height - myheight) / 2)));
	}
	{ // applicationTitleBox width
		int tidwidth, boxwidth, zzwidth, buttwidth, textwidth, myheight;
		titleIDText->GetSize(&tidwidth, NULL);
		titleIDBox->GetSize(&boxwidth, NULL);
		zerozero->GetSize(&zzwidth, NULL);
		titleIDButton->GetSize(&buttwidth, NULL);
		applicationTitleText->GetSize(&textwidth, NULL);
		applicationTitleBox->GetSize(NULL, &myheight);
		
		applicationTitleBox->SetSize((tidwidth + boxwidth + zzwidth + 3 + buttwidth) - (textwidth + 5), myheight);
	}
	
	{ // productCodeText
		int texty, boxy, panelwidth, mywidth, boxwidth, boxheight, textheight;
		buildpanel->GetSize(&panelwidth, NULL);
		productCodeText->GetSize(&mywidth, NULL);
		productCodeBox->GetSize(&boxwidth, NULL);
		applicationTitleText->GetSize(NULL, &textheight);
		applicationTitleBox->GetSize(NULL, &boxheight);
		applicationTitleText->GetPosition(NULL, &texty);
		applicationTitleBox->GetPosition(NULL, &boxy);
		
		productCodeText->Move((panelwidth - (mywidth + boxwidth)) / 2, (texty + textheight) > (boxy + boxheight) ? texty + textheight + 10 : boxy + boxheight + 10);
	}
	{ // productCodeBox
		int x, y, width, height, myheight;
		productCodeText->GetPosition(&x, &y);
		productCodeText->GetSize(&width, &height);
		productCodeBox->GetSize(NULL, &myheight);
		
		productCodeBox->Move(x + width, ((myheight > height) ? y - ((myheight - height) / 2) : y + ((height - myheight) / 2)));
	}
	
	{ // statusText
		int prodcodey, panelwidth, mywidth, myheight, panelheight, barheight, buildheight, cancelheight, prodcodeheight;
		buildpanel->GetSize(&panelwidth, &panelheight);
		buildBar->GetSize(NULL, &barheight);
		buildButton->GetSize(NULL, &buildheight);
		cancelButton->GetSize(NULL, &cancelheight);
		productCodeText->GetPosition(NULL, &prodcodey);
		productCodeText->GetSize(NULL, &prodcodeheight);
		statusText->GetSize(&mywidth, &myheight);
		if(panelheight > prodcodey + prodcodeheight + 15 + (myheight + 5 + barheight + 10 + buildheight + 2 + cancelheight + 15))
			statusText->Move((panelwidth - mywidth) / 2, panelheight - (myheight + 5 + barheight + 10 + buildheight + 2 + cancelheight + 15));
		else
			statusText->Move((panelwidth - mywidth) / 2, prodcodey + prodcodeheight + 15);
	}
	{ // buildBar
		int y, panelwidth, statusheight;
		statusText->GetPosition(NULL, &y);
		statusText->GetSize(NULL, &statusheight);
		buildpanel->GetSize(&panelwidth, NULL);
		
		buildBar->SetSize(panelwidth, 25);
		buildBar->Move(0, y + statusheight + 5);
	}
	{ // buildButton
		int y, mywidth, panelwidth, barheight;
		buildpanel->GetSize(&panelwidth, NULL);
		buildBar->GetPosition(NULL, &y);
		buildBar->GetSize(NULL, &barheight);
		buildButton->GetSize(&mywidth, NULL);
		
		buildButton->Move((panelwidth - mywidth) / 2, y + barheight + 10);
	}
	{ // cancelButt
		int y, mywidth, panelwidth, buildheight;
		buildpanel->GetSize(&panelwidth, NULL);
		buildButton->GetPosition(NULL, &y);
		buildButton->GetSize(NULL, &buildheight);
		cancelButton->GetSize(&mywidth, NULL);
		
		cancelButton->Move((panelwidth - mywidth) / 2, y + buildheight + 2);
	}
	{ // titleLogo
		int width, panelwidth;
		aboutpanel->GetSize(&panelwidth, NULL);
		titleLogo->GetSize(&width, NULL);
		
		titleLogo->Move((panelwidth - width) / 2, 20);
	}
	{ // byMeText
		int x, y, mywidth, titlewidth, titleheight;
		titleLogo->GetSize(&titlewidth, &titleheight);
		titleLogo->GetPosition(&x, &y);
		byMeText->GetSize(&mywidth, NULL);
		
		byMeText->Move(x + ((titlewidth - mywidth) / 2), y + titleheight);
	}
	{ // gitHubLinker
		int y, mywidth, myheight, bymeheight, panelwidth, panelheight, versionheight;
		aboutpanel->GetSize(&panelwidth, &panelheight);
		byMeText->GetPosition(NULL, &y);
		byMeText->GetSize(NULL, &bymeheight);
		versionText->GetSize(NULL, &versionheight);
		gitHubLinker->GetSize(&mywidth, &myheight);
		
		if(panelheight > y + bymeheight + myheight + versionheight)
			gitHubLinker->Move((panelwidth - mywidth) / 2, y + bymeheight + (((panelheight - (y + bymeheight)) - myheight - versionheight) / 2));
		else
			gitHubLinker->Move((panelwidth - mywidth) / 2, y + bymeheight);
	}
	{ // versionText
		int y, mywidth, myheight, bymeheight, panelwidth, panelheight, linkheight;
		aboutpanel->GetSize(&panelwidth, &panelheight);
		byMeText->GetPosition(NULL, &y);
		byMeText->GetSize(NULL, &bymeheight);
		gitHubLinker->GetSize(NULL, &linkheight);
		aboutpanel->GetSize(&panelwidth, &panelheight);
		versionText->GetSize(&mywidth, &myheight);
		
		if(panelheight > y + bymeheight + linkheight + myheight)
			versionText->Move((panelwidth - mywidth) / 2, panelheight - myheight);
		else
			versionText->Move((panelwidth - mywidth) / 2, y + bymeheight + linkheight);
	}
}

wxColour BackColor::panel;
wxColour BackColor::mainMenu;
wxColour BackColor::modeText;
wxColour BackColor::modeChoiceBox;
wxColour BackColor::bannerText;
wxColour BackColor::bannerBox;
wxColour BackColor::bannerBrowse;
wxColour BackColor::bannerError;
wxColour BackColor::iconText;
wxColour BackColor::iconBox;
wxColour BackColor::iconBrowse;
wxColour BackColor::iconError;
wxColour BackColor::shortnameText;
wxColour BackColor::shortnameBox;
wxColour BackColor::shortnameError;
wxColour BackColor::longnameText;
wxColour BackColor::longnameBox;
wxColour BackColor::longnameError;
wxColour BackColor::publisherText;
wxColour BackColor::publisherBox;
wxColour BackColor::publisherError;
wxColour BackColor::copyBox;
wxColour BackColor::copyCheck;
wxColour BackColor::bannerCustomText;
wxColour BackColor::bannerPreviewText;
wxColour BackColor::iconPreview;
wxColour BackColor::ffRewindCheck;
wxColour BackColor::dimCheck;
wxColour BackColor::multiBannerPreview;
wxColour BackColor::multiBannerPreviewIndex;
wxColour BackColor::multiBannerPreviewLeft;
wxColour BackColor::multiBannerPreviewRight;
wxColour BackColor::playerTitleText;
wxColour BackColor::moflexFileText;
wxColour BackColor::menuBannerText;
wxColour BackColor::mediaPanel;
wxColour BackColor::moflexBrowse;
wxColour BackColor::multiBannerBrowse;
wxColour BackColor::removeRow;
wxColour BackColor::appendRow;
wxColour BackColor::splitPatchButton;
wxColour BackColor::rowText;
wxColour BackColor::splitPatchUp;
wxColour BackColor::splitPatchDown;
wxColour BackColor::buildpanel;
wxColour BackColor::titleIDText;
wxColour BackColor::titleIDBox;
wxColour BackColor::zerozero;
wxColour BackColor::titleIDButton;
wxColour BackColor::applicationTitleText;
wxColour BackColor::applicationTitleBox;
wxColour BackColor::productCodeText;
wxColour BackColor::productCodeBox;
wxColour BackColor::statusText;
//wxColour BackColor::buildBar;
wxColour BackColor::buildButton;
wxColour BackColor::cancelButton;
wxColour BackColor::aboutpanel;
wxColour BackColor::byMeText;
wxColour BackColor::versionText;

wxColour ForeColor::panel;
wxColour ForeColor::mainMenu;
wxColour ForeColor::modeText;
wxColour ForeColor::modeChoiceBox;
wxColour ForeColor::bannerText;
wxColour ForeColor::bannerBox;
wxColour ForeColor::bannerBrowse;
wxColour ForeColor::bannerError;
wxColour ForeColor::iconText;
wxColour ForeColor::iconBox;
wxColour ForeColor::iconBrowse;
wxColour ForeColor::iconError;
wxColour ForeColor::shortnameText;
wxColour ForeColor::shortnameBox;
wxColour ForeColor::shortnameError;
wxColour ForeColor::longnameText;
wxColour ForeColor::longnameBox;
wxColour ForeColor::longnameError;
wxColour ForeColor::publisherText;
wxColour ForeColor::publisherBox;
wxColour ForeColor::publisherError;
wxColour ForeColor::copyBox;
wxColour ForeColor::copyCheck;
wxColour ForeColor::bannerCustomText;
wxColour ForeColor::bannerPreviewText;
wxColour ForeColor::iconPreview;
wxColour ForeColor::ffRewindCheck;
wxColour ForeColor::dimCheck;
wxColour ForeColor::multiBannerPreview;
wxColour ForeColor::multiBannerPreviewIndex;
wxColour ForeColor::multiBannerPreviewLeft;
wxColour ForeColor::multiBannerPreviewRight;
wxColour ForeColor::playerTitleText;
wxColour ForeColor::moflexFileText;
wxColour ForeColor::menuBannerText;
wxColour ForeColor::mediaPanel;
wxColour ForeColor::moflexBrowse;
wxColour ForeColor::multiBannerBrowse;
wxColour ForeColor::removeRow;
wxColour ForeColor::appendRow;
wxColour ForeColor::splitPatchButton;
wxColour ForeColor::rowText;
wxColour ForeColor::splitPatchUp;
wxColour ForeColor::splitPatchDown;
wxColour ForeColor::buildpanel;
wxColour ForeColor::titleIDText;
wxColour ForeColor::titleIDBox;
wxColour ForeColor::zerozero;
wxColour ForeColor::titleIDButton;
wxColour ForeColor::applicationTitleText;
wxColour ForeColor::applicationTitleBox;
wxColour ForeColor::productCodeText;
wxColour ForeColor::productCodeBox;
wxColour ForeColor::statusText;
//wxColour ForeColor::buildBar;
wxColour ForeColor::buildButton;
wxColour ForeColor::cancelButton;
wxColour ForeColor::aboutpanel;
wxColour ForeColor::byMeText;
wxColour ForeColor::versionText;

void theWidgets::getAppearance() {
	BackColor::panel = panel->GetBackgroundColour();
	BackColor::mainMenu = mainMenu->GetBackgroundColour();
	BackColor::modeText = modeText->GetBackgroundColour();
	BackColor::modeChoiceBox = modeChoiceBox->GetBackgroundColour();
	BackColor::bannerText = bannerText->GetBackgroundColour();
	BackColor::bannerBox = bannerBox->GetBackgroundColour();
	BackColor::bannerBrowse = bannerBrowse->GetBackgroundColour();
	BackColor::bannerError = bannerError->GetBackgroundColour();
	BackColor::iconText = iconText->GetBackgroundColour();
	BackColor::iconBox = iconBox->GetBackgroundColour();
	BackColor::iconBrowse = iconBrowse->GetBackgroundColour();
	BackColor::iconError = iconError->GetBackgroundColour();
	BackColor::shortnameText = shortnameText->GetBackgroundColour();
	BackColor::shortnameBox = shortnameBox->GetBackgroundColour();
	BackColor::shortnameError = shortnameError->GetBackgroundColour();
	BackColor::longnameText = longnameText->GetBackgroundColour();
	BackColor::longnameBox = longnameBox->GetBackgroundColour();
	BackColor::longnameError = longnameError->GetBackgroundColour();
	BackColor::publisherText = publisherText->GetBackgroundColour();
	BackColor::publisherBox = publisherBox->GetBackgroundColour();
	BackColor::publisherError = publisherError->GetBackgroundColour();
	BackColor::copyBox = copyBox->GetBackgroundColour();
	BackColor::copyCheck = copyCheck->GetBackgroundColour();
	BackColor::bannerCustomText = bannerCustomText->GetBackgroundColour();
	BackColor::bannerPreviewText = bannerPreviewText->GetBackgroundColour();
	BackColor::iconPreview = iconPreview->GetBackgroundColour();
	BackColor::ffRewindCheck = ffRewindCheck->GetBackgroundColour();
	BackColor::dimCheck = dimCheck->GetBackgroundColour();
	BackColor::multiBannerPreview = multiBannerPreview->GetBackgroundColour();
	BackColor::multiBannerPreviewIndex = multiBannerPreviewIndex->GetBackgroundColour();
	BackColor::multiBannerPreviewLeft = multiBannerPreviewLeft->GetBackgroundColour();
	BackColor::multiBannerPreviewRight = multiBannerPreviewRight->GetBackgroundColour();
	BackColor::playerTitleText = playerTitleText->GetBackgroundColour();
	BackColor::moflexFileText = moflexFileText->GetBackgroundColour();
	BackColor::menuBannerText = menuBannerText->GetBackgroundColour();
	BackColor::mediaPanel = mediaPanel->GetBackgroundColour();
	BackColor::moflexBrowse = moflexBrowse->GetBackgroundColour();
	BackColor::multiBannerBrowse = multiBannerBrowse->GetBackgroundColour();
	BackColor::removeRow = removeRow->GetBackgroundColour();
	BackColor::appendRow = appendRow->GetBackgroundColour();
	BackColor::splitPatchButton = splitPatchButton->GetBackgroundColour();
	BackColor::rowText = rowText->GetBackgroundColour();
	BackColor::splitPatchUp = splitPatchUp->GetBackgroundColour();
	BackColor::splitPatchDown = splitPatchDown->GetBackgroundColour();
	BackColor::buildpanel = buildpanel->GetBackgroundColour();
	BackColor::titleIDText = titleIDText->GetBackgroundColour();
	BackColor::titleIDBox = titleIDBox->GetBackgroundColour();
	BackColor::zerozero = zerozero->GetBackgroundColour();
	BackColor::titleIDButton = titleIDButton->GetBackgroundColour();
	BackColor::applicationTitleText = applicationTitleText->GetBackgroundColour();
	BackColor::applicationTitleBox = applicationTitleBox->GetBackgroundColour();
	BackColor::productCodeText = productCodeText->GetBackgroundColour();
	BackColor::productCodeBox = productCodeBox->GetBackgroundColour();
	BackColor::statusText = statusText->GetBackgroundColour();
	//BackColor::buildBar = buildBar->GetBackgroundColour();
	BackColor::buildButton = buildButton->GetBackgroundColour();
	BackColor::cancelButton = cancelButton->GetBackgroundColour();
	BackColor::aboutpanel = aboutpanel->GetBackgroundColour();
	BackColor::byMeText = byMeText->GetBackgroundColour();
	BackColor::versionText = versionText->GetBackgroundColour();
	
	ForeColor::panel = panel->GetForegroundColour();
	ForeColor::mainMenu = mainMenu->GetForegroundColour();
	ForeColor::modeText = modeText->GetForegroundColour();
	ForeColor::modeChoiceBox = modeChoiceBox->GetForegroundColour();
	ForeColor::bannerText = bannerText->GetForegroundColour();
	ForeColor::bannerBox = bannerBox->GetForegroundColour();
	ForeColor::bannerBrowse = bannerBrowse->GetForegroundColour();
	ForeColor::bannerError = bannerError->GetForegroundColour();
	ForeColor::iconText = iconText->GetForegroundColour();
	ForeColor::iconBox = iconBox->GetForegroundColour();
	ForeColor::iconBrowse = iconBrowse->GetForegroundColour();
	ForeColor::iconError = iconError->GetForegroundColour();
	ForeColor::shortnameText = shortnameText->GetForegroundColour();
	ForeColor::shortnameBox = shortnameBox->GetForegroundColour();
	ForeColor::shortnameError = shortnameError->GetForegroundColour();
	ForeColor::longnameText = longnameText->GetForegroundColour();
	ForeColor::longnameBox = longnameBox->GetForegroundColour();
	ForeColor::longnameError = longnameError->GetForegroundColour();
	ForeColor::publisherText = publisherText->GetForegroundColour();
	ForeColor::publisherBox = publisherBox->GetForegroundColour();
	ForeColor::publisherError = publisherError->GetForegroundColour();
	ForeColor::copyBox = copyBox->GetForegroundColour();
	ForeColor::copyCheck = copyCheck->GetForegroundColour();
	ForeColor::bannerCustomText = bannerCustomText->GetForegroundColour();
	ForeColor::bannerPreviewText = bannerPreviewText->GetForegroundColour();
	ForeColor::iconPreview = iconPreview->GetForegroundColour();
	ForeColor::ffRewindCheck = ffRewindCheck->GetForegroundColour();
	ForeColor::dimCheck = dimCheck->GetForegroundColour();
	ForeColor::multiBannerPreview = multiBannerPreview->GetForegroundColour();
	ForeColor::multiBannerPreviewIndex = multiBannerPreviewIndex->GetForegroundColour();
	ForeColor::multiBannerPreviewLeft = multiBannerPreviewLeft->GetForegroundColour();
	ForeColor::multiBannerPreviewRight = multiBannerPreviewRight->GetForegroundColour();
	ForeColor::playerTitleText = playerTitleText->GetForegroundColour();
	ForeColor::moflexFileText = moflexFileText->GetForegroundColour();
	ForeColor::menuBannerText = menuBannerText->GetForegroundColour();
	ForeColor::mediaPanel = mediaPanel->GetForegroundColour();
	ForeColor::moflexBrowse = moflexBrowse->GetForegroundColour();
	ForeColor::multiBannerBrowse = multiBannerBrowse->GetForegroundColour();
	ForeColor::removeRow = removeRow->GetForegroundColour();
	ForeColor::appendRow = appendRow->GetForegroundColour();
	ForeColor::splitPatchButton = splitPatchButton->GetForegroundColour();
	ForeColor::rowText = rowText->GetForegroundColour();
	ForeColor::splitPatchUp = splitPatchUp->GetForegroundColour();
	ForeColor::splitPatchDown = splitPatchDown->GetForegroundColour();
	ForeColor::buildpanel = buildpanel->GetForegroundColour();
	ForeColor::titleIDText = titleIDText->GetForegroundColour();
	ForeColor::titleIDBox = titleIDBox->GetForegroundColour();
	ForeColor::zerozero = zerozero->GetForegroundColour();
	ForeColor::titleIDButton = titleIDButton->GetForegroundColour();
	ForeColor::applicationTitleText = applicationTitleText->GetForegroundColour();
	ForeColor::applicationTitleBox = applicationTitleBox->GetForegroundColour();
	ForeColor::productCodeText = productCodeText->GetForegroundColour();
	ForeColor::productCodeBox = productCodeBox->GetForegroundColour();
	ForeColor::statusText = statusText->GetForegroundColour();
	//ForeColor::buildBar = buildBar->GetForegroundColour();
	ForeColor::buildButton = buildButton->GetForegroundColour();
	ForeColor::cancelButton = cancelButton->GetForegroundColour();
	ForeColor::aboutpanel = aboutpanel->GetForegroundColour();
	ForeColor::byMeText = byMeText->GetForegroundColour();
	ForeColor::versionText = versionText->GetForegroundColour();
}

void theWidgets::setAppearance(unsigned int Mode) {
	if(Mode < 2) { // 0 light 1 black
		panel->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		mainMenu->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		modeText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		modeChoiceBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		bannerText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		bannerBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		bannerBrowse->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		bannerError->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		iconText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		iconBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		iconBrowse->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		iconError->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		shortnameText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		shortnameBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		shortnameError->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		longnameText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		longnameBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		longnameError->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		publisherText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		publisherBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		publisherError->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		copyBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		copyCheck->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		bannerCustomText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		bannerPreviewText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		iconPreview->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		ffRewindCheck->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		dimCheck->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		multiBannerPreview->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		multiBannerPreviewIndex->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		multiBannerPreviewLeft->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		multiBannerPreviewRight->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		playerTitleText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		moflexFileText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		menuBannerText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		mediaPanel->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		moflexBrowse->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		multiBannerBrowse->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		removeRow->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		appendRow->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		splitPatchButton->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		rowText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		splitPatchUp->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		splitPatchDown->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		buildpanel->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		titleIDText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		titleIDBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		zerozero->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		titleIDButton->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		applicationTitleText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		applicationTitleBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		productCodeText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		productCodeBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		statusText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		//buildBar->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		buildButton->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		cancelButton->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		aboutpanel->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		byMeText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		versionText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		
		splitPatchLine->SetColour(*(Mode ? wxWHITE : wxBLACK));
		panel->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		mainMenu->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		modeText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		modeChoiceBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		bannerText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		bannerBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		bannerBrowse->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		bannerError->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		iconText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		iconBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		iconBrowse->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		iconError->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		shortnameText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		shortnameBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		shortnameError->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		longnameText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		longnameBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		longnameError->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		publisherText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		publisherBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		publisherError->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		copyBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		copyCheck->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		bannerCustomText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		bannerPreviewText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		iconPreview->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		ffRewindCheck->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		dimCheck->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		multiBannerPreview->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		multiBannerPreviewIndex->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		multiBannerPreviewLeft->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		multiBannerPreviewRight->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		playerTitleText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		moflexFileText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		menuBannerText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		mediaPanel->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		moflexBrowse->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		multiBannerBrowse->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		removeRow->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		appendRow->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		splitPatchButton->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		rowText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		splitPatchUp->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		splitPatchDown->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		buildpanel->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		titleIDText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		titleIDBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		zerozero->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		titleIDButton->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		applicationTitleText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		applicationTitleBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		productCodeText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		productCodeBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		statusText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		//buildBar->SetForegroundColour(*(Mode ? wxBLACK : wxWHITE)); // uhhhhhhh
		buildButton->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		cancelButton->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		aboutpanel->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		byMeText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		versionText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		
		{
			wxColor LightBlack = wxBLACK->GetRGB() + 0x141414;
			wxColor DarkWhite = wxWHITE->GetRGB() - 0x141414;
			
			int rows = 0;
			for(auto &row : PlayerTitles) {
				if(rows % 2) {
					row->SetBackgroundColour(Mode ? LightBlack : DarkWhite);
				}
				else {
					row->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
				}
				row->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
				++rows;
			}
			rows = 0;
			for(auto &row : MoflexFiles) {
				if(rows % 2) {
					row->SetBackgroundColour(Mode ? LightBlack : DarkWhite);
				}
				else {
					row->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
				}
				row->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
				++rows;
			}
			rows = 0;
			for(auto &row : MenuBanners) {
				if(rows % 2) {
					row->SetBackgroundColour(Mode ? LightBlack : DarkWhite);
				}
				else {
					row->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
				}
				row->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
				++rows;
			}
		}
		
		for(auto &row : MultiUp) {
			row->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
			row->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		}
		for(auto &row : MultiDown) {
			row->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
			row->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		}
	}
	else if(Mode == 2) {
		panel->SetBackgroundColour(BackColor::panel);
		mainMenu->SetBackgroundColour(BackColor::mainMenu);
		modeText->SetBackgroundColour(BackColor::modeText);
		modeChoiceBox->SetBackgroundColour(BackColor::modeChoiceBox);
		bannerText->SetBackgroundColour(BackColor::bannerText);
		bannerBox->SetBackgroundColour(BackColor::bannerBox);
		bannerBrowse->SetBackgroundColour(BackColor::bannerBrowse);
		bannerError->SetBackgroundColour(BackColor::bannerError);
		iconText->SetBackgroundColour(BackColor::iconText);
		iconBox->SetBackgroundColour(BackColor::iconBox);
		iconBrowse->SetBackgroundColour(BackColor::iconBrowse);
		iconError->SetBackgroundColour(BackColor::iconError);
		shortnameText->SetBackgroundColour(BackColor::shortnameText);
		shortnameBox->SetBackgroundColour(BackColor::shortnameBox);
		shortnameError->SetBackgroundColour(BackColor::shortnameError);
		longnameText->SetBackgroundColour(BackColor::longnameText);
		longnameBox->SetBackgroundColour(BackColor::longnameBox);
		longnameError->SetBackgroundColour(BackColor::longnameError);
		publisherText->SetBackgroundColour(BackColor::publisherText);
		publisherBox->SetBackgroundColour(BackColor::publisherBox);
		publisherError->SetBackgroundColour(BackColor::publisherError);
		copyBox->SetBackgroundColour(BackColor::copyBox);
		copyCheck->SetBackgroundColour(BackColor::copyCheck);
		bannerCustomText->SetBackgroundColour(BackColor::bannerCustomText);
		bannerPreviewText->SetBackgroundColour(BackColor::bannerPreviewText);
		iconPreview->SetBackgroundColour(BackColor::iconPreview);
		ffRewindCheck->SetBackgroundColour(BackColor::ffRewindCheck);
		dimCheck->SetBackgroundColour(BackColor::dimCheck);
		multiBannerPreview->SetBackgroundColour(BackColor::multiBannerPreview);
		multiBannerPreviewIndex->SetBackgroundColour(BackColor::multiBannerPreviewIndex);
		multiBannerPreviewLeft->SetBackgroundColour(BackColor::multiBannerPreviewLeft);
		multiBannerPreviewRight->SetBackgroundColour(BackColor::multiBannerPreviewRight);
		playerTitleText->SetBackgroundColour(BackColor::playerTitleText);
		moflexFileText->SetBackgroundColour(BackColor::moflexFileText);
		menuBannerText->SetBackgroundColour(BackColor::menuBannerText);
		mediaPanel->SetBackgroundColour(BackColor::mediaPanel);
		moflexBrowse->SetBackgroundColour(BackColor::moflexBrowse);
		multiBannerBrowse->SetBackgroundColour(BackColor::multiBannerBrowse);
		removeRow->SetBackgroundColour(BackColor::removeRow);
		appendRow->SetBackgroundColour(BackColor::appendRow);
		splitPatchButton->SetBackgroundColour(BackColor::splitPatchButton);
		rowText->SetBackgroundColour(BackColor::rowText);
		splitPatchUp->SetBackgroundColour(BackColor::splitPatchUp);
		splitPatchDown->SetBackgroundColour(BackColor::splitPatchDown);
		buildpanel->SetBackgroundColour(BackColor::buildpanel);
		titleIDText->SetBackgroundColour(BackColor::titleIDText);
		titleIDBox->SetBackgroundColour(BackColor::titleIDBox);
		zerozero->SetBackgroundColour(BackColor::zerozero);
		titleIDButton->SetBackgroundColour(BackColor::titleIDButton);
		applicationTitleText->SetBackgroundColour(BackColor::applicationTitleText);
		applicationTitleBox->SetBackgroundColour(BackColor::applicationTitleBox);
		productCodeText->SetBackgroundColour(BackColor::productCodeText);
		productCodeBox->SetBackgroundColour(BackColor::productCodeBox);
		statusText->SetBackgroundColour(BackColor::statusText);
		//buildBar->SetBackgroundColour(BackColor::buildBar);
		buildButton->SetBackgroundColour(BackColor::buildButton);
		cancelButton->SetBackgroundColour(BackColor::cancelButton);
		aboutpanel->SetBackgroundColour(BackColor::aboutpanel);
		byMeText->SetBackgroundColour(BackColor::byMeText);
		versionText->SetBackgroundColour(BackColor::versionText);
		
		panel->SetForegroundColour(ForeColor::panel);
		mainMenu->SetForegroundColour(ForeColor::mainMenu);
		modeText->SetForegroundColour(ForeColor::modeText);
		modeChoiceBox->SetForegroundColour(ForeColor::modeChoiceBox);
		bannerText->SetForegroundColour(ForeColor::bannerText);
		bannerBox->SetForegroundColour(ForeColor::bannerBox);
		bannerBrowse->SetForegroundColour(ForeColor::bannerBrowse);
		bannerError->SetForegroundColour(ForeColor::bannerError);
		iconText->SetForegroundColour(ForeColor::iconText);
		iconBox->SetForegroundColour(ForeColor::iconBox);
		iconBrowse->SetForegroundColour(ForeColor::iconBrowse);
		iconError->SetForegroundColour(ForeColor::iconError);
		shortnameText->SetForegroundColour(ForeColor::shortnameText);
		shortnameBox->SetForegroundColour(ForeColor::shortnameBox);
		shortnameError->SetForegroundColour(ForeColor::shortnameError);
		longnameText->SetForegroundColour(ForeColor::longnameText);
		longnameBox->SetForegroundColour(ForeColor::longnameBox);
		longnameError->SetForegroundColour(ForeColor::longnameError);
		publisherText->SetForegroundColour(ForeColor::publisherText);
		publisherBox->SetForegroundColour(ForeColor::publisherBox);
		publisherError->SetForegroundColour(ForeColor::publisherError);
		copyBox->SetForegroundColour(ForeColor::copyBox);
		copyCheck->SetForegroundColour(ForeColor::copyCheck);
		bannerCustomText->SetForegroundColour(ForeColor::bannerCustomText);
		bannerPreviewText->SetForegroundColour(ForeColor::bannerPreviewText);
		iconPreview->SetForegroundColour(ForeColor::iconPreview);
		ffRewindCheck->SetForegroundColour(ForeColor::ffRewindCheck);
		dimCheck->SetForegroundColour(ForeColor::dimCheck);
		multiBannerPreview->SetForegroundColour(ForeColor::multiBannerPreview);
		multiBannerPreviewIndex->SetForegroundColour(ForeColor::multiBannerPreviewIndex);
		multiBannerPreviewLeft->SetForegroundColour(ForeColor::multiBannerPreviewLeft);
		multiBannerPreviewRight->SetForegroundColour(ForeColor::multiBannerPreviewRight);
		playerTitleText->SetForegroundColour(ForeColor::playerTitleText);
		moflexFileText->SetForegroundColour(ForeColor::moflexFileText);
		menuBannerText->SetForegroundColour(ForeColor::menuBannerText);
		mediaPanel->SetForegroundColour(ForeColor::mediaPanel);
		moflexBrowse->SetForegroundColour(ForeColor::moflexBrowse);
		multiBannerBrowse->SetForegroundColour(ForeColor::multiBannerBrowse);
		removeRow->SetForegroundColour(ForeColor::removeRow);
		appendRow->SetForegroundColour(ForeColor::appendRow);
		splitPatchButton->SetForegroundColour(ForeColor::splitPatchButton);
		rowText->SetForegroundColour(ForeColor::rowText);
		splitPatchLine->SetColour(ForeColor::panel);
		splitPatchUp->SetForegroundColour(ForeColor::splitPatchUp);
		splitPatchDown->SetForegroundColour(ForeColor::splitPatchDown);
		buildpanel->SetForegroundColour(ForeColor::buildpanel);
		titleIDText->SetForegroundColour(ForeColor::titleIDText);
		titleIDBox->SetForegroundColour(ForeColor::titleIDBox);
		zerozero->SetForegroundColour(ForeColor::zerozero);
		titleIDButton->SetForegroundColour(ForeColor::titleIDButton);
		applicationTitleText->SetForegroundColour(ForeColor::applicationTitleText);
		applicationTitleBox->SetForegroundColour(ForeColor::applicationTitleBox);
		productCodeText->SetForegroundColour(ForeColor::productCodeText);
		productCodeBox->SetForegroundColour(ForeColor::productCodeBox);
		statusText->SetForegroundColour(ForeColor::statusText);
		//buildBar->SetForegroundColour(ForeColor::buildBar);
		buildButton->SetForegroundColour(ForeColor::buildButton);
		cancelButton->SetForegroundColour(ForeColor::cancelButton);
		aboutpanel->SetForegroundColour(ForeColor::aboutpanel);
		byMeText->SetForegroundColour(ForeColor::byMeText);
		versionText->SetForegroundColour(ForeColor::versionText);
		
		{
			wxColor BackOdd = (BackColor::bannerBox.GetRGB() < 0x7F7F7F) ? (BackColor::bannerBox.GetRGB() + 0x141414) : (BackColor::bannerBox.GetRGB() - 0x141414);//FF/2=7F
			//wxColor ForeOdd = (ForeColor::bannerBox.GetRGB() < 0x7F7F7F) ? (ForeColor::bannerBox.GetRGB() + 0x141414) : (ForeColor::bannerBox.GetRGB() - 0x141414);
			
			int rows = 0;
			for(auto &row : PlayerTitles) {
				if(rows % 2) {
					row->SetBackgroundColour(BackOdd);
				}
				else {
					row->SetBackgroundColour(BackColor::bannerBox); // not worth getting the color from these actual boxes
				}
				row->SetForegroundColour(ForeColor::bannerBox);
				++rows;
			}
			rows = 0;
			for(auto &row : MoflexFiles) {
				if(rows % 2) {
					row->SetBackgroundColour(BackOdd);
				}
				else {
					row->SetBackgroundColour(BackColor::bannerBox);
				}
				row->SetForegroundColour(ForeColor::bannerBox);
				++rows;
			}
			rows = 0;
			for(auto &row : MenuBanners) {
				if(rows % 2) {
					row->SetBackgroundColour(BackOdd);
				}
				else {
					row->SetBackgroundColour(BackColor::bannerBox);
				}
				row->SetForegroundColour(ForeColor::bannerBox);
				++rows;
			}
		}
		
		for(auto &row : MultiUp) {
			row->SetBackgroundColour(BackColor::splitPatchUp);
			row->SetForegroundColour(ForeColor::splitPatchUp);
		}
		for(auto &row : MultiDown) {
			row->SetBackgroundColour(BackColor::splitPatchDown);
			row->SetForegroundColour(ForeColor::splitPatchDown);
		}
	}
	{ // titleLogo
		int width, height;
		wxImage top;
		wxImage bottom;
		
		
		top.LoadFile(wxString::FromUTF8(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + imagePath + "/title_top.png"));
		bottom.LoadFile(wxString::FromUTF8(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + imagePath + "/title_bottom.png"));
		
		width = top.GetWidth();
		height = top.GetHeight();
		// maybe check bottom size? oh well they are supposed to be the same
		
		wxImage back(width, height); // LOOK LOOK IM RIGHT HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!
		
		wxBrush MY_BRUSH(((Settings::ColorMode < 2) ? *(Settings::ColorMode ? wxBLACK : wxWHITE) : aboutpanel->GetBackgroundColour()));
		//wxBrush MY_BRUSH(BackColor::aboutpanel); // what why not?
		
		
		// https://stackoverflow.com/a/35179780
		wxBitmap bitmap(back);
		wxMemoryDC memdc(bitmap);
		memdc.SetBackground(MY_BRUSH);
		memdc.Clear(); // fills the entire bitmap with green colour
		memdc.SelectObject(wxNullBitmap);
		back = bitmap.ConvertToImage(); // optionally
		back.InitAlpha();
		
		for(int y = 0; y < height; y++) {
			for(int x = 0; x < width; x++) {
				uint8_t r1 = bottom.GetRed(x, y);
				uint8_t g1 = bottom.GetGreen(x, y);
				uint8_t b1 = bottom.GetBlue(x, y);
				uint8_t a1 = bottom.GetAlpha(x, y);
				
				uint8_t r2 = back.GetRed(x, y);
				uint8_t g2 = back.GetGreen(x, y);
				uint8_t b2 = back.GetBlue(x, y);
				
				back.SetRGB(x, y, r1 - r2, g1 - g2, b1 - b2); // difference blend
				back.SetAlpha(x, y, a1);
			}
		}
		
		back.Paste(top, 0, 0, wxIMAGE_ALPHA_BLEND_COMPOSE);
		
		titleLogo->SetSize(width, height);
		titleLogo->SetBitmap(wxBitmap(back));
	}
	panel->Refresh();
	buildpanel->Refresh();
	aboutpanel->Refresh();
}

int theWidgets::loadParameters() {
	VI9P::Loading = true;
	int ret = 0;
	{ // -rr
		//wxMessageBox("holup");
		ret = executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -rr \"" + VI9P::WorkingFile + '\"'));
		
		if(ret >= 0) {
			parameters.rows = ret & 0xFF;
		}
		else {
			return ret;
		}
	}
	{ // -pp
		wxArrayString output;
		std::string pp;
		
		ret = executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -pp \"" + VI9P::WorkingFile + '\"'), &output);
		
		for (auto &s : output) {
			pp += std::string(s.ToUTF8()) + '\n';
		}
		
		{//mode
			std::string value = "";
			int outnum = 0;
			parsePP(pp.c_str(), IntMultiParam, &value);
			if(!ASCII2number<int>(&outnum, value)) {
				// tbh this should never ever happen
				parameters.mode = 0;
			}
			else {
				parameters.mode = ((outnum <= 2) ? outnum : 0);
			}
		}
		{//banner
			std::string value = "";
			parsePP(pp.c_str(), StrBannerParam, &value);
			parameters.banner = value;
		}
		{//icon
			std::string value = "";
			parsePP(pp.c_str(), StrIconParam, &value);
			parameters.icon = value;
		}
		{//iconBorder
			std::string value = "";
			int outnum = 0;
			parsePP(pp.c_str(), IntIconBorderParam, &value);
			if(!ASCII2number<int>(&outnum, value)) {
				//tbh this should never ever happen
				parameters.iconBorder = 2;
			}
			else {
				parameters.iconBorder = ((outnum > 2) ? 0 : outnum);
			}
		}
		{//Sname
			std::string value = "";
			parsePP(pp.c_str(), StrSNameParam, &value);
			parameters.Sname = value;
		}
		{//Lname
			std::string value = "";
			parsePP(pp.c_str(), StrLNameParam, &value);
			parameters.Lname = value;
		}
		{//publisher
			std::string value = "";
			parsePP(pp.c_str(), StrPublisherParam, &value);
			parameters.publisher = value;
		}
		{//copycheck
			std::string value = "";
			int outnum = 0;
			parsePP(pp.c_str(), IntCopycheckParam, &value);
			if(!ASCII2number<int>(&outnum, value)) {
				//tbh this should never ever happen
				parameters.copycheck = 0;
			}
			else {
				parameters.copycheck = (outnum ? 1 : 0);
			}
		}
		{//copyrightInfo
			std::string value = "";
			parsePP(pp.c_str(), StrCopyrightParam, &value);
			parameters.copyrightInfo = value;
		}
		{//FFrewind
			std::string value = "";
			int outnum = 0;
			parsePP(pp.c_str(), IntFFrewindParam, &value);
			if(!ASCII2number<int>(&outnum, value)) {
				//tbh this should never ever happen
				parameters.FFrewind = 1;
			}
			else {
				parameters.FFrewind = (outnum ? 1 : 0);
			}
		}
		{//FadeOpt
			std::string value = "";
			int outnum = 0;
			parsePP(pp.c_str(), IntFadeOptParam, &value);
			if(!ASCII2number<int>(&outnum, value)) {
				//tbh this should never ever happen
				parameters.FadeOpt = 1;
			}
			else {
				parameters.FadeOpt = (outnum ? 1 : 0);
			}
		}
		{//splitPos
			std::string value = "";
			int outnum = 0;
			parsePP(pp.c_str(), IntSplitPatchParam, &value);
			if(!ASCII2number<int>(&outnum, value)) {
				//tbh this should never ever happen
				parameters.splitPos = 0;
			}
			else {
				parameters.splitPos = ((outnum < parameters.rows - 1) ? outnum : parameters.rows - 1);
			}
		}
		{//PTitleVec
			parameters.PTitleVec.clear();
			for(uint8_t i = 0; i < parameters.rows; i++) {
				std::string value = "";
				parsePP(pp.c_str(), StrPTitleParam + '(' + std::to_string(i) + ')', &value);
				parameters.PTitleVec.push_back(value);
			}
		}
		{//StrMoflexParam
			parameters.MoflexVec.clear();
			for(uint8_t i = 0; i < parameters.rows; i++) {
				std::string value = "";
				parsePP(pp.c_str(), StrMoflexParam + '(' + std::to_string(i) + ')', &value);
				parameters.MoflexVec.push_back(value);
			}
		}
		{//StrMBannerParam
			parameters.MBannerVec.clear();
			for(uint8_t i = 0; i < parameters.rows; i++) {
				std::string value = "";
				parsePP(pp.c_str(), StrMBannerParam + '(' + std::to_string(i) + ')', &value);
				parameters.MBannerVec.push_back(value);
			}
		}
	}
	applyParameters();
	VI9P::Loading = false;
	return ret;
}

void theWidgets::applyMode() {
	if(parameters.mode == 0) {
		copyBox->Enable(false);
		copyCheck->Enable(false);
		
		ffRewindCheck->Enable(true);
		dimCheck->Enable(true);
		
		rowText->Show(false);
		multiBannerPreview->Enable(false);
		multiBannerPreviewIndex->Show(false);
		multiBannerPreviewLeft->Enable(false);
		multiBannerPreviewRight->Enable(false);
		if(parameters.rows > 1) {
			removeRow->Enable(true);
			splitPatchButton->Enable(true);
		}
		else {
			removeRow->Enable(false);
			splitPatchButton->Enable(false);
		}
		appendRow->Enable(false);
		bool first = true; // lol
		for(const auto &row : PlayerTitles) {
			if(!first) {
				row->Enable(false);
			}
			first = false;
		}
		first = true;
		for(const auto &row : MoflexFiles) {
			if(!first) {
				row->Enable(false);
			}
			first = false;
		}
		for(const auto &row : MenuBanners) {
			row->Enable(false);
		}
		multiBannerBrowse->Enable(false);
	}
	else if(parameters.mode == 1) {
		copyBox->Enable(copyCheck->GetValue());
		copyCheck->Enable(true);
		
		ffRewindCheck->Enable(true);
		dimCheck->Enable(true);
		
		rowText->Show(true);
		multiBannerPreview->Enable(true);
		multiBannerPreviewIndex->Show(true);
		
		EnableBannerLeftRight();
		splitPatchButton->Enable(true);
		
		size_t count = 0;
		for(const auto &row : PlayerTitles) {
			if(count < MAX_ROWS_MULTI) {
				row->Enable(true);
			}
			else {
				row->Enable(false);
			}
			++count;
		}
		count = 0;
		for(const auto &row : MoflexFiles) {
			row->Enable(true);
			if(count < MAX_ROWS_MULTI) {
				row->Enable(true);
			}
			else {
				row->Enable(false);
			}
			++count;
		}
		count = 0;
		for(const auto &row : MenuBanners) {
			row->Enable(true);
			if(count < MAX_ROWS_MULTI) {
				row->Enable(true);
			}
			else {
				row->Enable(false);
			}
			++count;
		}
		
		if(parameters.rows > 1 && parameters.rows < MAX_ROWS_MULTI) {
			appendRow->Enable(true);
			removeRow->Enable(true);
		}
		if(parameters.rows <= 1) {
			appendRow->Enable(true);
			removeRow->Enable(false);
		}
		else if(parameters.rows >= MAX_ROWS_MULTI) {
			appendRow->Enable(false);
			removeRow->Enable(true);
		}
		setRowIndex();
		
		multiBannerBrowse->Enable(true);
	}
	else if(parameters.mode == 2) {
		copyBox->Enable(false);
		copyCheck->Enable(false);
		
		ffRewindCheck->Enable(false);
		dimCheck->Enable(false);
		
		rowText->Show(true);
		multiBannerPreview->Enable(true);
		multiBannerPreviewIndex->Show(false);
		multiBannerPreviewLeft->Enable(false);
		multiBannerPreviewRight->Enable(false);
		
		splitPatchButton->Enable(false);
		
		for(const auto &row : PlayerTitles) {
			row->Enable(true);
		}
		for(const auto &row : MoflexFiles) {
			row->Enable(true);
		}
		bool first = true; // lol
		for(const auto &row : MenuBanners) {
			if(first) {
				row->Enable(true);
			}
			else {
				row->Enable(false);
			}
			first = false;
		}
		
		if(parameters.rows > 1 && parameters.rows < MAX_ROWS) {
			appendRow->Enable(true);
			removeRow->Enable(true);
		}
		if(parameters.rows <= 1) {
			appendRow->Enable(true);
			removeRow->Enable(false);
		}
		else if(parameters.rows >= MAX_ROWS) {
			appendRow->Enable(false);
			removeRow->Enable(true);
		}
		setRowIndex();
		
		multiBannerBrowse->Enable(true);
	}
	
	EnableBannerLeftRight();
	setToolTips();
	setCursors();
}

void theWidgets::applyParameters() {
	modeChoiceBox->SetSelection(parameters.mode);

	bannerBox->SetValue(wxString::FromUTF8(parameters.banner));
	bannerBox->SetFullPath(bannerBox->GetValue());
	bannerBox->DisplayShortPath();

	iconBox->SetValue(wxString::FromUTF8(parameters.icon));
	iconBox->SetFullPath(iconBox->GetValue());
	iconBox->DisplayShortPath();

	shortnameBox->SetValue(wxString::FromUTF8(parameters.Sname));
	longnameBox->SetValue(wxString::FromUTF8(parameters.Lname));
	publisherBox->SetValue(wxString::FromUTF8(parameters.publisher));

	copyCheck->SetValue(parameters.copycheck);
	copyBox->SetValue(wxString::FromUTF8(parameters.copyrightInfo));

	ffRewindCheck->SetValue(parameters.FFrewind);
	dimCheck->SetValue(parameters.FadeOpt);

	if(PlayerTitles.size() < parameters.rows) {
		int count = parameters.rows - PlayerTitles.size();
		if(static_cast<int>(PlayerTitles.size() & 0xFF) + count <= MAX_ROWS + 1) { // probably dangerous but i trust these enough
			doAddRows(count);
		}
	}
	else if(PlayerTitles.size() > parameters.rows) {
		int count = PlayerTitles.size() - parameters.rows;
		if(static_cast<int>(PlayerTitles.size() & 0xFF) - count >= 0) {
			for(uint8_t i = 0; i < count; i++) {
				PlayerTitles.back()->Destroy();
				PlayerTitles.pop_back();
			}
			for(uint8_t i = 0; i < count; i++) {
				MoflexFiles.back()->Destroy();
				MoflexFiles.pop_back();
			}
			for(uint8_t i = 0; i < count; i++) {
				MenuBanners.back()->Destroy();
				MenuBanners.pop_back();
			}
			for(uint8_t i = 0; i < count; i++) {
				MultiUp.back()->Destroy();
				MultiUp.pop_back();
			}
			for(uint8_t i = 0; i < count; i++) {
				MultiDown.back()->Destroy();
				MultiDown.pop_back();
			}
		}
	}
	
	for(size_t row = 0; row < parameters.PTitleVec.size(); row++) {
		PlayerTitles.at(row)->SetValue(wxString::FromUTF8(parameters.PTitleVec.at(row)));
	}
	for(size_t row = 0; row < parameters.MoflexVec.size(); row++) {
		MoflexFiles.at(row)->SetValue(wxString::FromUTF8(parameters.MoflexVec.at(row)));
		MoflexFiles.at(row)->SetFullPath(MoflexFiles.at(row)->GetValue());
		MoflexFiles.at(row)->DisplayShortPath();
	}
	for(size_t row = 0; row < parameters.MBannerVec.size(); row++) {
		MenuBanners.at(row)->SetValue(wxString::FromUTF8(parameters.MBannerVec.at(row)));
		MenuBanners.at(row)->SetFullPath(MenuBanners.at(row)->GetValue());
		MenuBanners.at(row)->DisplayShortPath();
	}
	
	splitPatchButton->SetValue(parameters.splitPos ? 1 : 0);
	
	applyMode();
}

void theWidgets::addRows(uint8_t count) {
	if(parameters.rows + count <= MAX_ROWS) {
		for(int i = 0; i < count; i++) {
			{ // -ar
				executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -ar \"" + VI9P::WorkingFile + "\" \"" + VI9P::WorkingFile + '\"'));
			}
		}
	}
	
	{ // -rr
		int ret = executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -rr \"" + VI9P::WorkingFile + '\"'));
		
		if(ret >= 0) {
			parameters.rows = ret & 0xFF;
		}
	}
	{ // -pp		
		wxArrayString output;
		std::string pp;
		
		executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -pp \"" + VI9P::WorkingFile + '\"'), &output);
		
		for (auto &s : output) {
			pp += std::string(s.ToUTF8()) + '\n';
		}
		
		{ // PTitleVec
			parameters.PTitleVec.clear();
			for(uint8_t i = 0; i < parameters.rows; i++) {
				std::string value = "";
				parsePP(pp.c_str(), StrPTitleParam + '(' + std::to_string(i) + ')', &value);
				parameters.PTitleVec.push_back(value);
			}
		}
		{ // StrMoflexParam
			parameters.MoflexVec.clear();
			for(uint8_t i = 0; i < parameters.rows; i++) {
				std::string value = "";
				parsePP(pp.c_str(), StrMoflexParam + '(' + std::to_string(i) + ')', &value);
				parameters.MoflexVec.push_back(value);
			}
		}
		{ // StrMBannerParam
			parameters.MBannerVec.clear();
			for(uint8_t i = 0; i < parameters.rows; i++) {
				std::string value = "";
				parsePP(pp.c_str(), StrMBannerParam + '(' + std::to_string(i) + ')', &value);
				parameters.MBannerVec.push_back(value);
			}
		}
		int count = parameters.rows - PlayerTitles.size();
		if(parameters.rows + count <= MAX_ROWS + 1) { // rows + (rows - size)
			doAddRows(count);
		}
	}
	positionWidgets();
}

void theWidgets::removeRows(uint8_t count) {
	if(parameters.rows - count >= 1) {
		for(int i = 0; i < count; i++) {
			{ // -sr
				executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sr \"" + VI9P::WorkingFile + "\" \"" + VI9P::WorkingFile + '\"'));
			}
		}
	}
	//loadParameters();
	{ // -rr
		int ret = executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -rr \"" + VI9P::WorkingFile + '\"'));
		
		if(ret >= 0) {
			parameters.rows = ret & 0xFF;
		}
	}
	{ // -pp
		wxArrayString output;
		std::string pp;
		
		executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -pp \"" + VI9P::WorkingFile + '\"'), &output);
		
		for (auto &s : output) {
			pp += std::string(s.ToUTF8()) + '\n';
		}
		
		{ // PTitleVec
			parameters.PTitleVec.clear();
			for(uint8_t i = 0; i < parameters.rows; i++) {
				std::string value = "";
				parsePP(pp.c_str(), StrPTitleParam + '(' + std::to_string(i) + ')', &value);
				parameters.PTitleVec.push_back(value);
			}
		}
		{ // StrMoflexParam
			parameters.MoflexVec.clear();
			for(uint8_t i = 0; i < parameters.rows; i++) {
				std::string value = "";
				parsePP(pp.c_str(), StrMoflexParam + '(' + std::to_string(i) + ')', &value);
				parameters.MoflexVec.push_back(value);
			}
		}
		{ // StrMBannerParam
			parameters.MBannerVec.clear();
			for(uint8_t i = 0; i < parameters.rows; i++) {
				std::string value = "";
				parsePP(pp.c_str(), StrMBannerParam + '(' + std::to_string(i) + ')', &value);
				parameters.MBannerVec.push_back(value);
			}
		}
		int count = PlayerTitles.size() - parameters.rows;
		if(parameters.rows - count >= 0) { // ???????????????????
			for(uint8_t i = 0; i < count; i++) {
				PlayerTitles.back()->Destroy();
				PlayerTitles.pop_back();
			}
			for(uint8_t i = 0; i < count; i++) {
				MoflexFiles.back()->Destroy();
				MoflexFiles.pop_back();
			}
			for(uint8_t i = 0; i < count; i++) {
				MenuBanners.back()->Destroy();
				MenuBanners.pop_back();
			}
			for(uint8_t i = 0; i < count; i++) {
				MultiUp.back()->Destroy();
				MultiUp.pop_back();
			}
			for(uint8_t i = 0; i < count; i++) {
				MultiDown.back()->Destroy();
				MultiDown.pop_back();
			}
		}
	}
	
	if(VI9P::MultiBannerIndex > MenuBanners.size() - 1) {
		VI9P::MultiBannerIndex = MenuBanners.size() - 1;
	}
	{ // -gp
		std::string imagePath = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + "/bannerpreview" + std::to_string(VI9P::MultiBannerIndex) + ".png";
		
		executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -gp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + (parameters.rows * 2) + VI9P::MultiBannerIndex) + " \"" + imagePath + '\"'));
		
		multiBannerPreview->SetBitmap(wxBitmap(wxString::FromUTF8(imagePath), wxBITMAP_TYPE_ANY));
	}
	EnableBannerLeftRight();

	positionWidgets();
}

void theWidgets::getBorders() {
	int ImageSize = 100; // width and height
	
	wxImage Picture = wxImage(ImageSize, ImageSize);
	wxBitmapButton* tempButton = new wxBitmapButton(panel, wxID_ANY, wxBitmap(Picture), wxDefaultPosition, wxDefaultSize);
	// i think it already does this
	tempButton->Fit();
	
	tempButton->GetSize(&Borders::width, &Borders::height);
	// border should be larger than the image in the button
	Borders::width -= ImageSize;
	Borders::height -= ImageSize;
	delete tempButton;
}

void theWidgets::setRowIndex() {
	if(parameters.mode == 1) {
		multiBannerPreviewIndex->SetLabel(wxString::FromUTF8(std::to_string(VI9P::MultiBannerIndex + 1) + "/" + std::to_string((MenuBanners.size() < MAX_ROWS_MULTI) ? MenuBanners.size() : MAX_ROWS_MULTI)));
		rowText->SetLabel(wxString::FromUTF8(std::to_string((parameters.rows < MAX_ROWS_MULTI) ? parameters.rows : 27) + "/" + std::to_string(MAX_ROWS_MULTI)));
	}
	else if(parameters.mode == 2) {
		rowText->SetLabel(wxString::FromUTF8(std::to_string(parameters.rows) + "/" + std::to_string(MAX_ROWS)));
	}
}
