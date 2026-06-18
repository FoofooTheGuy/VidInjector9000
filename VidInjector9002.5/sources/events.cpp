#include <strings.hpp>

#include "settings.hpp"
#include "widgets.hpp"
#include "vi9p.hpp"

void theWidgets::frame_wxEVT_WEBREQUEST_STATE(wxWebRequestEvent* event) {
	// https://docs.wxwidgets.org/3.3/classwx_web_request.html
	switch (event->GetState())
	{
		// Request completed
		case wxWebRequest::State_Completed:
		{
			std::string tag(event->GetResponse().AsString());
			
			// trim down the response to get the latest tag name
			std::string namestr = "\"name\":";
			
			size_t find = tag.find(namestr);
			if (find == std::string::npos) {
				return;
			}
			tag = tag.substr(find + namestr.size());
			
			find = tag.find("\"");
			if (find == std::string::npos) {
				return;
			}
			tag = tag.substr(find + 1);

			find = tag.find("\"");
			if (find == std::string::npos) {
				return;
			}
			tag = tag.substr(0, find);
			
			//wxMessageBox(tag);
			
			if(tag == internalTag) {
				return;
			}
			
			wxMessageDialog Updating = wxMessageDialog(frame, wxString::FromUTF8(downloadVersion), wxString::FromUTF8(updateAvailable + " " + tag), wxOK|wxCANCEL|wxCENTRE);
			int choice = Updating.ShowModal();
			if(choice == wxID_OK) {
				wxLaunchDefaultBrowser("https://github.com/" + githubRepo + "/releases/tag/" + tag);
			}
			
			break;
		}
		// Request failed
		case wxWebRequest::State_Failed:
			// if you can't access this, you must not want it
			//wxLogError(event->GetErrorDescription());
			break;
		default:
			break;
	}
};

void theWidgets::frame_wxEVT_CLOSE_WINDOW(wxCloseEvent* event) {
	if(Settings::DeleteTemp) {
		{ // clear temp
			std::error_code error;
			
			std::filesystem::remove_all(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath).c_str()), error);
			if (error) {
				wxMessageBox(wxString::FromUTF8(std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '\n' + error.message())), wxString::FromUTF8(ErrorText));
			}
			
			std::filesystem::remove_all(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLItempPath).c_str()), error);
			if (error) {
				wxMessageBox(wxString::FromUTF8(std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLItempPath + '\n' + error.message())), wxString::FromUTF8(ErrorText));
			}
		}
	}
	
	event->Skip();
}

void theWidgets::panel_wxEVT_SIZE() {
	//wxMessageBox(wxString::FromUTF8(std::to_string(event.GetSize().GetWidth()) + ", " + std::to_string(event.GetSize().GetHeight())));
	int width, height;
	frame->GetSize(&width, &height);
	Settings::FrameWidth = width;
	Settings::FrameHeight = height;
	saveSettings();
	positionWidgets();
}

void theWidgets::modeChoiceBox_wxEVT_CHOICE() {
	if(!modeChoiceBox->IsEnabled()) {
		return;
	}
	int selection = modeChoiceBox->GetSelection();
	if (selection != wxNOT_FOUND) {
		parameters.mode = selection;
	}
	{ // -sp
		executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 0 \"" + std::to_string(parameters.mode) + "\" \"" + VI9P::WorkingFile + '\"'));
	}
	
	// extended doesn't have patch or multiple top images
	// (do it here because stuff in widgets.cpp can't access event functions)
	if(parameters.mode == 1) {
		MenuBanners_wxEVT_TEXT(MenuBanners.at(VI9P::MultiBannerIndex));
	}
	else if(parameters.mode == 2) {
		VI9P::MultiBannerIndex = 0;
		MenuBanners_wxEVT_TEXT(MenuBanners.at(VI9P::MultiBannerIndex));
		splitPatchButton->SetValue(false);
		splitPatchButton_wxEVT_TOGGLEBUTTON();
	}
	applyMode();
}

void theWidgets::bannerBox_wxEVT_TEXT(bool setValue) {
	if(!bannerBox->IsEnabled()) {
		return;
	}
	if(setValue) {
		bannerBox->SetFullPath(bannerBox->GetValue());
		parameters.banner = std::string(bannerBox->GetFullPath().ToUTF8());
	}
	int ret = 0;
	
	{ // -sp
		executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 1 \"" + parameters.banner + "\" \"" + VI9P::WorkingFile + '\"'));
	}
	{ // -gp
		std::string bannerImagePath = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + "/bannerpreview.png";
		
		ret = executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -gp \"" + VI9P::WorkingFile + "\" 1 \"" + bannerImagePath + '\"'));
		
		bannerPreview->SetBitmap(wxBitmap(wxString::FromUTF8(bannerImagePath), wxBITMAP_TYPE_ANY));
	}
	if(ret == 17) { // custom non-video banner
		bannerCustomText->Show();
		bannerPreview->Show(false);
		bannerError->Show(false);
		return;
	}
	else {
		bannerCustomText->Show(false);
		bannerPreview->Show();
	}
	if(ret && !parameters.banner.empty()) {
		bannerError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + ImageInfoError + " (" + std::to_string(ret) + ") " + SeeLog));
		bannerError->Show();
	}
	else {
		bannerError->Show(false);
	}
}

