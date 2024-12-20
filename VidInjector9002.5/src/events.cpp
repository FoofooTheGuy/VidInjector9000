#include "events.hpp"

void panel_wxEVT_SIZE(InitWidgets* wid, VI9Pparameters* parameters) {
	//wxMessageBox(wxString::FromUTF8(std::to_string(event.GetSize().GetWidth()) + ", " + std::to_string(event.GetSize().GetHeight())));
	int width, height;
	wid->frame->GetSize(&width, &height);
	Settings::FrameWidth = width;
	Settings::FrameHeight = height;
	saveSettings();
	positionWidgets(wid, parameters);
}

void modeChoiceBox_wxEVT_CHOICE(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->modeChoiceBox->IsEnabled())
		return;
	int selection = wid->modeChoiceBox->GetSelection();
	if(selection != wxNOT_FOUND)
		parameters->mode = selection;
	{//-sp
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 0 \"" + std::to_string(parameters->mode) + "\" \"" + VI9P::WorkingFile + '\"');
		int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
	}
	
	applyMode(wid, parameters);
}

void bannerBox_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->bannerBox->IsEnabled())
		return;
	parameters->banner = std::string(wid->bannerBox->GetValue().ToUTF8());
	int ret = 0;
	
	{//-sp
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 1 \"" + parameters->banner + "\" \"" + VI9P::WorkingFile + '\"');
		ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
	}
	{//-gp
		std::string imagePath = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + "/bannerpreview.png";
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -gp \"" + VI9P::WorkingFile + "\" 1 \"" + imagePath + '\"');
		ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		
		wid->bannerPreview->SetBitmap(wxBitmap(wxString::FromUTF8(imagePath), wxBITMAP_TYPE_ANY));
	}
	if(ret == 17) {//custom non-video banner
		wid->bannerCustomText->Show();
		wid->bannerPreview->Show(false);
		wid->bannerError->Show(false);
		return;
	}
	else {
		wid->bannerCustomText->Show(false);
		wid->bannerPreview->Show();
	}
	if(ret && !parameters->banner.empty()) {
		wid->bannerError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + ImageInfoError + " (" + std::to_string(ret) + ") " + SeeLog));
		wid->bannerError->Show();
	}
	else wid->bannerError->Show(false);
}

void bannerBrowse_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->bannerBrowse->IsEnabled())
		return;
	wxFileDialog openFileDialog(wid->frame, wxEmptyString, wxString::FromUTF8(parameters->banner.empty() ? Settings::ImagesPath : parameters->banner.substr(0, parameters->banner.find_last_of("/\\"))), wxEmptyString, wxString::FromUTF8(allFiles), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	openFileDialog.SetFilterIndex(0);
	if (openFileDialog.ShowModal() == wxID_OK) {
		wid->bannerBox->SetValue(openFileDialog.GetPath());
	}
}

void iconBox_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->iconBox->IsEnabled())
		return;
	parameters->icon = std::string(wid->iconBox->GetValue().ToUTF8());
	int ret = 0;
	
	if(VI9P::Loading) {//so it doesnt mess up everything
		{//-gp
			std::string imagePath = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "iconpreview.png";
			wxArrayString output;
			wxArrayString errors;
			wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -gp \"" + VI9P::WorkingFile + "\" 2 \"" + imagePath + '\"');
			ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
			
			wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
			for (auto &s : output) {
				wid->consoleLog->LogTextAtLevel(0, s);
			}
			wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
			
			wid->iconPreview->SetBitmap(wxBitmap(wxString::FromUTF8(imagePath), wxBITMAP_TYPE_ANY));
		}
		if(ret && !parameters->icon.empty()) {
			wid->iconError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + ImageInfoError + " (" + std::to_string(ret) + ") " + SeeLog));
			wid->iconError->Show(true);
		}
		else wid->iconError->Show(false);
		return;
	}
	
	//we can just use ss since if you give that a normal image it will work like normal
	{//-ss
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -ss \"" + VI9P::WorkingFile + "\" \"" + parameters->icon + "\" \"" + VI9P::WorkingFile + '\"');
		ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		
		//if smdh (we dont want to keep that border)
		if(ret == 0) {
			parameters->iconBorder = 0;
			{//-sp
				wxArrayString output;
				wxArrayString errors;
				wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 3 " + std::to_string(parameters->iconBorder) + " \"" + VI9P::WorkingFile + '\"');
				ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);//overshadow?
				
				wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
				for (auto &s : output) {
					wid->consoleLog->LogTextAtLevel(0, s);
				}
				wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
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
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
			pp += std::string(s.ToUTF8()) + '\n';
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		
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
		
		wid->shortnameBox->SetValue(wxString::FromUTF8(parameters->Sname));
		wid->longnameBox->SetValue(wxString::FromUTF8(parameters->Lname));
		wid->publisherBox->SetValue(wxString::FromUTF8(parameters->publisher));
		{//iconBorder
			if(parameters->iconBorder > 2) parameters->iconBorder = 0;//why?
	
			{//-sp
				wxArrayString output;
				wxArrayString errors;
				wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 3 " + std::to_string(parameters->iconBorder) + " \"" + VI9P::WorkingFile + '\"');
				ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
				
				wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
				for (auto &s : output) {
					wid->consoleLog->LogTextAtLevel(0, s);
				}
				wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
			}
		}
	}
	{//-gp
		std::string imagePath = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "iconpreview.png";
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -gp \"" + VI9P::WorkingFile + "\" 2 \"" + imagePath + '\"');
		ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		
		wid->iconPreview->SetBitmap(wxBitmap(wxString::FromUTF8(imagePath), wxBITMAP_TYPE_ANY));
	}
	if(ret && !parameters->icon.empty()) {
		wid->iconError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + ImageInfoError + " (" + std::to_string(ret) + ") " + SeeLog));
		wid->iconError->Show(true);
	}
	else wid->iconError->Show(false);
}

