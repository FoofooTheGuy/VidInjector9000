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
	if(ret && !parameters->banner.empty()) {
		wid->bannerError->SetLabel(wxString::FromUTF8(ErrorText + ' ' + ImageInfoError + " (" + std::to_string(ret) + ") " + SeeLog));
		wid->bannerError->Show(true);
	}
	else wid->bannerError->Show(false);
}

void bannerBrowse_wxEVT_BUTTON(InitWidgets* wid) {
	wxFileDialog openFileDialog(wid->frame, wxEmptyString, wxEmptyString, wxEmptyString, "All Files (*.*)|*.*", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	openFileDialog.SetFilterIndex(0);
	if (openFileDialog.ShowModal() == wxID_OK) {
		wid->bannerBox->SetValue(openFileDialog.GetPath());
	}
}

void iconBox_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters) {
	parameters->icon = std::string(wid->iconBox->GetValue().ToUTF8());
	int ret = 0;
	
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

void iconBrowse_wxEVT_BUTTON(InitWidgets* wid) {
	wxFileDialog openFileDialog(wid->frame, wxEmptyString, wxEmptyString, wxEmptyString, "All Files (*.*)|*.*", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	openFileDialog.SetFilterIndex(0);
	if (openFileDialog.ShowModal() == wxID_OK) {
		wid->iconBox->SetValue(openFileDialog.GetPath());
	}
}

void shortnameBox_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters) {
	parameters->Sname = std::string(wid->shortnameBox->GetValue().ToUTF8());
	
	{//-sp
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 4 \"" + parameters->Sname + "\" \"" + VI9P::WorkingFile + '\"');
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
	parameters->Lname = std::string(wid->longnameBox->GetValue().ToUTF8());
	
	{//-sp
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 5 \"" + parameters->Lname + "\" \"" + VI9P::WorkingFile + '\"');
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
	parameters->publisher = std::string(wid->publisherBox->GetValue().ToUTF8());
	
	{//-sp
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 6 \"" + parameters->publisher + "\" \"" + VI9P::WorkingFile + '\"');
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
	parameters->copyrightInfo = std::string(wid->copyBox->GetValue().ToUTF8());
	
	{//-sp
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 8 \"" + parameters->copyrightInfo + "\" \"" + VI9P::WorkingFile + '\"');
		int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
	}
}

void copyCheck_wxEVT_CHECKBOX(InitWidgets* wid, VI9Pparameters* parameters) {
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
	parameters->FadeOpt = wid->dimCheck->GetValue();
	
	{//-sp
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" 9 \"" + std::to_string(parameters->FadeOpt) + "\" \"" + VI9P::WorkingFile + '\"');
		int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);
		
		wid->consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid->consoleLog->LogTextAtLevel(0, s);
		}
		wid->consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
	}
}

void multiBannerPreview_wxEVT_BUTTON(InitWidgets* wid) {
	wxFileDialog openFileDialog(wid->frame, wxEmptyString, wxEmptyString, wxEmptyString, "All Files (*.*)|*.*", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	openFileDialog.SetFilterIndex(0);
	if (openFileDialog.ShowModal() == wxID_OK) {
		wid->MenuBanners.at(VI9P::MultiBannerIndex)->SetValue((openFileDialog.GetPath()));
	}
}

void multiBannerPreviewLeft_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters) {
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
			parameters->PTitleVec.at(rowReal) = std::string(row->GetValue().ToUTF8());
			
			{//-sp
				wxArrayString output;
				wxArrayString errors;
				wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -sp \"" + VI9P::WorkingFile + "\" " + std::to_string(12 + rowReal) + " \"" + parameters->PTitleVec.at(rowReal) + "\" \"" + VI9P::WorkingFile + '\"');
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

void moflexBrowse_wxEVT_BUTTON(InitWidgets* wid) {
	wxFileDialog openFileDialog(wid->frame, wxEmptyString, wxEmptyString, wxEmptyString, moflexFiles, wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	openFileDialog.SetFilterIndex(0);
	if (openFileDialog.ShowModal() == wxID_OK) {
		//wid->iconBox->SetValue(openFileDialog.GetPath());
		for(const auto &row : wid->MoflexFiles) {
			if(std::string(row->GetValue().ToUTF8()).empty()) {
				row->SetValue(openFileDialog.GetPath());
				break;
			}
		}
	}
}

void multiBannerBrowse_wxEVT_BUTTON(InitWidgets* wid) {
	wxFileDialog openFileDialog(wid->frame, wxEmptyString, wxEmptyString, wxEmptyString, "All Files (*.*)|*.*", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	openFileDialog.SetFilterIndex(0);
	if (openFileDialog.ShowModal() == wxID_OK) {
		//wid->iconBox->SetValue(openFileDialog.GetPath());
		bool dolast = false;
		for(const auto &row : wid->MenuBanners) {
			if(std::string(row->GetValue().ToUTF8()).empty()) {
				row->SetValue(openFileDialog.GetPath());
				dolast = false;
				break;
			}
			dolast = true;
		}
		if(dolast) {
			wid->MenuBanners.back()->SetValue((openFileDialog.GetPath()));
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
	if(parameters->rows - 1 < parameters->splitPos) {
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
}

void splitPatchButton_wxEVT_TOGGLEBUTTON(InitWidgets* wid, VI9Pparameters* parameters) {
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
