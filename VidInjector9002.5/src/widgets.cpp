#include "widgets.hpp"

std::string Exports::CIA = "";
std::string Exports::TAR = "";
std::string Extracted::Archive = "";

void doAddRows(InitWidgets* wid, int rows) {
	for(int i = 0; i < rows; i++) {
		wxTextCtrl* box = new wxTextCtrl(wid->scrolledPanel, wxID_ANY, wxEmptyString);
		wid->PlayerTitles.push_back(box);
	}
	for(int i = 0; i < rows; i++) {
		wxTextCtrl* box = new wxTextCtrl(wid->scrolledPanel, wxID_ANY, wxEmptyString);
		wid->MoflexFiles.push_back(box);
	}
	for(int i = 0; i < rows; i++) {
		wxTextCtrl* box = new wxTextCtrl(wid->scrolledPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
		wid->MenuBanners.push_back(box);
	}
	
	for(int i = 0; i < rows; i++) {
		wxButton* button = new wxButton(wid->scrolledPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
		int width, height;
		wxFont f = button->GetFont();
		
		button->SetLabel(wxString::FromUTF8("↑"));
		
		button->GetTextExtent(button->GetLabel(), &width, &height, nullptr, nullptr, &f);
		button->SetSize(width, height);
		
		wid->MultiUp.push_back(button);
	}
	for(int i = 0; i < rows; i++) {
		wxButton* button = new wxButton(wid->scrolledPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
		int width, height;
		wxFont f = button->GetFont();
		
		button->SetLabel(wxString::FromUTF8("↓"));
		
		button->GetTextExtent(button->GetLabel(), &width, &height, nullptr, nullptr, &f);
		button->SetSize(width, height);
		
		wid->MultiDown.push_back(button);
	}
}

void initAllWidgets(InitWidgets* wid) {
	//main menu
	wid->menuItemFileNew->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW, wxART_MENU));
	wid->menuItemFileOpen->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_MENU));
	wid->menuItemFileSave->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_MENU));
	wid->menuItemFileSaveAs->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS, wxART_MENU));
	wid->menuItemFileExport->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_MENU));
	wid->menuItemFileExtract->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_MENU));
	
	wid->menuItemHelpAbout->SetBitmap(wxArtProvider::GetBitmap(wxART_HELP_BOOK, wxART_MENU));
	
	wid->mainMenu->Append(wid->menuFile, wxString::FromUTF8(file));
	wid->mainMenu->Append(wid->menuOptions, wxString::FromUTF8(options));
	wid->mainMenu->Append(wid->menuLanguage, wxString::FromUTF8(language));
	wid->mainMenu->Append(wid->menuHelp, wxString::FromUTF8(help));
	
	wid->frame->SetMenuBar(wid->mainMenu);
	wid->frame->SetIcon(wxString::FromUTF8(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + imagePath + "/icon.png"));
	
	//panel
	for (const auto &s : {wxString::FromUTF8(SingleVideo), wxString::FromUTF8(MultiVideo)})//https://github.com/gammasoft71/Examples_wxWidgets/blob/adbd395081bf25c9034f2b64eee62608a943441f/src/CommonControls/Choice/Choice.cpp#L10
        wid->modeChoiceBox->Append(s);
	//wid->modeChoiceBox->SetSelection(0);
	
	wid->scrolledPanel->SetScrollRate(10, 10);
	
	{//row stuff
		int rows = 1;//yes this is loop executes once. the plan is to reuse this code when adding many rows at once. yes, yes, I know...
		
		doAddRows(wid, rows);
		ShowMultiUpDown(wid);
	}
	
	/*{//splitPatchLine
	
	}*/
	{//splitPatchUp
		int width, height;
		wxFont f = wid->splitPatchUp->GetFont();
		
		wid->splitPatchUp->GetTextExtent(wid->splitPatchUp->GetLabel(), &width, &height, nullptr, nullptr, &f);
		wid->splitPatchUp->SetSize(width, height);
	}
	{//splitPatchDown
		int width, height;
		wxFont f = wid->splitPatchDown->GetFont();
		
		wid->splitPatchDown->GetTextExtent(wid->splitPatchDown->GetLabel(), &width, &height, nullptr, nullptr, &f);
		wid->splitPatchDown->SetSize(width, height);
	}

	{//buildButton
		int w, buttwidth, h, buttheight;
		wxFont f;
		
		wxButton button(wid->panel, wxID_ANY, wid->buildButton->GetLabel());
		button.Show(false);
		button.GetSize(&buttwidth, &buttheight);
		f = wid->buildButton->GetFont();
		wid->buildButton->GetTextExtent(wid->buildButton->GetLabel(), &w, &h, nullptr, nullptr, &f);
		buttwidth = buttwidth - w;
		buttheight = buttheight - h;

		wid->buildButton->SetSize(w + (buttwidth * 2), h + (buttheight * 2));
	}
	{//extractDialog
		wid->extractDialog->Destroy();
	}
}