void iconPreview_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->iconPreview->IsEnabled())
		return;
	++parameters->iconBorder;
	if(parameters->iconBorder > 2) parameters->iconBorder = 0;
	
	{//-sp
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 3 " + std::to_string(parameters->iconBorder) + " \"" + VI9P::WorkingFile + '\"');
		int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
	}
	{//-gp
		std::string imagePath = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "iconpreview.png";
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -gp \"" + VI9P::WorkingFile + "\" 2 \"" + imagePath + '\"');
		int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		
		wid->iconPreview->SetBitmap(wxBitmap(wxString::FromUTF8(imagePath), wxBITMAP_TYPE_ANY));
	}
}

void iconBrowse_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->iconBrowse->IsEnabled())
		return;
	wxFileDialog openFileDialog(wid->frame, wxEmptyString, wxString::FromUTF8(parameters->icon.empty() ? Settings::ImagesPath : parameters->icon.substr(0, parameters->icon.find_last_of("/\\"))), wxEmptyString, wxString::FromUTF8(allFiles), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	openFileDialog.SetFilterIndex(0);
	if (openFileDialog.ShowModal() == wxID_OK) {
		wid->iconBox->SetValue(openFileDialog.GetPath());
	}
}

void shortnameBox_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->shortnameBox->IsEnabled())
		return;
	parameters->Sname = std::string(wid->shortnameBox->GetValue().ToUTF8());
	
	{//-sp
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 4 \"" + fixDoubleQuote(parameters->Sname) + "\" \"" + VI9P::WorkingFile + '\"');
		int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
	}
	size_t UTF16Len = chrcount(parameters->Sname);//should this have null terminator?
	if(UTF16Len > 64) {//0x80/2=0x40=64
		wid->shortnameError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (" + std::to_string(UTF16Len) + "/64)"));
		wid->shortnameError->Show(true);
	}
	else wid->shortnameError->Show(false);
}

void longnameBox_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->longnameBox->IsEnabled())
		return;
	parameters->Lname = std::string(wid->longnameBox->GetValue().ToUTF8());
	
	{//-sp
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 5 \"" + fixDoubleQuote(parameters->Lname) + "\" \"" + VI9P::WorkingFile + '\"');
		int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
	}
	size_t UTF16Len = chrcount(parameters->Lname);
	if(UTF16Len > 128) {//0x100/2=0x80=128
		wid->longnameError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (" + std::to_string(UTF16Len) + "/128)"));
		wid->longnameError->Show(true);
	}
	else wid->longnameError->Show(false);
}

void publisherBox_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->publisherBox->IsEnabled())
		return;
	parameters->publisher = std::string(wid->publisherBox->GetValue().ToUTF8());
	
	{//-sp
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 6 \"" + fixDoubleQuote(parameters->publisher) + "\" \"" + VI9P::WorkingFile + '\"');
		int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
	}
	size_t UTF16Len = chrcount(parameters->publisher);
	if(UTF16Len > 64) {//0x80/2=0x40=64
		wid->publisherError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (" + std::to_string(UTF16Len) + "/64)"));
		wid->publisherError->Show(true);
	}
	else wid->publisherError->Show(false);
}

void copyBox_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->copyBox->IsEnabled())
		return;
	parameters->copyrightInfo = std::string(wid->copyBox->GetValue().ToUTF8());
	
	{//-sp
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 8 \"" + fixDoubleQuote(parameters->copyrightInfo) + "\" \"" + VI9P::WorkingFile + '\"');
		int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
	}
}