void theWidgets::bannerBrowse_wxEVT_BUTTON() {
	if(!bannerBrowse->IsEnabled()) {
		return;
	}
	wxFileDialog openFileDialog(frame, wxEmptyString, wxString::FromUTF8(parameters.banner.empty() ? Settings::ImagesPath : parameters.banner.substr(0, parameters.banner.find_last_of("/\\"))), wxEmptyString, wxString::FromUTF8(allFiles), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	openFileDialog.SetFilterIndex(0);
	if (openFileDialog.ShowModal() == wxID_OK) {
		bannerBox->SetValue(openFileDialog.GetPath());
	}
	
	if(!bannerBox->HasFocus()) {
		bannerBox->DisplayShortPath();
	}
}

void theWidgets::iconBox_wxEVT_TEXT(bool setValue) {
	if(!iconBox->IsEnabled()) {
		return;
	}
	if(setValue) {
		iconBox->SetFullPath(iconBox->GetValue());
		parameters.icon = std::string(iconBox->GetFullPath().ToUTF8());
	}
	int ret = 0;
	
	if (VI9P::Loading) { // so it doesnt mess up everything
		{ // -gp
			std::string iconImagePath = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "iconpreview.png";
			
			ret = executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -gp \"" + VI9P::WorkingFile + "\" 2 \"" + iconImagePath + '\"'));
			
			iconPreview->SetBitmap(wxBitmap(wxString::FromUTF8(iconImagePath), wxBITMAP_TYPE_ANY));
		}
		if(ret && !parameters.icon.empty()) {
			iconError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + ImageInfoError + " (" + std::to_string(ret) + ") " + SeeLog));
			iconError->Show(true);
		}
		else iconError->Show(false);
		return;
	}
	
	// we can just use ss since if you give that a normal image it will work like normal
	{ // -ss
		{
			ret = executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -ss \"" + VI9P::WorkingFile + "\" \"" + parameters.icon + "\" \"" + VI9P::WorkingFile + '\"'));
		}
		
		// if smdh (we don't want to keep that border)
		if (ret == 0) {
			parameters.iconBorder = 0;
			{ // -sp
				executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 3 " + std::to_string(parameters.iconBorder) + " \"" + VI9P::WorkingFile + '\"'));
			}
		}
	}
	// this causes an infinite loop for some reason, probably because it changes the text of this box
	//loadParameters();
	
	// do this in case of smdh
	{ // -pp
		std::string pp;
		{
			wxArrayString output;
			
			executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -pp \"" + VI9P::WorkingFile + '\"'), &output);
			
			for (auto &s : output) {
				pp += std::string(s.ToUTF8()) + '\n';
			}
		}
		
		{ // iconBorder
			std::string value = "";
			int outnum = 0;
			parsePP(pp.c_str(), IntIconBorderParam, &value);
			if(!ASCII2number<int>(&outnum, value)) {
				// tbh this should never ever happen
				parameters.iconBorder = 2;
			}
			else {
				parameters.iconBorder = ((outnum > 2) ? 0 : outnum);
			}
		}
		{ // Sname
			std::string value = "";
			parsePP(pp.c_str(), StrSNameParam, &value);
			parameters.Sname = value;
		}
		{ // Lname
			std::string value = "";
			parsePP(pp.c_str(), StrLNameParam, &value);
			parameters.Lname = value;
		}
		{ // publisher
			std::string value = "";
			parsePP(pp.c_str(), StrPublisherParam, &value);
			parameters.publisher = value;
		}
		
		shortnameBox->SetValue(wxString::FromUTF8(parameters.Sname));
		longnameBox->SetValue(wxString::FromUTF8(parameters.Lname));
		publisherBox->SetValue(wxString::FromUTF8(parameters.publisher));
		{ // iconBorder
			if(parameters.iconBorder > 2) {
				parameters.iconBorder = 0; // yea
			}
	
			{ // -sp
				executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 3 " + std::to_string(parameters.iconBorder) + " \"" + VI9P::WorkingFile + '\"'));
			}
		}
	}
	{ // -gp
		std::string iconImagePath = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "iconpreview.png";
		
		ret = executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -gp \"" + VI9P::WorkingFile + "\" 2 \"" + iconImagePath + '\"'));
		
		iconPreview->SetBitmap(wxBitmap(wxString::FromUTF8(iconImagePath), wxBITMAP_TYPE_ANY));
	}
	if(ret && !parameters.icon.empty()) {
		iconError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + ImageInfoError + " (" + std::to_string(ret) + ") " + SeeLog));
		iconError->Show(true);
	}
	else {
		iconError->Show(false);
	}
}

void theWidgets::iconPreview_wxEVT_BUTTON() {
	if(!iconPreview->IsEnabled()) {
		return;
	}
	++parameters.iconBorder;
	if(parameters.iconBorder > 2) {
		parameters.iconBorder = 0;
	}
	
	{ // -sp
		executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 3 " + std::to_string(parameters.iconBorder) + " \"" + VI9P::WorkingFile + '\"'));
	}
	{ // -gp
		std::string iconImagePath = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "iconpreview.png";
		
		executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -gp \"" + VI9P::WorkingFile + "\" 2 \"" + iconImagePath + '\"'));
		
		iconPreview->SetBitmap(wxBitmap(wxString::FromUTF8(iconImagePath), wxBITMAP_TYPE_ANY));
	}
}

