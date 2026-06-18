#include "appInitializer.hpp"
#include "language.hpp"
#include "settings.hpp"
#include "widgets.hpp"
//#include "events.hpp"

/*
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
*/

int progmain(int argc, char* argv[]) {
	AppInitializer appInitializer;

	{// get program dir
		std::string programpath(std::filesystem::absolute(std::filesystem::path((const char8_t*)&*std::string(argv[0]).c_str())).string());
		programpath = programpath.substr(0, programpath.find_last_of("\\/"));
		ProgramDir = wxString::FromUTF8(programpath);
		//wxMessageBox(ProgramDir);
	}
	
	// load settings and handle errors
	{
		std::vector<int> ret = loadSettings();
		// contain the repetitive setting strings (You will likely want to put your stuff here when adding new Settings)
		std::vector<std::string> SettingStrings = {
			IntDefaultLanguage,
			IntShowLog,
			IntColorMode,
			IntDeleteTemp,
			IntUpdateCheck,
			IntFrameWidth,
			IntFrameHeight,
		};
		for(const auto &i : ret) {
			int cases = 0;
			if(i == cases++) { // 0
				continue; // :)
			}
			if(i == cases++) { // 1
				continue; // uhhh i dont care
			}
			if(i == cases++) { // 2
				continue; // this is actually bad so there should probably be a message here even though it's rare
			}
			
			for(const auto &s : SettingStrings) {
				(void)s;
				//std::cout << std::to_string((i - 3) / 2) << std::endl;
				if(i == cases++) {
					wxMessageBox(wxString::FromUTF8(BadValue + ' ' + SettingStrings.at((i - 3) / 2) + '\n' + ValueNoChange), wxString::FromUTF8(ErrorText + ' ' + BadValue), wxICON_ERROR);
					continue;
				}
				if(i == cases++) {
					wxMessageBox(wxString::FromUTF8(FailedToFindVar + ' ' + SettingStrings.at((i - (3 + 1)) / 2) + '\n' + ValueNoChange), wxString::FromUTF8(ErrorText + ' ' + MissingVariableError), wxICON_ERROR);
					continue;
				}
			}
			
			if(i == cases++) {
				wxMessageBox(wxString::FromUTF8(FailedToFindVar + ' ' + StrImagesPath + '\n' + ValueNoChange), wxString::FromUTF8(ErrorText + ' ' + MissingVariableError), wxICON_ERROR);
				continue;
			}
			if(i == cases++) {
				wxMessageBox(wxString::FromUTF8(FailedToFindVar + ' ' + StrVideosPath + '\n' + ValueNoChange), wxString::FromUTF8(ErrorText + ' ' + MissingVariableError), wxICON_ERROR);
				continue;
			}
		}
		
	}
	
	// init widgets
	theWidgets wid;
	
	if(VI9P::WorkingFile.empty()) {
		VI9P::WorkingFile = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "parameters.vi9p";
	}
	
	// apply settings
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
	
	if(Settings::DeleteTemp) {
		wid.menuItemOptionsDeleteTemp->Check();
	}
	if(Settings::UpdateCheck) {
		wid.menuItemOptionsUpdateCheck->Check();
	}
	
	wid.consoleLog->Show(Settings::ShowLog);
	
	wid.initLanguage();
	if(Settings::DefaultLanguage < Languages::LanguageFiles.size()) {
		if(loadLanguage(Languages::LanguageFiles.at(Settings::DefaultLanguage).File)) {
			wid.applyLanguage();
		}
	}
	
	wid.setFonts();
	//wid.positionWidgets();
	wid.setToolTips();
	wid.getAppearance();
	wid.setAppearance(Settings::ColorMode);
	wid.setCursors();
	
	{ // test CLI
		int ret = wid.executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + '\"'));
		
		if (ret != 1) { // 1 is the expected return value of that command
			wxMessageBox(wxString::FromUTF8(CLIError), wxString::FromUTF8(ErrorText), wxICON_ERROR);
		}
	}
	
	auto applyAddRows = [&]() {
		// row stuff is here too
		// well what do you know... it worked
		for(const auto &row : wid.PlayerTitles) {
			row->Bind(wxEVT_TEXT, [&](wxCommandEvent& WXUNUSED(event)) { // memory leak city??? (no, it get's unbound when destroyed... or does it?)
				wid.PlayerTitles_wxEVT_TEXT(row);
			});
		}
		for(const auto &row : wid.MoflexFiles) {
			row->Bind(wxEVT_TEXT, [&](wxCommandEvent& WXUNUSED(event)) {
				wid.MoflexFiles_wxEVT_TEXT(row);
			});
		}
		for(const auto &row : wid.MenuBanners) {
			row->Bind(wxEVT_TEXT, [&](wxCommandEvent& WXUNUSED(event)) {
				wid.MenuBanners_wxEVT_TEXT(row, true);
			});
			row->Bind(wxEVT_TEXT_ENTER, [&](wxCommandEvent& WXUNUSED(event)) {
				wid.MenuBanners_EVT_TEXT_ENTER(row);
			});
		}
		for(const auto &row : wid.MultiUp) {
			row->Bind(wxEVT_BUTTON, [&](wxCommandEvent& WXUNUSED(event)) {
				wid.MultiUp_wxEVT_BUTTON(row);
			});
		}
		for(const auto &row : wid.MultiDown) {
			row->Bind(wxEVT_BUTTON, [&](wxCommandEvent& WXUNUSED(event)) {
				wid.MultiDown_wxEVT_BUTTON(row);
			});
		}
		
		wid.setAppearance(Settings::ColorMode);
		wid.splitPatchLine->Show(wid.parameters.splitPos);
		wid.ShowPatchUpDown();
		wid.ShowMultiUpDown();
		wid.setToolTips();
		wid.setCursors();
		
		wid.setRowIndex();
		
		wid.multiBannerPreviewIndex->SetLabel(wxString::FromUTF8(std::to_string(VI9P::MultiBannerIndex + 1) + "/" + std::to_string(wid.MenuBanners.size())));
	};
	
	auto loadVI9P = [&](std::string VI9Pfile) {
		if(VI9P::Loading) {
			return;
		}
		VI9P::Loading = true;
		int flags = Execution::flags; // save flags
		Execution::flags = wxEXEC_BLOCK; // set flags

		VI9P::WorkingFile = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "parameters.vi9p";
		VI9P::OutFile = VI9Pfile;
		
		std::error_code error;
		error = copyfile(VI9P::OutFile, VI9P::WorkingFile); // copy chosen file to temp
		if (error) {
			wxMessageBox(wxString::FromUTF8(CopyFileError + '\n' + VI9P::OutFile + " -> " +  VI9P::WorkingFile + '\n' + error.message()), wxString::FromUTF8(ErrorText));
		}
		
		VI9P::MultiBannerIndex = 0;
		wid.loadParameters();
		wid.positionWidgets();
		
		applyAddRows();
		VI9P::Loading = false;
		
		wid.MenuBanners_wxEVT_TEXT(wid.MenuBanners.at(VI9P::MultiBannerIndex));
		Execution::flags = flags; // restore flags
	};
	
	wid.frame->Bind(wxEVT_WEBREQUEST_STATE, [&](wxWebRequestEvent& event) {
		wid.frame_wxEVT_WEBREQUEST_STATE(&event);
	});
	
	wid.frame->Bind(wxEVT_CLOSE_WINDOW, [&](wxCloseEvent& event) {
		wid.frame_wxEVT_CLOSE_WINDOW(&event);
	});
	
	wid.panel->Bind(wxEVT_SIZE, [&](wxSizeEvent& WXUNUSED(event)) {
		wid.panel_wxEVT_SIZE();
	});
	
	wid.modeChoiceBox->Bind(wxEVT_CHOICE, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.modeChoiceBox_wxEVT_CHOICE();
	});
	
	wid.bannerBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.bannerBox_wxEVT_TEXT();
	});
	
	wid.bannerBrowse->Bind(wxEVT_BUTTON, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.bannerBrowse_wxEVT_BUTTON();
	});
	
	wid.iconBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.iconBox_wxEVT_TEXT();
	});
	
	wid.iconPreview->Bind(wxEVT_BUTTON, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.iconPreview_wxEVT_BUTTON();
	});
	
	wid.iconBrowse->Bind(wxEVT_BUTTON, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.iconBrowse_wxEVT_BUTTON();
	});
	
	wid.shortnameBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.shortnameBox_wxEVT_TEXT();
	});
	
	wid.longnameBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.longnameBox_wxEVT_TEXT();
	});
	
	wid.publisherBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.publisherBox_wxEVT_TEXT();
	});
	
	wid.copyBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.copyBox_wxEVT_TEXT();
	});
	
	wid.copyCheck->Bind(wxEVT_CHECKBOX, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.copyCheck_wxEVT_CHECKBOX();
	});
	
	wid.ffRewindCheck->Bind(wxEVT_CHECKBOX, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.ffRewindCheck_wxEVT_CHECKBOX();
	});
	
	wid.dimCheck->Bind(wxEVT_CHECKBOX, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.dimCheck_wxEVT_CHECKBOX();
	});
	
	wid.multiBannerPreview->Bind(wxEVT_BUTTON, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.multiBannerPreview_wxEVT_BUTTON();
	});
	
	wid.multiBannerPreviewLeft->Bind(wxEVT_BUTTON, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.multiBannerPreviewLeft_wxEVT_BUTTON();
	});
	
	wid.multiBannerPreviewRight->Bind(wxEVT_BUTTON, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.multiBannerPreviewRight_wxEVT_BUTTON();
	});
	
	applyAddRows(); // make sure the row info is ok before we add events to it
	
	wid.moflexBrowse->Bind(wxEVT_BUTTON, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.moflexBrowse_wxEVT_BUTTON();
	});
	
	wid.multiBannerBrowse->Bind(wxEVT_BUTTON, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.multiBannerBrowse_wxEVT_BUTTON();
	});
	
	wid.removeRow->Bind(wxEVT_BUTTON, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.removeRow_wxEVT_BUTTON();
	});
	
	// add row
	wid.appendRow->Bind(wxEVT_BUTTON, [&](wxCommandEvent& WXUNUSED(event)) {
		
		if(!wid.appendRow->IsEnabled()) {
			return;
		}
		wid.removeRow->Enable(false); // disable so you cant press it too much
		wid.appendRow->Enable(false);
		
		wid.addRows();
		
		applyAddRows();
		
		if(wid.parameters.mode == 0) {
			wid.splitPatchButton->Enable(false);
			wid.rowText->Show(false);
			wid.appendRow->Enable(false);
		}
		else if(wid.parameters.mode == 1) {
			wid.splitPatchButton->Enable((wid.parameters.rows > 1));
			wid.EnableBannerLeftRight();
			
			wid.rowText->Show(true);
			if(wid.parameters.rows > 1 && wid.parameters.rows < MAX_ROWS_MULTI) {
				wid.appendRow->Enable(true);
				wid.removeRow->Enable(true);
			}
			else if(wid.parameters.rows <= 1) {
				wid.appendRow->Enable(true);
				wid.removeRow->Enable(false);
			}
			else if(wid.parameters.rows >= MAX_ROWS_MULTI) {
				wid.appendRow->Enable(false);
				wid.removeRow->Enable(true);
			}
			if(wid.parameters.rows > MAX_ROWS_MULTI) { // is there a better place for this?
				for(size_t i = MAX_ROWS_MULTI; i < wid.parameters.rows; i++) {
					wid.PlayerTitles.at(i)->Enable(false);
					wid.MoflexFiles.at(i)->Enable(false);
					wid.MenuBanners.at(i)->Enable(false);
				}
			}
		}
		else if(wid.parameters.mode == 2) {
			wid.splitPatchButton->Enable(false);
			
			wid.rowText->Show(true);
			if(wid.parameters.rows > 1 && wid.parameters.rows < MAX_ROWS) {
				wid.appendRow->Enable(true);
				wid.removeRow->Enable(true);
				for(size_t i = 1; i < wid.parameters.rows; i++) {
					wid.MenuBanners.at(i)->Enable(false);
				}
				//wid.setCursors();
			}
			else if(wid.parameters.rows <= 1) {
				wid.appendRow->Enable(true);
				wid.removeRow->Enable(false);
			}
			else if(wid.parameters.rows >= MAX_ROWS) {
				wid.appendRow->Enable(false);
				wid.removeRow->Enable(true);
			}
		}
		wid.removeRow->Enable(true);
		wid.setCursors();
	});
	
	wid.splitPatchButton->Bind(wxEVT_TOGGLEBUTTON, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.splitPatchButton_wxEVT_TOGGLEBUTTON();
	});
	
	wid.splitPatchUp->Bind(wxEVT_BUTTON, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.splitPatchUp_wxEVT_BUTTON();
	});
	
	wid.splitPatchDown->Bind(wxEVT_BUTTON, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.splitPatchDown_wxEVT_BUTTON();
	});
	
	wid.mainMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {
		switch(event.GetId()) {
			case wxID_NEW:
				{
					{ // clear temp
						std::error_code error;
						
						std::filesystem::remove_all(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath).c_str()), error);
						if (error) {
							wxMessageBox(wxString::FromUTF8(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '\n' + error.message()), wxString::FromUTF8(ErrorText));
						}
						
						std::filesystem::remove_all(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLItempPath).c_str()), error);
						if (error) {
							wxMessageBox(wxString::FromUTF8(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLItempPath + '\n' + error.message()), wxString::FromUTF8(ErrorText));
						}
						
						std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath).c_str()), error);
						if (error) {
							wxMessageBox(wxString::FromUTF8(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '\n' + error.message()), wxString::FromUTF8(ErrorText));
						}
					}
					
					VI9P::WorkingFile = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "parameters.vi9p";
					VI9P::OutFile = VI9P::WorkingFile;
					
					{ // -n
						wid.executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -n \"" + VI9P::WorkingFile + '\"'));
					}
					
					loadVI9P(VI9P::WorkingFile);					
				}
				break;
			case wxID_OPEN:
				{
					wxFileDialog openFileDialog(wid.frame, wxEmptyString, wxEmptyString, wxEmptyString, wxString::FromUTF8(vi9pFiles), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
					openFileDialog.SetFilterIndex(0);
					if (openFileDialog.ShowModal() == wxID_OK) {
						loadVI9P(std::string(openFileDialog.GetPath().ToUTF8()));
					}
				}
				break;
			case wxID_SAVE:
				{
					if(strcmp(VI9P::OutFile.c_str(), std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "parameters.vi9p").c_str()) == 0 || VI9P::OutFile.empty()) {
						wxFileDialog saveFileDialog(wid.frame, wxEmptyString, wxEmptyString, wxString::FromUTF8(vi9pFile), wxString::FromUTF8(vi9pFiles), wxFD_SAVE);
						if (saveFileDialog.ShowModal() == wxID_OK) {
							//wxMessageBox(saveFileDialog.GetPath(), "Placeholder");
							VI9P::OutFile = std::string(saveFileDialog.GetPath().ToUTF8());
							
							VI9P::OutFile = addMissingFileExtension(VI9P::OutFile, ".vi9p");
							
							std::error_code error;
							error = copyfile(VI9P::WorkingFile, VI9P::OutFile);
							if (error) {
								wxMessageBox(wxString::FromUTF8(CopyFileError + '\n' + VI9P::WorkingFile + " -> " + VI9P::OutFile + '\n' + error.message()), wxString::FromUTF8(ErrorText));
							}
							else {
								wxMessageBox(wxString::FromUTF8(FileSaved + '\n' + VI9P::OutFile));
							}
						}
					}
					else {
						std::error_code error;
						error = copyfile(VI9P::WorkingFile, VI9P::OutFile);
						if (error) {
							wxMessageBox(wxString::FromUTF8(CopyFileError + '\n' + VI9P::WorkingFile + " -> " + VI9P::OutFile + '\n' + error.message()), wxString::FromUTF8(ErrorText));
						}
						else {
							wxMessageBox(wxString::FromUTF8(FileSaved + '\n' + VI9P::OutFile));
						}
					}
				}
				break;
			case wxID_SAVEAS:
				{
					wxFileDialog saveFileDialog(wid.frame, wxEmptyString, wxEmptyString, wxEmptyString, wxString::FromUTF8(vi9pFiles), wxFD_SAVE);
					if (saveFileDialog.ShowModal() == wxID_OK) {
						//wxMessageBox(saveFileDialog.GetPath(), "Placeholder");
						VI9P::OutFile = std::string(saveFileDialog.GetPath().ToUTF8());
						
						VI9P::OutFile = addMissingFileExtension(VI9P::OutFile, ".vi9p");
						
						std::error_code error;
						error = copyfile(VI9P::WorkingFile, VI9P::OutFile);
						if (error) {
							wxMessageBox(wxString::FromUTF8(CopyFileError + '\n' + VI9P::WorkingFile + " -> " +  VI9P::OutFile + '\n' + error.message()), wxString::FromUTF8(ErrorText));
						}
						else {
							wxMessageBox(wxString::FromUTF8(FileSaved + '\n' + VI9P::OutFile));
						}
					}
				}
				break;
			case ID_EXPORT:
				{
					wid.titleIDButton_wxEVT_BUTTON(); // randomize TID
					wid.applicationTitleBox->SetValue(wxString::FromUTF8("video"));
					wid.productCodeBox->SetValue(wxString::FromUTF8("VDIJ"));
					wid.buildframe->SetSize(600, 500);
					wid.buildframe->Show();
				}
				break;
			case ID_EXTRACT:
				{
					wxFileDialog openFileDialog(wid.frame, wxEmptyString, wxEmptyString, wxEmptyString, wxString::FromUTF8(extractFiles + '|' + ciaFiles + '|' + tarFiles), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
					openFileDialog.SetFilterIndex(0);
					if (openFileDialog.ShowModal() == wxID_OK) {
						VI9P::WorkingFile = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "parameters.vi9p";
						
						size_t find = tolowerstr(std::string(openFileDialog.GetPath().ToUTF8())).rfind(".cia");//chosen .cia file
						if(find != std::string::npos) {
							if(strcmp(tolowerstr(std::string(openFileDialog.GetPath().ToUTF8())).substr(find).c_str(), ".cia") == 0) {								
								Extracted::Archive = std::string(openFileDialog.GetPath().ToUTF8()).substr(0, find);
								size_t start = Extracted::Archive.find_last_of("\\/");
								if(start == std::string::npos) {
									return; // yeah ok
								}
								
								// choose what dir to extract it to
								wxDirDialog openDirectoryDialog(wid.frame, wxString::FromUTF8(chooseDirSave), wxString::FromUTF8(Extracted::Archive.substr(0, start)), wxDD_DIR_MUST_EXIST);
								if (openDirectoryDialog.ShowModal() != wxID_OK) {
									return;
								}
								Extracted::Archive = std::string(openDirectoryDialog.GetPath().ToUTF8()) + '/' + Extracted::Archive.substr(start + 1);
								// async execution
								{ // -ec
									wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -ec \"" + std::string(openFileDialog.GetPath().ToUTF8()) + "\" \"" + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + seedFile + "\" \"" + Extracted::Archive + '\"');
									wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
									
									wid.extractArchive->Redirect();
									
									long PID = wxExecute(command, wxEXEC_MAKE_GROUP_LEADER|wxEXEC_ASYNC, wid.extractArchive);
									
									if (PID == 0) {
										wxLogError(wxString::FromUTF8(ErrorText + ' ' + command));
										return;
									}
								}
							}
						}
						
						find = tolowerstr(std::string(openFileDialog.GetPath().ToUTF8())).rfind(".tar"); // chosen .tar file
						if(find != std::string::npos) {
							if(strcmp(tolowerstr(std::string(openFileDialog.GetPath().ToUTF8())).substr(find).c_str(), ".tar") == 0) {
								Extracted::Archive = std::string(openFileDialog.GetPath().ToUTF8()).substr(0, find);
								size_t start = Extracted::Archive.find_last_of("\\/");
								if(start == std::string::npos) {
									return; // yeah ok
								}
								// choose what dir to extract it to
								wxDirDialog openDirectoryDialog(wid.frame, wxString::FromUTF8(chooseDirSave), wxString::FromUTF8(Extracted::Archive.substr(0, start)), wxDD_DIR_MUST_EXIST);
								if (openDirectoryDialog.ShowModal() != wxID_OK) {
									return;
								}
								Extracted::Archive = std::string(openDirectoryDialog.GetPath().ToUTF8()) + '/' + Extracted::Archive.substr(start + 1);
								// async execution
								{ // -ec
									wxString command = wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -et \"" + std::string(openFileDialog.GetPath().ToUTF8()) + "\" \"" + Extracted::Archive + '\"');
									wid.consoleLog->LogTextAtLevel(0, command + "\n==========\n");
									
									wid.extractArchive->Redirect();
									
									long PID = wxExecute(command, wxEXEC_MAKE_GROUP_LEADER|wxEXEC_ASYNC, wid.extractArchive);
									
									if (PID == 0) {
										wxLogError(wxString::FromUTF8(ErrorText + ' ' + command));
										return;
									}
								}
							}
						}
						
						wid.frame->Enable(false);
						wid.setCursors();
						wid.extractDialog = new wxProgressDialog(wxString::FromUTF8(extractingArchive), wxString::FromUTF8(clickToCancel), 1, wid.frame, wxPD_APP_MODAL|wxPD_CAN_ABORT);//dialog main source
						wid.extractDialog->Bind(wxEVT_CLOSE_WINDOW, [&](wxCloseEvent& event) {
							wid.extractDialog_wxEVT_CLOSE_WINDOW(&event);
							//wid.extractDialog->Destroy();
						});
						wid.extractPulser->Start(100);
						wid.extractLogger->Start(1);
					}
				}
				break;
			case ID_IMPORTSEED:
				{
					wxFileDialog openFileDialog(wid.frame, wxEmptyString, wxEmptyString, wxEmptyString, wxString::FromUTF8(seeddbFiles), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
					openFileDialog.SetFilterIndex(0);
					if (openFileDialog.ShowModal() == wxID_OK) {
						std::error_code error;
						error = copyfile(std::string(openFileDialog.GetPath().ToUTF8()), std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + seedFile);//copy chosen file to temp
						if (error) {
							wxMessageBox(wxString::FromUTF8(CopyFileError + '\n' + std::string(openFileDialog.GetPath().ToUTF8()) + " -> " + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + seedFile + '\n' + error.message()), wxString::FromUTF8(ErrorText));
						}
					}
				}
				break;
			case ID_QUIT:
				{
					wid.frame->Destroy();
				}
				break;
			// settings
			case ID_LOGBOOL:
				{
					Settings::ShowLog = wid.menuItemOptionsLog->IsChecked();
					wid.consoleLog->Show(Settings::ShowLog);
					saveSettings();
				}
				break;
			case ID_SYSTEM:
				{
					// 2
					Settings::ColorMode = 2;
					wid.setAppearance(Settings::ColorMode);
					saveSettings();
				}
				break;
			case ID_LIGHT:
				{
					// 0
					Settings::ColorMode = 0;
					wid.setAppearance(Settings::ColorMode);
					saveSettings();
				}
				break;
			case ID_DARK:
				{
					// 1
					Settings::ColorMode = 1;
					wid.setAppearance(Settings::ColorMode);
					saveSettings();
				}
				break;
			case ID_DELETETEMP:
				{
					Settings::DeleteTemp = wid.menuItemOptionsDeleteTemp->IsChecked();
					saveSettings();
				}
				break;
			case ID_UPDATECHECK:
				{
					Settings::UpdateCheck = wid.menuItemOptionsUpdateCheck->IsChecked();
					saveSettings();
				}
				break;
			// About
			case ID_ABOUT:
				{
					wid.aboutframe->SetSize(650, 350);
					wid.aboutframe->Show();
				}
				break;
		}
		// boo hoo no more switch statement
		{ // language
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
			if(event.GetId() >= ID_LANG1 && event.GetId() <= ID_LANG10) {
				for(size_t i = 0; i < LANG_IDs.size(); i++) {
					if(event.GetId() == LANG_IDs.at(i)) {
						if(loadLanguage(Languages::LanguageFiles.at(i).File)) {
							wid.applyLanguage();
						}
						wid.setFonts();
						wid.positionWidgets();
						wid.setToolTips();
						Settings::DefaultLanguage = i;
						saveSettings();
					}
				}
			}
		}
	});
	
	wid.buildframe->Bind(wxEVT_CLOSE_WINDOW, [&](wxCloseEvent& event) {
		wid.buildframe_wxEVT_CLOSE_WINDOW(&event);
	});
	
	wid.buildpanel->Bind(wxEVT_SIZE, [&](wxSizeEvent& WXUNUSED(event)) {
		wid.buildpanel_wxEVT_SIZE();
	});
	
	wid.titleIDBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.titleIDBox_wxEVT_TEXT();
	});
	
	wid.titleIDButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.titleIDButton_wxEVT_BUTTON();
	});
	
	wid.exportArchive->Bind(wxEVT_END_PROCESS, [&](wxProcessEvent& event) {
		wid.exportArchive_wxEVT_END_PROCESS(&event);
	});
	
	wid.buildButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.buildButton_wxEVT_BUTTON();
	});
	
	wid.cancelButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& WXUNUSED(event)) {
		wid.cancelButton_wxEVT_BUTTON();
	});
	
	wid.barPulser->Bind(wxEVT_TIMER, [&](wxTimerEvent& WXUNUSED(event)) {
		wid.barPulser_wxEVT_TIMER();
	});

	wid.exportLogger->Bind(wxEVT_TIMER, [&](wxTimerEvent& WXUNUSED(event)) {
		wid.exportLogger_wxEVT_TIMER();
	});
	
	wid.extractArchive->Bind(wxEVT_END_PROCESS, [&](wxProcessEvent& event) {
		wid.extractArchive_wxEVT_END_PROCESS(&event);
		wid.extractDialog->Destroy();
		
		/*std::error_code error;
		if(std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(Extracted::Archive).c_str()), error)) {
			wxMessageBox(wxString::FromUTF8(Extracted::Archive), wxString::FromUTF8(SuccessfullyExtracted));
		}
		else {
			wxMessageBox(wxString::FromUTF8(BuildError + " (" + Extracted::Archive + ')'), wxString::FromUTF8(ErrorText), wxICON_ERROR);
		}
		if(error) {
			wxMessageBox(wxString::FromUTF8(BuildError + " (" + Extracted::Archive + ")\n" + error.message()), wxString::FromUTF8(ErrorText), wxICON_ERROR);
		}*/
		// cant load here because it's still in async or something idk but it freezes if you try
		loadVI9P(Extracted::Archive + "/parameters.vi9p");
	});

	wid.extractPulser->Bind(wxEVT_TIMER, [&](wxTimerEvent& WXUNUSED(event)) {
		wid.extractPulser_wxEVT_TIMER();
	});

	wid.extractLogger->Bind(wxEVT_TIMER, [&](wxTimerEvent& WXUNUSED(event)) {
		wid.extractLogger_wxEVT_TIMER();
	});
	
	wid.aboutframe->Bind(wxEVT_CLOSE_WINDOW, [&](wxCloseEvent& event) {
		wid.aboutframe_wxEVT_CLOSE_WINDOW(&event);
	});
	
	wid.aboutpanel->Bind(wxEVT_SIZE, [&](wxSizeEvent& WXUNUSED(event)) {
		wid.aboutpanel_wxEVT_SIZE();
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
	
	{ // deal temp
		std::error_code error;
		
		if(!std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath).c_str()), error)) {
			std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath).c_str()), error);
			if (error) {
				wxMessageBox(wxString::FromUTF8(std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '\n' + error.message())), wxString::FromUTF8(ErrorText));
			}
			
			{ // -n (for temp dir)
				wid.executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -n \"" + VI9P::WorkingFile + '\"'));
			}
		}
		if(error) {
			// this probably wont work either
			{ // -n (for temp dir)
				wid.executeCommand(wxString::FromUTF8('\"' + std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLIFile + "\" -n \"" + VI9P::WorkingFile + '\"'));
			}
		}
	}
	
	// plz call this last
	{
		std::string VI9Pnow = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + "/parameters.vi9p";
		
		// get the project path from args
		for (int i = 0; i < argc; i++) {
			std::string v = argv[i];
			size_t find = 0;
			find = v.find_last_of(".");
			if (find > v.size()) find = 0; // if find_first_of fails, this flings in the first numeral
			if (v.substr(find) == ".vi9p") {
				VI9Pnow = v;
			}
		}
		
		loadVI9P(VI9Pnow);
	}
	
	// nvm this stuff really has to be after that
	// correct sizes
	{
		int w, h;
		wid.iconPreview->GetSize(&w, &h);
		wid.iconPreview->SetSize(w + Borders::width, h + Borders::height);
	}
	{
		int w, h;
		wid.multiBannerPreview->GetSize(&w, &h);
		wid.multiBannerPreview->SetSize(w + Borders::width, h + Borders::height);
	}
	
	// make sure the window looks good
	wid.positionWidgets();
	
	if(Settings::UpdateCheck) {
		wid.updateCheck->Start();
	}
	
	wid.frame->Show();
	wxTheApp->OnRun();
	return 0;
}

