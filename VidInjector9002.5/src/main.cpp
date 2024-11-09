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
				case 8:
					{
						//bad value
					}
					break;
				case 9:
					{
						wxMessageBox(wxString::FromUTF8(FailedToFindVar + ' ' + IntFrameWidth + '\n' + ValueNoChange), wxString::FromUTF8(ErrorText + ' ' + MissingVariableError), wxICON_ERROR);
					}
					break;
				case 10:
					{
						//bad value
					}
					break;
				case 11:
					{
						wxMessageBox(wxString::FromUTF8(FailedToFindVar + ' ' + IntFrameHeight + '\n' + ValueNoChange), wxString::FromUTF8(ErrorText + ' ' + MissingVariableError), wxICON_ERROR);
					}
					break;
			}
			
		}
		
	}
	
	//init widgets
	InitWidgets wid;
	
	{//clear temp
		std::error_code error;
		
		std::filesystem::remove_all(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath).c_str()), error);
		if (error)
			wxMessageBox(wxString::FromUTF8(std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '\n' + error.message())), wxString::FromUTF8(ErrorText));

		std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath).c_str()), error);
		if (error)
			wxMessageBox(wxString::FromUTF8(std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '\n' + error.message())), wxString::FromUTF8(ErrorText));
	}
	
	if(VI9P::WorkingFile.empty())
		VI9P::WorkingFile = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "parameters.vi9p";
	VI9Pparameters parameters;

	{//-n (for temp dir)
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -n \"" + VI9P::WorkingFile + '\"');
		int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

		wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid.consoleLog->LogTextAtLevel(0, s);
		}
		wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
	}
	
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

	//temporary
	//wid.bannerPreview->SetBitmap(wxBitmap(wxString::FromUTF8(std::string(ProgramDir.ToUTF8()) + '/' + tempPath + '/' + "bannerpreview.png"), wxBITMAP_TYPE_ANY));
	  //wid.multiBannerPreview->SetBitmap(wxBitmap(wxString::FromUTF8(std::string(ProgramDir.ToUTF8()) + '/' + tempPath + '/' + "bannerpreview.png"), wxBITMAP_TYPE_ANY));
	//VI9P::WorkingFile = "/home/user/Desktop/VidInjector9000/VidInjector9002-CLI/test/test.vi9p";
	//loadParameters(&wid, &parameters);
	//wxMessageBox(wxString::FromUTF8(parameters.Sname));

	//wxMessageBox(wxString::FromUTF8(std::string(ProgramDir.ToUTF8())));
	//wxMessageBox(wxString::FromUTF8(std::to_string(wid.modeChoiceBox->GetSelection())));
	
	//console example (temporary)
	/*{
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8(resourcesPath + '/' + CLIFile + " -pp " + VI9P::WorkingFile);
		int ret = wxExecute(command, output, errors);
		std::string pp;

		wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid.consoleLog->LogTextAtLevel(0, s);
			pp += std::string(s.ToUTF8()) + '\n';
		}
		wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret)));
		wxMessageBox(wxString::FromUTF8(pp));
	}*/

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
		int width, height;
		wid.frame->GetSize(&width, &height);
		Settings::FrameWidth = width;
		Settings::FrameHeight = height;
		saveSettings();
		positionWidgets(&wid);
	});
	
	wid.modeChoiceBox->Bind(wxEVT_CHOICE, [&](wxCommandEvent& event) {
		int selection = wid.modeChoiceBox->GetSelection();
		if(selection != wxNOT_FOUND)
			parameters.mode = selection;
		{//-sp
			wxArrayString output;
			wxArrayString errors;
			wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 0 \"" + std::to_string(parameters.mode) + "\" \"" + VI9P::WorkingFile + '\"');
			int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

			wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
			for (auto &s : output) {
				wid.consoleLog->LogTextAtLevel(0, s);
			}
			wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		}
		
		if(parameters.mode) {
			wid.rowText->Show(true);
		}
		else {
			wid.rowText->Show(false);
		}
	});
	
	wid.bannerBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {
		parameters.banner = std::string(wid.bannerBox->GetValue().ToUTF8());
		int ret = 0;
		
		{//-sp
			wxArrayString output;
			wxArrayString errors;
			wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 1 \"" + parameters.banner + "\" \"" + VI9P::WorkingFile + '\"');
			ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

			wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
			for (auto &s : output) {
				wid.consoleLog->LogTextAtLevel(0, s);
			}
			wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		}
		{//-gp
			std::string imagePath = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + "/bannerpreview.png";
			wxArrayString output;
			wxArrayString errors;
			wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -gp \"" + VI9P::WorkingFile + "\" 1 \"" + imagePath + '\"');
			ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
			
			wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
			for (auto &s : output) {
				wid.consoleLog->LogTextAtLevel(0, s);
			}
			wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
			
			wid.bannerPreview->SetBitmap(wxBitmap(wxString::FromUTF8(imagePath), wxBITMAP_TYPE_ANY));
		}
		if(ret && !parameters.banner.empty()) {
			wid.bannerError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + ImageInfoError + " (" + std::to_string(ret) + ") " + SeeLog));
			wid.bannerError->Show(true);
		}
		else wid.bannerError->Show(false);
	});
	
	wid.bannerBrowse->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		{
			wxFileDialog openFileDialog(wid.frame, wxEmptyString, wxEmptyString, wxEmptyString, "All Files (*.*)|*.*", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
			openFileDialog.SetFilterIndex(0);
			if (openFileDialog.ShowModal() == wxID_OK) {
				wid.bannerBox->SetValue(openFileDialog.GetPath());
			}
		}
	});

	wid.iconBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {
		parameters.icon = std::string(wid.iconBox->GetValue().ToUTF8());
		int ret = 0;
		
		//we can just use ss since if you give that a normal image it will work like normal
		{//-ss
			wxArrayString output;
			wxArrayString errors;
			wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -ss \"" + VI9P::WorkingFile + "\" \"" + parameters.icon + "\" \"" + VI9P::WorkingFile + '\"');
			ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

			wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
			for (auto &s : output) {
				wid.consoleLog->LogTextAtLevel(0, s);
			}
			wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
			
			//if smdh (we dont want to keep that border)
			if(ret == 0) {
				parameters.iconBorder = 0;
				{//-sp
					wxArrayString output;
					wxArrayString errors;
					wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 3 " + std::to_string(parameters.iconBorder) + " \"" + VI9P::WorkingFile + '\"');
					ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);//overshadow?
					
					wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
					for (auto &s : output) {
						wid.consoleLog->LogTextAtLevel(0, s);
					}
					wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
				}
			}
		}
		//this causes an infinite loop for some reason, probably because it changes the text of this box
		//loadParameters(&wid, &parameters);
		
		//do this in case of smdh
		{//-pp
			wxArrayString output;
			wxArrayString errors;
			wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -pp \"" + VI9P::WorkingFile + '\"');
			ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
			std::string pp;

			wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
			for (auto &s : output) {
				wid.consoleLog->LogTextAtLevel(0, s);
				pp += std::string(s.ToUTF8()) + '\n';
			}
			wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));

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
			
			wid.shortnameBox->SetValue(wxString::FromUTF8(parameters.Sname));
			wid.longnameBox->SetValue(wxString::FromUTF8(parameters.Lname));
			wid.publisherBox->SetValue(wxString::FromUTF8(parameters.publisher));
			{//iconBorder
				if(parameters.iconBorder > 2) parameters.iconBorder = 0;//why?
		
				{//-sp
					wxArrayString output;
					wxArrayString errors;
					wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 3 " + std::to_string(parameters.iconBorder) + " \"" + VI9P::WorkingFile + '\"');
					ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
					
					wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
					for (auto &s : output) {
						wid.consoleLog->LogTextAtLevel(0, s);
					}
					wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
				}
			}
		}
		{//-gp
			std::string imagePath = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "iconpreview.png";
			wxArrayString output;
			wxArrayString errors;
			wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -gp \"" + VI9P::WorkingFile + "\" 2 \"" + imagePath + '\"');
			ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
			
			wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
			for (auto &s : output) {
				wid.consoleLog->LogTextAtLevel(0, s);
			}
			wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
			
			wid.iconPreview->SetBitmap(wxBitmap(wxString::FromUTF8(imagePath), wxBITMAP_TYPE_ANY));
		}
		if(ret && !parameters.icon.empty()) {
			wid.iconError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + ImageInfoError + " (" + std::to_string(ret) + ") " + SeeLog));
			wid.iconError->Show(true);
		}
		else wid.iconError->Show(false);
	});
	
	wid.iconPreview->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		++parameters.iconBorder;
		if(parameters.iconBorder > 2) parameters.iconBorder = 0;
		
		{//-sp
			wxArrayString output;
			wxArrayString errors;
			wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 3 " + std::to_string(parameters.iconBorder) + " \"" + VI9P::WorkingFile + '\"');
			int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

			wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
			for (auto &s : output) {
				wid.consoleLog->LogTextAtLevel(0, s);
			}
			wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		}
		{//-gp
			std::string imagePath = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "iconpreview.png";
			wxArrayString output;
			wxArrayString errors;
			wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -gp \"" + VI9P::WorkingFile + "\" 2 \"" + imagePath + '\"');
			int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

			wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
			for (auto &s : output) {
				wid.consoleLog->LogTextAtLevel(0, s);
			}
			wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
			
			wid.iconPreview->SetBitmap(wxBitmap(wxString::FromUTF8(imagePath), wxBITMAP_TYPE_ANY));
		}
	});

	wid.iconBrowse->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		wxFileDialog openFileDialog(wid.frame, wxEmptyString, wxEmptyString, wxEmptyString, "All Files (*.*)|*.*", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
		openFileDialog.SetFilterIndex(0);
		if (openFileDialog.ShowModal() == wxID_OK) {
			wid.iconBox->SetValue(openFileDialog.GetPath());
		}
	});
	
	wid.shortnameBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {
		parameters.Sname = std::string(wid.shortnameBox->GetValue().ToUTF8());
		
		{//-sp
			wxArrayString output;
			wxArrayString errors;
			wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 4 \"" + parameters.Sname + "\" \"" + VI9P::WorkingFile + '\"');
			int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

			wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
			for (auto &s : output) {
				wid.consoleLog->LogTextAtLevel(0, s);
			}
			wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		}
		size_t UTF16Len = chrcount(parameters.Sname);//should this have null terminator?
		if(UTF16Len > 64) {//0x80/2=0x40=64
			wid.shortnameError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (" + std::to_string(UTF16Len) + "/64)"));
			wid.shortnameError->Show(true);
		} else wid.shortnameError->Show(false);
	});

	wid.longnameBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {
		parameters.Lname = std::string(wid.longnameBox->GetValue().ToUTF8());
		
		{//-sp
			wxArrayString output;
			wxArrayString errors;
			wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 5 \"" + parameters.Lname + "\" \"" + VI9P::WorkingFile + '\"');
			int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

			wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
			for (auto &s : output) {
				wid.consoleLog->LogTextAtLevel(0, s);
			}
			wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		}
		size_t UTF16Len = chrcount(parameters.Lname);
		if(UTF16Len > 128) {//0x100/2=0x80=128
			wid.longnameError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (" + std::to_string(UTF16Len) + "/128)"));
			wid.longnameError->Show(true);
		} else wid.longnameError->Show(false);
	});
	
	wid.publisherBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {
		parameters.publisher = std::string(wid.publisherBox->GetValue().ToUTF8());
		
		{//-sp
			wxArrayString output;
			wxArrayString errors;
			wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 6 \"" + parameters.publisher + "\" \"" + VI9P::WorkingFile + '\"');
			int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

			wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
			for (auto &s : output) {
				wid.consoleLog->LogTextAtLevel(0, s);
			}
			wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		}
		size_t UTF16Len = chrcount(parameters.publisher);
		if(UTF16Len > 64) {//0x80/2=0x40=64
			wid.publisherError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (" + std::to_string(UTF16Len) + "/64)"));
			wid.publisherError->Show(true);
		} else wid.publisherError->Show(false);
	});

	wid.copyBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {
		parameters.copyrightInfo = std::string(wid.copyBox->GetValue().ToUTF8());
		
		{//-sp
			wxArrayString output;
			wxArrayString errors;
			wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 8 \"" + parameters.copyrightInfo + "\" \"" + VI9P::WorkingFile + '\"');
			int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

			wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
			for (auto &s : output) {
				wid.consoleLog->LogTextAtLevel(0, s);
			}
			wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		}
	});

	wid.copyCheck->Bind(wxEVT_CHECKBOX, [&](wxCommandEvent& event) {
		parameters.copycheck = wid.copyCheck->GetValue();
		
		{//-sp
			wxArrayString output;
			wxArrayString errors;
			wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 7 \"" + std::to_string(parameters.copycheck) + "\" \"" + VI9P::WorkingFile + '\"');
			int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

			wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
			for (auto &s : output) {
				wid.consoleLog->LogTextAtLevel(0, s);
			}
			wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		}
	});

	wid.ffRewindCheck->Bind(wxEVT_CHECKBOX, [&](wxCommandEvent& event) {
		parameters.FFrewind = wid.ffRewindCheck->GetValue();
		
		{//-sp
			wxArrayString output;
			wxArrayString errors;
			wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 9 \"" + std::to_string(parameters.FFrewind) + "\" \"" + VI9P::WorkingFile + '\"');
			int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

			wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
			for (auto &s : output) {
				wid.consoleLog->LogTextAtLevel(0, s);
			}
			wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		}
	});

	wid.dimCheck->Bind(wxEVT_CHECKBOX, [&](wxCommandEvent& event) {
		parameters.FadeOpt = wid.dimCheck->GetValue();
		
		{//-sp
			wxArrayString output;
			wxArrayString errors;
			wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 9 \"" + std::to_string(parameters.FadeOpt) + "\" \"" + VI9P::WorkingFile + '\"');
			int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

			wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
			for (auto &s : output) {
				wid.consoleLog->LogTextAtLevel(0, s);
			}
			wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		}
	});
	
	for(const auto &row : wid.PlayerTitles) {
		row->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {//memory leak city???
			size_t rowReal;
			for(rowReal = 0; rowReal < wid.PlayerTitles.size(); rowReal++) {//get row
				if(reinterpret_cast<intptr_t>(wid.PlayerTitles.at(rowReal)) == reinterpret_cast<intptr_t>(row)) {//compare pointers
					parameters.PTitleVec.at(rowReal) = std::string(row->GetValue().ToUTF8());
					
					{//-sp
						wxArrayString output;
						wxArrayString errors;
						wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + rowReal) + " \"" + parameters.PTitleVec.at(rowReal) + "\" \"" + VI9P::WorkingFile + '\"');
						int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

						wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
						for (auto &s : output) {
							wid.consoleLog->LogTextAtLevel(0, s);
						}
						wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
					}
				}
			}
		});
	}

	for(const auto &row : wid.MoflexFiles) {
		row->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {//memory leak city???
			size_t rowReal;
			for(rowReal = 0; rowReal < wid.MoflexFiles.size(); rowReal++) {//get row
				if(reinterpret_cast<intptr_t>(wid.MoflexFiles.at(rowReal)) == reinterpret_cast<intptr_t>(row)) {//compare pointers
					parameters.MoflexVec.at(rowReal) = std::string(row->GetValue().ToUTF8());
					
					{//-sp
						wxArrayString output;
						wxArrayString errors;
						wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + parameters.rows + rowReal) + " \"" + parameters.MoflexVec.at(rowReal) + "\" \"" + VI9P::WorkingFile + '\"');
						int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

						wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
						for (auto &s : output) {
							wid.consoleLog->LogTextAtLevel(0, s);
						}
						wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
					}
				}
			}
		});
	}
	
	for(const auto &row : wid.MenuBanners) {
		row->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {//memory leak city???
			size_t rowReal;
			for(rowReal = 0; rowReal < wid.MenuBanners.size(); rowReal++) {//get row
				if(reinterpret_cast<intptr_t>(wid.MenuBanners.at(rowReal)) == reinterpret_cast<intptr_t>(row)) {//compare pointers
					parameters.MBannerVec.at(rowReal) = std::string(row->GetValue().ToUTF8());
					
					{//-sp
						wxArrayString output;
						wxArrayString errors;
						wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + (parameters.rows * 2) + rowReal) + " \"" + parameters.MBannerVec.at(rowReal) + "\" \"" + VI9P::WorkingFile + '\"');
						int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

						wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
						for (auto &s : output) {
							wid.consoleLog->LogTextAtLevel(0, s);
						}
						wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
					}
					{//-gp
						std::string imagePath = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + "/bannerpreview" + std::to_string(rowReal) + ".png";
						wxArrayString output;
						wxArrayString errors;
						wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -gp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + (parameters.rows * 2) + rowReal) + " \"" + imagePath + '\"');
						int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
						
						wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
						for (auto &s : output) {
							wid.consoleLog->LogTextAtLevel(0, s);
						}
						wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
						
						wid.multiBannerPreview->SetBitmap(wxBitmap(wxString::FromUTF8(imagePath), wxBITMAP_TYPE_ANY));
					}
				}
			}
		});
	}
	
	wid.moflexBrowse->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		wxFileDialog openFileDialog(wid.frame, wxEmptyString, wxEmptyString, wxEmptyString, moflexFiles, wxFD_OPEN|wxFD_FILE_MUST_EXIST);
		openFileDialog.SetFilterIndex(0);
		if (openFileDialog.ShowModal() == wxID_OK) {
			//wid.iconBox->SetValue(openFileDialog.GetPath());
			for(const auto &row : wid.MoflexFiles) {
				if(std::string(row->GetValue().ToUTF8()).empty()) {
					row->SetValue(openFileDialog.GetPath());
					break;
				}
			}
		}
	});

	wid.multiBannerBrowse->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		wxFileDialog openFileDialog(wid.frame, wxEmptyString, wxEmptyString, wxEmptyString, "All Files (*.*)|*.*", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
		openFileDialog.SetFilterIndex(0);
		if (openFileDialog.ShowModal() == wxID_OK) {
			//wid.iconBox->SetValue(openFileDialog.GetPath());
			bool dolast = false;
			for(const auto &row : wid.MenuBanners) {
				if(std::string(row->GetValue().ToUTF8()).empty()) {
					row->SetValue(openFileDialog.GetPath());
					dolast = false;
					break;
				}
				dolast = true;
			}
			if(dolast) {
				wid.MenuBanners.back()->SetValue((openFileDialog.GetPath()));
			}
		}
	});
	
	wid.removeRow->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		if(wid.removeRow->IsEnabled()) {
			wid.removeRow->Enable(false);//disable so you cant press it too much
			wid.appendRow->Enable(false);
		
			removeRows(&wid, &parameters);
			wid.rowText->SetLabel(wxString::FromUTF8(std::to_string(parameters.rows) + "/27"));
		}
		
		if(parameters.mode) {
			wid.rowText->Show(true);
		}
		else {
			wid.rowText->Show(false);
		}
		
		if(parameters.rows <= 1) {
			wid.removeRow->Enable(false);
		}
		else wid.removeRow->Enable(true);
		
		wid.appendRow->Enable(true);
	});

	wid.appendRow->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {//TODO: fix the why the hec it doesnt work the first time you click it
		if(wid.removeRow->IsEnabled()) {
			wid.removeRow->Enable(false);//disable so you cant press it too much
			wid.appendRow->Enable(false);
			
			addRows(&wid, &parameters);
			
			//well what do you know... it worked
			for(const auto &row : wid.PlayerTitles) {
				row->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {//memory leak city???
					size_t rowReal;
					for(rowReal = 0; rowReal < wid.PlayerTitles.size(); rowReal++) {//get row
						if(reinterpret_cast<intptr_t>(wid.PlayerTitles.at(rowReal)) == reinterpret_cast<intptr_t>(row)) {//compare pointers
							parameters.PTitleVec.at(rowReal) = std::string(row->GetValue().ToUTF8());
							
							{//-sp
								wxArrayString output;
								wxArrayString errors;
								wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + rowReal) + " \"" + parameters.PTitleVec.at(rowReal) + "\" \"" + VI9P::WorkingFile + '\"');
								int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

								wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
								for (auto &s : output) {
									wid.consoleLog->LogTextAtLevel(0, s);
								}
								wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
							}
						}
					}
				});
			}
			
			for(const auto &row : wid.MoflexFiles) {
				row->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {//memory leak city???
					size_t rowReal;
					for(rowReal = 0; rowReal < wid.MoflexFiles.size(); rowReal++) {//get row
						if(reinterpret_cast<intptr_t>(wid.MoflexFiles.at(rowReal)) == reinterpret_cast<intptr_t>(row)) {//compare pointers
							parameters.MoflexVec.at(rowReal) = std::string(row->GetValue().ToUTF8());
							
							{//-sp
								wxArrayString output;
								wxArrayString errors;
								wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + parameters.rows + rowReal) + " \"" + parameters.MoflexVec.at(rowReal) + "\" \"" + VI9P::WorkingFile + '\"');
								int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

								wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
								for (auto &s : output) {
									wid.consoleLog->LogTextAtLevel(0, s);
								}
								wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
							}
						}
					}
				});
			}
			
			for(const auto &row : wid.MenuBanners) {
				row->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {//memory leak city???
					size_t rowReal;
					for(rowReal = 0; rowReal < wid.MenuBanners.size(); rowReal++) {//get row
						if(reinterpret_cast<intptr_t>(wid.MenuBanners.at(rowReal)) == reinterpret_cast<intptr_t>(row)) {//compare pointers
							parameters.MBannerVec.at(rowReal) = std::string(row->GetValue().ToUTF8());
							
							{//-sp
								wxArrayString output;
								wxArrayString errors;
								wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + (parameters.rows * 2) + rowReal) + " \"" + parameters.MBannerVec.at(rowReal) + "\" \"" + VI9P::WorkingFile + '\"');
								int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

								wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
								for (auto &s : output) {
									wid.consoleLog->LogTextAtLevel(0, s);
								}
								wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
							}
							{//-gp
								std::string imagePath = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + "/bannerpreview" + std::to_string(rowReal) + ".png";
								wxArrayString output;
								wxArrayString errors;
								wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -gp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + (parameters.rows * 2) + rowReal) + " \"" + imagePath + '\"');
								int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
								
								wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
								for (auto &s : output) {
									wid.consoleLog->LogTextAtLevel(0, s);
								}
								wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
								
								wid.multiBannerPreview->SetBitmap(wxBitmap(wxString::FromUTF8(imagePath), wxBITMAP_TYPE_ANY));
							}
						}
					}
				});
			}
			
			wid.rowText->SetLabel(wxString::FromUTF8(std::to_string(parameters.rows) + "/27"));
		}
		
		if(parameters.mode) {
			wid.rowText->Show(true);
		}
		else {
			wid.rowText->Show(false);
		}
		
		if(parameters.rows >= 27) {
			wid.appendRow->Enable(false);
		}
		else wid.appendRow->Enable(true);
		
		wid.removeRow->Enable(true);
	});
	
	wid.mainMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {
		switch(event.GetId()) {
			case wxID_OPEN:
				{
					wxFileDialog openFileDialog(wid.frame, wxEmptyString, wxEmptyString, wxEmptyString, "VI9P Files (*.vi9p)|*.vi9p", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
					openFileDialog.SetFilterIndex(0);
					if (openFileDialog.ShowModal() == wxID_OK) {
						//label->SetLabelText(wxString::Format("File = %s",  openFileDialog.GetPath()));
						//wxMessageBox(openFileDialog.GetPath(), "Placeholder");
						VI9P::WorkingFile = std::string(openFileDialog.GetPath().ToUTF8());
						loadParameters(&wid, &parameters);
						positionWidgets(&wid);
						
						for(const auto &row : wid.PlayerTitles) {
							row->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {//memory leak city???
								size_t rowReal;
								for(rowReal = 0; rowReal < wid.PlayerTitles.size(); rowReal++) {//get row
									if(reinterpret_cast<intptr_t>(wid.PlayerTitles.at(rowReal)) == reinterpret_cast<intptr_t>(row)) {//compare pointers
										parameters.PTitleVec.at(rowReal) = std::string(row->GetValue().ToUTF8());
										
										{//-sp
											wxArrayString output;
											wxArrayString errors;
											wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + rowReal) + " \"" + parameters.PTitleVec.at(rowReal) + "\" \"" + VI9P::WorkingFile + '\"');
											int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

											wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
											for (auto &s : output) {
												wid.consoleLog->LogTextAtLevel(0, s);
											}
											wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
										}
									}
								}
							});
						}
						
						for(const auto &row : wid.MoflexFiles) {
							row->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {//memory leak city???
								size_t rowReal;
								for(rowReal = 0; rowReal < wid.MoflexFiles.size(); rowReal++) {//get row
									if(reinterpret_cast<intptr_t>(wid.MoflexFiles.at(rowReal)) == reinterpret_cast<intptr_t>(row)) {//compare pointers
										parameters.MoflexVec.at(rowReal) = std::string(row->GetValue().ToUTF8());
										
										{//-sp
											wxArrayString output;
											wxArrayString errors;
											wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + parameters.rows + rowReal) + " \"" + parameters.MoflexVec.at(rowReal) + "\" \"" + VI9P::WorkingFile + '\"');
											int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

											wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
											for (auto &s : output) {
												wid.consoleLog->LogTextAtLevel(0, s);
											}
											wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
										}
									}
								}
							});
						}
						
						for(const auto &row : wid.MenuBanners) {
							row->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {//memory leak city???
								size_t rowReal;
								for(rowReal = 0; rowReal < wid.MenuBanners.size(); rowReal++) {//get row
									if(reinterpret_cast<intptr_t>(wid.MenuBanners.at(rowReal)) == reinterpret_cast<intptr_t>(row)) {//compare pointers
										parameters.MBannerVec.at(rowReal) = std::string(row->GetValue().ToUTF8());
										
										{//-sp
											wxArrayString output;
											wxArrayString errors;
											wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + (parameters.rows * 2) + rowReal) + " \"" + parameters.MBannerVec.at(rowReal) + "\" \"" + VI9P::WorkingFile + '\"');
											int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

											wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
											for (auto &s : output) {
												wid.consoleLog->LogTextAtLevel(0, s);
											}
											wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
										}
										{//-gp
											std::string imagePath = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + "/bannerpreview" + std::to_string(rowReal) + ".png";
											wxArrayString output;
											wxArrayString errors;
											wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -gp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + (parameters.rows * 2) + rowReal) + " \"" + imagePath + '\"');
											int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
											
											wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
											for (auto &s : output) {
												wid.consoleLog->LogTextAtLevel(0, s);
											}
											wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
											
											wid.multiBannerPreview->SetBitmap(wxBitmap(wxString::FromUTF8(imagePath), wxBITMAP_TYPE_ANY));
										}
									}
								}
							});
						}
					
						wid.rowText->SetLabel(wxString::FromUTF8(std::to_string(parameters.rows) + "/27"));
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
	
	//plz call this last
	loadParameters(&wid, &parameters);
	
	wid.frame->Show();
	wxTheApp->OnRun();
}