void theWidgets::iconBrowse_wxEVT_BUTTON() {
	if(!iconBrowse->IsEnabled()) {
		return;
	}
	wxFileDialog openFileDialog(frame, wxEmptyString, wxString::FromUTF8(parameters.icon.empty() ? Settings::ImagesPath : parameters.icon.substr(0, parameters.icon.find_last_of("/\\"))), wxEmptyString, wxString::FromUTF8(allFiles), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	openFileDialog.SetFilterIndex(0);
	if(openFileDialog.ShowModal() == wxID_OK) {
		iconBox->SetValue(openFileDialog.GetPath());
	}
	
	if(!iconBox->HasFocus()) {
		iconBox->DisplayShortPath();
	}
}

void theWidgets::shortnameBox_wxEVT_TEXT() {
	if(!shortnameBox->IsEnabled()) {
		return;
	}
	parameters.Sname = std::string(shortnameBox->GetValue().ToUTF8());
	
	{ // -sp
		executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 4 \"" + fixDoubleQuote(parameters.Sname) + "\" \"" + VI9P::WorkingFile + '\"'));
	}
	size_t UTF16Len = chrcount(parameters.Sname); // should this have null terminator?
	if(UTF16Len > 64) { // 0x80 / 2 = 0x40 = 64
		shortnameError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (" + std::to_string(UTF16Len) + "/64)"));
		shortnameError->Show(true);
	}
	else {
		shortnameError->Show(false);
	}
}

void theWidgets::longnameBox_wxEVT_TEXT() {
	if(!longnameBox->IsEnabled()) {
		return;
	}
	parameters.Lname = std::string(longnameBox->GetValue().ToUTF8());
	
	{ // -sp
		executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 5 \"" + fixDoubleQuote(parameters.Lname) + "\" \"" + VI9P::WorkingFile + '\"'));
	}
	size_t UTF16Len = chrcount(parameters.Lname);
	if(UTF16Len > 128) { // 0x100 / 2 = 0x80 = 128
		longnameError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (" + std::to_string(UTF16Len) + "/128)"));
		longnameError->Show(true);
	}
	else {
		longnameError->Show(false);
	}
}

void theWidgets::publisherBox_wxEVT_TEXT() {
	if(!publisherBox->IsEnabled()) {
		return;
	}
	parameters.publisher = std::string(publisherBox->GetValue().ToUTF8());
	
	{ // -sp
		executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 6 \"" + fixDoubleQuote(parameters.publisher) + "\" \"" + VI9P::WorkingFile + '\"'));
	}
	size_t UTF16Len = chrcount(parameters.publisher);
	if(UTF16Len > 64) { // 0x80 / 2 = 0x40 = 64
		publisherError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (" + std::to_string(UTF16Len) + "/64)"));
		publisherError->Show(true);
	}
	else {
		publisherError->Show(false);
	}
}

void theWidgets::copyBox_wxEVT_TEXT() {
	if(!copyBox->IsEnabled()) {
		return;
	}
	parameters.copyrightInfo = std::string(copyBox->GetValue().ToUTF8());
	
	{ // -sp
		executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 8 \"" + fixDoubleQuote(parameters.copyrightInfo) + "\" \"" + VI9P::WorkingFile + '\"'));
	}
}

void theWidgets::copyCheck_wxEVT_CHECKBOX() {
	if(!copyCheck->IsEnabled()) {
		return;
	}
	parameters.copycheck = copyCheck->GetValue();
	
	{//-sp
		executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 7 \"" + std::to_string(parameters.copycheck) + "\" \"" + VI9P::WorkingFile + '\"'));
	}
	copyBox->Enable(parameters.copycheck);
}

void theWidgets::ffRewindCheck_wxEVT_CHECKBOX() {
	if(!ffRewindCheck->IsEnabled()) {
		return;
	}
	parameters.FFrewind = ffRewindCheck->GetValue();
	
	{//-sp
		executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 9 \"" + std::to_string(parameters.FFrewind) + "\" \"" + VI9P::WorkingFile + '\"'));
	}
}

void theWidgets::dimCheck_wxEVT_CHECKBOX() {
	if(!dimCheck->IsEnabled()) {
		return;
	}
	parameters.FadeOpt = dimCheck->GetValue();
	
	{//-sp
		executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 10 \"" + std::to_string(parameters.FadeOpt) + "\" \"" + VI9P::WorkingFile + '\"'));
	}
}