void copyCheck_wxEVT_CHECKBOX(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->copyCheck->IsEnabled())
		return;
	parameters->copycheck = wid->copyCheck->GetValue();
	
	{//-sp
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 7 \"" + std::to_string(parameters->copycheck) + "\" \"" + VI9P::WorkingFile + '\"');
		int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
	}
	wid->copyBox->Enable(parameters->copycheck);
}

void ffRewindCheck_wxEVT_CHECKBOX(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->ffRewindCheck->IsEnabled())
		return;
	parameters->FFrewind = wid->ffRewindCheck->GetValue();
	
	{//-sp
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 9 \"" + std::to_string(parameters->FFrewind) + "\" \"" + VI9P::WorkingFile + '\"');
		int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
	}
}

void dimCheck_wxEVT_CHECKBOX(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->dimCheck->IsEnabled())
		return;
	parameters->FadeOpt = wid->dimCheck->GetValue();
	
	{//-sp
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 10 \"" + std::to_string(parameters->FadeOpt) + "\" \"" + VI9P::WorkingFile + '\"');
		int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
	}
}

void multiBannerPreview_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->multiBannerPreview->IsEnabled())
		return;
	wxFileDialog openFileDialog(wid->frame, wxEmptyString, wxString::FromUTF8(parameters->MBannerVec.at(VI9P::MultiBannerIndex).empty() ? Settings::ImagesPath : parameters->MBannerVec.at(VI9P::MultiBannerIndex).substr(0, parameters->MBannerVec.at(VI9P::MultiBannerIndex).find_last_of("/\\"))), wxEmptyString, wxString::FromUTF8(allFiles), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	openFileDialog.SetFilterIndex(0);
	if (openFileDialog.ShowModal() == wxID_OK) {
		wid->MenuBanners.at(VI9P::MultiBannerIndex)->SetValue((openFileDialog.GetPath()));
	}
}

void multiBannerPreviewLeft_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->multiBannerPreviewLeft->IsEnabled())
		return;
	if(VI9P::MultiBannerIndex > 0) --VI9P::MultiBannerIndex;
	if(VI9P::MultiBannerIndex <= 0) wid->multiBannerPreviewLeft->Enable(false);

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
	wid->multiBannerPreviewIndex->SetLabel(wxString::FromUTF8(std::to_string(VI9P::MultiBannerIndex + 1) + "/" + std::to_string(wid->MenuBanners.size())));
	wid->multiBannerPreviewRight->Enable(true);

	wid->multiBannerPreviewLeft->SetCursor(wid->multiBannerPreviewLeft->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	wid->multiBannerPreviewRight->SetCursor(wid->multiBannerPreviewRight->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
}

void multiBannerPreviewRight_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->multiBannerPreviewRight->IsEnabled())
		return;
	if(VI9P::MultiBannerIndex < wid->MenuBanners.size() - 1) ++VI9P::MultiBannerIndex;
	if(VI9P::MultiBannerIndex >= wid->MenuBanners.size() - 1) wid->multiBannerPreviewRight->Enable(false);
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
	wid->multiBannerPreviewIndex->SetLabel(wxString::FromUTF8(std::to_string(VI9P::MultiBannerIndex + 1) + "/" + std::to_string(wid->MenuBanners.size())));
	wid->multiBannerPreviewLeft->Enable(true);
	
	wid->multiBannerPreviewLeft->SetCursor(wid->multiBannerPreviewLeft->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	wid->multiBannerPreviewRight->SetCursor(wid->multiBannerPreviewRight->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
}

void PlayerTitles_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters, wxTextCtrl* row) {
	size_t rowReal;
	for(rowReal = 0; rowReal < wid->PlayerTitles.size(); rowReal++) {//get row
		if(reinterpret_cast<intptr_t>(wid->PlayerTitles.at(rowReal)) == reinterpret_cast<intptr_t>(row)) {//compare pointers
			if(!row->IsEnabled())
				return;
			parameters->PTitleVec.at(rowReal) = std::string(row->GetValue().ToUTF8());
			
			{//-sp
				wxArrayString output;
				wxArrayString errors;
				wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + rowReal) + " \"" + fixDoubleQuote(parameters->PTitleVec.at(rowReal)) + "\" \"" + VI9P::WorkingFile + '\"');
				int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
				
				wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
				for (auto &s : output) {
					wid->consoleLog->LogTextAtLevel(0, s);
				}
				wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
			}
		}
	}
}

