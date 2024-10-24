//Pretty much all of this is taken from https://github.com/gammasoft71/Examples_wxWidgets

#pragma once

#include <algorithm>
#include <map>

#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/artprov.h>
#include <wx/fontdlg.h>
#include <wx/process.h>
#include <wx/colour.h>
#include <wx/wx.h>

#include "settings.hpp"
#include "strings.hpp"

enum wxOwnedID {
	ID_LOGBOOL,
	ID_SYSTEM,
	ID_LIGHT,
	ID_DARK,
};

/*struct MediaPanelMedia {//just use the pointer as a unique ID number
	uint8_t index;
	wxTextCtrl* box;
};*/

struct InitWidgets {
	wxFrame* frame = new wxFrame(nullptr, wxID_ANY, wxString::FromUTF8(frameText), wxDefaultPosition, {1000, 700});
	wxPanel* panel = new wxPanel(frame);

	//main menu
	wxMenuBar* mainMenu = new wxMenuBar();

	wxMenu* menuFile = new wxMenu();
	wxMenuItem* menuItemFileNew = menuFile->Append(wxID_NEW, wxString::FromUTF8(fileNew + "\tCtrl+N"));
	wxMenuItem* menuItemFileOpen = menuFile->Append(wxID_OPEN, wxString::FromUTF8(fileOpen + "\tCtrl+O"));
	wxMenuItem* menuItemFileSave = menuFile->Append(wxID_SAVE, wxString::FromUTF8(fileSave + "\tCtrl+S"));
	wxMenuItem* menuItemFileSaveAs = menuFile->Append(wxID_SAVEAS, wxString::FromUTF8(fileSaveAs + "\tCtrl+Shift+S"));
	wxMenu* menuOptions = new wxMenu();
	wxMenuItem* menuItemOptionsLog = menuOptions->Append(ID_LOGBOOL, wxString::FromUTF8(optionsShowLog), "", wxITEM_CHECK);
	wxMenuItem* menuItemOptionsSystem = menuOptions->Append(ID_SYSTEM, wxString::FromUTF8(optionsSystemMode), "", wxITEM_RADIO);
	wxMenuItem* menuItemOptionsLight = menuOptions->Append(ID_LIGHT, wxString::FromUTF8(optionsLightMode), "", wxITEM_RADIO);
	wxMenuItem* menuItemOptionsDark = menuOptions->Append(ID_DARK, wxString::FromUTF8(optionsDarkMode), "", wxITEM_RADIO);
	
	//frame
	wxLogWindow* consoleLog = new wxLogWindow(frame, wxString::FromUTF8(logFrameText), false);

	//panel
	wxStaticText* modeText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(ModeText), {10, 10});
	wxChoice* modeChoiceBox = new wxChoice(panel, wxID_ANY);
	
	wxStaticText* bannerText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(BannerText));
	wxTextCtrl* bannerBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString);
	wxButton* bannerBrowse = new wxButton(panel, wxID_ANY, wxString::FromUTF8(Browse), wxDefaultPosition, {120, 35});
	wxStaticText* bannerError = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(ErrorText + ' ' + ImageInfoError));
	
	wxStaticText* iconText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(IconText));
	wxTextCtrl* iconBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString);
	wxButton* iconBrowse = new wxButton(panel, wxID_ANY, wxString::FromUTF8(Browse), wxDefaultPosition, {120, 35});
	wxStaticText* iconError = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(ErrorText + ' ' + ImageInfoError));
	
	wxStaticText* shortnameText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(ShortNameText));
	wxTextCtrl* shortnameBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	wxStaticText* shortnameError = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (/)"));

	wxStaticText* longnameText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(LongNameText));
	wxTextCtrl* longnameBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	wxStaticText* longnameError = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (/)"));

	wxStaticText* publisherText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(PublisherText));
	wxTextCtrl* publisherBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	wxStaticText* publisherError = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (/)"));

	wxTextCtrl* copyBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, {300, 200}, wxTE_MULTILINE | wxTE_CENTRE);
	wxCheckBox* copyCheck = new wxCheckBox(panel, wxID_ANY, wxString::FromUTF8(CopyrightCheckText));
	
	wxStaticBitmap* bannerPreview = new wxStaticBitmap(panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, {264, 154}, wxBORDER_NONE);
	wxStaticText* bannerPreviewText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(BannerPreviewText));
	
	wxButton* iconPreview = new wxButton(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, {52, 52});
	
	wxCheckBox* ffRewindCheck = new wxCheckBox(panel, wxID_ANY, wxString::FromUTF8(FFrewindCheckText));
	
	wxCheckBox* dimCheck = new wxCheckBox(panel, wxID_ANY, wxString::FromUTF8(DimCheckText));
	
	wxStaticBitmap* multiBannerPreview = new wxStaticBitmap(panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, {264, 154}, wxBORDER_NONE);
	
	wxStaticText* multiBannerPreviewIndex = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8("/"));
	wxButton* multiBannerPreviewLeft = new wxButton(panel, wxID_ANY, wxString::FromUTF8("←"));
	wxButton* multiBannerPreviewRight = new wxButton(panel, wxID_ANY, wxString::FromUTF8("→"));
	
	wxStaticText* playerTitleText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(PlayerTitleText));
	wxStaticText* moflexFileText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(MoflexFileText));
	wxStaticText* menuBannerText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(MenuBannerText));
	
	wxPanel* mediaPanel = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxBORDER_THEME);
	wxScrolled<wxPanel>* scrolledPanel = new wxScrolled<wxPanel>(mediaPanel);
	
	std::vector<wxTextCtrl*> PlayerTitles;
	std::vector<wxTextCtrl*> MoflexFiles;
	std::vector<wxTextCtrl*> MenuBanners;
	
	std::vector<wxButton*> MultiUp;
	std::vector<wxButton*> MultiDown;
	
	wxButton* moflexBrowse = new wxButton(scrolledPanel, wxID_ANY, wxString::FromUTF8(Browse), wxDefaultPosition, {120, 35});
	wxButton* multiBannerBrowse = new wxButton(scrolledPanel, wxID_ANY, wxString::FromUTF8(Browse), wxDefaultPosition, {120, 35});
	
	wxButton* removeRow = new wxButton(scrolledPanel, wxID_ANY, wxString::FromUTF8("-"));
	wxButton* appendRow = new wxButton(scrolledPanel, wxID_ANY, wxString::FromUTF8("+"));
	
	wxButton* splitPatchButton = new wxButton(scrolledPanel, wxID_ANY, wxString::FromUTF8(SplitIntoAPatch));
	
	wxStaticText* rowText = new wxStaticText(scrolledPanel, wxID_ANY, wxString::FromUTF8("/"));
};