void theWidgets::multiBannerPreview_wxEVT_BUTTON() {
	if(!multiBannerPreview->IsEnabled()) {
		return;
	}
	wxFileDialog openFileDialog(frame, wxEmptyString, wxString::FromUTF8(parameters.MBannerVec.at(VI9P::MultiBannerIndex).empty() ? Settings::ImagesPath : parameters.MBannerVec.at(VI9P::MultiBannerIndex).substr(0, parameters.MBannerVec.at(VI9P::MultiBannerIndex).find_last_of("/\\"))), wxEmptyString, wxString::FromUTF8(allFiles), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	openFileDialog.SetFilterIndex(0);
	if (openFileDialog.ShowModal() == wxID_OK) {
		MenuBanners.at(VI9P::MultiBannerIndex)->SetValue((openFileDialog.GetPath()));
	}
}

void theWidgets::multiBannerPreviewLeft_wxEVT_BUTTON() {
	if(!multiBannerPreviewLeft->IsEnabled()) {
		return;
	}
	if(VI9P::MultiBannerIndex > 0) {
		--VI9P::MultiBannerIndex;
	}

	{ // -gp
		std::string bannerImagePath = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + "/bannerpreview" + std::to_string(VI9P::MultiBannerIndex) + ".png";
		
		executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -gp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + (parameters.rows * 2) + VI9P::MultiBannerIndex) + " \"" + bannerImagePath + '\"'));
		
		multiBannerPreview->SetBitmap(wxBitmap(wxString::FromUTF8(bannerImagePath), wxBITMAP_TYPE_ANY));
	}
	
	setRowIndex();
	EnableBannerLeftRight();
}

void theWidgets::multiBannerPreviewRight_wxEVT_BUTTON() {
	if(!multiBannerPreviewRight->IsEnabled()) {
		return;
	}
	if(VI9P::MultiBannerIndex < MenuBanners.size() - 1) {
		++VI9P::MultiBannerIndex;
	}
	if(VI9P::MultiBannerIndex >= MAX_ROWS_MULTI - 1) {
		VI9P::MultiBannerIndex = MAX_ROWS_MULTI - 1;
	}
	{ // -gp
		std::string bannerImagePath = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + "/bannerpreview" + std::to_string(VI9P::MultiBannerIndex) + ".png";
		
		executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -gp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + (parameters.rows * 2) + VI9P::MultiBannerIndex) + " \"" + bannerImagePath + '\"'));
		
		multiBannerPreview->SetBitmap(wxBitmap(wxString::FromUTF8(bannerImagePath), wxBITMAP_TYPE_ANY));
	}
	setRowIndex();
	EnableBannerLeftRight();
}

void theWidgets::PlayerTitles_wxEVT_TEXT(wxTextCtrl* row) {
	size_t rowReal;
	for(rowReal = 0; rowReal < PlayerTitles.size(); rowReal++) { // get row
		if(reinterpret_cast<intptr_t>(PlayerTitles.at(rowReal)) == reinterpret_cast<intptr_t>(row)) { // compare pointers
			parameters.PTitleVec.at(rowReal) = std::string(row->GetValue().ToUTF8());
			
			{ // -sp
				executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + rowReal) + " \"" + fixDoubleQuote(parameters.PTitleVec.at(rowReal)) + "\" \"" + VI9P::WorkingFile + '\"'));
			}
		}
	}
}

void theWidgets::MoflexFiles_wxEVT_TEXT(wxPathCtrl* row) {
	size_t rowReal;
	for(rowReal = 0; rowReal < MoflexFiles.size(); rowReal++) { // get row
		if(reinterpret_cast<intptr_t>(MoflexFiles.at(rowReal)) == reinterpret_cast<intptr_t>(row)) { // compare pointers
			row->SetFullPath(row->GetValue());
			parameters.MoflexVec.at(rowReal) = std::string(row->GetFullPath().ToUTF8());
			
			{ // -sp
				executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + parameters.rows + rowReal) + " \"" + parameters.MoflexVec.at(rowReal) + "\" \"" + VI9P::WorkingFile + '\"'));
			}
			
			if(!row->HasFocus()) {
				row->DisplayShortPath();
			}
		}
	}
}

void theWidgets::MenuBanners_wxEVT_TEXT(wxPathCtrl* row, bool setValue) {
	size_t rowReal;
	for(rowReal = 0; rowReal < MenuBanners.size(); rowReal++) { // get row
		if(reinterpret_cast<intptr_t>(MenuBanners.at(rowReal)) == reinterpret_cast<intptr_t>(row)) { // compare pointers
			size_t disabled_row;
			for(disabled_row = 0; disabled_row < MenuBanners.size(); disabled_row++) {
				if(!MenuBanners.at(disabled_row)->IsEnabled()) {
					break;
				}
			}
			if(static_cast<int>(rowReal & 0xFF) < disabled_row) {
				VI9P::MultiBannerIndex = rowReal;
			}
			
			if(setValue) {
				row->SetFullPath(row->GetValue());
				parameters.MBannerVec.at(rowReal) = std::string(row->GetFullPath().ToUTF8());
			}
			
			{ // -sp
				executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + (parameters.rows * 2) + rowReal) + " \"" + parameters.MBannerVec.at(rowReal) + "\" \"" + VI9P::WorkingFile + '\"'));
			}
			{ // -gp
				std::string bannerImagePath = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + "/bannerpreview" + std::to_string(VI9P::MultiBannerIndex) + ".png";
				
				executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -gp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + (parameters.rows * 2) + VI9P::MultiBannerIndex) + " \"" + bannerImagePath + '\"'));
				
				multiBannerPreview->SetBitmap(wxBitmap(wxString::FromUTF8(bannerImagePath), wxBITMAP_TYPE_ANY));
			}
			setRowIndex();
			EnableBannerLeftRight();
			
			if(!row->HasFocus()) {
				row->DisplayShortPath();
			}
		}
	}
}

void theWidgets::MenuBanners_EVT_TEXT_ENTER(wxPathCtrl* row) {
	MenuBanners_wxEVT_TEXT(row); // lool
}