void MoflexFiles_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters, wxTextCtrl* row) {
	size_t rowReal;
	for(rowReal = 0; rowReal < wid->MoflexFiles.size(); rowReal++) {//get row
		if(reinterpret_cast<intptr_t>(wid->MoflexFiles.at(rowReal)) == reinterpret_cast<intptr_t>(row)) {//compare pointers
			if(!row->IsEnabled())
				return;
			parameters->MoflexVec.at(rowReal) = std::string(row->GetValue().ToUTF8());
			
			{//-sp
				wxArrayString output;
				wxArrayString errors;
				wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + parameters->rows + rowReal) + " \"" + parameters->MoflexVec.at(rowReal) + "\" \"" + VI9P::WorkingFile + '\"');
				int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
				
				wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
				for (auto &s : output) {
					wid->consoleLog->LogTextAtLevel(0, s);
				}
				wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
			}
		}
	}
}

void MenuBanners_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters, wxTextCtrl* row) {
	size_t rowReal;
	for(rowReal = 0; rowReal < wid->MenuBanners.size(); rowReal++) {//get row
		if(reinterpret_cast<intptr_t>(wid->MenuBanners.at(rowReal)) == reinterpret_cast<intptr_t>(row)) {//compare pointers
			if(!row->IsEnabled())
				return;
			VI9P::MultiBannerIndex = rowReal;
			parameters->MBannerVec.at(VI9P::MultiBannerIndex) = std::string(row->GetValue().ToUTF8());
			
			{//-sp
				wxArrayString output;
				wxArrayString errors;
				wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + (parameters->rows * 2) + VI9P::MultiBannerIndex) + " \"" + parameters->MBannerVec.at(rowReal) + "\" \"" + VI9P::WorkingFile + '\"');
				int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
				
				wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
				for (auto &s : output) {
					wid->consoleLog->LogTextAtLevel(0, s);
				}
				wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
			}
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
			wid->multiBannerPreviewIndex->SetLabel(wxString::FromUTF8(std::to_string(VI9P::MultiBannerIndex + 1) + "/" + std::to_string(wid->MenuBanners.size())));
		}
	}
}

void MultiUp_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters, wxButton* row) {
	size_t rowReal;
	for(rowReal = 0; rowReal < wid->MultiUp.size(); rowReal++) {//get row
		if(reinterpret_cast<intptr_t>(wid->MultiUp.at(rowReal)) == reinterpret_cast<intptr_t>(row)) {//compare pointers
			if(!row->IsEnabled())
				return;
			if(!row->IsShown())
				return;
			if(rowReal > 0) {
				//the stuff will get saved if the text in the boxes changes so dont save here
				{//PlayerTitles
					wxString tempStr(wid->PlayerTitles.at(rowReal)->GetValue());
					wid->PlayerTitles.at(rowReal)->SetValue(wid->PlayerTitles.at(rowReal - 1)->GetValue());
					wid->PlayerTitles.at(rowReal - 1)->SetValue(tempStr);
				}
				{//MoflexFiles
					wxString tempStr(wid->MoflexFiles.at(rowReal)->GetValue());
					wid->MoflexFiles.at(rowReal)->SetValue(wid->MoflexFiles.at(rowReal - 1)->GetValue());
					wid->MoflexFiles.at(rowReal - 1)->SetValue(tempStr);
				}
				{//MenuBanners
					wxString tempStr(wid->MenuBanners.at(rowReal)->GetValue());
					wid->MenuBanners.at(rowReal)->SetValue(wid->MenuBanners.at(rowReal - 1)->GetValue());
					wid->MenuBanners.at(rowReal - 1)->SetValue(tempStr);
				}
			}
		}
	}
}

void MultiDown_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters, wxButton* row) {
	size_t rowReal;
	for(rowReal = 0; rowReal < wid->MultiDown.size(); rowReal++) {//get row
		if(reinterpret_cast<intptr_t>(wid->MultiDown.at(rowReal)) == reinterpret_cast<intptr_t>(row)) {//compare pointers
			if(!row->IsEnabled())
				return;
			if(!row->IsShown())
				return;
			if(rowReal < wid->MultiDown.size()) {
				//the stuff will get saved if the text in the boxes changes so dont save here
				{//PlayerTitles
					wxString tempStr(wid->PlayerTitles.at(rowReal)->GetValue());
					wid->PlayerTitles.at(rowReal)->SetValue(wid->PlayerTitles.at(rowReal + 1)->GetValue());
					wid->PlayerTitles.at(rowReal + 1)->SetValue(tempStr);
				}
				{//MoflexFiles
					wxString tempStr(wid->MoflexFiles.at(rowReal)->GetValue());
					wid->MoflexFiles.at(rowReal)->SetValue(wid->MoflexFiles.at(rowReal + 1)->GetValue());
					wid->MoflexFiles.at(rowReal + 1)->SetValue(tempStr);
				}
				{//MenuBanners
					wxString tempStr(wid->MenuBanners.at(rowReal)->GetValue());
					wid->MenuBanners.at(rowReal)->SetValue(wid->MenuBanners.at(rowReal + 1)->GetValue());
					wid->MenuBanners.at(rowReal + 1)->SetValue(tempStr);
				}
			}
		}
	}
}