void initAllWidgets(InitWidgets* initwidgets);
void setToolTips(InitWidgets* initwidgets);

void positionWidgets(InitWidgets* wid);

//dumb?
class ForeColor
{
	public:
	static wxColour panel;
	static wxColour mainMenu;
	static wxColour modeText;
	static wxColour modeChoiceBox;
	static wxColour bannerText;
	static wxColour bannerBox;
	static wxColour bannerBrowse;
	static wxColour bannerError;
	static wxColour iconText;
	static wxColour iconBox;
	static wxColour iconBrowse;
	static wxColour iconError;
	static wxColour shortnameText;
	static wxColour shortnameBox;
	static wxColour shortnameError;
	static wxColour longnameText;
	static wxColour longnameBox;
	static wxColour longnameError;
	static wxColour publisherText;
	static wxColour publisherBox;
	static wxColour publisherError;
	static wxColour copyBox;
	static wxColour copyCheck;
	static wxColour bannerPreviewText;
	static wxColour iconPreview;
	static wxColour ffRewindCheck;
	static wxColour dimCheck;
	static wxColour multiBannerPreviewIndex;
	static wxColour multiBannerPreviewLeft;
	static wxColour multiBannerPreviewRight;
	static wxColour playerTitleText;
	static wxColour moflexFileText;
	static wxColour menuBannerText;
	static wxColour mediaPanel;
	static wxColour moflexBrowse;
	static wxColour multiBannerBrowse;
	static wxColour removeRow;
	static wxColour appendRow;
	static wxColour splitPatchButton;
	static wxColour rowText;
};

class BackColor
{
	public:
	static wxColour panel;
	static wxColour mainMenu;
	static wxColour modeText;
	static wxColour modeChoiceBox;
	static wxColour bannerText;
	static wxColour bannerBox;
	static wxColour bannerBrowse;
	static wxColour bannerError;
	static wxColour iconText;
	static wxColour iconBox;
	static wxColour iconBrowse;
	static wxColour iconError;
	static wxColour shortnameText;
	static wxColour shortnameBox;
	static wxColour shortnameError;
	static wxColour longnameText;
	static wxColour longnameBox;
	static wxColour longnameError;
	static wxColour publisherText;
	static wxColour publisherBox;
	static wxColour publisherError;
	static wxColour copyBox;
	static wxColour copyCheck;
	static wxColour bannerPreviewText;
	static wxColour iconPreview;
	static wxColour ffRewindCheck;
	static wxColour dimCheck;
	static wxColour multiBannerPreviewIndex;
	static wxColour multiBannerPreviewLeft;
	static wxColour multiBannerPreviewRight;
	static wxColour playerTitleText;
	static wxColour moflexFileText;
	static wxColour menuBannerText;
	static wxColour mediaPanel;
	static wxColour moflexBrowse;
	static wxColour multiBannerBrowse;
	static wxColour removeRow;
	static wxColour appendRow;
	static wxColour splitPatchButton;
	static wxColour rowText;
};

void getAppearance(InitWidgets* wid);

//set all widgets on panel to color mode
//0 light 1 dark 2 system
void setAppearance(InitWidgets* wid, int Mode);