void theWidgets::MultiUp_wxEVT_BUTTON(wxButton* row) {
	size_t rowReal;
	for(rowReal = 0; rowReal < MultiUp.size(); rowReal++) { // get row
		if(reinterpret_cast<intptr_t>(MultiUp.at(rowReal)) == reinterpret_cast<intptr_t>(row)) { // compare pointers
			if(!row->IsEnabled()) {
				return;
			}
			if(!row->IsShown()) {
				return;
			}
			row->Enable(false); // disable so you can't press it too much
			if(rowReal > 0) {
				// the stuff will get saved if the text in the boxes changes so don't save here
				{ // PlayerTitles
					wxString tempStr(PlayerTitles.at(rowReal)->GetValue());
					PlayerTitles.at(rowReal)->SetValue(PlayerTitles.at(rowReal - 1)->GetValue());
					PlayerTitles.at(rowReal - 1)->SetValue(tempStr);
				}
				{ // MoflexFiles
					wxString tempStr(MoflexFiles.at(rowReal)->GetFullPath());
					MoflexFiles.at(rowReal)->SetValue(MoflexFiles.at(rowReal - 1)->GetFullPath());
					MoflexFiles.at(rowReal - 1)->SetValue(tempStr);
					
					MoflexFiles.at(rowReal)->DisplayShortPath();
					MoflexFiles.at(rowReal - 1)->DisplayShortPath();
				}
				{ // MenuBanners
					wxString tempStr(MenuBanners.at(rowReal)->GetFullPath());
					MenuBanners.at(rowReal)->SetValue(MenuBanners.at(rowReal - 1)->GetFullPath());
					MenuBanners.at(rowReal - 1)->SetValue(tempStr);
					
					MenuBanners.at(rowReal)->DisplayShortPath();
					MenuBanners.at(rowReal - 1)->DisplayShortPath();
				}
			}
			row->Enable(true);
			setCursors();
		}
	}
}

void theWidgets::MultiDown_wxEVT_BUTTON(wxButton* row) {
	size_t rowReal;
	for(rowReal = 0; rowReal < MultiDown.size(); rowReal++) { // get row
		if(reinterpret_cast<intptr_t>(MultiDown.at(rowReal)) == reinterpret_cast<intptr_t>(row)) { // compare pointers
			if(!row->IsEnabled()) {
				return;
			}
			if(!row->IsShown()) {
				return;
			}
			row->Enable(false); // disable so you can't press it too much
			if(rowReal < MultiDown.size()) {
				// the stuff will get saved if the text in the boxes changes so don't save here
				{ // PlayerTitles
					wxString tempStr(PlayerTitles.at(rowReal)->GetValue());
					PlayerTitles.at(rowReal)->SetValue(PlayerTitles.at(rowReal + 1)->GetValue());
					PlayerTitles.at(rowReal + 1)->SetValue(tempStr);
				}
				{ // MoflexFiles
					wxString tempStr(MoflexFiles.at(rowReal)->GetFullPath());
					MoflexFiles.at(rowReal)->SetValue(MoflexFiles.at(rowReal + 1)->GetFullPath());
					MoflexFiles.at(rowReal + 1)->SetValue(tempStr);
					
					MoflexFiles.at(rowReal)->DisplayShortPath();
					MoflexFiles.at(rowReal + 1)->DisplayShortPath();
				}
				{ // MenuBanners
					wxString tempStr(MenuBanners.at(rowReal)->GetFullPath());
					MenuBanners.at(rowReal)->SetValue(MenuBanners.at(rowReal + 1)->GetFullPath());
					MenuBanners.at(rowReal + 1)->SetValue(tempStr);
					
					MenuBanners.at(rowReal)->DisplayShortPath();
					MenuBanners.at(rowReal + 1)->DisplayShortPath();
				}
			}
			row->Enable(true);
			setCursors();
		}
	}
}

void theWidgets::moflexBrowse_wxEVT_BUTTON() {
	if(!moflexBrowse->IsEnabled()) {
		return;
	}
	size_t row;
	for(row = 0; row < MoflexFiles.size() - 1; row++) {
		if(std::string(MoflexFiles.at(row)->GetValue().ToUTF8()).empty()) {
			break;
		}
	}
	wxArrayString paths;
	wxFileDialog openFileDialog(frame, wxEmptyString, wxString::FromUTF8(parameters.MoflexVec.at(row ? row - 1 : row).empty() ? Settings::VideosPath : parameters.MoflexVec.at(row ? row - 1 : row).substr(0, parameters.MoflexVec.at(row ? row - 1 : row).find_last_of("/\\"))), wxEmptyString, moflexFiles, wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_MULTIPLE);
	openFileDialog.SetFilterIndex(0);
	if (openFileDialog.ShowModal() == wxID_OK) {
		openFileDialog.GetPaths(paths);
		paths.Sort(); // it already sorts them without this but meh
		if(paths.GetCount() == 1) {
			MoflexFiles.at(row)->SetValue(paths.Last());
		}
		else {
			for(size_t i = 0; i < paths.GetCount(); i++) {
				if(i < MoflexFiles.size()) {
					MoflexFiles.at(i)->SetValue(paths.Item(i));
				}
			}
		}
	}
}