void moflexBrowse_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->moflexBrowse->IsEnabled())
		return;
	size_t row;
	for(row = 0; row < wid->MoflexFiles.size() - 1; row++) {
		if(std::string(wid->MoflexFiles.at(row)->GetValue().ToUTF8()).empty()) {
			break;
		}
	}
	wxArrayString paths;
	wxFileDialog openFileDialog(wid->frame, wxEmptyString, wxString::FromUTF8(parameters->MoflexVec.at(row ? row - 1 : row).empty() ? Settings::VideosPath : parameters->MoflexVec.at(row ? row - 1 : row).substr(0, parameters->MoflexVec.at(row ? row - 1 : row).find_last_of("/\\"))), wxEmptyString, moflexFiles, wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_MULTIPLE);
	openFileDialog.SetFilterIndex(0);
	if (openFileDialog.ShowModal() == wxID_OK) {
		openFileDialog.GetPaths(paths);
		paths.Sort();
		if(paths.GetCount() == 1) {
			wid->MoflexFiles.at(row)->SetValue(paths.Last());
		}
		for(size_t i = 0; i < paths.GetCount(); i++) {
			if(i < wid->MoflexFiles.size())
				wid->MoflexFiles.at(i)->SetValue(paths.Item(i));
		}
	}
}

void multiBannerBrowse_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->multiBannerBrowse->IsEnabled())
		return;
	size_t row;
	for(row = 0; row < wid->MenuBanners.size() - 1; row++) {
		if(std::string(wid->MenuBanners.at(row)->GetValue().ToUTF8()).empty()) {
			break;
		}
	}
	wxArrayString paths;
	wxFileDialog openFileDialog(wid->frame, wxEmptyString, wxString::FromUTF8(parameters->MBannerVec.at(row ? row - 1 : row).empty() ? Settings::ImagesPath : parameters->MBannerVec.at(row ? row - 1 : row).substr(0, parameters->MBannerVec.at(row ? row - 1 : row).find_last_of("/\\"))), wxEmptyString, wxString::FromUTF8(allFiles), wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_MULTIPLE);
	openFileDialog.SetFilterIndex(0);
	if (openFileDialog.ShowModal() == wxID_OK) {
		openFileDialog.GetPaths(paths);
		paths.Sort();
		if(paths.GetCount() == 1) {
			wid->MenuBanners.at(row)->SetValue(paths.Last());
		}
		for(size_t i = 0; i < paths.GetCount(); i++) {
			if(i < wid->MenuBanners.size())
				wid->MenuBanners.at(i)->SetValue(paths.Item(i));
		}
	}
	
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
	wid->multiBannerPreviewLeft->SetCursor(wid->multiBannerPreviewLeft->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
	wid->multiBannerPreviewRight->SetCursor(wid->multiBannerPreviewRight->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
}

void removeRow_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->removeRow->IsEnabled())
		return;
	if(parameters->rows - 1 <= parameters->splitPos) {
		wid->splitPatchButton->SetValue(false);
		splitPatchButton_wxEVT_TOGGLEBUTTON(wid, parameters);
	}
	if(wid->removeRow->IsEnabled()) {
		wid->removeRow->Enable(false);//disable so you cant press it too much
		wid->appendRow->Enable(false);
	
		removeRows(wid, parameters);
		wid->rowText->SetLabel(wxString::FromUTF8(std::to_string(parameters->rows) + "/27"));
		wid->multiBannerPreviewIndex->SetLabel(wxString::FromUTF8(std::to_string(VI9P::MultiBannerIndex + 1) + "/" + std::to_string(wid->MenuBanners.size())));
	}
	
	if(parameters->mode) {
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
		wid->multiBannerPreviewLeft->SetCursor(wid->multiBannerPreviewLeft->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
		wid->multiBannerPreviewRight->SetCursor(wid->multiBannerPreviewRight->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
		
		wid->rowText->Show(true);
		if(parameters->rows > 1 && parameters->rows < 27) {
			wid->appendRow->Enable(true);
			wid->removeRow->Enable(true);
			wid->splitPatchButton->Enable(true);
		}
		if(parameters->rows <= 1) {
			wid->appendRow->Enable(true);
			wid->removeRow->Enable(false);
			wid->splitPatchButton->Enable(false);
		}
		else if(parameters->rows >= 27) {
			wid->appendRow->Enable(false);
			wid->removeRow->Enable(true);
			wid->splitPatchButton->Enable(true);
		}
	}
	else {
		wid->rowText->Show(false);
		if(parameters->rows > 1) {
			wid->removeRow->Enable(true);
			wid->splitPatchButton->Enable(true);
		}
		else {
			wid->removeRow->Enable(false);
			wid->splitPatchButton->Enable(false);
		}
		wid->appendRow->Enable(false);
	}
	ShowPatchUpDown(wid, parameters);
	ShowMultiUpDown(wid);
	setCursors(wid);
	setToolTips(wid);
}

void splitPatchButton_wxEVT_TOGGLEBUTTON(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->splitPatchButton->IsEnabled())
		return;
	parameters->splitPos = wid->splitPatchButton->GetValue();
	
	{//-sp
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 11 \"" + std::to_string(parameters->splitPos) + "\" \"" + VI9P::WorkingFile + '\"');
		int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
	}
	wid->splitPatchLine->Show(parameters->splitPos);
	ShowPatchUpDown(wid, parameters);
	positionWidgets(wid, parameters);
}