// entry point (real)
#ifndef _WIN32
int main(int argc, char** argv) {
	return progmain(argc, argv);
}
#else

// https://stackoverflow.com/a/41665246
std::string WstrToUtf8Str(const std::wstring& wstr) {
	std::string retStr = "";
	if (!wstr.empty()) {
		int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
		
		if (sizeRequired > 0) {
			retStr.resize(sizeRequired);
			int bytesConverted = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &retStr[0], sizeRequired, NULL, NULL);
			if (!bytesConverted) {
				std::wcout << L"WstrToUtf8Str failed to convert wstring '" << wstr.c_str() << L"'";
				return "";
			}
		}
	}
	return retStr;
}	

int wmain(int argc, wchar_t** argv) {
	// put argv in std::vector (yay!)
	std::vector<std::string> argvecmb;
	for(size_t i = 0; i < argc; i++) {
		std::string arg = WstrToUtf8Str(argv[i]);
		argvecmb.push_back(arg);
	}

	// string vector to char**
	char** charcharArray = new char*[argvecmb.size()];

	for (size_t i = 0; i < argvecmb.size(); i++) {
		charcharArray[i] = new char[argvecmb.at(i).length() + 1];
		strcpy_s(charcharArray[i], argvecmb.at(i).length() + 1, argvecmb.at(i).c_str());
	}
	
	int ret = progmain(argc, charcharArray);
	
	//delete everything from the char**
	for (size_t i = 0; i < argvecmb.size(); i++) {
		delete[] charcharArray[i];
	}
	delete[] charcharArray;
	
	return ret;
}
#endif