void theWidgets::multiBannerBrowse_wxEVT_BUTTON() {
	if(!multiBannerBrowse->IsEnabled()) {
		return;
	}
	size_t row;
	for(row = 0; row < MenuBanners.size() - 1; row++) {
		if(!MenuBanners.at(row)->IsEnabled() && row > 0) {
			row--;
			break;
		}
		if(std::string(MenuBanners.at(row)->GetValue().ToUTF8()).empty()) {
			break;
		}
	}
	wxArrayString paths;
	wxFileDialog openFileDialog(frame, wxEmptyString, wxString::FromUTF8(parameters.MBannerVec.at(row ? row - 1 : row).empty() ? Settings::ImagesPath : parameters.MBannerVec.at(row ? row - 1 : row).substr(0, parameters.MBannerVec.at(row ? row - 1 : row).find_last_of("/\\"))), wxEmptyString, wxString::FromUTF8(allFiles), wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_MULTIPLE);
	openFileDialog.SetFilterIndex(0);
	if (openFileDialog.ShowModal() == wxID_OK) {
		openFileDialog.GetPaths(paths);
		paths.Sort();
		if(paths.GetCount() == 1) {
			MenuBanners.at(row)->SetValue(paths.Last());
		}
		else {
			for(size_t i = 0; i < paths.GetCount(); i++) {
				if(i < MenuBanners.size()) {
					MenuBanners.at(i)->SetValue(paths.Item(i));
				}
			}
		}
	}
	
	EnableBannerLeftRight();
}

void theWidgets::removeRow_wxEVT_BUTTON() {
	if(!removeRow->IsEnabled()) {
		return;
	}
	if(parameters.rows - 1 <= parameters.splitPos) {
		splitPatchButton->SetValue(false);
		splitPatchButton_wxEVT_TOGGLEBUTTON();
	}
	if(removeRow->IsEnabled()) {
		removeRow->Enable(false); // disable so you cant press it too much
		appendRow->Enable(false);
	
		removeRows();
		setRowIndex();
	}
	
	if(parameters.mode == 0) {
		rowText->Show(false);
		if(parameters.rows > 1) {
			removeRow->Enable(true);
			splitPatchButton->Enable(true);
		}
		else {
			removeRow->Enable(false);
			splitPatchButton->Enable(false);
		}
		appendRow->Enable(false);
	}
	else if(parameters.mode == 1) {
		EnableBannerLeftRight();
		
		rowText->Show(true);
		if(parameters.rows > 1 && parameters.rows < MAX_ROWS_MULTI) {
			appendRow->Enable(true);
			removeRow->Enable(true);
			splitPatchButton->Enable(true);
		}
		if(parameters.rows <= 1) {
			appendRow->Enable(true);
			removeRow->Enable(false);
			splitPatchButton->Enable(false);
		}
		else if(parameters.rows >= MAX_ROWS_MULTI) {
			appendRow->Enable(false);
			removeRow->Enable(true);
			splitPatchButton->Enable(true);
		}
	}
	else if(parameters.mode == 2) {
		rowText->Show(true);
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
	}
	ShowPatchUpDown();
	ShowMultiUpDown();
	setCursors();
	setToolTips();
}

void theWidgets::splitPatchButton_wxEVT_TOGGLEBUTTON() {
	if(!splitPatchButton->IsEnabled()) {
		return;
	}
	parameters.splitPos = splitPatchButton->GetValue();
	
	{ // -sp
		executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 11 \"" + std::to_string(parameters.splitPos) + "\" \"" + VI9P::WorkingFile + '\"'));
	}
	splitPatchLine->Show(parameters.splitPos);
	ShowPatchUpDown();
	positionWidgets();
}

void theWidgets::splitPatchUp_wxEVT_BUTTON() {
	if(!splitPatchButton->GetValue() || !splitPatchUp->IsEnabled() || !splitPatchUp->IsShown()) {
		return;
	}
	--parameters.splitPos;
	
	{ // -sp
		executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 11 \"" + std::to_string(parameters.splitPos) + "\" \"" + VI9P::WorkingFile + '\"'));
	}
	
	ShowPatchUpDown();
	positionWidgets();
}

void theWidgets::splitPatchDown_wxEVT_BUTTON() {
	if(!splitPatchButton->GetValue() || !splitPatchDown->IsEnabled() || !splitPatchDown->IsShown()) {
		return;
	}
	++parameters.splitPos;
	
	{ // -sp
		executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 11 \"" + std::to_string(parameters.splitPos) + "\" \"" + VI9P::WorkingFile + '\"'));
	}
	
	ShowPatchUpDown();
	positionWidgets();
}

void theWidgets::buildframe_wxEVT_CLOSE_WINDOW(wxCloseEvent* event) {
	event->Veto();
	buildframe->Show(false);
	
	cancelButton_wxEVT_BUTTON();
}

void theWidgets::buildpanel_wxEVT_SIZE() {
	positionWidgets();
}

void theWidgets::titleIDBox_wxEVT_TEXT() {
	titleIDBox->ForceUpper();
	titleIDBox->SetInsertionPoint(titleIDBox->GetInsertionPoint() + 1);
}

void theWidgets::titleIDButton_wxEVT_BUTTON() {
	if(!titleIDButton->IsEnabled()) {
		return;
	}
	uint32_t uniqueID = RandomTID();
	char uniqueIDstr [6];
	sprintf(uniqueIDstr, "%05X", uniqueID);
	
	titleIDBox->SetValue(wxString::FromUTF8(std::string(uniqueIDstr)));
}