void splitPatchUp_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->splitPatchUp->IsEnabled())
		return;
	if(!wid->splitPatchUp->IsShown())
		return;
	--parameters->splitPos;
	
	{//-sp
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 11 \"" + std::to_string(parameters->splitPos) + "\" \"" + VI9P::WorkingFile + '\"');
		int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
	}
	
	ShowPatchUpDown(wid, parameters);
	positionWidgets(wid, parameters);
}

void splitPatchDown_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->splitPatchDown->IsEnabled())
		return;
	if(!wid->splitPatchDown->IsShown())
		return;
	++parameters->splitPos;
	
	{//-sp
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 11 \"" + std::to_string(parameters->splitPos) + "\" \"" + VI9P::WorkingFile + '\"');
		int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
	}
	
	ShowPatchUpDown(wid, parameters);
	positionWidgets(wid, parameters);
}

void buildframe_wxEVT_CLOSE_WINDOW(InitWidgets* wid, wxCloseEvent* event) {
	event->Veto();
	wid->buildframe->Show(false);
	
	cancelButton_wxEVT_BUTTON(wid);
}

void buildpanel_wxEVT_SIZE(InitWidgets* wid, VI9Pparameters* parameters) {
	positionWidgets(wid, parameters);
}

void titleIDButton_wxEVT_BUTTON(InitWidgets* wid) {
	if(!wid->titleIDButton->IsEnabled())
		return;
	uint32_t uniqueID = RandomTID();
	char uniqueIDstr [6];
	sprintf(uniqueIDstr, "%05X", uniqueID);
	
	wid->titleIDBox->SetValue(wxString::FromUTF8(std::string(uniqueIDstr)));
}

void exportArchive_wxEVT_END_PROCESS(InitWidgets* wid, wxProcessEvent* event) {
	if(event->GetPid() != 0) {
		if(event->GetExitCode() != 0) {
			wxMessageBox(wxString::FromUTF8(ErrorText + ' ' + BuildError + " (" + std::to_string(event->GetExitCode()) + ")\n" + SeeLog), wxString::FromUTF8(ErrorText), wxICON_ERROR, wid->buildframe);
		}
		else {
			std::error_code error;
			
			error = copyfile(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "out.cia", Exports::CIA);
			if (error)
				wxMessageBox(wxString::FromUTF8(CopyFileError + '\n' + std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "out.cia") + " -> " +  Exports::CIA + '\n' + error.message()), wxString::FromUTF8(ErrorText), wxICON_ERROR, wid->buildframe);
			
			if(std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(Exports::CIA).c_str()), error))
				wxMessageBox(wxString::FromUTF8(Exports::CIA), wxString::FromUTF8(SuccessfullyBuilt), wxOK|wxCENTRE, wid->buildframe);
			else
				wxMessageBox(wxString::FromUTF8(BuildError + " (" + Exports::CIA + ')'), wxString::FromUTF8(ErrorText), wxICON_ERROR, wid->buildframe);
			if(error)
				wxMessageBox(wxString::FromUTF8(BuildError + " (" + Exports::CIA + ")\n" + error.message()), wxString::FromUTF8(ErrorText), wxICON_ERROR, wid->buildframe);
			
			if(!Exports::TAR.empty()) {
				error = copyfile(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "out.tar", Exports::TAR);
				if (error)
					wxMessageBox(wxString::FromUTF8(CopyFileError + '\n' + std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "out.tar") + " -> " +  Exports::TAR + '\n' + error.message()), wxString::FromUTF8(ErrorText), wxICON_ERROR, wid->buildframe);
				
				if(std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(Exports::TAR).c_str()), error))
					wxMessageBox(wxString::FromUTF8(Exports::TAR), wxString::FromUTF8(SuccessfullyBuilt), wxOK|wxCENTRE, wid->buildframe);
				else
					wxMessageBox(wxString::FromUTF8(BuildError + " (" + Exports::TAR + ')'), wxString::FromUTF8(ErrorText), wxICON_ERROR, wid->buildframe);
				if(error)
					wxMessageBox(wxString::FromUTF8(BuildError + " (" + Exports::TAR + ")\n" + error.message()), wxString::FromUTF8(ErrorText), wxICON_ERROR, wid->buildframe);
			}
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(event->GetExitCode()) + '\n'));
	}
	wid->statusText->Show(false);
	wid->exportLogger->Stop();
	wid->barPulser->Stop();
	wid->buildBar->SetValue(0);
	wid->frame->Enable(true);
	setCursors(wid);
}

