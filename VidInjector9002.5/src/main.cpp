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

int main(int argc, char* argv[]) {
	AppInitializer appInitializer;
	wxInitAllImageHandlers();

	{//get program dir
		std::string programpath(std::filesystem::absolute(std::filesystem::path((const char8_t*)&*std::string(argv[0]).c_str())).string());
		programpath = programpath.substr(0, programpath.find_last_of("\\/"));
		ProgramDir = wxString::FromUTF8(programpath);
		//wxMessageBox(ProgramDir);
	}
	
	//load settings and handle errors
	{
		std::vector<int> ret = loadSettings();
		//contain the repetitive setting strings (You will likely want to put your stuff here when adding new Settings)
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
			if(i == cases++)//0
				continue;//:)
			if(i == cases++)//1
				continue;//uhhh i dont care
			if(i == cases++)//2
				continue;//this is actually bad so there should probably be a message here even though it's rare
			
			for(const auto &s : SettingStrings) {
				(void)s;
				//std::cout << std::to_string((i - 3) / 2) << std::endl;
				if(i == cases++) {
					wxMessageBox(wxString::FromUTF8(BadValue + ' ' + SettingStrings.at((i - 3) / 2) + '\n' + ValueNoChange), wxString::FromUTF8(ErrorText + ' ' + BadValue), wxICON_ERROR);
					continue;
				}
				if(i == cases++) {
					wxMessageBox(wxString::FromUTF8(FailedToFindVar + ' ' + SettingStrings.at((i - 4) / 2) + '\n' + ValueNoChange), wxString::FromUTF8(ErrorText + ' ' + MissingVariableError), wxICON_ERROR);
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
	
	//init widgets
	InitWidgets wid;
	
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
	if(Settings::DeleteTemp) {
		wid.menuItemOptionsDeleteTemp->Check();
	}
	if(Settings::UpdateCheck) {
		wid.menuItemOptionsUpdateCheck->Check();
	}
	
	wid.consoleLog->Show(Settings::ShowLog);
	
	getBorders(&wid);
	initLanguage(&wid);
	if(Settings::DefaultLanguage < Languages::LanguageFiles.size())
		if(loadLanguage(Languages::LanguageFiles.at(Settings::DefaultLanguage).File))
			applyLanguage(&wid, &parameters);
	initAllWidgets(&wid);
	setFonts(&wid);
	positionWidgets(&wid, &parameters);
	setToolTips(&wid);
	getAppearance(&wid);
	setAppearance(&wid, Settings::ColorMode);
	setCursors(&wid);
	
	auto applyAddRows = [&]() {
		//row stuff is here too
		//well what do you know... it worked
		for(const auto &row : wid.PlayerTitles) {
			row->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {//memory leak city???
				(void)event;//do as I say not as I do
				PlayerTitles_wxEVT_TEXT(&wid, &parameters, row);
			});
		}
		for(const auto &row : wid.MoflexFiles) {
			row->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {//memory leak city???
				(void)event;
				MoflexFiles_wxEVT_TEXT(&wid, &parameters, row);
			});
		}
		for(const auto &row : wid.MenuBanners) {
			row->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {//memory leak city???
				(void)event;
				MenuBanners_wxEVT_TEXT(&wid, &parameters, row);
			});
			row->Bind(wxEVT_TEXT_ENTER, [&](wxCommandEvent& event) {//memory leak city???
				(void)event;
				MenuBanners_EVT_TEXT_ENTER(&wid, &parameters, row);
			});
		}
		for(const auto &row : wid.MultiUp) {
			row->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {//memory leak city???
				(void)event;
				MultiUp_wxEVT_BUTTON(&wid, row);
			});
		}
		for(const auto &row : wid.MultiDown) {
			row->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {//memory leak city???
				(void)event;
				MultiDown_wxEVT_BUTTON(&wid, row);
			});
		}
		
		setAppearance(&wid, Settings::ColorMode);
		wid.splitPatchLine->Show(parameters.splitPos);
		ShowPatchUpDown(&wid, &parameters);
		ShowMultiUpDown(&wid);
		setCursors(&wid);
		setToolTips(&wid);
		
		wid.rowText->SetLabel(wxString::FromUTF8(std::to_string(parameters.rows) + "/" + std::to_string(MAX_ROWS)));
		wid.multiBannerPreviewIndex->SetLabel(wxString::FromUTF8(std::to_string(VI9P::MultiBannerIndex + 1) + "/" + std::to_string(wid.MenuBanners.size())));
	};
	
	auto loadVI9P = [&](std::string VI9Pfile) {
		VI9P::WorkingFile = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "parameters.vi9p";
		VI9P::OutFile = VI9Pfile;
		
		std::error_code error;
		error = copyfile(VI9P::OutFile, VI9P::WorkingFile);//copy chosen file to temp
		if (error) {
			wxMessageBox(wxString::FromUTF8(CopyFileError + '\n' + VI9P::OutFile + " -> " +  VI9P::WorkingFile + '\n' + error.message()), wxString::FromUTF8(ErrorText));
		}
		
		VI9P::MultiBannerIndex = 0;
		loadParameters(&wid, &parameters);
		positionWidgets(&wid, &parameters);
		
		applyAddRows();
	};
	
	wid.frame->Bind(wxEVT_WEBREQUEST_STATE, [&](wxWebRequestEvent& event) {
		frame_wxEVT_WEBREQUEST_STATE(&wid, &event);
	});
	
	wid.frame->Bind(wxEVT_CLOSE_WINDOW, [&](wxCloseEvent& event) {
		frame_wxEVT_CLOSE_WINDOW(&event);
	});
	
	wid.panel->Bind(wxEVT_SIZE, [&](wxSizeEvent& event) {
		(void)event;
		panel_wxEVT_SIZE(&wid, &parameters);
	});
	
	wid.modeChoiceBox->Bind(wxEVT_CHOICE, [&](wxCommandEvent& event) {
		(void)event;
		modeChoiceBox_wxEVT_CHOICE(&wid, &parameters);
	});
	
	wid.bannerBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {
		(void)event;
		bannerBox_wxEVT_TEXT(&wid, &parameters);
	});
	
	wid.bannerBrowse->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		(void)event;
		bannerBrowse_wxEVT_BUTTON(&wid, &parameters);
	});

	wid.iconBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {
		(void)event;
		iconBox_wxEVT_TEXT(&wid, &parameters);
	});
	
	wid.iconPreview->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		(void)event;
		iconPreview_wxEVT_BUTTON(&wid, &parameters);
	});

	wid.iconBrowse->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		(void)event;
		iconBrowse_wxEVT_BUTTON(&wid, &parameters);
	});
	
	wid.shortnameBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {
		(void)event;
		shortnameBox_wxEVT_TEXT(&wid, &parameters);
	});

	wid.longnameBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {
		(void)event;
		longnameBox_wxEVT_TEXT(&wid, &parameters);
	});
	
	wid.publisherBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {
		(void)event;
		publisherBox_wxEVT_TEXT(&wid, &parameters);
	});

	wid.copyBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {
		(void)event;
		copyBox_wxEVT_TEXT(&wid, &parameters);
	});

	wid.copyCheck->Bind(wxEVT_CHECKBOX, [&](wxCommandEvent& event) {
		(void)event;
		copyCheck_wxEVT_CHECKBOX(&wid, &parameters);
	});

	wid.ffRewindCheck->Bind(wxEVT_CHECKBOX, [&](wxCommandEvent& event) {
		(void)event;
		ffRewindCheck_wxEVT_CHECKBOX(&wid, &parameters);
	});

	wid.dimCheck->Bind(wxEVT_CHECKBOX, [&](wxCommandEvent& event) {
		(void)event;
		dimCheck_wxEVT_CHECKBOX(&wid, &parameters);
	});
	
	wid.multiBannerPreview->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		(void)event;
		multiBannerPreview_wxEVT_BUTTON(&wid, &parameters);
	});

	wid.multiBannerPreviewLeft->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		(void)event;
		multiBannerPreviewLeft_wxEVT_BUTTON(&wid, &parameters);
	});

	wid.multiBannerPreviewRight->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		(void)event;
		multiBannerPreviewRight_wxEVT_BUTTON(&wid, &parameters);
	});
	
	applyAddRows();
	
	wid.moflexBrowse->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		(void)event;
		moflexBrowse_wxEVT_BUTTON(&wid, &parameters);
	});

	wid.multiBannerBrowse->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		(void)event;
		multiBannerBrowse_wxEVT_BUTTON(&wid, &parameters);
	});
	
	wid.removeRow->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		(void)event;
		removeRow_wxEVT_BUTTON(&wid, &parameters);
	});
	
	//add row
	wid.appendRow->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		(void)event;
		if(!wid.appendRow->IsEnabled())
			return;
		wid.removeRow->Enable(false);//disable so you cant press it too much
		wid.appendRow->Enable(false);
		
		addRows(&wid, &parameters);
		
		applyAddRows();
		
		if(parameters.mode) {
			wid.splitPatchButton->Enable((parameters.rows > 1));
			EnableBannerLeftRight(&wid);
			
			wid.rowText->Show(true);
			if(parameters.rows > 1 && parameters.rows < MAX_ROWS) {
				wid.appendRow->Enable(true);
				wid.removeRow->Enable(true);
			}
			else if(parameters.rows <= 1) {
				wid.appendRow->Enable(true);
				wid.removeRow->Enable(false);
			}
			else if(parameters.rows >= MAX_ROWS) {
				wid.appendRow->Enable(false);
				wid.removeRow->Enable(true);
			}
		}
		else {
			wid.splitPatchButton->Enable(false);
			wid.rowText->Show(false);
			wid.appendRow->Enable(false);
		}
		wid.removeRow->Enable(true);
	});
	
	wid.splitPatchButton->Bind(wxEVT_TOGGLEBUTTON, [&](wxCommandEvent& event) {
		(void)event;
		splitPatchButton_wxEVT_TOGGLEBUTTON(&wid, &parameters);
	});
	
	wid.splitPatchUp->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		(void)event;
		splitPatchUp_wxEVT_BUTTON(&wid, &parameters);
	});

	wid.splitPatchDown->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		(void)event;
		splitPatchDown_wxEVT_BUTTON(&wid, &parameters);
	});
	
	wid.mainMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {
		switch(event.GetId()) {
			case wxID_NEW:
				{
					{//clear temp
						std::error_code error;
						
						std::filesystem::remove_all(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath).c_str()), error);
						if (error)
							wxMessageBox(wxString::FromUTF8(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '\n' + error.message()), wxString::FromUTF8(ErrorText));

						std::filesystem::remove_all(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLItempPath).c_str()), error);
						if (error)
							wxMessageBox(wxString::FromUTF8(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + CLItempPath + '\n' + error.message()), wxString::FromUTF8(ErrorText));

						std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath).c_str()), error);
						if (error)
							wxMessageBox(wxString::FromUTF8(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '\n' + error.message()), wxString::FromUTF8(ErrorText));
					}
					
					VI9P::WorkingFile = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '/' + "parameters.vi9p";
					VI9P::OutFile = VI9P::WorkingFile;
					
					{//-n
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
					
					loadVI9P(VI9P::WorkingFile);
					
					//MenuBanners_wxEVT_TEXT(&wid, &parameters, wid.MenuBanners.at(VI9P::MultiBannerIndex));
				}
				break;
			case wxID_OPEN:
				{
					wxFileDialog openFileDialog(wid.frame, wxEmptyString, wxEmptyString, wxEmptyString, wxString::FromUTF8(vi9pFiles), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
					openFileDialog.SetFilterIndex(0);
					if (openFileDialog.ShowModal() == wxID_OK) {
						loadVI9P(std::string(openFileDialog.GetPath().ToUTF8()));
						MenuBanners_wxEVT_TEXT(&wid, &parameters, wid.MenuBanners.at(VI9P::MultiBannerIndex));
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
					titleIDButton_wxEVT_BUTTON(&wid);//randomize TID
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
								if(start == std::string::npos)
									return;//yeah ok
								
								//choose what dir to extract it to
								wxDirDialog openDirectoryDialog(wid.frame, wxString::FromUTF8(chooseDirSave), wxString::FromUTF8(Extracted::Archive.substr(0, start)), wxDD_DIR_MUST_EXIST);
								if (openDirectoryDialog.ShowModal() != wxID_OK)
									return;
								Extracted::Archive = std::string(openDirectoryDialog.GetPath().ToUTF8()) + '/' + Extracted::Archive.substr(start + 1);
								//async execution
								{//-ec
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
						
						find = tolowerstr(std::string(openFileDialog.GetPath().ToUTF8())).rfind(".tar");//chosen .tar file
						if(find != std::string::npos) {
							if(strcmp(tolowerstr(std::string(openFileDialog.GetPath().ToUTF8())).substr(find).c_str(), ".tar") == 0) {
								Extracted::Archive = std::string(openFileDialog.GetPath().ToUTF8()).substr(0, find);
								size_t start = Extracted::Archive.find_last_of("\\/");
								if(start == std::string::npos)
									return;//yeah ok
								//choose what dir to extract it to
								wxDirDialog openDirectoryDialog(wid.frame, wxString::FromUTF8(chooseDirSave), wxString::FromUTF8(Extracted::Archive.substr(0, start)), wxDD_DIR_MUST_EXIST);
								if (openDirectoryDialog.ShowModal() != wxID_OK)
									return;
								Extracted::Archive = std::string(openDirectoryDialog.GetPath().ToUTF8()) + '/' + Extracted::Archive.substr(start + 1);
								//async execution
								{//-ec
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
						setCursors(&wid);
						wid.extractDialog = new wxProgressDialog(wxString::FromUTF8(extractingArchive), wxString::FromUTF8(clickToCancel), 1, wid.frame, wxPD_APP_MODAL|wxPD_CAN_ABORT);//dialog main source
						wid.extractDialog->Bind(wxEVT_CLOSE_WINDOW, [&](wxCloseEvent& event) {
							extractDialog_wxEVT_CLOSE_WINDOW(&wid, &event);
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
			//settings
			case ID_LOGBOOL:
				{
					Settings::ShowLog = wid.menuItemOptionsLog->IsChecked();
					wid.consoleLog->Show(Settings::ShowLog);
					saveSettings();
				}
				break;
			case ID_SYSTEM:
				{
					//2
					Settings::ColorMode = 2;
					setAppearance(&wid, Settings::ColorMode);
					saveSettings();
				}
				break;
			case ID_LIGHT:
				{
					//0
					Settings::ColorMode = 0;
					setAppearance(&wid, Settings::ColorMode);
					saveSettings();
				}
				break;
			case ID_DARK:
				{
					//1
					Settings::ColorMode = 1;
					setAppearance(&wid, Settings::ColorMode);
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
			//About
			case ID_ABOUT:
				{
					wid.aboutframe->SetSize(650, 350);
					wid.aboutframe->Show();
				}
				break;
		}
		//boo hoo no more switch statement
		{//language
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
						if(loadLanguage(Languages::LanguageFiles.at(i).File))
							applyLanguage(&wid, &parameters);
						setFonts(&wid);
						positionWidgets(&wid, &parameters);
						setToolTips(&wid);
						Settings::DefaultLanguage = i;
						saveSettings();
					}
				}
			}
		}
	});
	
	wid.buildframe->Bind(wxEVT_CLOSE_WINDOW, [&](wxCloseEvent& event) {
		buildframe_wxEVT_CLOSE_WINDOW(&wid, &event);
	});
	
	wid.buildpanel->Bind(wxEVT_SIZE, [&](wxSizeEvent& event) {
		(void)event;
		buildpanel_wxEVT_SIZE(&wid, &parameters);
	});

	wid.titleIDBox->Bind(wxEVT_TEXT, [&](wxCommandEvent& event) {
		(void)event;
		titleIDBox_wxEVT_TEXT(&wid);
	});

	wid.titleIDButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		(void)event;
		titleIDButton_wxEVT_BUTTON(&wid);
	});
	
	wid.exportArchive->Bind(wxEVT_END_PROCESS, [&](wxProcessEvent& event) {
		exportArchive_wxEVT_END_PROCESS(&wid, &event);
	});

	wid.buildButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		(void)event;
		buildButton_wxEVT_BUTTON(&wid, &parameters);
	});

	wid.cancelButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
		(void)event;
		cancelButton_wxEVT_BUTTON(&wid);
	});
	
	wid.barPulser->Bind(wxEVT_TIMER, [&](wxTimerEvent& event) {
		(void)event;
		barPulser_wxEVT_TIMER(&wid);
	});

	wid.exportLogger->Bind(wxEVT_TIMER, [&](wxTimerEvent& event) {
		(void)event;
		exportLogger_wxEVT_TIMER(&wid);
	});
	
	wid.extractArchive->Bind(wxEVT_END_PROCESS, [&](wxProcessEvent& event) {
		extractArchive_wxEVT_END_PROCESS(&wid, &event);
		wid.extractDialog->Destroy();
		//cant load here because it's still in async or something idk but it doesnt work if you try
		//loadVI9P(Extracted::Archive + "/parameters.vi9p");
		//MenuBanners_wxEVT_TEXT(&wid, &parameters, wid.MenuBanners.at(VI9P::MultiBannerIndex));
	});

	wid.extractPulser->Bind(wxEVT_TIMER, [&](wxTimerEvent& event) {
		(void)event;
		extractPulser_wxEVT_TIMER(&wid);
	});

	wid.extractLogger->Bind(wxEVT_TIMER, [&](wxTimerEvent& event) {
		(void)event;
		extractLogger_wxEVT_TIMER(&wid);
	});
	
	wid.aboutframe->Bind(wxEVT_CLOSE_WINDOW, [&](wxCloseEvent& event) {
		aboutframe_wxEVT_CLOSE_WINDOW(&wid, &event);
	});
	
	wid.aboutpanel->Bind(wxEVT_SIZE, [&](wxSizeEvent& event) {
		(void)event;
		aboutpanel_wxEVT_SIZE(&wid, &parameters);
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
	
	{//deal temp
		std::error_code error;
		
		if(!std::filesystem::exists(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath).c_str()), error)) {
			std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath).c_str()), error);
			if (error)
				wxMessageBox(wxString::FromUTF8(std::string(std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + '\n' + error.message())), wxString::FromUTF8(ErrorText));
			
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
		}
		if(error) {
			//this probably wont work either
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
		}
	}
	
	//plz call this last
	{
		std::string VI9Pnow = std::string(ProgramDir.ToUTF8()) + '/' + resourcesPath + '/' + tempPath + "/parameters.vi9p";
		
		//get the project path from args
		for (int i = 0; i < argc; i++) {
			std::string v = argv[i];
			size_t find = 0;
			find = v.find_last_of(".");
			if (find > v.size()) find = 0;//if find_first_of fails, this flings in the first numeral
			if (v.substr(find) == ".vi9p") {
				VI9Pnow = v;
			}
		}
		
		loadVI9P(VI9Pnow);
		MenuBanners_wxEVT_TEXT(&wid, &parameters, wid.MenuBanners.at(VI9P::MultiBannerIndex));
	}
	
	//correct sizes
	wid.iconPreview->Fit();
	wid.multiBannerPreview->Fit();
	
	if(Settings::UpdateCheck)
		wid.updateCheck->Start();
	
	wid.frame->Show();
	wxTheApp->OnRun();
}