void theWidgets::exportArchive_wxEVT_END_PROCESS(wxProcessEvent* event) {
	if(event->GetPid() != 0) {
		if(event->GetExitCode() != 0) {
			wxMessageBox(wxString::FromUTF8(ErrorText + ' ' + BuildError + " (" + std::to_string(event->GetExitCode()) + ")\n" + SeeLog), wxString::FromUTF8(ErrorText), wxICON_ERROR, buildframe);
		}
		else {
			std::error_code error;
			
			error = copyfile(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "out.cia", Exports::CIA);
			if (error) {
				wxMessageBox(wxString::FromUTF8(CopyFileError + '\n' + std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "out.cia") + " -> " +  Exports::CIA + '\n' + error.message()), wxString::FromUTF8(ErrorText), wxICON_ERROR, buildframe);
			}
			
			if(std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(Exports::CIA).c_str()), error)) {
				wxMessageBox(wxString::FromUTF8(Exports::CIA), wxString::FromUTF8(SuccessfullyBuilt), wxOK|wxCENTRE, buildframe);
			}
			else {
				wxMessageBox(wxString::FromUTF8(BuildError + " (" + Exports::CIA + ')'), wxString::FromUTF8(ErrorText), wxICON_ERROR, buildframe);
			}
			if(error) {
				wxMessageBox(wxString::FromUTF8(BuildError + " (" + Exports::CIA + ")\n" + error.message()), wxString::FromUTF8(ErrorText), wxICON_ERROR, buildframe);
			}
			
			if(!Exports::TAR.empty()) {
				error = copyfile(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "out.tar", Exports::TAR);
				if (error) {
					wxMessageBox(wxString::FromUTF8(CopyFileError + '\n' + std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "out.tar") + " -> " +  Exports::TAR + '\n' + error.message()), wxString::FromUTF8(ErrorText), wxICON_ERROR, buildframe);
				}
				
				if(std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(Exports::TAR).c_str()), error)) {
					wxMessageBox(wxString::FromUTF8(Exports::TAR), wxString::FromUTF8(SuccessfullyBuilt), wxOK|wxCENTRE, buildframe);
				}
				else {
					wxMessageBox(wxString::FromUTF8(BuildError + " (" + Exports::TAR + ')'), wxString::FromUTF8(ErrorText), wxICON_ERROR, buildframe);
				}
				if(error) {
					wxMessageBox(wxString::FromUTF8(BuildError + " (" + Exports::TAR + ")\n" + error.message()), wxString::FromUTF8(ErrorText), wxICON_ERROR, buildframe);
				}
			}
		}
		consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(event->GetExitCode()) + '\n'));
	}
	statusText->Show(false);
	exportLogger->Stop();
	barPulser->Stop();
	buildBar->SetValue(0);
	frame->Enable(true);
	setCursors();
}

void theWidgets::buildButton_wxEVT_BUTTON() {
	if(!buildButton->IsEnabled()) {
		return;
	}
	std::string uniqueID = toupperstr(std::string(titleIDBox->GetValue().ToUTF8()));
	std::string appName = std::string(applicationTitleBox->GetValue().ToUTF8());
	std::string prodCode = std::string(productCodeBox->GetValue().ToUTF8());
	
	Exports::CIA = "";
	Exports::TAR = "";
	wxFileDialog saveCIADialog(frame, wxEmptyString, wxEmptyString, wxString::FromUTF8(std::string(longnameBox->GetValue().ToUTF8()) + " [000400000" + uniqueID + "00].cia"), wxString::FromUTF8(ciaFiles), wxFD_SAVE);
	if (saveCIADialog.ShowModal() != wxID_OK) {
		return;
	}
	Exports::CIA = addMissingFileExtension(std::string(saveCIADialog.GetPath().ToUTF8()), ".cia");
	
	if(parameters.mode == 1 && parameters.splitPos) {
		wxFileDialog saveTARDialog(frame, wxEmptyString, wxString::FromUTF8(Exports::CIA.substr(0, Exports::CIA.find_last_of("\\/") == std::string::npos ? 0 : Exports::CIA.find_last_of("\\/"))), wxString::FromUTF8(std::string(longnameBox->GetValue().ToUTF8()) + " [000400000" + std::string(titleIDBox->GetValue().ToUTF8()) + "00].tar"), wxString::FromUTF8(tarFiles), wxFD_SAVE);
		if (saveTARDialog.ShowModal() == wxID_OK) {
			Exports::TAR = addMissingFileExtension(std::string(saveTARDialog.GetPath().ToUTF8()), ".tar");
		}
	}
	
	if(Exports::CIA.empty()) { // just make sure
		return;
	}
	
	// execute async process
	wxString command = "";
	if(Exports::TAR.empty()) {
		command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -bc \"" + VI9P::WorkingFile + "\" \"" + uniqueID + "\" \"" + appName + "\" \"" + prodCode + "\" \"" + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + "/out.cia\"");
	}
	else {
		command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -bc \"" + VI9P::WorkingFile + "\" \"" + uniqueID + "\" \"" + appName + "\" \"" + prodCode + "\" \"" + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + "/out.cia\" \"" + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + "/out.tar\"");
	}
	
	consoleLog->LogTextAtLevel(0, command + "\n==========\n");
	
	exportArchive->Redirect();
	
	long PID = wxExecute(command, wxEXEC_MAKE_GROUP_LEADER|wxEXEC_ASYNC, exportArchive);
	
	if (PID == 0) {
		wxLogError(wxString::FromUTF8(ErrorText + ' ' + command));
		return;
	}
	
	frame->Enable(false);
	setCursors();
	barPulser->Start(100);
	statusText->Show();
	exportLogger->Start(1); // poll for output every X milliseconds... i guess
}