void buildButton_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters) {
	if(!wid->buildButton->IsEnabled())
		return;
	std::string uniqueID = std::string(wid->titleIDBox->GetValue().ToUTF8());
	std::string appName = std::string(wid->applicationTitleBox->GetValue().ToUTF8());
	std::string prodCode = std::string(wid->productCodeBox->GetValue().ToUTF8());
	
	Exports::CIA = "";
	Exports::TAR = "";
	wxFileDialog saveCIADialog(wid->frame, wxEmptyString, wxEmptyString, wxString::FromUTF8(std::string(wid->longnameBox->GetValue().ToUTF8()) + " [000400000" + std::string(wid->titleIDBox->GetValue().ToUTF8()) + "00].cia"), wxString::FromUTF8(ciaFiles), wxFD_SAVE);
	if (saveCIADialog.ShowModal() != wxID_OK) {
		return;
	}
	Exports::CIA = addMissingFileExtension(std::string(saveCIADialog.GetPath().ToUTF8()), ".cia");
	
	if(parameters->mode && parameters->splitPos) {
		wxFileDialog saveTARDialog(wid->frame, wxEmptyString, wxString::FromUTF8(Exports::CIA.substr(0, Exports::CIA.find_last_of("\\/") == std::string::npos ? 0 : Exports::CIA.find_last_of("\\/"))), wxString::FromUTF8(std::string(wid->longnameBox->GetValue().ToUTF8()) + " [000400000" + std::string(wid->titleIDBox->GetValue().ToUTF8()) + "00].tar"), wxString::FromUTF8(tarFiles), wxFD_SAVE);
		if (saveTARDialog.ShowModal() == wxID_OK) {
			Exports::TAR = addMissingFileExtension(std::string(saveTARDialog.GetPath().ToUTF8()), ".tar");
		}
	}
	
	if(Exports::CIA.empty())//just make sure
		return;
	
	//execute async process
	wxString command = "";
	if(Exports::TAR.empty())
		command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -bc \"" + VI9P::WorkingFile + "\" \"" + uniqueID + "\" \"" + appName + "\" \"" + prodCode + "\" \"" + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + "/out.cia\"");
	else
		command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -bc \"" + VI9P::WorkingFile + "\" \"" + uniqueID + "\" \"" + appName + "\" \"" + prodCode + "\" \"" + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + "/out.cia\" \"" + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + "/out.tar\"");
	
	wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
	
	wid->exportArchive->Redirect();
	
	long PID = wxExecute(command, wxEXEC_MAKE_GROUP_LEADER|wxEXEC_ASYNC, wid->exportArchive);
	
	if (PID = 0) {
		wxLogError(wxString::FromUTF8(ErrorText + ' ' + command));
		return;
	}
	
	wid->frame->Enable(false);
	setCursors(wid);
	wid->barPulser->Start(100);
	wid->statusText->Show();
	wid->exportLogger->Start(1);//poll for output every X milliseconds... i guess
}

void cancelButton_wxEVT_BUTTON(InitWidgets* wid) {
	if(!wid->cancelButton->IsEnabled())
		return;
	if(wid->statusText->IsShown()) {//dumb but it is sure to work
		int ret = 0;
		ret = wxProcess::Kill(wid->exportArchive->GetPid(), wxSIGTERM, wxKILL_CHILDREN);
		if(ret != wxKILL_OK) {
			wxLogError(wxString::FromUTF8(ErrorText + ' ' + std::to_string(ret) + "\n(" + std::to_string(ret) + ')'));
			return;
		}
	}
	wxProcessEvent event(0, 0, 0);
	exportArchive_wxEVT_END_PROCESS(wid, &event);
}

