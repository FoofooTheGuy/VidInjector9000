#include "appInitializer.hpp"
#include "settings.hpp"
#include "widgets.hpp"
#include "events.hpp"

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
	
	//TODO: load language
	
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
	
	{//test CLI
		wxArrayString output;
		wxArrayString errors;
		wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + '\"');
		int ret = wxExecute(command, output, errors, wxEXEC_SYNC | wxEXEC_NODISABLE);

		wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
		for (auto &s : output) {
			wid.consoleLog->LogTextAtLevel(0, s);
		}
		wid.consoleLog->LogTextAtLevel(0, wxString::FromUTF8("\n==========\n" + Return + " : " + std::to_string(ret) + '\n'));
		
		if(ret != 1) {//1 is the expected return value of that command
			wxMessageBox(wxString::FromUTF8(CLIError), wxString::FromUTF8(ErrorText), wxICON_ERROR);
		}
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
	
	setCursors(&wid);
	
	wid.panel->Bind(wxEVT_SIZE, [&](wxSizeEvent& event) {
		panel_wxEVT_SIZE(&wid);
	});
	
	wid.modeChoiceBox->Bind(wxEVT_CHOICE, [&](wxCommandEvent& event) {
		modeChoiceBox_wxEVT_CHOICE(&wid, &parameters);
	});
	
	wid.bannerBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {
		bannerBox_wxEVT_TEXT(&wid, &parameters);
	});
	
	wid.bannerBrowse->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		bannerBrowse_wxEVT_BUTTON(&wid);
	});

	wid.iconBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {
		iconBox_wxEVT_TEXT(&wid, &parameters);
	});
	
	wid.iconPreview->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		iconPreview_wxEVT_BUTTON(&wid, &parameters);
	});

	wid.iconBrowse->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		iconBrowse_wxEVT_BUTTON(&wid);
	});
	
	wid.shortnameBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {
		shortnameBox_wxEVT_TEXT(&wid, &parameters);
	});

	wid.longnameBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {
		longnameBox_wxEVT_TEXT(&wid, &parameters);
	});
	
	wid.publisherBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {
		publisherBox_wxEVT_TEXT(&wid, &parameters);
	});

	wid.copyBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {
		copyBox_wxEVT_TEXT(&wid, &parameters);
	});

	wid.copyCheck->Bind(wxEVT_CHECKBOX, [&](wxCommandEvent& event) {
		copyCheck_wxEVT_CHECKBOX(&wid, &parameters);
	});

	wid.ffRewindCheck->Bind(wxEVT_CHECKBOX, [&](wxCommandEvent& event) {
		ffRewindCheck_wxEVT_CHECKBOX(&wid, &parameters);
	});

	wid.dimCheck->Bind(wxEVT_CHECKBOX, [&](wxCommandEvent& event) {
		dimCheck_wxEVT_CHECKBOX(&wid, &parameters);
	});
	
	wid.multiBannerPreview->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		multiBannerPreview_wxEVT_BUTTON(&wid);
	});

	wid.multiBannerPreviewLeft->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		multiBannerPreviewLeft_wxEVT_BUTTON(&wid, &parameters);
	});

	wid.multiBannerPreviewRight->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		multiBannerPreviewRight_wxEVT_BUTTON(&wid, &parameters);
	});
	
	for(const auto &row : wid.PlayerTitles) {
		row->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {//memory leak city???
			PlayerTitles_wxEVT_TEXT(&wid, &parameters, row);
		});
	}

	for(const auto &row : wid.MoflexFiles) {
		row->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {//memory leak city???
			MoflexFiles_wxEVT_TEXT(&wid, &parameters, row);
		});
	}
	
	for(const auto &row : wid.MenuBanners) {
		row->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {//memory leak city???
			MenuBanners_wxEVT_TEXT(&wid, &parameters, row);
		});
	}

	for(const auto &row : wid.MultiUp) {
		row->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {//memory leak city???
			MultiUp_wxEVT_BUTTON(&wid, &parameters, row);
		});
	}

	for(const auto &row : wid.MultiDown) {
		row->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {//memory leak city???
			MultiDown_wxEVT_BUTTON(&wid, &parameters, row);
		});
	}
	
	wid.moflexBrowse->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		moflexBrowse_wxEVT_BUTTON(&wid);
	});

	wid.multiBannerBrowse->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		multiBannerBrowse_wxEVT_BUTTON(&wid);
	});
	
	wid.removeRow->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		removeRow_wxEVT_BUTTON(&wid, &parameters);
	});

	wid.appendRow->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		if(wid.appendRow->IsEnabled()) {
			wid.removeRow->Enable(false);//disable so you cant press it too much
			wid.appendRow->Enable(false);
			
			addRows(&wid, &parameters);
			
			//well what do you know... it worked
			for(const auto &row : wid.PlayerTitles) {
				row->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {//memory leak city???
					PlayerTitles_wxEVT_TEXT(&wid, &parameters, row);
				});
			}
			for(const auto &row : wid.MoflexFiles) {
				row->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {//memory leak city???
					MoflexFiles_wxEVT_TEXT(&wid, &parameters, row);
				});
			}
			for(const auto &row : wid.MenuBanners) {
				row->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {//memory leak city???
					MenuBanners_wxEVT_TEXT(&wid, &parameters, row);
				});
			}
			for(const auto &row : wid.MultiUp) {
				row->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {//memory leak city???
					MultiUp_wxEVT_BUTTON(&wid, &parameters, row);
				});
			}
			for(const auto &row : wid.MultiDown) {
				row->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {//memory leak city???
					MultiDown_wxEVT_BUTTON(&wid, &parameters, row);
				});
			}
			setAppearance(&wid, Settings::ColorMode);
			
			wid.rowText->SetLabel(wxString::FromUTF8(std::to_string(parameters.rows) + "/27"));
			wid.multiBannerPreviewIndex->SetLabel(wxString::FromUTF8(std::to_string(VI9P::MultiBannerIndex + 1) + "/" + std::to_string(wid.MenuBanners.size())));
		}
		
		if(parameters.mode) {
			if(wid.MenuBanners.size() != 1) {
				if(VI9P::MultiBannerIndex > 0 && VI9P::MultiBannerIndex < wid.MenuBanners.size() - 1) {
					wid.multiBannerPreviewLeft->Enable(true);
					wid.multiBannerPreviewRight->Enable(true);
				}
				if(VI9P::MultiBannerIndex <= 0) {
					wid.multiBannerPreviewLeft->Enable(false);
					wid.multiBannerPreviewRight->Enable(true);
				}
				if(VI9P::MultiBannerIndex >= wid.MenuBanners.size() - 1) {
					wid.multiBannerPreviewLeft->Enable(true);
					wid.multiBannerPreviewRight->Enable(false);
				}
			}
			else {
				wid.multiBannerPreviewLeft->Enable(false);
				wid.multiBannerPreviewRight->Enable(false);
			}
		
			wid.rowText->Show(true);
			if(parameters.rows > 1 && parameters.rows < 27) {
				wid.appendRow->Enable(true);
				wid.removeRow->Enable(true);
			}
			else if(parameters.rows <= 1) {
				wid.appendRow->Enable(true);
				wid.removeRow->Enable(false);
			}
			else if(parameters.rows >= 27) {
				wid.appendRow->Enable(false);
				wid.removeRow->Enable(true);
			}
		}
		else {
			wid.rowText->Show(false);
			wid.appendRow->Enable(false);
		}
		wid.multiBannerPreviewLeft->SetCursor(wid.multiBannerPreviewLeft->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
		wid.multiBannerPreviewRight->SetCursor(wid.multiBannerPreviewRight->IsEnabled() ? wxCURSOR_HAND : wxCURSOR_ARROW);
		
		wid.removeRow->Enable(true);
		
		setCursors(&wid);
	});
	
	wid.splitPatchButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		
	});
	
	wid.mainMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {
		switch(event.GetId()) {
			case wxID_OPEN:
				{
					wxFileDialog openFileDialog(wid.frame, wxEmptyString, wxEmptyString, wxEmptyString, vi9pFiles, wxFD_OPEN|wxFD_FILE_MUST_EXIST);
					openFileDialog.SetFilterIndex(0);
					if (openFileDialog.ShowModal() == wxID_OK) {
						//label->SetLabelText(wxString::Format("File = %s",  openFileDialog.GetPath()));
						//wxMessageBox(openFileDialog.GetPath(), "Placeholder");
						VI9P::WorkingFile = std::string(openFileDialog.GetPath().ToUTF8());
						VI9P::MultiBannerIndex = 0;
						loadParameters(&wid, &parameters);
						positionWidgets(&wid);
						
						//well what do you know... it worked
						for(const auto &row : wid.PlayerTitles) {
							row->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {//memory leak city???
								PlayerTitles_wxEVT_TEXT(&wid, &parameters, row);
							});
						}
						for(const auto &row : wid.MoflexFiles) {
							row->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {//memory leak city???
								MoflexFiles_wxEVT_TEXT(&wid, &parameters, row);
							});
						}
						for(const auto &row : wid.MenuBanners) {
							row->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {//memory leak city???
								MenuBanners_wxEVT_TEXT(&wid, &parameters, row);
							});
						}
						for(const auto &row : wid.MultiUp) {
							row->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {//memory leak city???
								MultiUp_wxEVT_BUTTON(&wid, &parameters, row);
							});
						}
						for(const auto &row : wid.MultiDown) {
							row->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {//memory leak city???
								MultiDown_wxEVT_BUTTON(&wid, &parameters, row);
							});
						}
						setAppearance(&wid, Settings::ColorMode);
						setCursors(&wid);
						
						wid.rowText->SetLabel(wxString::FromUTF8(std::to_string(parameters.rows) + "/27"));
						wid.multiBannerPreviewIndex->SetLabel(wxString::FromUTF8(std::to_string(VI9P::MultiBannerIndex + 1) + "/" + std::to_string(wid.MenuBanners.size())));
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
