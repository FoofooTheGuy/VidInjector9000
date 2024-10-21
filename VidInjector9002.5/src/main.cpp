#include "appInitializer.hpp"
#include "settings.hpp"
#include "widgets.hpp"

//https://github.com/gammasoft71/Examples_wxWidgets/blob/adbd395081bf25c9034f2b64eee62608a943441f/src/HelloWorld/HelloWorld/HelloWorld.cpp#L4
#if defined(__WXOSX__)
int PointsToNativeFontGraphicsUntit(int size) {
	return static_cast<float>(size) / wxScreenDC().GetPPI().GetHeight() * 96.0f;  // font is in pixels and not in points
}
#else
int PointsToNativeFontGraphicsUntit(int size) {
	return size;  // font is in points
}
#endif

int main(int argc, char* argv[]) {
	AppInitializer appInitializer;
	wxInitAllImageHandlers();

	//https://stackoverflow.com/a/37726650
	wxFileName f(wxStandardPaths::Get().GetExecutablePath());
	ProgramDir = f.GetPath();
	
	//load settings
	{
		std::vector<int> ret = loadSettings();
		for(const auto &i : ret) {
			switch(i) {
				case 0:
					{
						
					}
					break;
				case 1:
					{
						
					}
					break;
				case 2:
					{
						
					}
					break;
				case 3:
					{
						wxMessageBox(wxString::FromUTF8(FailedToFindVar + ' ' + StrDefaultLanguage + '\n' + ValueNoChange), wxString::FromUTF8(ErrorText + ' ' + MissingVariableError), wxICON_ERROR);
					}
					break;
				case 4:
					{
						
						//xtd::forms::message_box::show(*this, xtd::ustring::format("{} ({})\n{} {}", BadValue, outstr, IntAutoSaveParams, BeANumber), xtd::ustring::format("{} {}", ErrorText, BadValue), xtd::forms::message_box_buttons::ok, xtd::forms::message_box_icon::error);
					}
					break;
				case 5:
					{
						wxMessageBox(wxString::FromUTF8(FailedToFindVar + ' ' + IntShowLog + '\n' + ValueNoChange), wxString::FromUTF8(ErrorText + ' ' + MissingVariableError), wxICON_ERROR);
					}
					break;
				case 6:
					{
						//bad value
					}
					break;
				case 7:
					{
						wxMessageBox(wxString::FromUTF8(FailedToFindVar + ' ' + IntColorMode + '\n' + ValueNoChange), wxString::FromUTF8(ErrorText + ' ' + MissingVariableError), wxICON_ERROR);
					}
					break;
			}
			
		}
		
	}
	
	//init widgets
	InitWidgets wid;
	
	//apply settings
	if(Settings::ShowLog) {
		wid.menuItemOptionsLog->Check();
	}
	if(Settings::ColorMode == 0) {
		wid.menuItemOptionsLight->Check();
	}
	else if(Settings::ColorMode == 1) {
		wid.menuItemOptionsDark->Check();
	}
	else if(Settings::ColorMode == 2) {
		wid.menuItemOptionsSystem->Check();
	}
	wid.consoleLog->Show(Settings::ShowLog);
	
	initAllWidgets(&wid);

	positionWidgets(&wid);
	
	setToolTips(&wid);
	
	getAppearance(&wid);
	
	setAppearance(&wid, Settings::ColorMode);

	//temp
	wid.bannerPreview->SetBitmap(wxBitmap(wxString::FromUTF8(std::string(ProgramDir.ToUTF8()) + '/' + tempPath + '/' + "bannerpreview.png"), wxBITMAP_TYPE_ANY));
	wid.multiBannerPreview->SetBitmap(wxBitmap(wxString::FromUTF8(std::string(ProgramDir.ToUTF8()) + '/' + tempPath + '/' + "bannerpreview.png"), wxBITMAP_TYPE_ANY));
	
	//wxMessageBox(wxString::FromUTF8(std::string(ProgramDir.ToUTF8())));
	//wxMessageBox(wxString::FromUTF8(std::to_string(wid.modeChoiceBox->GetSelection())));
	
	//console example (temporary example)
	{
		wxArrayString output;
		wxArrayString errors;
		int ret = wxExecute(wxString::FromUTF8(resourcesPath + '/' + CLIFile + " -h"), output, errors);

		wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8(resourcesPath + '/' + CLIFile + " -h" + "\n==========\n"));
		for (auto &s : output) {
			wid.consoleLog->LogTextAtLevel(0, s);
		}
		wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret)));
	}

	//Binds must be done here for some reason
	//main menu
	
	/*
	// Create the request object (no work? oh well)
	wxWebRequest request = wxWebSession::GetDefault().CreateRequest(wid.frame, "https://api.github.com/repos/FoofooTheGuy/VidInjector9000/releases/latest");

	if (!request.IsOk()) {
		// This is not expected, but handle the error somehow.
	}

	// Bind state event
	wid.frame->Bind(wxEVT_WEBREQUEST_STATE, [](wxWebRequestEvent& evt) {
		switch (evt.GetState())
		{
			// Request completed
		case wxWebRequest::State_Completed:
		{
			wxString response = evt.GetResponse().AsString();
			//if (logoImage.IsOk())
				//wxLogInfo("Image successfully downloaded");

			//... do something with it ...
			wxMessageBox(response);

				break;
		}
		// Request failed
		case wxWebRequest::State_Failed:
			wxLogError("Could not load page: %s", evt.GetErrorDescription());
			break;
		}

	});*/
	
	wid.panel->Bind(wxEVT_SIZE, [&](wxSizeEvent& event) {
		//wxMessageBox(wxString::FromUTF8(std::to_string(event.GetSize().GetWidth()) + ", " + std::to_string(event.GetSize().GetHeight())));
		positionWidgets(&wid);
	});
	
	wid.bannerBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {
		wxArrayString output;
		wxArrayString errors;
		int ret = wxExecute(wxString::FromUTF8(resourcesPath + '/' + CLIFile + " -h"), output, errors);

		wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8(resourcesPath + '/' + CLIFile + " -h" + "\n==========\n"));
		for (auto &s : output) {
			wid.consoleLog->LogTextAtLevel(0, s);
		}
		wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret)));
	});
	
	for(auto &row : wid.PlayerTitles) {
		row->Bind(wxEVT_LEFT_UP, [&](wxMouseEvent& event) {//memory leak city???
			wxMessageBox(wxString::FromUTF8(row->GetLineText(0)));
		});
	}
	
	wid.mainMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {
		switch(event.GetId()) {
			case wxID_OPEN:
				{
					wxFileDialog openFileDialog(wid.frame, wxEmptyString, wxEmptyString, wxEmptyString, "Text Files (*.txt)|*.txt|All Files (*.*)|*.*", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
					openFileDialog.SetFilterIndex(0);
					if (openFileDialog.ShowModal() == wxID_OK) {
						//label->SetLabelText(wxString::Format("File = %s",  openFileDialog.GetPath()));
						wxMessageBox(openFileDialog.GetPath(), "Placeholder");
					}
				}
				break;
			case wxID_SAVEAS:
				{
					wxFileDialog saveFileDialog(wid.frame, wxEmptyString, "", "MyFile.txt", "Text Files (*.txt)|*.txt|All Files (*.*)|*.*", wxFD_SAVE);
					if (saveFileDialog.ShowModal() == wxID_OK) {
						//label->SetLabelText(wxString::Format("File = %s", saveFileDialog.GetPath()));
						wxMessageBox(saveFileDialog.GetPath(), "Placeholder");
					}
				}
				break;
			case ID_LOGBOOL:
				{
					bool checked = wid.menuItemOptionsLog->IsChecked();
					Settings::ShowLog = checked;
					wid.consoleLog->Show(Settings::ShowLog);
					saveSettings();
				}
				break;
			case ID_SYSTEM:
				{
					//2
					bool checked = wid.menuItemOptionsSystem->IsChecked();
					Settings::ColorMode = 2;
					setAppearance(&wid, Settings::ColorMode);
					saveSettings();
				}
				break;
			case ID_LIGHT:
				{
					//0
					bool checked = wid.menuItemOptionsLight->IsChecked();
					Settings::ColorMode = 0;
					setAppearance(&wid, Settings::ColorMode);
					saveSettings();
				}
				break;
			case ID_DARK:
				{
					//1
					bool checked = wid.menuItemOptionsDark->IsChecked();
					Settings::ColorMode = 1;
					setAppearance(&wid, Settings::ColorMode);
					saveSettings();
				}
				break;
		}
	});
	//consoleLog->LogTextAtLevel(0, wxString::Format("OUTPUT: %s", s));
	
	/*fontbutton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		wxFontData fontData;
		//fontData.SetInitialFont(consoleLog->GetFont());
		//fontData.SetColour(consoleLog->GetForegroundColour());
		wxFontDialog fontDialog(frame, fontData);
		if (fontDialog.ShowModal() == wxID_OK) {
			//consoleLog->SetFont(fontDialog.GetFontData().GetChosenFont());
			//label->SetForegroundColour(fontDialog.GetFontData().GetColour());
		}
	});*/
  
	//staticText1->SetFont({PointsToNativeFontGraphicsUntit(32), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD});
	//staticText1->SetForegroundColour({0, 139, 0});
  
	wid.frame->Show();
	wxTheApp->OnRun();
}