void barPulser_wxEVT_TIMER(InitWidgets* wid) {
	wid->buildBar->Pulse();
}

void exportLogger_wxEVT_TIMER(InitWidgets* wid) {
	//https://github.com/wxWidgets/wxWidgets/blob/master/samples/exec/exec.cpp#L1373
	if (wid->exportArchive->IsInputAvailable()) {
		wxTextInputStream Stream(*wid->exportArchive->GetInputStream());

		// this assumes that the output is always line buffered
		wxString msg;
		msg << Stream.ReadLine();
		//wxMessageBox(msg);
		
		wid->consoleLog->LogTextAtLevel(0, msg);
		
		{//statusText
			wid->statusText->SetLabel(msg);
			
			int w, h;
			wxFont f;
			
			f = wid->statusText->GetFont();
			
			wid->statusText->GetTextExtent(wid->statusText->GetLabel(), &w, &h, nullptr, nullptr, &f);
			wid->statusText->SetSize(w, h);
		}
		{//statusText
			int y, panelwidth, mywidth;
			wid->buildpanel->GetSize(&panelwidth, NULL);
			wid->statusText->GetSize(&mywidth, NULL);
			wid->statusText->GetPosition(NULL, &y);
			
			wid->statusText->Move((panelwidth - mywidth) / 2, y);
		}
	}
}

void extractDialog_wxEVT_CLOSE_WINDOW(InitWidgets* wid, wxCloseEvent* event) {
	event->Veto();
}

void extractArchive_wxEVT_END_PROCESS(InitWidgets* wid, wxProcessEvent* event) {
	if(event->GetPid() != 0) {
		if(event->GetExitCode() != 0) {
			wxMessageBox(wxString::FromUTF8(ErrorText + ' ' + BuildError + " (" + std::to_string(event->GetExitCode()) + ")\n" + SeeLog), wxString::FromUTF8(ErrorText), wxICON_ERROR);
		}
		else {
			//VI9P::OutFile = Extracted::Archive + "/parameters.vi9p";//generated vi9p from extraction parameters.vi9p
			
			std::error_code error;
			if(std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(Extracted::Archive).c_str()), error))
				wxMessageBox(wxString::FromUTF8(Extracted::Archive), wxString::FromUTF8(SuccessfullyExtracted));
			else
				wxMessageBox(wxString::FromUTF8(BuildError + " (" + Extracted::Archive + ')'), wxString::FromUTF8(ErrorText), wxICON_ERROR);
			if(error)
				wxMessageBox(wxString::FromUTF8(BuildError + " (" + Extracted::Archive + ")\n" + error.message()), wxString::FromUTF8(ErrorText), wxICON_ERROR);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(event->GetExitCode()) + '\n'));
	}
	wid->extractLogger->Stop();
	wid->extractPulser->Stop();
	//wid->extractDialog->SetValue(0);
	wid->extractDialog->Show(false);
	wid->frame->Enable(true);
	setCursors(wid);
}

void extractPulser_wxEVT_TIMER(InitWidgets* wid) {
	wid->extractDialog->Pulse();
	
	//cancel
	if(wid->extractDialog->WasCancelled()) {
		int ret = 0;
		ret = wxProcess::Kill(wid->extractArchive->GetPid(), wxSIGTERM, wxKILL_CHILDREN);
		if(ret != wxKILL_OK) {
			wxLogError(wxString::FromUTF8(ErrorText + ' ' + std::to_string(ret) + "\n(" + std::to_string(ret) + ')'));
			return;
		}
		wxProcessEvent event(0, 0, 0);
		extractArchive_wxEVT_END_PROCESS(wid, &event);
	}
}

void extractLogger_wxEVT_TIMER(InitWidgets* wid) {
	//https://github.com/wxWidgets/wxWidgets/blob/master/samples/exec/exec.cpp#L1373
	if (wid->extractArchive->IsInputAvailable()) {
		wxTextInputStream Stream(*wid->extractArchive->GetInputStream());

		// this assumes that the output is always line buffered
		wxString msg;
		msg << Stream.ReadLine();
		//wxMessageBox(msg);
		
		wid->consoleLog->LogTextAtLevel(0, msg);//why no get
		
		wid->extractDialog->Update(0, msg);//what even
	}
}

void aboutframe_wxEVT_CLOSE_WINDOW(InitWidgets* wid, wxCloseEvent* event) {
	event->Veto();
	wid->aboutframe->Show(false);
}

void aboutpanel_wxEVT_SIZE(InitWidgets* wid, VI9Pparameters* parameters) {
	positionWidgets(wid, parameters);
}