void setFonts(InitWidgets* wid) {
	{//modeText
		int w, h;
		wxFont f(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		
		wid->modeText->SetFont(f);
		wid->modeText->GetTextExtent(wid->modeText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->modeText->SetSize(w, h);
	}
	
	{//bannerText
		int w, h;
		wxFont f;
		
		f = wid->modeText->GetFont();
		
		wid->bannerText->SetFont(f);
		wid->bannerText->GetTextExtent(wid->bannerText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->bannerText->SetSize(w, h);
	}
	{//bannerBrowse
		int w, h;
		wxFont f;
		
		f = wid->bannerBrowse->GetFont();
		wid->bannerBrowse->GetTextExtent(wid->bannerBrowse->GetLabel(), &w, &h, nullptr, nullptr, &f);

		wid->bannerBrowse->SetSize((w / 2) + (w * 2), (h / 2) + (h * 2));
	}
	{//bannerError
		int w, h;
		wxFont f;
		
		f = wid->modeText->GetFont().Scale(0.666F).Bold();
		
		wid->bannerError->SetFont(f);
		wid->bannerError->GetTextExtent(wid->bannerError->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->bannerError->SetSize(w, h);
		
		//wid->bannerError->Show(false);
	}
	
	{//iconText
		int w, h;
		wxFont f;
		
		f = wid->modeText->GetFont();
		
		wid->iconText->SetFont(f);
		wid->iconText->GetTextExtent(wid->iconText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->iconText->SetSize(w, h);
	}
	{//iconBrowse
		int w, h;
		wxFont f;

		f = wid->iconBrowse->GetFont();
		wid->iconBrowse->GetTextExtent(wid->iconBrowse->GetLabel(), &w, &h, nullptr, nullptr, &f);

		wid->iconBrowse->SetSize((w / 2) + (w * 2), (h / 2) + (h * 2));
	}
	{//iconError
		int w, h;
		wxFont f;
		
		f = wid->bannerError->GetFont().Bold();
		
		wid->iconError->SetFont(f);
		wid->iconError->GetTextExtent(wid->iconError->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->iconError->SetSize(w, h);
		
		//wid->iconError->Show(false);
	}

	{//shortnameText
		int w, h;
		wxFont f;
		
		f = wid->modeText->GetFont();
		
		wid->shortnameText->SetFont(f);
		wid->shortnameText->GetTextExtent(wid->shortnameText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->shortnameText->SetSize(w, h);
	}
	{//shortnameBox
		int w, h;
		wxFont f;
		
		f = wid->shortnameBox->GetFont();
		
		wid->shortnameBox->SetFont(f);
		wid->shortnameBox->GetTextExtent("A", &w, &h, nullptr, nullptr, &f);
		wid->shortnameBox->GetSize(&w, NULL);
		wid->shortnameBox->SetSize(w, (h * 2) + 10);//uhhhhhh
	}
	{//shortnameError
		int w, h;
		wxFont f;
		
		f = wid->bannerError->GetFont();
		f = f.Bold();
		
		wid->shortnameError->SetFont(f);
		wid->shortnameError->GetTextExtent(wid->shortnameError->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->shortnameError->SetSize(w, h);
	}
	
	{//longnameText
		int w, h;
		wxFont f;
		
		f = wid->modeText->GetFont();
		
		wid->longnameText->SetFont(f);
		wid->longnameText->GetTextExtent(wid->longnameText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->longnameText->SetSize(w, h);
	}
	{//longnameBox
		int w, h;
		wxFont f;
		
		f = wid->longnameBox->GetFont();
		
		wid->longnameBox->SetFont(f);
		wid->longnameBox->GetTextExtent("A", &w, &h, nullptr, nullptr, &f);
		wid->longnameBox->GetSize(&w, NULL);
		wid->longnameBox->SetSize(w, (h * 2) + 10);//uhhhhhh
	}
	{//longnameError
		int w, h;
		wxFont f;
		
		f = wid->bannerError->GetFont().Bold();
		
		wid->longnameError->SetFont(f);
		wid->longnameError->GetTextExtent(wid->longnameError->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->longnameError->SetSize(w, h);
	}
	
	{//publisherText
		int w, h;
		wxFont f;
		
		f = wid->modeText->GetFont();
		
		wid->publisherText->SetFont(f);
		wid->publisherText->GetTextExtent(wid->publisherText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->publisherText->SetSize(w, h);
	}
	{//publisherBox
		int w, h;
		wxFont f;
		
		f = wid->publisherBox->GetFont();
		
		wid->publisherBox->SetFont(f);
		wid->publisherBox->GetTextExtent("A", &w, &h, nullptr, nullptr, &f);
		wid->publisherBox->GetSize(&w, NULL);
		wid->publisherBox->SetSize(w, (h * 2) + 10);//uhhhhhh
	}
	{//publisherError
		int w, h;
		wxFont f;
		
		f = wid->bannerError->GetFont().Bold();
		
		wid->publisherError->SetFont(f);
		wid->publisherError->GetTextExtent(wid->publisherError->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->publisherError->SetSize(w, h);
	}
	
	{//bannerCustomText
		int w, h;
		wxFont f;
		
		f = wid->bannerError->GetFont().GetBaseFont();
		
		wid->bannerCustomText->SetFont(f);
		wid->bannerCustomText->GetTextExtent(wid->bannerCustomText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->bannerCustomText->SetSize(w, h);
	}
	{//bannerPreviewText
		int w, h;
		wxFont f;
		
		f = wid->bannerError->GetFont().GetBaseFont();
		
		wid->bannerPreviewText->SetFont(f);
		wid->bannerPreviewText->GetTextExtent(wid->bannerPreviewText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->bannerPreviewText->SetSize(w, h);
	}

	{//copyCheck
		int w, width, mywidth, boxwidth, h;
		wxFont f;
		
		wxCheckBox Check(wid->panel, wxID_ANY, wid->copyCheck->GetLabel());
		f = Check.GetFont();
		Check.GetTextExtent(wid->copyCheck->GetLabel(), &width, NULL, nullptr, nullptr, &f);
		Check.GetSize(&mywidth, NULL);
		Check.Show(false);
		
		boxwidth = mywidth - width;
		
		f = wid->modeText->GetFont();
		
		wid->copyCheck->SetFont(f);
		wid->copyCheck->GetTextExtent(wid->copyCheck->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->copyCheck->SetSize(w + boxwidth, h);
	}
	
	{//ffRewindCheck
		int w, width, mywidth, boxwidth, h;
		wxFont f;
		
		wxCheckBox Check(wid->panel, wxID_ANY, wid->ffRewindCheck->GetLabel());
		f = Check.GetFont();
		Check.GetTextExtent(wid->ffRewindCheck->GetLabel(), &width, NULL, nullptr, nullptr, &f);
		Check.GetSize(&mywidth, NULL);
		Check.Show(false);
		
		boxwidth = mywidth - width;
		
		f = wid->modeText->GetFont().Scale(0.9F);
		
		wid->ffRewindCheck->SetFont(f);
		wid->ffRewindCheck->GetTextExtent(wid->ffRewindCheck->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->ffRewindCheck->SetSize(w + boxwidth, h);
	}
	{//dimCheck
		int w, width, mywidth, boxwidth, h;
		wxFont f;
		
		wxCheckBox Check(wid->panel, wxID_ANY, wid->ffRewindCheck->GetLabel());
		f = Check.GetFont();
		Check.GetTextExtent(wid->dimCheck->GetLabel(), &width, NULL, nullptr, nullptr, &f);
		Check.GetSize(&mywidth, NULL);
		Check.Show(false);
		
		boxwidth = mywidth - width;
		
		f = wid->ffRewindCheck->GetFont();
		
		wid->dimCheck->SetFont(f);
		wid->dimCheck->GetTextExtent(wid->dimCheck->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->dimCheck->SetSize(w + boxwidth, h);
	}

	{//multiBannerPreviewIndex
		int w, h;
		wxFont f;
		
		f = wid->bannerPreviewText->GetFont();
		
		wid->multiBannerPreviewIndex->SetFont(f);
		wid->multiBannerPreviewIndex->GetTextExtent(wid->multiBannerPreviewIndex->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->multiBannerPreviewIndex->SetSize(w, h);
	}
	
	{//playerTitleText
		int w, h;
		wxFont f;
		
		f = wid->modeText->GetFont();
		
		wid->playerTitleText->SetFont(f);
		wid->playerTitleText->GetTextExtent(wid->playerTitleText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->playerTitleText->SetSize(w, h);
	}
	{//moflexFileText
		int w, h;
		wxFont f;
		
		f = wid->modeText->GetFont();
		
		wid->moflexFileText->SetFont(f);
		wid->moflexFileText->GetTextExtent(wid->moflexFileText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->moflexFileText->SetSize(w, h);
	}
	{//menuBannerText
		int w, h;
		wxFont f;
		
		f = wid->modeText->GetFont();
		
		wid->menuBannerText->SetFont(f);
		wid->menuBannerText->GetTextExtent(wid->menuBannerText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->menuBannerText->SetSize(w, h);
	}

	{//moflexBrowse
		int w, h;
		wxFont f;
		
		f = wid->moflexBrowse->GetFont();
		wid->moflexBrowse->GetTextExtent(wid->moflexBrowse->GetLabel(), &w, &h, nullptr, nullptr, &f);

		wid->moflexBrowse->SetSize((w / 2) + (w * 2), (h / 2) + (h * 2));
	}
	{//multiBannerBrowse
		int w, h;
		wxFont f;
		
		f = wid->multiBannerBrowse->GetFont();
		wid->multiBannerBrowse->GetTextExtent(wid->multiBannerBrowse->GetLabel(), &w, &h, nullptr, nullptr, &f);

		wid->multiBannerBrowse->SetSize((w / 2) + (w * 2), (h / 2) + (h * 2));
	}
	
	{//removeRow
		int w, h;
		wxFont f;
		
		f = wid->modeText->GetFont().Scale(1.1F);
		
		wid->removeRow->SetFont(f);
		wid->removeRow->GetTextExtent(wid->removeRow->GetLabel(), NULL, &h, nullptr, nullptr, &f);
		wid->removeRow->GetSize(&w, NULL);
		wid->removeRow->SetSize(w, h + (h / 2));
	}
	{//appendRow
		int w, h;
		wxFont f;
		
		f = wid->removeRow->GetFont();
		
		wid->appendRow->SetFont(f);
		wid->appendRow->GetTextExtent(wid->appendRow->GetLabel(), NULL, &h, nullptr, nullptr, &f);
		wid->appendRow->GetSize(&w, NULL);
		wid->appendRow->SetSize(w, h + (h / 2));
	}

	{//splitPatchButton
		int w, h;
		wxFont f;
		
		f = wid->ffRewindCheck->GetFont();
		
		wid->splitPatchButton->SetFont(f);
		wid->splitPatchButton->GetTextExtent(wid->splitPatchButton->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->splitPatchButton->SetSize(w + (w / 4), h + (h / 2));
	}
	
	{//rowText
		int w, h;
		wxFont f;
		
		f = wid->bannerPreviewText->GetFont();
		
		wid->rowText->SetFont(f);
		wid->rowText->GetTextExtent(wid->rowText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->rowText->SetSize(w, h);
	}
	//buildpanel
	{//titleIDText
		int w, h;
		wxFont f;
		
		f = wid->modeText->GetFont();
		
		wid->titleIDText->SetFont(f);
		wid->titleIDText->GetTextExtent(wid->titleIDText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->titleIDText->SetSize(w, h);
	}
	{//titleIDBox
		int w, h;
		wxFont f;
		
		f = wid->titleIDBox->GetFont();
		f.SetFamily(wxFONTFAMILY_TELETYPE);
		
		wid->titleIDBox->SetFont(f);
		wid->titleIDBox->GetTextExtent(wxString::FromUTF8("12345"), &w, &h, nullptr, nullptr, &f);
		wid->titleIDBox->SetSize(w + 20, h + 10);
	}
	{//zerozero
		int w, h;
		wxFont f;
		
		f = wid->modeText->GetFont();
		
		wid->zerozero->SetFont(f);
		wid->zerozero->GetTextExtent(wid->zerozero->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->zerozero->SetSize(w, h);
	}
	{//titleIDButton
		int w, h;
		wxFont f;
		
		f = wid->modeText->GetFont().Scale(2);
		
		wid->titleIDButton->SetFont(f);
		wid->titleIDButton->GetTextExtent(wid->titleIDButton->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->titleIDButton->SetSize(w > h ? w : h, w > h ? w : h);
	}
	
	{//applicationTitleText
		int w, h;
		wxFont f;
		
		f = wid->modeText->GetFont();
		
		wid->applicationTitleText->SetFont(f);
		wid->applicationTitleText->GetTextExtent(wid->applicationTitleText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->applicationTitleText->SetSize(w, h);
	}
	{//applicationTitleBox
		wxFont f;
		
		f = wid->productCodeBox->GetFont();
		f.SetFamily(wxFONTFAMILY_TELETYPE);
		
		wid->applicationTitleBox->SetFont(f);
	}
	
	{//productCodeText
		int w, h;
		wxFont f;
		
		f = wid->modeText->GetFont();
		
		wid->productCodeText->SetFont(f);
		wid->productCodeText->GetTextExtent(wid->productCodeText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->productCodeText->SetSize(w, h);
	}
	{//productCodeBox
		int w, h;
		wxFont f;
		
		f = wid->productCodeBox->GetFont();
		f.SetFamily(wxFONTFAMILY_TELETYPE);
		
		wid->productCodeBox->SetFont(f);
		wid->productCodeBox->GetTextExtent(wxString::FromUTF8("1234"), &w, &h, nullptr, nullptr, &f);
		wid->productCodeBox->SetSize(w + 20, h + 10);
	}
	
	{//statusText
		int w, h;
		wxFont f;
		
		f = wid->modeText->GetFont();
		
		wid->statusText->SetFont(f);
		wid->statusText->GetTextExtent(wid->statusText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->statusText->SetSize(w, h);
		
		wid->statusText->Show(false);
	}
	
	{//gitHubLinker
		int w, h;
		wxFont f;
		
		f = wid->modeText->GetFont();
		
		wid->gitHubLinker->SetFont(f);
		wid->gitHubLinker->GetTextExtent(wid->gitHubLinker->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->gitHubLinker->SetSize(w, h);
	}
}

void EnableBannerLeftRight(InitWidgets* wid) {
	if(wid->MenuBanners.size() != 1) {
		if(VI9P::MultiBannerIndex > 0 && VI9P::MultiBannerIndex < wid->MenuBanners.size() - 1) {
			wid->multiBannerPreviewLeft->Enable(true);
			wid->multiBannerPreviewRight->Enable(true);
		}
		if(VI9P::MultiBannerIndex <= 0) {
			wid->multiBannerPreviewLeft->Enable(false);
			wid->multiBannerPreviewRight->Enable(true);
		}
		if(VI9P::MultiBannerIndex >= wid->MenuBanners.size() - 1) {
			wid->multiBannerPreviewLeft->Enable(true);
			wid->multiBannerPreviewRight->Enable(false);
		}
	}
	else {
		wid->multiBannerPreviewLeft->Enable(false);
		wid->multiBannerPreviewRight->Enable(false);
	}
}

void ShowMultiUpDown(InitWidgets* wid) {
	for(const auto &row : wid->MultiUp) {
		row->Enable(true);
		row->Show(true);
	}
	wid->MultiUp.front()->Enable(false);
	wid->MultiUp.front()->Show(false);
	for(const auto &row : wid->MultiDown) {
		row->Enable(true);
		row->Show(true);
	}
	wid->MultiDown.back()->Enable(false);
	wid->MultiDown.back()->Show(false);
}

void ShowPatchUpDown(InitWidgets* wid, VI9Pparameters* parameters) {
	wid->splitPatchUp->Enable(parameters->splitPos > 1);
	wid->splitPatchUp->Show(parameters->splitPos > 1);
	wid->splitPatchDown->Enable(parameters->splitPos < parameters->rows - 1);
	wid->splitPatchDown->Show(parameters->splitPos < parameters->rows - 1);
}

void setToolTips(InitWidgets* wid) {
	wid->modeChoiceBox->SetToolTip(wxString::FromUTF8(modeChoiceBoxTip));
	wid->bannerBox->SetToolTip(wxString::FromUTF8(bannerBoxTip));
	wid->bannerBrowse->SetToolTip(wxString::FromUTF8(bannerBrowseTip));
	wid->iconBox->SetToolTip(wxString::FromUTF8(iconBoxTip));
	wid->iconBrowse->SetToolTip(wxString::FromUTF8(iconBrowseTip));
	wid->shortnameBox->SetToolTip(wxString::FromUTF8(shortnameBoxTip));
	wid->longnameBox->SetToolTip(wxString::FromUTF8(longnameBoxTip));
	wid->publisherBox->SetToolTip(wxString::FromUTF8(publisherBoxTip));
	wid->copyBox->SetToolTip(wxString::FromUTF8(copyBoxTip));
	wid->copyCheck->SetToolTip(wxString::FromUTF8(copyCheckTip + '\n' + onlyMultiVideo));
	wid->iconPreview->SetToolTip(wxString::FromUTF8(iconPreviewTip));
	wid->ffRewindCheck->SetToolTip(wxString::FromUTF8(ffRewindCheckTip));
	wid->dimCheck->SetToolTip(wxString::FromUTF8(dimCheckTip));
	wid->multiBannerPreview->SetToolTip(wxString::FromUTF8(multiBannerPreviewTip + '\n' + onlyMultiVideo));
	wid->multiBannerPreviewLeft->SetToolTip(wxString::FromUTF8(multiBannerPreviewLeftTip));
	wid->multiBannerPreviewRight->SetToolTip(wxString::FromUTF8(multiBannerPreviewRightTip));
	wid->moflexFileText->SetToolTip(wxString::FromUTF8(moflexFileTip));
	for(size_t row = 0; row < wid->PlayerTitles.size(); row++) {
		wid->PlayerTitles.at(row)->SetToolTip(wxString::FromUTF8(playerTitlesTip + " (" + std::to_string(row + 1) + ')'));
	}
	for(size_t row = 0; row < wid->MoflexFiles.size(); row++) {
		wid->MoflexFiles.at(row)->SetToolTip(wxString::FromUTF8(moflexFilesTip + " (" + std::to_string(row + 1) + ')'));
	}
	for(size_t row = 0; row < wid->MenuBanners.size(); row++) {
		wid->MenuBanners.at(row)->SetToolTip(wxString::FromUTF8(menuBannersTip + " (" + std::to_string(row + 1) + ")\n" + onlyMultiVideo));
	}
	for(size_t row = 0; row < wid->MultiUp.size(); row++) {
		wid->MultiUp.at(row)->SetToolTip(wxString::FromUTF8(multiUpTip));
	}
	for(size_t row = 0; row < wid->MultiDown.size(); row++) {
		wid->MultiDown.at(row)->SetToolTip(wxString::FromUTF8(multiDownTip));
	}
	wid->moflexBrowse->SetToolTip(wxString::FromUTF8(moflexBrowseTip));
	wid->multiBannerBrowse->SetToolTip(wxString::FromUTF8(multiBannerBrowseTip));
	wid->removeRow->SetToolTip(wxString::FromUTF8(removeRowTip));
	wid->appendRow->SetToolTip(wxString::FromUTF8(appendRowTip + '\n' + onlyMultiVideo));
	wid->splitPatchButton->SetToolTip(wxString::FromUTF8(splitPatchTip));
	wid->splitPatchUp->SetToolTip(wxString::FromUTF8(splitPatchUpTip));
	wid->splitPatchDown->SetToolTip(wxString::FromUTF8(splitPatchDownTip));
	
	wid->titleIDBox->SetToolTip(wxString::FromUTF8(titleIDBoxTip));
	wid->titleIDButton->SetToolTip(wxString::FromUTF8(titleIDButtonTip));
	wid->applicationTitleBox->SetToolTip(wxString::FromUTF8(applicationTitleBoxTip));
	wid->productCodeBox->SetToolTip(wxString::FromUTF8(productCodeBoxTip));
	wid->buildButton->SetToolTip(wxString::FromUTF8(buildButtonTip));
	wid->cancelButton->SetToolTip(wxString::FromUTF8(cancelButtonTip));
	
	wid->titleLogo->SetToolTip(wxString::FromUTF8(frameText));
	wid->gitHubLinker->SetToolTip(wxString::FromUTF8(githubRepoLink));
}

void setCursors(InitWidgets* wid) {
	wid->modeChoiceBox->SetCursor(wid->modeChoiceBox->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	wid->bannerBox->SetCursor(wid->bannerBox->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_ARROW);
	wid->bannerBrowse->SetCursor(wid->bannerBrowse->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	wid->iconBox->SetCursor(wid->iconBox->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_ARROW);
	wid->iconBrowse->SetCursor(wid->iconBrowse->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	wid->shortnameBox->SetCursor(wid->shortnameBox->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_ARROW);
	wid->longnameBox->SetCursor(wid->longnameBox->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_ARROW);
	wid->publisherBox->SetCursor(wid->publisherBox->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_ARROW);
	wid->copyBox->SetCursor(wid->copyBox->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_ARROW);
	wid->copyCheck->SetCursor(wid->copyCheck->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	wid->iconPreview->SetCursor(wid->iconPreview->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	wid->ffRewindCheck->SetCursor(wid->ffRewindCheck->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	wid->dimCheck->SetCursor(wid->dimCheck->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	wid->multiBannerPreview->SetCursor(wid->multiBannerPreview->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_NO_ENTRY);
	wid->multiBannerPreviewLeft->SetCursor(wid->multiBannerPreviewLeft->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	wid->multiBannerPreviewRight->SetCursor(wid->multiBannerPreviewRight->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	for(const auto& row : wid->PlayerTitles) {
		row->SetCursor(row->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_ARROW);
	}
	for(const auto& row : wid->MoflexFiles) {
		row->SetCursor(row->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_ARROW);
	}
	for(const auto& row : wid->MenuBanners) {
		row->SetCursor(row->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_ARROW);
	}
	for(const auto& row : wid->MultiUp) {
		row->SetCursor(row->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	}
	for(const auto& row : wid->MultiDown) {
		row->SetCursor(row->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	}
	wid->moflexBrowse->SetCursor(wid->moflexBrowse->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	wid->multiBannerBrowse->SetCursor(wid->multiBannerBrowse->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	wid->removeRow->SetCursor(wid->removeRow->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	wid->appendRow->SetCursor(wid->appendRow->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	wid->splitPatchButton->SetCursor(wid->splitPatchButton->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	wid->splitPatchUp->SetCursor(wid->splitPatchButton->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	wid->splitPatchDown->SetCursor(wid->splitPatchButton->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	wid->frame->SetCursor(wxCURSOR_ARROW);
	wid->panel->SetCursor(wxCURSOR_ARROW);
	wid->mediaPanel->SetCursor(wxCURSOR_ARROW);
	
	wid->titleIDBox->SetCursor(wid->titleIDButton->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_ARROW);
	wid->titleIDButton->SetCursor(wid->titleIDButton->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	wid->applicationTitleBox->SetCursor(wid->titleIDButton->IsEnabled() ? wxCURSOR_IBEAM : wxCURSOR_ARROW);
}

void positionWidgets(InitWidgets* wid, VI9Pparameters* parameters) {
	{//modeChoiceBox
		int x, y, height;
		wid->modeText->GetPosition(&x, &y);
		wid->modeText->GetSize(NULL, &height);
		
		wid->modeChoiceBox->Move(x, y + height);
	}
	
	{//bannerText
		int x, y, height;
		wid->modeChoiceBox->GetPosition(&x, &y);
		wid->modeChoiceBox->GetSize(NULL, &height);
		
		wid->bannerText->Move(x, y + (height * 2));
	}
	{//bannerBox
		int x, y, width, height, myheight;
		wid->bannerText->GetPosition(&x, &y);
		wid->bannerText->GetSize(&width, &height);
		wid->bannerBox->GetSize(NULL, &myheight);
		
		wid->bannerBox->Move(x + width + 5, y + ((height - myheight) / 2));
	}
	{//bannerBrowse y
		int myx, y, height, myheight;
		wid->bannerBox->GetPosition(NULL, &y);
		wid->bannerBox->GetSize(NULL, &height);
		wid->bannerBrowse->GetPosition(&myx, NULL);
		wid->bannerBrowse->GetSize(NULL, &myheight);
		
		wid->bannerBrowse->Move(myx, y + ((height - myheight) / 2));
	}
	{//bannerError
		int x, y, height;
		wid->bannerBox->GetPosition(&x, NULL);
		wid->bannerBrowse->GetPosition(NULL, &y);
		wid->bannerBrowse->GetSize(NULL, &height);
		
		wid->bannerError->Move(x, y + height);
	}
	
	{//iconText
		int x, y, height, myheight, browseheight;
		wid->modeText->GetPosition(&x, NULL);
		wid->bannerError->GetPosition(NULL, &y);
		wid->bannerError->GetSize(NULL, &height);
		wid->iconText->GetSize(NULL, &myheight);
		wid->iconBrowse->GetSize(NULL, &browseheight);
		
		wid->iconText->Move(x, y + height + ((browseheight - myheight) / 2));
	}
	{//iconBox
		int x, y, width, height, myheight;
		wid->iconText->GetPosition(&x, &y);
		wid->iconText->GetSize(&width, &height);
		wid->iconBox->GetSize(NULL, &myheight);
		
		wid->iconBox->Move(x + width + 5, y + ((height - myheight) / 2));
		//wid->iconBox->SetSize(, &myheight);
	}
	{//iconBrowse y
		int myx, y, height;
		wid->iconBrowse->GetPosition(&myx, NULL);
		wid->bannerError->GetPosition(NULL, &y);
		wid->bannerError->GetSize(NULL, &height);
		
		wid->iconBrowse->Move(myx, y + height);
	}
	{//iconError
		int x, y, height;
		wid->iconBox->GetPosition(&x, NULL);
		wid->iconBrowse->GetPosition(NULL, &y);
		wid->iconBrowse->GetSize(NULL, &height);
		
		wid->iconError->Move(x, y + height);
	}
	
	{//bannerPreview y
		int x, y;
		wid->modeText->GetPosition(NULL, &y);
		wid->bannerPreview->GetPosition(&x, NULL);
		
		wid->bannerPreview->Move(x, y);
	}
	
	{//shortnameText
		int x, y, bannerPreviewY, iconBrowseY, height, bannerPreviewHeight, bannerPreviewTextHeight, iconPreviewHeight;
		wid->modeText->GetPosition(&x, NULL);
		wid->iconError->GetPosition(NULL, &y);
		wid->iconError->GetSize(NULL, &height);
		
		wid->bannerPreview->GetPosition(NULL, &bannerPreviewY);
		wid->bannerPreview->GetSize(NULL, &bannerPreviewHeight);
		wid->bannerPreviewText->GetSize(NULL, &bannerPreviewTextHeight);
		wid->iconBrowse->GetPosition(NULL, &iconBrowseY);
		wid->iconPreview->GetSize(NULL, &iconPreviewHeight);
		
		std::vector<int> sizes(3, 0);
		sizes.at(0) = (bannerPreviewY + bannerPreviewHeight + bannerPreviewTextHeight);//banner preview
		sizes.at(1) = (y + height);//icon error
		sizes.at(2) = (iconBrowseY + iconPreviewHeight);//icon preview
		
		std::vector<int>::iterator Heightest = std::max_element(sizes.begin(), sizes.end());//dont even worry
		
		wid->shortnameText->Move(x, (*Heightest) + 6);
	}
	{//shortnameBox
		int x, y, width;
		wid->shortnameText->GetPosition(&x, &y);
		wid->shortnameText->GetSize(&width, NULL);
		
		wid->shortnameBox->Move(x + width + 5, y);
	}
	{//shortnameError
		int x, y, height;
		wid->shortnameBox->GetPosition(&x, &y);
		wid->shortnameBox->GetSize(NULL, &height);
		
		wid->shortnameError->Move(x, y + height);
	}
	
	{//longnameText
		int x, y, height;
		wid->modeText->GetPosition(&x, NULL);
		wid->shortnameError->GetPosition(NULL, &y);
		wid->shortnameError->GetSize(NULL, &height);
		
		wid->longnameText->Move(x, y + height);
	}
	{//longnameBox
		int x, y, width;
		wid->longnameText->GetPosition(&x, &y);
		wid->longnameText->GetSize(&width, NULL);
		
		wid->longnameBox->Move(x + width + 5, y);
	}
	{//longnameError
		int x, y, height;
		wid->longnameBox->GetPosition(&x, &y);
		wid->longnameBox->GetSize(NULL, &height);
		
		wid->longnameError->Move(x, y + height);
	}
	
	{//publisherText
		int x, y, height;
		wid->modeText->GetPosition(&x, NULL);
		wid->longnameError->GetPosition(NULL, &y);
		wid->longnameError->GetSize(NULL, &height);
		
		wid->publisherText->Move(x, y + height);
	}
	{//publisherBox
		int x, y, width;
		wid->publisherText->GetPosition(&x, &y);
		wid->publisherText->GetSize(&width, NULL);
		
		wid->publisherBox->Move(x + width + 5, y);
	}
	{//publisherError
		int x, y, height;
		wid->publisherBox->GetPosition(&x, &y);
		wid->publisherBox->GetSize(NULL, &height);
		
		wid->publisherError->Move(x, y + height);
	}
	
	{//ffRewindCheck
		int x, y, height;
		wid->publisherError->GetPosition(&x, &y);
		wid->publisherError->GetSize(NULL, &height);
		
		wid->ffRewindCheck->Move(x, y + height);
	}
	{//dimCheck
		int x, y, height;
		wid->ffRewindCheck->GetPosition(&x, &y);
		wid->ffRewindCheck->GetSize(NULL, &height);
		
		wid->dimCheck->Move(x, y + height);
	}
	
	//stuff that moves or changes size
	{//copyBox
		int x, y, width, mywidth, checkwidth, checkheight;
		wid->modeText->GetPosition(&x, &y);
		wid->panel->GetSize(&width, NULL);
		wid->copyBox->GetSize(&mywidth, NULL);
		wid->copyCheck->GetSize(&checkwidth, NULL);
		wid->copyCheck->GetSize(NULL, &checkheight);
		
		int iconErrorX, iconErrorW, iconPreviewW, bannerPreviewW, bannerPreviewTextW;
		wid->iconError->GetPosition(&iconErrorX, NULL);
		wid->iconError->GetSize(&iconErrorW, NULL);
		wid->iconPreview->GetSize(&iconPreviewW, NULL);
		wid->bannerPreview->GetSize(&bannerPreviewW, NULL);
		wid->bannerPreviewText->GetSize(&bannerPreviewTextW, NULL);
		
		if(width >= iconErrorX + iconErrorW + iconPreviewW + 5 + (bannerPreviewW > bannerPreviewTextW ? bannerPreviewW : bannerPreviewTextW) + 15 + (mywidth > checkwidth ? mywidth : checkwidth) + x) {
			wid->copyBox->Move(width - (mywidth > checkwidth ? mywidth : checkwidth) - x, y + checkheight);
		}
		else {
			wid->copyBox->Move(iconErrorX + iconErrorW + iconPreviewW + 5 + (bannerPreviewW > bannerPreviewTextW ? bannerPreviewW : bannerPreviewTextW) + 15, y + checkheight);
		}
	}
	{//copyCheck
		int x, y, myheight;
		wid->copyBox->GetPosition(&x, &y);
		wid->copyCheck->GetSize(NULL, &myheight);
		
		wid->copyCheck->Move(x, y - myheight);//why not just put this in before the box?
	}
	
	{//bannerPreview x
		int x, y, mywidth, bannerPreviewTextW;
		wid->copyCheck->GetPosition(&x, NULL);
		wid->bannerPreview->GetPosition(NULL, &y);
		wid->bannerPreview->GetSize(&mywidth, NULL);
		wid->bannerPreviewText->GetSize(&bannerPreviewTextW, NULL);
		
		wid->bannerPreview->Move(x - (mywidth > bannerPreviewTextW ? mywidth : ((bannerPreviewTextW - mywidth) * 2)) - (mywidth > bannerPreviewTextW ? 15 : 15 * 2), y);
	}
	{//bannerCustomText
		int x, y, width, mywidth, height, myheight;
		wid->bannerPreview->GetPosition(&x, &y);
		wid->bannerPreview->GetSize(&width, &height);
		wid->bannerCustomText->GetSize(&mywidth, &myheight);
		
		wid->bannerCustomText->Move(x + ((width - mywidth) / 2), y + ((height - myheight) / 2));
	}
	{//bannerPreviewText
		int x, y, width, mywidth, height;
		wid->bannerPreview->GetPosition(&x, &y);
		wid->bannerPreview->GetSize(&width, &height);
		wid->bannerPreviewText->GetSize(&mywidth, NULL);
		
		wid->bannerPreviewText->Move(x + ((width - mywidth) / 2), y + height);
	}
	
	{//iconPreview
		int x, bannerPreviewTextx, y, mywidth;
		wid->iconBrowse->GetPosition(NULL, &y);
		wid->bannerPreview->GetPosition(&x, NULL);
		wid->iconPreview->GetSize(&mywidth, NULL);
		wid->bannerPreviewText->GetPosition(&bannerPreviewTextx, NULL);
		
		wid->iconPreview->Move((x < bannerPreviewTextx ? x : bannerPreviewTextx) - mywidth - 5, y);
	}
	{//bannerBrowse x
		int x, bannerPreviewTextx, myy, mywidth;
		wid->bannerPreview->GetPosition(&x, NULL);
		wid->bannerBrowse->GetPosition(NULL, &myy);
		wid->bannerBrowse->GetSize(&mywidth, NULL);
		wid->bannerPreviewText->GetPosition(&bannerPreviewTextx, NULL);
		
		wid->bannerBrowse->Move((x < bannerPreviewTextx ? x : bannerPreviewTextx) - mywidth - 5, myy);
	}
	{//iconBrowse x
		int x, myy, mywidth;
		wid->iconPreview->GetPosition(&x, NULL);
		wid->iconBrowse->GetPosition(NULL, &myy);
		wid->iconBrowse->GetSize(&mywidth, NULL);
		
		wid->iconBrowse->Move(x - mywidth - 2, myy);
	}
	{//bannerBox width
		int myx, browsex, myheight;
		wid->bannerBrowse->GetPosition(&browsex, NULL);
		wid->bannerBox->GetPosition(&myx, NULL);
		wid->bannerBox->GetSize(NULL, &myheight);
		
		wid->bannerBox->SetSize(browsex - myx - 2, myheight);
	}
	{//iconBox width
		int myx, browsex, myheight;
		wid->iconBrowse->GetPosition(&browsex, NULL);
		wid->iconBox->GetPosition(&myx, NULL);
		wid->iconBox->GetSize(NULL, &myheight);
		
		wid->iconBox->SetSize(browsex - myx - 2, myheight);
	}
	{//shortnameBox width
		int myx, x, bannerPreviewTextx, width, bannerPreviewTextW, myheight;
		wid->bannerPreview->GetPosition(&x, NULL);
		wid->bannerPreview->GetSize(&width, NULL);
		wid->bannerPreviewText->GetPosition(&bannerPreviewTextx, NULL);
		wid->bannerPreviewText->GetSize(&bannerPreviewTextW, NULL);
		wid->shortnameBox->GetPosition(&myx, NULL);
		wid->shortnameBox->GetSize(NULL, &myheight);
		
		wid->shortnameBox->SetSize(((x < bannerPreviewTextx ? x : bannerPreviewTextx) + (width > bannerPreviewTextW ? width : bannerPreviewTextW)) - myx, myheight);
	}
	{//longnameBox width
		int myx, x, bannerPreviewTextx, width, bannerPreviewTextW, myheight;
		wid->bannerPreview->GetPosition(&x, NULL);
		wid->bannerPreview->GetSize(&width, NULL);
		wid->bannerPreviewText->GetPosition(&bannerPreviewTextx, NULL);
		wid->bannerPreviewText->GetSize(&bannerPreviewTextW, NULL);
		wid->longnameBox->GetPosition(&myx, NULL);
		wid->longnameBox->GetSize(NULL, &myheight);
		
		wid->longnameBox->SetSize(((x < bannerPreviewTextx ? x : bannerPreviewTextx) + (width > bannerPreviewTextW ? width : bannerPreviewTextW)) - myx, myheight);
	}
	{//publisherBox width
		int myx, x, bannerPreviewTextx, width, bannerPreviewTextW, myheight;
		wid->bannerPreview->GetPosition(&x, NULL);
		wid->bannerPreview->GetSize(&width, NULL);
		wid->bannerPreviewText->GetPosition(&bannerPreviewTextx, NULL);
		wid->bannerPreviewText->GetSize(&bannerPreviewTextW, NULL);
		wid->publisherBox->GetPosition(&myx, NULL);
		wid->publisherBox->GetSize(NULL, &myheight);
		
		wid->publisherBox->SetSize(((x < bannerPreviewTextx ? x : bannerPreviewTextx) + (width > bannerPreviewTextW ? width : bannerPreviewTextW)) - myx, myheight);
	}
	{//multiBannerPreview
		int x, y, width, mywidth, height;
		wid->copyBox->GetPosition(&x, &y);
		wid->copyBox->GetSize(&width, &height);
		wid->multiBannerPreview->GetSize(&mywidth, NULL);
		
		wid->multiBannerPreview->Move(x + ((width - mywidth) / 2), y + height + 10);
	}
	{//multiBannerPreviewIndex
		int x, y, width, mywidth, height;
		wid->multiBannerPreview->GetPosition(&x, &y);
		wid->multiBannerPreview->GetSize(&width, &height);
		wid->multiBannerPreviewIndex->GetSize(&mywidth, NULL);
		
		wid->multiBannerPreviewIndex->Move(x + ((width - mywidth) / 2), y + height);
	}
	{//multiBannerPreviewLeft
		int x, y, height;
		wid->multiBannerPreview->GetPosition(&x, &y);
		wid->multiBannerPreview->GetSize(NULL, &height);
		//wid->multiBannerIndex->GetSize(&mywidth, NULL);
		
		wid->multiBannerPreviewLeft->Move(x, y + height);
	}
	{//multiBannerPreviewRight
		int x, y, width, mywidth, height;
		wid->multiBannerPreview->GetPosition(&x, &y);
		wid->multiBannerPreview->GetSize(&width, &height);
		wid->multiBannerPreviewRight->GetSize(&mywidth, NULL);
		
		wid->multiBannerPreviewRight->Move((x + width) - mywidth, y + height);
	}
	{//playerTitleText y
		int x, y, checky, height, checkheight;
		wid->multiBannerPreviewLeft->GetPosition(NULL, &y);
		wid->dimCheck->GetPosition(NULL, &checky);
		wid->dimCheck->GetSize(NULL, &checkheight);
		wid->multiBannerPreviewLeft->GetSize(NULL, &height);
		wid->playerTitleText->GetPosition(&x, NULL);
		
		wid->playerTitleText->Move(x, (y > checky ? (y + height + 5) : (checky + checkheight + 5)));
	}
	{//moflexFileText y
		int x, y, checky, height, checkheight;
		wid->multiBannerPreviewLeft->GetPosition(NULL, &y);
		wid->dimCheck->GetPosition(NULL, &checky);
		wid->dimCheck->GetSize(NULL, &checkheight);
		wid->multiBannerPreviewLeft->GetSize(NULL, &height);
		wid->moflexFileText->GetPosition(&x, NULL);
		
		wid->moflexFileText->Move(x, (y > checky ? (y + height + 5) : (checky + checkheight + 5)));
	}
	{//menuBannerText y
		int x, y, checky, height, checkheight;
		wid->multiBannerPreviewLeft->GetPosition(NULL, &y);
		wid->multiBannerPreviewLeft->GetSize(NULL, &height);
		wid->dimCheck->GetPosition(NULL, &checky);
		wid->dimCheck->GetSize(NULL, &checkheight);
		wid->menuBannerText->GetPosition(&x, NULL);
		
		wid->menuBannerText->Move(x, (y > checky ? (y + height + 5) : (checky + checkheight + 5)));
	}
	{//mediaPanel
		int x, myx, y, myy, width, height;
		wid->modeText->GetPosition(&x, NULL);
		wid->playerTitleText->GetPosition(NULL, &y);
		wid->playerTitleText->GetSize(NULL, &height);
		
		wid->mediaPanel->Move(x, y + height + 1);
		
		wid->panel->GetSize(&width, &height);
		wid->mediaPanel->GetPosition(&myx, &myy);
		
		wid->mediaPanel->SetSize(width - (myx * 2), height - myy - myx);
	}
	{
		std::vector<int> sizes(5, 0);
		wid->playerTitleText->GetSize(&sizes[0], NULL);
		wid->moflexFileText->GetSize(&sizes[1], NULL);
		wid->menuBannerText->GetSize(&sizes[2], NULL);
		wid->moflexBrowse->GetSize(&sizes[3], NULL);
		wid->multiBannerBrowse->GetSize(&sizes[4], NULL);
		std::vector<int>::iterator widestText = std::max_element(sizes.begin(), sizes.end());
		
		{//PlayerTitles
			for(int row = 0; row < wid->PlayerTitles.size(); row++) {
				int scrolledx, scrolledy, ppux, ppuy, previousy, width, upwidth, downwidth, upheight, height;
				wid->mediaPanel->GetSize(&width, NULL);
				wid->scrolledPanel->GetViewStart(&scrolledx, &scrolledy);
				wid->scrolledPanel->GetScrollPixelsPerUnit(&ppux, &ppuy);
				wid->MultiUp.at(0)->GetSize(&upwidth, NULL);
				wid->MultiDown.at(0)->GetSize(&downwidth, NULL);
				wid->PlayerTitles.at(row)->GetSize(NULL, &height);
				
				if(wid->PlayerTitles.size() == 1) {
					if((width - (ppux * 2)) / 3 >= *widestText) {
						wid->PlayerTitles.at(row)->SetSize((width - (ppux * 2)) / 3, height);
					}
					else {
						wid->PlayerTitles.at(row)->SetSize(*widestText, height);
					}
				}
				else {
					if((width - (upwidth + 3 + downwidth + (ppux * 2))) / 3 >= *widestText) {
						wid->PlayerTitles.at(row)->SetSize((width - (upwidth + 3 + downwidth + (ppux * 2))) / 3, height);
					}
					else {
						wid->PlayerTitles.at(row)->SetSize(*widestText, height);
					}
				}
				
				if(row > 0) {
					wid->PlayerTitles.at(row - 1)->GetPosition(NULL, &previousy);
					if(row == parameters->splitPos) {
						wid->splitPatchUp->GetSize(NULL, &upheight);
						wid->PlayerTitles.at(row)->Move(0 - (scrolledx * ppux), (previousy + height + upheight));
					}
					else {
						wid->PlayerTitles.at(row)->Move(0 - (scrolledx * ppux), (previousy + height));
					}
				}
				else {
					wid->PlayerTitles.at(row)->Move(0 - (scrolledx * ppux), (height * row) - (scrolledy * ppuy));
				}
			}
		}
		{//MoflexFiles
			for(int row = 0; row < wid->MoflexFiles.size(); row++) {
				int scrolledx, scrolledy, ppux, ppuy, previousy, width, upwidth, downwidth, upheight, height;
				wid->mediaPanel->GetSize(&width, NULL);
				wid->scrolledPanel->GetViewStart(&scrolledx, &scrolledy);
				wid->scrolledPanel->GetScrollPixelsPerUnit(&ppux, &ppuy);
				wid->MultiUp.at(0)->GetSize(&upwidth, NULL);
				wid->MultiDown.at(0)->GetSize(&downwidth, NULL);
				wid->MoflexFiles.at(row)->GetSize(NULL, &height);
				
				if(wid->MoflexFiles.size() == 1) {
					if((width - (ppux * 2)) / 3 >= *widestText) {
						wid->MoflexFiles.at(row)->SetSize((width - (ppux * 2)) / 3, height);
					}
					else {
						wid->MoflexFiles.at(row)->SetSize(*widestText, height);
					}
				}
				else {
					if((width - (upwidth + 3 + downwidth + (ppux * 2))) / 3 >= *widestText) {
						wid->MoflexFiles.at(row)->SetSize((width - (upwidth + 3 + downwidth + (ppux * 2))) / 3, height);
					}
					else {
						wid->MoflexFiles.at(row)->SetSize(*widestText, height);
					}
				}
				
				wid->MoflexFiles.at(row)->GetSize(&width, NULL);
				
				//row->Move((width) - (scrolledx * ppux), (height * currentrow) - (scrolledy * ppuy));
				if(row > 0) {
					wid->MoflexFiles.at(row - 1)->GetPosition(NULL, &previousy);
					if(row == parameters->splitPos) {
						wid->splitPatchUp->GetSize(NULL, &upheight);
						wid->MoflexFiles.at(row)->Move((width) - (scrolledx * ppux), (previousy + height + upheight));
					}
					else {
						wid->MoflexFiles.at(row)->Move((width) - (scrolledx * ppux), (previousy + height));
					}
				}
				else {
					wid->MoflexFiles.at(row)->Move((width) - (scrolledx * ppux), (height * row) - (scrolledy * ppuy));
				}
			}
		}
		{//MenuBanners
			for(int row = 0; row < wid->MenuBanners.size(); row++) {
				int scrolledx, scrolledy, ppux, ppuy, previousy, width, upwidth, downwidth, upheight, height;
				wid->mediaPanel->GetSize(&width, NULL);
				wid->scrolledPanel->GetViewStart(&scrolledx, &scrolledy);
				wid->scrolledPanel->GetScrollPixelsPerUnit(&ppux, &ppuy);
				wid->MultiUp.at(0)->GetSize(&upwidth, NULL);
				wid->MultiDown.at(0)->GetSize(&downwidth, NULL);
				wid->MenuBanners.at(row)->GetSize(NULL, &height);
				
				if(wid->MenuBanners.size() == 1) {
					if((width - (ppux * 2)) / 3 >= *widestText) {
						wid->MenuBanners.at(row)->SetSize((width - (ppux * 2)) / 3, height);
					}
					else {
						wid->MenuBanners.at(row)->SetSize(*widestText, height);
					}
				}
				else {
					if((width - (upwidth + 3 + downwidth + (ppux * 2))) / 3 >= *widestText) {
						wid->MenuBanners.at(row)->SetSize((width - (upwidth + 3 + downwidth + (ppux * 2))) / 3, height);
					}
					else {
						wid->MenuBanners.at(row)->SetSize(*widestText, height);
					}
				}
				
				wid->MenuBanners.at(row)->GetSize(&width, NULL);
				
				if(row > 0) {
					wid->MenuBanners.at(row - 1)->GetPosition(NULL, &previousy);
					if(row == parameters->splitPos) {
						wid->splitPatchUp->GetSize(NULL, &upheight);
						wid->MenuBanners.at(row)->Move((width * 2) - (scrolledx * ppux), (previousy + height + upheight));
					}
					else {
						wid->MenuBanners.at(row)->Move((width * 2) - (scrolledx * ppux), (previousy + height));
					}
				}
				else {
					wid->MenuBanners.at(row)->Move((width * 2) - (scrolledx * ppux), (height * row) - (scrolledy * ppuy));
				}
			}
		}
	}
	{//playerTitleText x
		int x, boxx, y, width, mywidth;
		wid->mediaPanel->GetPosition(&x, NULL);
		wid->PlayerTitles.at(0)->GetSize(&width, NULL);
		wid->PlayerTitles.at(0)->GetPosition(&boxx, NULL);
		wid->playerTitleText->GetSize(&mywidth, NULL);
		wid->playerTitleText->GetPosition(NULL, &y);
		
		wid->playerTitleText->Move(x + boxx + ((width - mywidth) / 2), y);
	}
	{//moflexFileText x
		int x, boxx, y, width, mywidth;
		wid->mediaPanel->GetPosition(&x, NULL);
		wid->MoflexFiles.at(0)->GetSize(&width, NULL);
		wid->MoflexFiles.at(0)->GetPosition(&boxx, NULL);
		wid->moflexFileText->GetSize(&mywidth, NULL);
		wid->moflexFileText->GetPosition(NULL, &y);
		
		wid->moflexFileText->Move(x + boxx + ((width - mywidth) / 2), y);
	}
	{//menuBannerText x
		int x, boxx, y, width, mywidth;
		wid->mediaPanel->GetPosition(&x, NULL);
		wid->MenuBanners.at(0)->GetSize(&width, NULL);
		wid->MenuBanners.at(0)->GetPosition(&boxx, NULL);
		wid->menuBannerText->GetSize(&mywidth, NULL);
		wid->menuBannerText->GetPosition(NULL, &y);
		
		wid->menuBannerText->Move(x + boxx + ((width - mywidth) / 2), y);
	}
	{//MultiUp
		int x, y, width, height, myheight;
		for(int i = 0; i < wid->MenuBanners.size(); i++) {
			wid->MenuBanners.at(i)->GetSize(&width, &height);
			wid->MenuBanners.at(i)->GetPosition(&x, &y);
			wid->MultiUp.at(i)->GetSize(NULL, &myheight);
			
			wid->MultiUp.at(i)->Move(x + width, y + ((height - myheight) / 2));
		}
	}
	{//MultiDown
		int x, y, width;
		for(int i = 0; i < wid->MultiUp.size(); i++) {
			wid->MultiUp.at(i)->GetSize(&width, NULL);
			wid->MultiUp.at(i)->GetPosition(&x, &y);
			
			wid->MultiDown.at(i)->Move(x + width + 3, y);
		}
	}
	{//moflexBrowse
		int x, y, width, mywidth, height;
		wid->MoflexFiles.back()->GetSize(&width, &height);
		wid->MoflexFiles.back()->GetPosition(&x, &y);
		wid->moflexBrowse->GetSize(&mywidth, NULL);
		
		wid->moflexBrowse->Move(x + ((width - mywidth) / 2), y + height);
	}
	{//multiBannerBrowse
		int x, y, width, mywidth, height;
		wid->MenuBanners.back()->GetSize(&width, &height);
		wid->MenuBanners.back()->GetPosition(&x, &y);
		wid->multiBannerBrowse->GetSize(&mywidth, NULL);
		
		wid->multiBannerBrowse->Move(x + ((width - mywidth) / 2), y + height);
	}
	{//removeRow
		int y, width, mywidth, appendrowwidth, height;
		wid->multiBannerBrowse->GetSize(NULL, &height);
		wid->multiBannerBrowse->GetPosition(NULL, &y);
		wid->mediaPanel->GetSize(&width, NULL);
		wid->removeRow->GetSize(&mywidth, NULL);
		wid->appendRow->GetSize(&appendrowwidth, NULL);
		
		wid->removeRow->Move(((width - (mywidth + 2 + appendrowwidth)) / 2), y + height + 2);
	}
	{//appendRow
		int x, y, width;
		wid->removeRow->GetSize(&width, NULL);
		wid->removeRow->GetPosition(&x, &y);
		
		wid->appendRow->Move(x + width + 2, y);
	}
	{//splitPatchButton
		int x, y, width, mywidth, appendrowwidth, height;
		wid->removeRow->GetSize(&width, &height);
		wid->removeRow->GetPosition(&x, &y);
		wid->appendRow->GetSize(&appendrowwidth, NULL);
		wid->splitPatchButton->GetSize(&mywidth, NULL);
		
		wid->splitPatchButton->Move(x + (((width + 2 + appendrowwidth) - mywidth) / 2), y + height + 2);
	}
	{//splitPatchDown
		int x, y, width, mywidth, myheight;
		wid->MenuBanners.at(parameters->splitPos)->GetSize(&width, NULL);
		wid->MenuBanners.at(parameters->splitPos)->GetPosition(&x, &y);
		wid->splitPatchDown->GetSize(&mywidth, &myheight);
		
		wid->splitPatchDown->Move((x + width) - mywidth, y - myheight);
	}
	{//splitPatchUp
		int x, y, mywidth;
		wid->splitPatchDown->GetPosition(&x, &y);
		wid->splitPatchUp->GetSize(&mywidth, NULL);
		
		wid->splitPatchUp->Move(x - 3 - mywidth, y);
	}
	{//splitPatchLine
		int x, upx, upy, downwidth, upwidth, upheight;
		wid->PlayerTitles.at(parameters->splitPos)->GetPosition(&x, NULL);
		wid->splitPatchUp->GetPosition(&upx, &upy);
		wid->splitPatchUp->GetSize(&upwidth, &upheight);
		wid->splitPatchDown->GetSize(&downwidth, NULL);
		
		wid->splitPatchLine->Move(x + upwidth + 3 + downwidth + 3, upy + ((upheight - 3) / 2));
		wid->splitPatchLine->SetSize(upx - 3 - x - upwidth - 3 - downwidth - 3, 3);
	}
	{//rowText
		int x, y, width, mywidth, height;
		wid->splitPatchButton->GetSize(&width, &height);
		wid->splitPatchButton->GetPosition(&x, &y);
		wid->rowText->GetSize(&mywidth, NULL);
		
		wid->rowText->Move(x + ((width - mywidth) / 2), y + height + 2);
	}
	{//scrolledPanel
		int width, boxwidth, upwidth, height, boxheight, browseheight, removeheight, splitpatchheight, splitupheight, rowtextheight;//todo: splitpatch line
		wid->mediaPanel->GetSize(&width, &height);
		wid->PlayerTitles.at(0)->GetSize(&boxwidth, &boxheight);
		wid->MultiUp.at(0)->GetSize(&upwidth, NULL);//up and down are the same size so just use this (or are they...?)
		wid->moflexBrowse->GetSize(NULL, &browseheight);
		wid->removeRow->GetSize(NULL, &removeheight);//- and + are the same size
		wid->splitPatchButton->GetSize(NULL, &splitpatchheight);
		wid->splitPatchUp->GetSize(NULL, &splitupheight);
		wid->rowText->GetSize(NULL, &rowtextheight);
		//wid->splitPatchButton->GetSize(NULL, &virtheight);

		wid->scrolledPanel->SetSize(width, height);
		if(wid->PlayerTitles.size() == 1) {
			wid->scrolledPanel->SetVirtualSize(boxwidth * 3, boxheight * wid->PlayerTitles.size() + splitupheight + browseheight + 2 + removeheight + 2 + splitpatchheight + 2 + rowtextheight);//size to contain all widgets (dont use coordinates)
		}
		else {
			wid->scrolledPanel->SetVirtualSize((boxwidth * 3) + upwidth + 3 + upwidth, boxheight * wid->PlayerTitles.size() + splitupheight + browseheight + 2 + removeheight + 2 + splitpatchheight + 2 + rowtextheight);//size to contain all widgets (dont use coordinates)
		}
	}
	//buildpanel stuff
	{//titleIDText
		int panelwidth, mywidth, boxwidth, zzwidth, buttwidth;
		wid->buildpanel->GetSize(&panelwidth, NULL);
		wid->titleIDText->GetSize(&mywidth, NULL);
		wid->titleIDBox->GetSize(&boxwidth, NULL);
		wid->zerozero->GetSize(&zzwidth, NULL);
		wid->titleIDButton->GetSize(&buttwidth, NULL);
		
		wid->titleIDText->Move((panelwidth - (mywidth + boxwidth + zzwidth + 3 + buttwidth)) / 2, 15);
	}
	{//titleIDBox
		int x, y, width, height, myheight;
		wid->titleIDText->GetPosition(&x, &y);
		wid->titleIDText->GetSize(&width, &height);
		wid->titleIDBox->GetSize(NULL, &myheight);
		
		wid->titleIDBox->Move(x + width, ((myheight > height) ? y - ((myheight - height) / 2) : y + ((height - myheight) / 2)));
	}
	{//zerozero
		int x, y, width;
		wid->titleIDText->GetPosition(NULL, &y);
		wid->titleIDBox->GetPosition(&x, NULL);
		wid->titleIDBox->GetSize(&width, NULL);
		
		wid->zerozero->Move(x + width, y);
	}
	{//titleIDButton
		int x, y, width, height, myheight;
		wid->zerozero->GetPosition(&x, &y);
		wid->zerozero->GetSize(&width, &height);
		wid->titleIDButton->GetSize(NULL, &myheight);
		
		wid->titleIDButton->Move(x + width + 3, ((myheight > height) ? y - ((myheight - height) / 2) : y + ((height - myheight) / 2)));
	}
	
	{//applicationTitleText
		int x, y, height;
		wid->titleIDButton->GetSize(NULL, &height);//this is probably the largest thing
		wid->titleIDText->GetPosition(&x, NULL);
		wid->titleIDButton->GetPosition(NULL, &y);
		
		wid->applicationTitleText->Move(x, y + height + 10);
	}
	{//applicationTitleBox
		int x, y, width, height, myheight;
		wid->applicationTitleText->GetPosition(&x, &y);
		wid->applicationTitleText->GetSize(&width, &height);
		wid->applicationTitleBox->GetSize(NULL, &myheight);
		
		wid->applicationTitleBox->Move(x + width + 5, ((myheight > height) ? y - ((myheight - height) / 2) : y + ((height - myheight) / 2)));
	}
	{//applicationTitleBox width
		int tidwidth, boxwidth, zzwidth, buttwidth, textwidth, myheight;
		wid->titleIDText->GetSize(&tidwidth, NULL);
		wid->titleIDBox->GetSize(&boxwidth, NULL);
		wid->zerozero->GetSize(&zzwidth, NULL);
		wid->titleIDButton->GetSize(&buttwidth, NULL);
		wid->applicationTitleText->GetSize(&textwidth, NULL);
		wid->applicationTitleBox->GetSize(NULL, &myheight);
		
		wid->applicationTitleBox->SetSize((tidwidth + boxwidth + zzwidth + 3 + buttwidth) - (textwidth + 5), myheight);
	}
	
	{//productCodeText
		int texty, boxy, panelwidth, mywidth, boxwidth, boxheight, textheight;
		wid->buildpanel->GetSize(&panelwidth, NULL);
		wid->productCodeText->GetSize(&mywidth, NULL);
		wid->productCodeBox->GetSize(&boxwidth, NULL);
		wid->applicationTitleText->GetSize(NULL, &textheight);
		wid->applicationTitleBox->GetSize(NULL, &boxheight);
		wid->applicationTitleText->GetPosition(NULL, &texty);
		wid->applicationTitleBox->GetPosition(NULL, &boxy);
		
		wid->productCodeText->Move((panelwidth - (mywidth + boxwidth)) / 2, (texty + textheight) > (boxy + boxheight) ? texty + textheight + 10 : boxy + boxheight + 10);
	}
	{//productCodeBox
		int x, y, width, height, myheight;
		wid->productCodeText->GetPosition(&x, &y);
		wid->productCodeText->GetSize(&width, &height);
		wid->productCodeBox->GetSize(NULL, &myheight);
		
		wid->productCodeBox->Move(x + width, ((myheight > height) ? y - ((myheight - height) / 2) : y + ((height - myheight) / 2)));
	}
	
	{//statusText
		int prodcodey, panelwidth, mywidth, myheight, panelheight, barheight, buildheight, cancelheight, prodcodeheight;
		wid->buildpanel->GetSize(&panelwidth, &panelheight);
		wid->buildBar->GetSize(NULL, &barheight);
		wid->buildButton->GetSize(NULL, &buildheight);
		wid->cancelButton->GetSize(NULL, &cancelheight);
		wid->productCodeText->GetPosition(NULL, &prodcodey);
		wid->productCodeText->GetSize(NULL, &prodcodeheight);
		wid->statusText->GetSize(&mywidth, &myheight);
		if(panelheight > prodcodey + prodcodeheight + 15 + (myheight + 5 + barheight + 10 + buildheight + 2 + cancelheight + 15))
			wid->statusText->Move((panelwidth - mywidth) / 2, panelheight - (myheight + 5 + barheight + 10 + buildheight + 2 + cancelheight + 15));
		else
			wid->statusText->Move((panelwidth - mywidth) / 2, prodcodey + prodcodeheight + 15);
	}
	{//buildBar
		int y, panelwidth, statusheight;
		wid->statusText->GetPosition(NULL, &y);
		wid->statusText->GetSize(NULL, &statusheight);
		wid->buildpanel->GetSize(&panelwidth, NULL);
		
		wid->buildBar->SetSize(panelwidth, 25);
		wid->buildBar->Move(0, y + statusheight + 5);
	}
	{//buildButton
		int y, mywidth, panelwidth, barheight;
		wid->buildpanel->GetSize(&panelwidth, NULL);
		wid->buildBar->GetPosition(NULL, &y);
		wid->buildBar->GetSize(NULL, &barheight);
		wid->buildButton->GetSize(&mywidth, NULL);
		
		wid->buildButton->Move((panelwidth - mywidth) / 2, y + barheight + 10);
	}
	{//cancelButt
		int y, mywidth, panelwidth, buildheight;
		wid->buildpanel->GetSize(&panelwidth, NULL);
		wid->buildButton->GetPosition(NULL, &y);
		wid->buildButton->GetSize(NULL, &buildheight);
		wid->cancelButton->GetSize(&mywidth, NULL);
		
		wid->cancelButton->Move((panelwidth - mywidth) / 2, y + buildheight + 2);
	}
	{//titleLogo
		int width, panelwidth;
		wid->aboutpanel->GetSize(&panelwidth, NULL);
		wid->titleLogo->GetSize(&width, NULL);
		
		wid->titleLogo->Move((panelwidth - width) / 2, 20);
	}
	{//byMeText
		int x, y, mywidth, titlewidth, titleheight;
		wid->titleLogo->GetSize(&titlewidth, &titleheight);
		wid->titleLogo->GetPosition(&x, &y);
		wid->byMeText->GetSize(&mywidth, NULL);
		
		wid->byMeText->Move(x + ((titlewidth - mywidth) / 2), y + titleheight);
	}
	{//gitHubLinker
		int y, mywidth, myheight, bymeheight, panelwidth, panelheight, versionheight;
		wid->aboutpanel->GetSize(&panelwidth, &panelheight);
		wid->byMeText->GetPosition(NULL, &y);
		wid->byMeText->GetSize(NULL, &bymeheight);
		wid->versionText->GetSize(NULL, &versionheight);
		wid->gitHubLinker->GetSize(&mywidth, &myheight);
		
		if(panelheight > y + bymeheight + myheight + versionheight)
			wid->gitHubLinker->Move((panelwidth - mywidth) / 2, y + bymeheight + (((panelheight - (y + bymeheight)) - myheight - versionheight) / 2));
		else
			wid->gitHubLinker->Move((panelwidth - mywidth) / 2, y + bymeheight);
	}
	{//versionText
		int y, mywidth, myheight, bymeheight, panelwidth, panelheight, linkheight;
		wid->aboutpanel->GetSize(&panelwidth, &panelheight);
		wid->byMeText->GetPosition(NULL, &y);
		wid->byMeText->GetSize(NULL, &bymeheight);
		wid->gitHubLinker->GetSize(NULL, &linkheight);
		wid->aboutpanel->GetSize(&panelwidth, &panelheight);
		wid->versionText->GetSize(&mywidth, &myheight);
		
		if(panelheight > y + bymeheight + linkheight + myheight)
			wid->versionText->Move((panelwidth - mywidth) / 2, panelheight - myheight);
		else
			wid->versionText->Move((panelwidth - mywidth) / 2, y + bymeheight + linkheight);
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

void getAppearance(InitWidgets* wid) {
	BackColor::panel = wid->panel->GetBackgroundColour();
	BackColor::mainMenu = wid->mainMenu->GetBackgroundColour();
	BackColor::modeText = wid->modeText->GetBackgroundColour();
	BackColor::modeChoiceBox = wid->modeChoiceBox->GetBackgroundColour();
	BackColor::bannerText = wid->bannerText->GetBackgroundColour();
	BackColor::bannerBox = wid->bannerBox->GetBackgroundColour();
	BackColor::bannerBrowse = wid->bannerBrowse->GetBackgroundColour();
	BackColor::bannerError = wid->bannerError->GetBackgroundColour();
	BackColor::iconText = wid->iconText->GetBackgroundColour();
	BackColor::iconBox = wid->iconBox->GetBackgroundColour();
	BackColor::iconBrowse = wid->iconBrowse->GetBackgroundColour();
	BackColor::iconError = wid->iconError->GetBackgroundColour();
	BackColor::shortnameText = wid->shortnameText->GetBackgroundColour();
	BackColor::shortnameBox = wid->shortnameBox->GetBackgroundColour();
	BackColor::shortnameError = wid->shortnameError->GetBackgroundColour();
	BackColor::longnameText = wid->longnameText->GetBackgroundColour();
	BackColor::longnameBox = wid->longnameBox->GetBackgroundColour();
	BackColor::longnameError = wid->longnameError->GetBackgroundColour();
	BackColor::publisherText = wid->publisherText->GetBackgroundColour();
	BackColor::publisherBox = wid->publisherBox->GetBackgroundColour();
	BackColor::publisherError = wid->publisherError->GetBackgroundColour();
	BackColor::copyBox = wid->copyBox->GetBackgroundColour();
	BackColor::copyCheck = wid->copyCheck->GetBackgroundColour();
	BackColor::bannerCustomText = wid->bannerCustomText->GetBackgroundColour();
	BackColor::bannerPreviewText = wid->bannerPreviewText->GetBackgroundColour();
	BackColor::iconPreview = wid->iconPreview->GetBackgroundColour();
	BackColor::ffRewindCheck = wid->ffRewindCheck->GetBackgroundColour();
	BackColor::dimCheck = wid->dimCheck->GetBackgroundColour();
	BackColor::multiBannerPreview = wid->multiBannerPreview->GetBackgroundColour();
	BackColor::multiBannerPreviewIndex = wid->multiBannerPreviewIndex->GetBackgroundColour();
	BackColor::multiBannerPreviewLeft = wid->multiBannerPreviewLeft->GetBackgroundColour();
	BackColor::multiBannerPreviewRight = wid->multiBannerPreviewRight->GetBackgroundColour();
	BackColor::playerTitleText = wid->playerTitleText->GetBackgroundColour();
	BackColor::moflexFileText = wid->moflexFileText->GetBackgroundColour();
	BackColor::menuBannerText = wid->menuBannerText->GetBackgroundColour();
	BackColor::mediaPanel = wid->mediaPanel->GetBackgroundColour();
	BackColor::moflexBrowse = wid->moflexBrowse->GetBackgroundColour();
	BackColor::multiBannerBrowse = wid->multiBannerBrowse->GetBackgroundColour();
	BackColor::removeRow = wid->removeRow->GetBackgroundColour();
	BackColor::appendRow = wid->appendRow->GetBackgroundColour();
	BackColor::splitPatchButton = wid->splitPatchButton->GetBackgroundColour();
	BackColor::rowText = wid->rowText->GetBackgroundColour();
	BackColor::splitPatchUp = wid->splitPatchUp->GetBackgroundColour();
	BackColor::splitPatchDown = wid->splitPatchDown->GetBackgroundColour();
	BackColor::buildpanel = wid->buildpanel->GetBackgroundColour();
	BackColor::titleIDText = wid->titleIDText->GetBackgroundColour();
	BackColor::titleIDBox = wid->titleIDBox->GetBackgroundColour();
	BackColor::zerozero = wid->zerozero->GetBackgroundColour();
	BackColor::titleIDButton = wid->titleIDButton->GetBackgroundColour();
	BackColor::applicationTitleText = wid->applicationTitleText->GetBackgroundColour();
	BackColor::applicationTitleBox = wid->applicationTitleBox->GetBackgroundColour();
	BackColor::productCodeText = wid->productCodeText->GetBackgroundColour();
	BackColor::productCodeBox = wid->productCodeBox->GetBackgroundColour();
	BackColor::statusText = wid->statusText->GetBackgroundColour();
	//BackColor::buildBar = wid->buildBar->GetBackgroundColour();
	BackColor::buildButton = wid->buildButton->GetBackgroundColour();
	BackColor::cancelButton = wid->cancelButton->GetBackgroundColour();
	BackColor::aboutpanel = wid->aboutpanel->GetBackgroundColour();
	BackColor::byMeText = wid->byMeText->GetBackgroundColour();
	BackColor::versionText = wid->versionText->GetBackgroundColour();
	
	ForeColor::panel = wid->panel->GetForegroundColour();
	ForeColor::mainMenu = wid->mainMenu->GetForegroundColour();
	ForeColor::modeText = wid->modeText->GetForegroundColour();
	ForeColor::modeChoiceBox = wid->modeChoiceBox->GetForegroundColour();
	ForeColor::bannerText = wid->bannerText->GetForegroundColour();
	ForeColor::bannerBox = wid->bannerBox->GetForegroundColour();
	ForeColor::bannerBrowse = wid->bannerBrowse->GetForegroundColour();
	ForeColor::bannerError = wid->bannerError->GetForegroundColour();
	ForeColor::iconText = wid->iconText->GetForegroundColour();
	ForeColor::iconBox = wid->iconBox->GetForegroundColour();
	ForeColor::iconBrowse = wid->iconBrowse->GetForegroundColour();
	ForeColor::iconError = wid->iconError->GetForegroundColour();
	ForeColor::shortnameText = wid->shortnameText->GetForegroundColour();
	ForeColor::shortnameBox = wid->shortnameBox->GetForegroundColour();
	ForeColor::shortnameError = wid->shortnameError->GetForegroundColour();
	ForeColor::longnameText = wid->longnameText->GetForegroundColour();
	ForeColor::longnameBox = wid->longnameBox->GetForegroundColour();
	ForeColor::longnameError = wid->longnameError->GetForegroundColour();
	ForeColor::publisherText = wid->publisherText->GetForegroundColour();
	ForeColor::publisherBox = wid->publisherBox->GetForegroundColour();
	ForeColor::publisherError = wid->publisherError->GetForegroundColour();
	ForeColor::copyBox = wid->copyBox->GetForegroundColour();
	ForeColor::copyCheck = wid->copyCheck->GetForegroundColour();
	ForeColor::bannerCustomText = wid->bannerCustomText->GetForegroundColour();
	ForeColor::bannerPreviewText = wid->bannerPreviewText->GetForegroundColour();
	ForeColor::iconPreview = wid->iconPreview->GetForegroundColour();
	ForeColor::ffRewindCheck = wid->ffRewindCheck->GetForegroundColour();
	ForeColor::dimCheck = wid->dimCheck->GetForegroundColour();
	ForeColor::multiBannerPreview = wid->multiBannerPreview->GetForegroundColour();
	ForeColor::multiBannerPreviewIndex = wid->multiBannerPreviewIndex->GetForegroundColour();
	ForeColor::multiBannerPreviewLeft = wid->multiBannerPreviewLeft->GetForegroundColour();
	ForeColor::multiBannerPreviewRight = wid->multiBannerPreviewRight->GetForegroundColour();
	ForeColor::playerTitleText = wid->playerTitleText->GetForegroundColour();
	ForeColor::moflexFileText = wid->moflexFileText->GetForegroundColour();
	ForeColor::menuBannerText = wid->menuBannerText->GetForegroundColour();
	ForeColor::mediaPanel = wid->mediaPanel->GetForegroundColour();
	ForeColor::moflexBrowse = wid->moflexBrowse->GetForegroundColour();
	ForeColor::multiBannerBrowse = wid->multiBannerBrowse->GetForegroundColour();
	ForeColor::removeRow = wid->removeRow->GetForegroundColour();
	ForeColor::appendRow = wid->appendRow->GetForegroundColour();
	ForeColor::splitPatchButton = wid->splitPatchButton->GetForegroundColour();
	ForeColor::rowText = wid->rowText->GetForegroundColour();
	ForeColor::splitPatchUp = wid->splitPatchUp->GetForegroundColour();
	ForeColor::splitPatchDown = wid->splitPatchDown->GetForegroundColour();
	ForeColor::buildpanel = wid->buildpanel->GetForegroundColour();
	ForeColor::titleIDText = wid->titleIDText->GetForegroundColour();
	ForeColor::titleIDBox = wid->titleIDBox->GetForegroundColour();
	ForeColor::zerozero = wid->zerozero->GetForegroundColour();
	ForeColor::titleIDButton = wid->titleIDButton->GetForegroundColour();
	ForeColor::applicationTitleText = wid->applicationTitleText->GetForegroundColour();
	ForeColor::applicationTitleBox = wid->applicationTitleBox->GetForegroundColour();
	ForeColor::productCodeText = wid->productCodeText->GetForegroundColour();
	ForeColor::productCodeBox = wid->productCodeBox->GetForegroundColour();
	ForeColor::statusText = wid->statusText->GetForegroundColour();
	//ForeColor::buildBar = wid->buildBar->GetForegroundColour();
	ForeColor::buildButton = wid->buildButton->GetForegroundColour();
	ForeColor::cancelButton = wid->cancelButton->GetForegroundColour();
	ForeColor::aboutpanel = wid->aboutpanel->GetForegroundColour();
	ForeColor::byMeText = wid->byMeText->GetForegroundColour();
	ForeColor::versionText = wid->versionText->GetForegroundColour();
}

void setAppearance(InitWidgets* wid, int Mode) {
	if(Mode < 2) {//0 light 1 black
		wid->panel->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->mainMenu->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->modeText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->modeChoiceBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->bannerText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->bannerBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->bannerBrowse->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->bannerError->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->iconText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->iconBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->iconBrowse->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->iconError->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->shortnameText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->shortnameBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->shortnameError->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->longnameText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->longnameBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->longnameError->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->publisherText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->publisherBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->publisherError->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->copyBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->copyCheck->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->bannerCustomText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->bannerPreviewText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->iconPreview->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->ffRewindCheck->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->dimCheck->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->multiBannerPreview->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->multiBannerPreviewIndex->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->multiBannerPreviewLeft->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->multiBannerPreviewRight->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->playerTitleText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->moflexFileText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->menuBannerText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->mediaPanel->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->moflexBrowse->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->multiBannerBrowse->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->removeRow->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->appendRow->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->splitPatchButton->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->rowText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->splitPatchUp->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->splitPatchDown->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->buildpanel->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->titleIDText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->titleIDBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->zerozero->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->titleIDButton->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->applicationTitleText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->applicationTitleBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->productCodeText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->productCodeBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->statusText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		//wid->buildBar->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->buildButton->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->cancelButton->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->aboutpanel->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->byMeText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->versionText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		
		wid->splitPatchLine->SetColour(*(Mode ? wxWHITE : wxBLACK));
		wid->panel->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->mainMenu->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->modeText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->modeChoiceBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->bannerText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->bannerBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->bannerBrowse->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->bannerError->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->iconText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->iconBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->iconBrowse->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->iconError->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->shortnameText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->shortnameBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->shortnameError->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->longnameText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->longnameBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->longnameError->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->publisherText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->publisherBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->publisherError->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->copyBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->copyCheck->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->bannerCustomText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->bannerPreviewText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->iconPreview->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->ffRewindCheck->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->dimCheck->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->multiBannerPreview->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->multiBannerPreviewIndex->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->multiBannerPreviewLeft->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->multiBannerPreviewRight->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->playerTitleText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->moflexFileText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->menuBannerText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->mediaPanel->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->moflexBrowse->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->multiBannerBrowse->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->removeRow->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->appendRow->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->splitPatchButton->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->rowText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->splitPatchUp->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->splitPatchDown->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->buildpanel->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->titleIDText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->titleIDBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->zerozero->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->titleIDButton->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->applicationTitleText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->applicationTitleBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->productCodeText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->productCodeBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->statusText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		//wid->buildBar->SetForegroundColour(*(Mode ? wxBLACK : wxWHITE));//uhhhhhhh
		wid->buildButton->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->cancelButton->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->aboutpanel->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->byMeText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->versionText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		
		{
			wxColor LightBlack = wxBLACK->GetRGB() + 0x141414;
			wxColor DarkWhite = wxWHITE->GetRGB() - 0x141414;
			
			int rows = 0;
			for(auto &row : wid->PlayerTitles) {
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
			for(auto &row : wid->MoflexFiles) {
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
			for(auto &row : wid->MenuBanners) {
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
		
		for(auto &row : wid->MultiUp) {
			row->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
			row->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		}
		for(auto &row : wid->MultiDown) {
			row->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
			row->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		}
	}
	else if(Mode == 2) {
		wid->panel->SetBackgroundColour(BackColor::panel);
		wid->mainMenu->SetBackgroundColour(BackColor::mainMenu);
		wid->modeText->SetBackgroundColour(BackColor::modeText);
		wid->modeChoiceBox->SetBackgroundColour(BackColor::modeChoiceBox);
		wid->bannerText->SetBackgroundColour(BackColor::bannerText);
		wid->bannerBox->SetBackgroundColour(BackColor::bannerBox);
		wid->bannerBrowse->SetBackgroundColour(BackColor::bannerBrowse);
		wid->bannerError->SetBackgroundColour(BackColor::bannerError);
		wid->iconText->SetBackgroundColour(BackColor::iconText);
		wid->iconBox->SetBackgroundColour(BackColor::iconBox);
		wid->iconBrowse->SetBackgroundColour(BackColor::iconBrowse);
		wid->iconError->SetBackgroundColour(BackColor::iconError);
		wid->shortnameText->SetBackgroundColour(BackColor::shortnameText);
		wid->shortnameBox->SetBackgroundColour(BackColor::shortnameBox);
		wid->shortnameError->SetBackgroundColour(BackColor::shortnameError);
		wid->longnameText->SetBackgroundColour(BackColor::longnameText);
		wid->longnameBox->SetBackgroundColour(BackColor::longnameBox);
		wid->longnameError->SetBackgroundColour(BackColor::longnameError);
		wid->publisherText->SetBackgroundColour(BackColor::publisherText);
		wid->publisherBox->SetBackgroundColour(BackColor::publisherBox);
		wid->publisherError->SetBackgroundColour(BackColor::publisherError);
		wid->copyBox->SetBackgroundColour(BackColor::copyBox);
		wid->copyCheck->SetBackgroundColour(BackColor::copyCheck);
		wid->bannerCustomText->SetBackgroundColour(BackColor::bannerCustomText);
		wid->bannerPreviewText->SetBackgroundColour(BackColor::bannerPreviewText);
		wid->iconPreview->SetBackgroundColour(BackColor::iconPreview);
		wid->ffRewindCheck->SetBackgroundColour(BackColor::ffRewindCheck);
		wid->dimCheck->SetBackgroundColour(BackColor::dimCheck);
		wid->multiBannerPreview->SetBackgroundColour(BackColor::multiBannerPreview);
		wid->multiBannerPreviewIndex->SetBackgroundColour(BackColor::multiBannerPreviewIndex);
		wid->multiBannerPreviewLeft->SetBackgroundColour(BackColor::multiBannerPreviewLeft);
		wid->multiBannerPreviewRight->SetBackgroundColour(BackColor::multiBannerPreviewRight);
		wid->playerTitleText->SetBackgroundColour(BackColor::playerTitleText);
		wid->moflexFileText->SetBackgroundColour(BackColor::moflexFileText);
		wid->menuBannerText->SetBackgroundColour(BackColor::menuBannerText);
		wid->mediaPanel->SetBackgroundColour(BackColor::mediaPanel);
		wid->moflexBrowse->SetBackgroundColour(BackColor::moflexBrowse);
		wid->multiBannerBrowse->SetBackgroundColour(BackColor::multiBannerBrowse);
		wid->removeRow->SetBackgroundColour(BackColor::removeRow);
		wid->appendRow->SetBackgroundColour(BackColor::appendRow);
		wid->splitPatchButton->SetBackgroundColour(BackColor::splitPatchButton);
		wid->rowText->SetBackgroundColour(BackColor::rowText);
		wid->splitPatchUp->SetBackgroundColour(BackColor::splitPatchUp);
		wid->splitPatchDown->SetBackgroundColour(BackColor::splitPatchDown);
		wid->buildpanel->SetBackgroundColour(BackColor::buildpanel);
		wid->titleIDText->SetBackgroundColour(BackColor::titleIDText);
		wid->titleIDBox->SetBackgroundColour(BackColor::titleIDBox);
		wid->zerozero->SetBackgroundColour(BackColor::zerozero);
		wid->titleIDButton->SetBackgroundColour(BackColor::titleIDButton);
		wid->applicationTitleText->SetBackgroundColour(BackColor::applicationTitleText);
		wid->applicationTitleBox->SetBackgroundColour(BackColor::applicationTitleBox);
		wid->productCodeText->SetBackgroundColour(BackColor::productCodeText);
		wid->productCodeBox->SetBackgroundColour(BackColor::productCodeBox);
		wid->statusText->SetBackgroundColour(BackColor::statusText);
		//wid->buildBar->SetBackgroundColour(BackColor::buildBar);
		wid->buildButton->SetBackgroundColour(BackColor::buildButton);
		wid->cancelButton->SetBackgroundColour(BackColor::cancelButton);
		wid->aboutpanel->SetBackgroundColour(BackColor::aboutpanel);
		wid->byMeText->SetBackgroundColour(BackColor::byMeText);
		wid->versionText->SetBackgroundColour(BackColor::versionText);
		
		wid->panel->SetForegroundColour(ForeColor::panel);
		wid->mainMenu->SetForegroundColour(ForeColor::mainMenu);
		wid->modeText->SetForegroundColour(ForeColor::modeText);
		wid->modeChoiceBox->SetForegroundColour(ForeColor::modeChoiceBox);
		wid->bannerText->SetForegroundColour(ForeColor::bannerText);
		wid->bannerBox->SetForegroundColour(ForeColor::bannerBox);
		wid->bannerBrowse->SetForegroundColour(ForeColor::bannerBrowse);
		wid->bannerError->SetForegroundColour(ForeColor::bannerError);
		wid->iconText->SetForegroundColour(ForeColor::iconText);
		wid->iconBox->SetForegroundColour(ForeColor::iconBox);
		wid->iconBrowse->SetForegroundColour(ForeColor::iconBrowse);
		wid->iconError->SetForegroundColour(ForeColor::iconError);
		wid->shortnameText->SetForegroundColour(ForeColor::shortnameText);
		wid->shortnameBox->SetForegroundColour(ForeColor::shortnameBox);
		wid->shortnameError->SetForegroundColour(ForeColor::shortnameError);
		wid->longnameText->SetForegroundColour(ForeColor::longnameText);
		wid->longnameBox->SetForegroundColour(ForeColor::longnameBox);
		wid->longnameError->SetForegroundColour(ForeColor::longnameError);
		wid->publisherText->SetForegroundColour(ForeColor::publisherText);
		wid->publisherBox->SetForegroundColour(ForeColor::publisherBox);
		wid->publisherError->SetForegroundColour(ForeColor::publisherError);
		wid->copyBox->SetForegroundColour(ForeColor::copyBox);
		wid->copyCheck->SetForegroundColour(ForeColor::copyCheck);
		wid->bannerCustomText->SetForegroundColour(ForeColor::bannerCustomText);
		wid->bannerPreviewText->SetForegroundColour(ForeColor::bannerPreviewText);
		wid->iconPreview->SetForegroundColour(ForeColor::iconPreview);
		wid->ffRewindCheck->SetForegroundColour(ForeColor::ffRewindCheck);
		wid->dimCheck->SetForegroundColour(ForeColor::dimCheck);
		wid->multiBannerPreview->SetForegroundColour(ForeColor::multiBannerPreview);
		wid->multiBannerPreviewIndex->SetForegroundColour(ForeColor::multiBannerPreviewIndex);
		wid->multiBannerPreviewLeft->SetForegroundColour(ForeColor::multiBannerPreviewLeft);
		wid->multiBannerPreviewRight->SetForegroundColour(ForeColor::multiBannerPreviewRight);
		wid->playerTitleText->SetForegroundColour(ForeColor::playerTitleText);
		wid->moflexFileText->SetForegroundColour(ForeColor::moflexFileText);
		wid->menuBannerText->SetForegroundColour(ForeColor::menuBannerText);
		wid->mediaPanel->SetForegroundColour(ForeColor::mediaPanel);
		wid->moflexBrowse->SetForegroundColour(ForeColor::moflexBrowse);
		wid->multiBannerBrowse->SetForegroundColour(ForeColor::multiBannerBrowse);
		wid->removeRow->SetForegroundColour(ForeColor::removeRow);
		wid->appendRow->SetForegroundColour(ForeColor::appendRow);
		wid->splitPatchButton->SetForegroundColour(ForeColor::splitPatchButton);
		wid->rowText->SetForegroundColour(ForeColor::rowText);
		wid->splitPatchLine->SetColour(ForeColor::panel);
		wid->splitPatchUp->SetForegroundColour(ForeColor::splitPatchUp);
		wid->splitPatchDown->SetForegroundColour(ForeColor::splitPatchDown);
		wid->buildpanel->SetForegroundColour(ForeColor::buildpanel);
		wid->titleIDText->SetForegroundColour(ForeColor::titleIDText);
		wid->titleIDBox->SetForegroundColour(ForeColor::titleIDBox);
		wid->zerozero->SetForegroundColour(ForeColor::zerozero);
		wid->titleIDButton->SetForegroundColour(ForeColor::titleIDButton);
		wid->applicationTitleText->SetForegroundColour(ForeColor::applicationTitleText);
		wid->applicationTitleBox->SetForegroundColour(ForeColor::applicationTitleBox);
		wid->productCodeText->SetForegroundColour(ForeColor::productCodeText);
		wid->productCodeBox->SetForegroundColour(ForeColor::productCodeBox);
		wid->statusText->SetForegroundColour(ForeColor::statusText);
		//wid->buildBar->SetForegroundColour(ForeColor::buildBar);
		wid->buildButton->SetForegroundColour(ForeColor::buildButton);
		wid->cancelButton->SetForegroundColour(ForeColor::cancelButton);
		wid->aboutpanel->SetForegroundColour(ForeColor::aboutpanel);
		wid->byMeText->SetForegroundColour(ForeColor::byMeText);
		wid->versionText->SetForegroundColour(ForeColor::versionText);
		
		{
			wxColor BackOdd = (BackColor::bannerBox.GetRGB() < 0x7F7F7F) ? (BackColor::bannerBox.GetRGB() + 0x141414) : (BackColor::bannerBox.GetRGB() - 0x141414);//FF/2=7F
			//wxColor ForeOdd = (ForeColor::bannerBox.GetRGB() < 0x7F7F7F) ? (ForeColor::bannerBox.GetRGB() + 0x141414) : (ForeColor::bannerBox.GetRGB() - 0x141414);
			
			int rows = 0;
			for(auto &row : wid->PlayerTitles) {
				if(rows % 2) {
					row->SetBackgroundColour(BackOdd);
				}
				else {
					row->SetBackgroundColour(BackColor::bannerBox);//not worth getting the color from these actual boxes
				}
				row->SetForegroundColour(ForeColor::bannerBox);
				++rows;
			}
			rows = 0;
			for(auto &row : wid->MoflexFiles) {
				if(rows % 2) {
					row->SetBackgroundColour(BackOdd);
				}
				else {
					row->SetBackgroundColour(BackColor::bannerBox);//not worth getting the color from these actual boxes
				}
				row->SetForegroundColour(ForeColor::bannerBox);
				++rows;
			}
			rows = 0;
			for(auto &row : wid->MenuBanners) {
				if(rows % 2) {
					row->SetBackgroundColour(BackOdd);
				}
				else {
					row->SetBackgroundColour(BackColor::bannerBox);//not worth getting the color from these actual boxes
				}
				row->SetForegroundColour(ForeColor::bannerBox);
				++rows;
			}
		}
		
		for(auto &row : wid->MultiUp) {
			row->SetBackgroundColour(BackColor::splitPatchUp);
			row->SetForegroundColour(ForeColor::splitPatchUp);
		}
		for(auto &row : wid->MultiDown) {
			row->SetBackgroundColour(BackColor::splitPatchDown);
			row->SetForegroundColour(ForeColor::splitPatchDown);
		}
	}
	{//titleLogo
		int width, height;
		wxImage top;
		wxImage bottom;
		
		
		top.LoadFile(wxString::FromUTF8(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + imagePath + "/title_top.png"));
		bottom.LoadFile(wxString::FromUTF8(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + imagePath + "/title_bottom.png"));
		
		width = top.GetWidth();
		height = top.GetHeight();
		//maybe check bottom size? oh well they are supposed to be the same
		
		wxImage back(width, height);//LOOK LOOK IM RIGHT HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!
		
		wxBrush MY_BRUSH(((Settings::ColorMode < 2) ? *(Settings::ColorMode ? wxBLACK : wxWHITE) : wid->aboutpanel->GetBackgroundColour()));
		//wxBrush MY_BRUSH(BackColor::aboutpanel);//what why not?
		
		
		//https://stackoverflow.com/a/35179780
		wxBitmap bitmap(back);
		wxMemoryDC memdc(bitmap);
		memdc.SetBackground(MY_BRUSH);
		memdc.Clear();    //fills the entire bitmap with green colour
		memdc.SelectObject(wxNullBitmap);
		back = bitmap.ConvertToImage();    //optionally
		back.InitAlpha();
		
		for(int y = 0; y < height; y++)
			for(int x = 0; x < width; x++) {
				uint8_t r1 = bottom.GetRed(x, y);
				uint8_t g1 = bottom.GetGreen(x, y);
				uint8_t b1 = bottom.GetBlue(x, y);
				uint8_t a1 = bottom.GetAlpha(x, y);
				
				uint8_t r2 = back.GetRed(x, y);
				uint8_t g2 = back.GetGreen(x, y);
				uint8_t b2 = back.GetBlue(x, y);
				
				back.SetRGB(x, y, r1 - r2, g1 - g2, b1 - b2);//difference blend
				back.SetAlpha(x, y, a1);
			}
		
		back.Paste(top, 0, 0, wxIMAGE_ALPHA_BLEND_COMPOSE);
		
		wid->titleLogo->SetSize(width, height);
		wid->titleLogo->SetBitmap(wxBitmap(back));
	}
	wid->panel->Refresh();
	wid->buildpanel->Refresh();
	wid->aboutpanel->Refresh();
}

int loadParameters(InitWidgets* wid, VI9Pparameters* parameters) {
	VI9P::Loading = true;
	int ret = 0;
	{//-rr
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -rr \"" + VI9P::WorkingFile + '\"');
		ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		
		if(ret > -1) {
			parameters->rows = ret & 0xFF;
		}
		else return ret;
	}
	{//-pp
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -pp \"" + VI9P::WorkingFile + '\"');
		ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		std::string pp;
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
			pp += std::string(s.ToUTF8()) + '\n';
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		
		{//mode
			std::string value = "";
			int outnum = 0;
			parsePP(pp.c_str(), IntMultiParam, &value);
			if(!ASCII2number<int>(&outnum, value)) {
				//tbh this should never ever happen
				parameters->mode = 0;
			}
			else {
				parameters->mode = (outnum ? 1 : 0);
			}
		}
		{//banner
			std::string value = "";
			parsePP(pp.c_str(), StrBannerParam, &value);
			parameters->banner = value;
		}
		{//icon
			std::string value = "";
			parsePP(pp.c_str(), StrIconParam, &value);
			parameters->icon = value;
		}
		{//iconBorder
			std::string value = "";
			int outnum = 0;
			parsePP(pp.c_str(), IntIconBorderParam, &value);
			if(!ASCII2number<int>(&outnum, value)) {
				//tbh this should never ever happen
				parameters->iconBorder = 2;
			}
			else {
				parameters->iconBorder = ((outnum > 2) ? 0 : outnum);
			}
		}
		{//Sname
			std::string value = "";
			parsePP(pp.c_str(), StrSNameParam, &value);
			parameters->Sname = value;
		}
		{//Lname
			std::string value = "";
			parsePP(pp.c_str(), StrLNameParam, &value);
			parameters->Lname = value;
		}
		{//publisher
			std::string value = "";
			parsePP(pp.c_str(), StrPublisherParam, &value);
			parameters->publisher = value;
		}
		{//copycheck
			std::string value = "";
			int outnum = 0;
			parsePP(pp.c_str(), IntCopycheckParam, &value);
			if(!ASCII2number<int>(&outnum, value)) {
				//tbh this should never ever happen
				parameters->copycheck = 0;
			}
			else {
				parameters->copycheck = (outnum ? 1 : 0);
			}
		}
		{//copyrightInfo
			std::string value = "";
			parsePP(pp.c_str(), StrCopyrightParam, &value);
			parameters->copyrightInfo = value;
		}
		{//FFrewind
			std::string value = "";
			int outnum = 0;
			parsePP(pp.c_str(), IntFFrewindParam, &value);
			if(!ASCII2number<int>(&outnum, value)) {
				//tbh this should never ever happen
				parameters->FFrewind = 1;
			}
			else {
				parameters->FFrewind = (outnum ? 1 : 0);
			}
		}
		{//FadeOpt
			std::string value = "";
			int outnum = 0;
			parsePP(pp.c_str(), IntFadeOptParam, &value);
			if(!ASCII2number<int>(&outnum, value)) {
				//tbh this should never ever happen
				parameters->FadeOpt = 1;
			}
			else {
				parameters->FadeOpt = (outnum ? 1 : 0);
			}
		}
		{//splitPos
			std::string value = "";
			int outnum = 0;
			parsePP(pp.c_str(), IntSplitPatchParam, &value);
			if(!ASCII2number<int>(&outnum, value)) {
				//tbh this should never ever happen
				parameters->splitPos = 0;
			}
			else {
				parameters->splitPos = ((outnum < parameters->rows - 1) ? outnum : parameters->rows - 1);
			}
		}
		{//PTitleVec
			parameters->PTitleVec.clear();
			for(uint8_t i = 0; i < parameters->rows; i++) {
				std::string value = "";
				parsePP(pp.c_str(), StrPTitleParam + '(' + std::to_string(i) + ')', &value);
				parameters->PTitleVec.push_back(value);
			}
		}
		{//StrMoflexParam
			parameters->MoflexVec.clear();
			for(uint8_t i = 0; i < parameters->rows; i++) {
				std::string value = "";
				parsePP(pp.c_str(), StrMoflexParam + '(' + std::to_string(i) + ')', &value);
				parameters->MoflexVec.push_back(value);
			}
		}
		{//StrMBannerParam
			parameters->MBannerVec.clear();
			for(uint8_t i = 0; i < parameters->rows; i++) {
				std::string value = "";
				parsePP(pp.c_str(), StrMBannerParam + '(' + std::to_string(i) + ')', &value);
				parameters->MBannerVec.push_back(value);
			}
		}
	}
	applyParameters(wid, parameters);
	VI9P::Loading = false;
	return ret;
}

void applyMode(InitWidgets* wid, VI9Pparameters* parameters) {
	if(parameters->mode) {//the main stuff that changes base on what mode youre in
		wid->copyBox->Enable(wid->copyCheck->GetValue());
		wid->copyCheck->Enable(true);
		wid->rowText->Show(true);
		wid->multiBannerPreview->Enable(true);
		wid->multiBannerPreviewIndex->Show(true);
		
		EnableBannerLeftRight(wid);
		
		if(parameters->rows > 1 && parameters->rows < MAX_ROWS) {
			wid->appendRow->Enable(true);
			wid->removeRow->Enable(true);
			wid->splitPatchButton->Enable(true);
		}
		if(parameters->rows <= 1) {
			wid->appendRow->Enable(true);
			wid->removeRow->Enable(false);
			wid->splitPatchButton->Enable(false);
		}
		else if(parameters->rows >= MAX_ROWS) {
			wid->appendRow->Enable(false);
			wid->removeRow->Enable(true);
			wid->splitPatchButton->Enable(true);
		}
		for(const auto &row : wid->PlayerTitles) {
			row->Enable(true);
		}
		for(const auto &row : wid->MoflexFiles) {
			row->Enable(true);
		}
		for(const auto &row : wid->MenuBanners) {
			row->Enable(true);
		}

		wid->multiBannerBrowse->Enable(true);
	}
	else {
		wid->copyBox->Enable(false);
		wid->copyCheck->Enable(false);
		wid->rowText->Show(false);
		wid->multiBannerPreview->Enable(false);
		wid->multiBannerPreviewIndex->Show(false);
		wid->multiBannerPreviewLeft->Enable(false);
		wid->multiBannerPreviewRight->Enable(false);
		if(parameters->rows > 1) {
			wid->removeRow->Enable(true);
			wid->splitPatchButton->Enable(true);
		}
		else {
			wid->removeRow->Enable(false);
			wid->splitPatchButton->Enable(false);
		}
		wid->appendRow->Enable(false);
		bool first = true;//lol
		for(const auto &row : wid->PlayerTitles) {
			if(!first) {
				row->Enable(false);
			}
			first = false;
		}
		first = true;
		for(const auto &row : wid->MoflexFiles) {
			if(!first) {
				row->Enable(false);
			}
			first = false;
		}
		for(const auto &row : wid->MenuBanners) {
			row->Enable(false);
		}
		wid->multiBannerBrowse->Enable(false);
	}
	setCursors(wid);
}

void applyParameters(InitWidgets* wid, VI9Pparameters* parameters) {
	wid->modeChoiceBox->SetSelection(parameters->mode);

	wid->bannerBox->SetValue(wxString::FromUTF8(parameters->banner));

	wid->iconBox->SetValue(wxString::FromUTF8(parameters->icon));

	wid->shortnameBox->SetValue(wxString::FromUTF8(parameters->Sname));
	wid->longnameBox->SetValue(wxString::FromUTF8(parameters->Lname));
	wid->publisherBox->SetValue(wxString::FromUTF8(parameters->publisher));

	wid->copyCheck->SetValue(parameters->copycheck);
	wid->copyBox->SetValue(wxString::FromUTF8(parameters->copyrightInfo));

	wid->ffRewindCheck->SetValue(parameters->FFrewind);
	wid->dimCheck->SetValue(parameters->FadeOpt);

	if(wid->PlayerTitles.size() < parameters->rows) {
		int count = parameters->rows - wid->PlayerTitles.size();
		if(wid->PlayerTitles.size() + count <= 28) {
			doAddRows(wid, count);
		}
	}
	else if(wid->PlayerTitles.size() > parameters->rows) {
		int count = wid->PlayerTitles.size() - parameters->rows;
		if(wid->PlayerTitles.size() - count >= 0) {
			for(uint8_t i = 0; i < count; i++) {
				wid->PlayerTitles.back()->Destroy();
				wid->PlayerTitles.pop_back();
			}
			for(uint8_t i = 0; i < count; i++) {
				wid->MoflexFiles.back()->Destroy();
				wid->MoflexFiles.pop_back();
			}
			for(uint8_t i = 0; i < count; i++) {
				wid->MenuBanners.back()->Destroy();
				wid->MenuBanners.pop_back();
			}
			for(uint8_t i = 0; i < count; i++) {
				wid->MultiUp.back()->Destroy();
				wid->MultiUp.pop_back();
			}
			for(uint8_t i = 0; i < count; i++) {
				wid->MultiDown.back()->Destroy();
				wid->MultiDown.pop_back();
			}
		}
	}
	
	for(int row = 0; row < parameters->PTitleVec.size(); row++) {
		wid->PlayerTitles.at(row)->SetValue(wxString::FromUTF8(parameters->PTitleVec.at(row)));
	}
	for(int row = 0; row < parameters->MoflexVec.size(); row++) {
		wid->MoflexFiles.at(row)->SetValue(wxString::FromUTF8(parameters->MoflexVec.at(row)));
	}
	for(int row = 0; row < parameters->MBannerVec.size(); row++) {
		wid->MenuBanners.at(row)->SetValue(wxString::FromUTF8(parameters->MBannerVec.at(row)));
	}
	
	wid->splitPatchButton->SetValue(parameters->splitPos ? 1 : 0);
	
	applyMode(wid, parameters);
}

void addRows(InitWidgets* wid, VI9Pparameters* parameters, uint8_t count) {
	if(parameters->rows + count <= MAX_ROWS) {
		for(int i = 0; i < count; i++) {
			{//-ar
				wxArrayString output;
				wxArrayString errors;
				wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -ar \"" + VI9P::WorkingFile + "\" \"" + VI9P::WorkingFile + '\"');
				int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
				
				wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
				for (auto &s : output) {
					wid->consoleLog->LogTextAtLevel(0, s);
				}
				wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
			}
		}
	}
	
	int ret = 0;
	{//-rr
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -rr \"" + VI9P::WorkingFile + '\"');
		ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		
		if(ret > -1) {
			parameters->rows = ret & 0xFF;
		}
		//else return ret;
	}
	{//-pp
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -pp \"" + VI9P::WorkingFile + '\"');
		ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		std::string pp;

		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
			pp += std::string(s.ToUTF8()) + '\n';
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		{//PTitleVec
			parameters->PTitleVec.clear();
			for(uint8_t i = 0; i < parameters->rows; i++) {
				std::string value = "";
				parsePP(pp.c_str(), StrPTitleParam + '(' + std::to_string(i) + ')', &value);
				parameters->PTitleVec.push_back(value);
			}
		}
		{//StrMoflexParam
			parameters->MoflexVec.clear();
			for(uint8_t i = 0; i < parameters->rows; i++) {
				std::string value = "";
				parsePP(pp.c_str(), StrMoflexParam + '(' + std::to_string(i) + ')', &value);
				parameters->MoflexVec.push_back(value);
			}
		}
		{//StrMBannerParam
			parameters->MBannerVec.clear();
			for(uint8_t i = 0; i < parameters->rows; i++) {
				std::string value = "";
				parsePP(pp.c_str(), StrMBannerParam + '(' + std::to_string(i) + ')', &value);
				parameters->MBannerVec.push_back(value);
			}
		}
		int count = parameters->rows - wid->PlayerTitles.size();
		if(parameters->rows + count <= 28) {//??????? im sure this makes sense in some other universe
			doAddRows(wid, count);
		}
	}
	positionWidgets(wid, parameters);
}

void removeRows(InitWidgets* wid, VI9Pparameters* parameters, uint8_t count) {
	if(parameters->rows - count >= 1) {
		for(int i = 0; i < count; i++) {
			{//-sr
				wxArrayString output;
				wxArrayString errors;
				wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sr \"" + VI9P::WorkingFile + "\" \"" + VI9P::WorkingFile + '\"');
				int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
				
				wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
				for (auto &s : output) {
					wid->consoleLog->LogTextAtLevel(0, s);
				}
				wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
			}
		}
	}
	//loadParameters(wid, parameters);
	int ret = 0;
	{//-rr
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -rr \"" + VI9P::WorkingFile + '\"');
		ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		
		if(ret > -1) {
			parameters->rows = ret & 0xFF;
		}
		//else return ret;
	}
	{//-pp
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -pp \"" + VI9P::WorkingFile + '\"');
		ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		std::string pp;

		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
			pp += std::string(s.ToUTF8()) + '\n';
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		{//PTitleVec
			parameters->PTitleVec.clear();
			for(uint8_t i = 0; i < parameters->rows; i++) {
				std::string value = "";
				parsePP(pp.c_str(), StrPTitleParam + '(' + std::to_string(i) + ')', &value);
				parameters->PTitleVec.push_back(value);
			}
		}
		{//StrMoflexParam
			parameters->MoflexVec.clear();
			for(uint8_t i = 0; i < parameters->rows; i++) {
				std::string value = "";
				parsePP(pp.c_str(), StrMoflexParam + '(' + std::to_string(i) + ')', &value);
				parameters->MoflexVec.push_back(value);
			}
		}
		{//StrMBannerParam
			parameters->MBannerVec.clear();
			for(uint8_t i = 0; i < parameters->rows; i++) {
				std::string value = "";
				parsePP(pp.c_str(), StrMBannerParam + '(' + std::to_string(i) + ')', &value);
				parameters->MBannerVec.push_back(value);
			}
		}
		int count = wid->PlayerTitles.size() - parameters->rows;
		if(parameters->rows - count >= 0) {//???????????????????
			for(uint8_t i = 0; i < count; i++) {
				wid->PlayerTitles.back()->Destroy();
				wid->PlayerTitles.pop_back();
			}
			for(uint8_t i = 0; i < count; i++) {
				wid->MoflexFiles.back()->Destroy();
				wid->MoflexFiles.pop_back();
			}
			for(uint8_t i = 0; i < count; i++) {
				wid->MenuBanners.back()->Destroy();
				wid->MenuBanners.pop_back();
			}
			for(uint8_t i = 0; i < count; i++) {
				wid->MultiUp.back()->Destroy();
				wid->MultiUp.pop_back();
			}
			for(uint8_t i = 0; i < count; i++) {
				wid->MultiDown.back()->Destroy();
				wid->MultiDown.pop_back();
			}
		}
	}
	
	if(VI9P::MultiBannerIndex > wid->MenuBanners.size() - 1)
		VI9P::MultiBannerIndex = wid->MenuBanners.size() - 1;
	{//-gp
		std::string imagePath = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + "/bannerpreview" + std::to_string(VI9P::MultiBannerIndex) + ".png";
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -gp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + (parameters->rows * 2) + VI9P::MultiBannerIndex) + " \"" + imagePath + '\"');
		int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		
		wid->multiBannerPreview->SetBitmap(wxBitmap(wxString::FromUTF8(imagePath), wxBITMAP_TYPE_ANY));
	}
	EnableBannerLeftRight(wid);

	positionWidgets(wid, parameters);
}