void theWidgets::cancelButton_wxEVT_BUTTON() {
	if(!cancelButton->IsEnabled()) {
		return;
	}
	if(statusText->IsShown()) { // dumb but it is sure to work
		int ret = 0;
		ret = wxProcess::Kill(exportArchive->GetPid(), wxSIGKILL, wxKILL_CHILDREN);
		if(ret != wxKILL_OK) {
			wxLogError(wxString::FromUTF8(ErrorText + ' ' + std::to_string(ret) + "\n(" + std::to_string(ret) + ')'));
			return;
		}
	}
	wxProcessEvent event(0, 0, 0);
	exportArchive_wxEVT_END_PROCESS(&event);
}

void theWidgets::barPulser_wxEVT_TIMER() {
	buildBar->Pulse();
}

void theWidgets::exportLogger_wxEVT_TIMER() {
	// https://github.com/wxWidgets/wxWidgets/blob/master/samples/exec/exec.cpp#L1373
	if (exportArchive->IsInputAvailable()) {
		wxTextInputStream Stream(*exportArchive->GetInputStream());

		// this assumes that the output is always line buffered
		wxString msg;
		msg << Stream.ReadLine();
		//wxMessageBox(msg);
		
		consoleLog->LogTextAtLevel(0, msg);
		
		{ // statusText
			statusText->SetLabel(msg);
			
			int w, h;
			wxFont f;
			
			f = statusText->GetFont();
			
			statusText->GetTextExtent(statusText->GetLabel(), &w, &h, nullptr, nullptr, &f);
			statusText->SetSize(w, h);
		}
		{ // statusText
			int y, panelwidth, mywidth;
			buildpanel->GetSize(&panelwidth, NULL);
			statusText->GetSize(&mywidth, NULL);
			statusText->GetPosition(NULL, &y);
			
			statusText->Move((panelwidth - mywidth) / 2, y);
		}
	}
}

void theWidgets::extractDialog_wxEVT_CLOSE_WINDOW(wxCloseEvent* event) {
	event->Veto();
	int ret = 0;
	ret = wxProcess::Kill(extractArchive->GetPid(), wxSIGKILL, wxKILL_CHILDREN);
	if(ret != wxKILL_OK) {
		wxLogError(wxString::FromUTF8(ErrorText + ' ' + std::to_string(ret) + "\n(" + std::to_string(ret) + ')'));
		return;
	}
	wxProcessEvent noEvent(0, 0, 0);
	extractArchive_wxEVT_END_PROCESS(&noEvent);
}

void theWidgets::extractArchive_wxEVT_END_PROCESS(wxProcessEvent* event) {
	if(event->GetPid() != 0) {
		if(event->GetExitCode() != 0) {
			wxMessageBox(wxString::FromUTF8(ErrorText + ' ' + BuildError + " (" + std::to_string(event->GetExitCode()) + ')'), wxString::FromUTF8(ErrorText), wxICON_ERROR);
		}
		else {
			std::error_code error;
			if(std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(Extracted::Archive).c_str()), error)) {
				wxMessageBox(wxString::FromUTF8(Extracted::Archive), wxString::FromUTF8(SuccessfullyExtracted));
			}
			else {
				wxMessageBox(wxString::FromUTF8(BuildError + " (" + Extracted::Archive + ')'), wxString::FromUTF8(ErrorText), wxICON_ERROR);
			}
			if(error) {
				wxMessageBox(wxString::FromUTF8(BuildError + " (" + Extracted::Archive + ")\n" + error.message()), wxString::FromUTF8(ErrorText), wxICON_ERROR);
			}
		}
		consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(event->GetExitCode()) + '\n'));
	}
	extractLogger->Stop();
	extractPulser->Stop();
	frame->Enable(true);
	setCursors();
}

void theWidgets::extractPulser_wxEVT_TIMER() {
	extractDialog->Pulse();
	
	// cancel
	if(extractDialog->WasCancelled()) {
		extractDialog->Close();
	}
}

void theWidgets::extractLogger_wxEVT_TIMER() {
	// https://github.com/wxWidgets/wxWidgets/blob/master/samples/exec/exec.cpp#L1373
	if (extractArchive->IsInputAvailable()) {
		wxTextInputStream Stream(*extractArchive->GetInputStream());

		// this assumes that the output is always line buffered
		wxString msg;
		msg << Stream.ReadLine();
		//wxMessageBox(msg);
		
		consoleLog->LogTextAtLevel(0, msg);
	}
}

void theWidgets::aboutframe_wxEVT_CLOSE_WINDOW(wxCloseEvent* event) {
	event->Veto();
	aboutframe->Show(false);
}

void theWidgets::aboutpanel_wxEVT_SIZE() {
	positionWidgets();
}
