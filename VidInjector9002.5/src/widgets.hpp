//Pretty much all of this is taken from https://github.com/gammasoft71/Examples_wxWidgets

#pragma once

#include <algorithm>
#include <map>

#include <wx/hyperlink.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/artprov.h>
#include <wx/fontdlg.h>
#include <wx/process.h>
#include <wx/progdlg.h>
#include <wx/txtstrm.h>
#include <wx/colour.h>
#include <wx/tglbtn.h>
#include <wx/wx.h>

#include "formatting.hpp"
#include "settings.hpp"
#include "strings.hpp"
#include "vi9p.hpp"

/*
OutCIA: the output .cia that you choose when exporting
OutTAR: the output .tar that you optionally choose when exporting (for splitting into patch)
PID: process ID of the subprocess for exporting it all
*/
class Exports
{
	public:
	static std::string CIA;
	static std::string TAR;
};

class Extracted
{
	public:
	static std::string Archive;
};

enum wxOwnedID {
	ID_EXPORT,
	ID_EXTRACT,
	ID_IMPORTSEED,
	ID_LOGBOOL,
	ID_SYSTEM,
	ID_LIGHT,
	ID_DARK,
	ID_DELETETEMP,
	ID_LANG1,
	ID_LANG2,
	ID_LANG3,
	ID_LANG4,
	ID_LANG5,
	ID_LANG6,
	ID_LANG7,
	ID_LANG8,
	ID_LANG9,
	ID_LANG10,//sorry if you speak more than 10 languages
	ID_ABOUT,
};

/*struct MediaPanelMedia {//just use the pointer as a unique ID number
	uint8_t index;
	wxTextCtrl* box;
};*/

//https://github.com/gammasoft71/Examples_wxWidgets/blob/0337d5a59aaf5753d9a2f11bc39198f7875c9677/wxCore/UserControls/Line/Line.cpp#L5
class wxColouredLine : public wxPanel {
public:
	wxColouredLine(wxWindow* parent, const wxColour& colour, const wxPoint& position = wxDefaultPosition, const wxSize& size = wxDefaultSize) : wxPanel {parent, wxID_ANY, position, size} {
		SetColour(colour);
	}
	
	void SetColour(const wxColour& colour) {SetBackgroundColour(colour);}
	wxColour GetColour() const {return GetBackgroundColour();}
};

struct InitWidgets {
	wxFrame* frame = new wxFrame(nullptr, wxID_ANY, wxString::FromUTF8(frameText), wxDefaultPosition, {Settings::FrameWidth, Settings::FrameHeight});
	wxPanel* panel = new wxPanel(frame);

	//main menu
	wxMenuBar* mainMenu = new wxMenuBar();

	wxMenu* menuFile = new wxMenu();
	wxMenuItem* menuItemFileNew = menuFile->Append(wxID_NEW, wxString::FromUTF8(fileNew + "\tCtrl+N"));
	wxMenuItem* menuItemFileOpen = menuFile->Append(wxID_OPEN, wxString::FromUTF8(fileOpen + "\tCtrl+O"));
	wxMenuItem* menuItemFileSave = menuFile->Append(wxID_SAVE, wxString::FromUTF8(fileSave + "\tCtrl+S"));
	wxMenuItem* menuItemFileSaveAs = menuFile->Append(wxID_SAVEAS, wxString::FromUTF8(fileSaveAs + "\tCtrl+Shift+S"));
	wxMenuItem* menuItemFileExport = menuFile->Append(ID_EXPORT, wxString::FromUTF8(fileExport + "\tCtrl+E"));
	wxMenuItem* menuItemFileExtract = menuFile->Append(ID_EXTRACT, wxString::FromUTF8(fileExtract + "\tCtrl+Alt+E"));
	wxMenuItem* menuItemFileImportSeed = menuFile->Append(ID_IMPORTSEED, wxString::FromUTF8(fileImportSeed));
	wxMenu* menuOptions = new wxMenu();
	wxMenuItem* menuItemOptionsLog = menuOptions->Append(ID_LOGBOOL, wxString::FromUTF8(optionsShowLog), "", wxITEM_CHECK);
	wxMenuItem* menuItemOptionsSystem = menuOptions->Append(ID_SYSTEM, wxString::FromUTF8(optionsSystemMode), "", wxITEM_RADIO);
	wxMenuItem* menuItemOptionsLight = menuOptions->Append(ID_LIGHT, wxString::FromUTF8(optionsLightMode), "", wxITEM_RADIO);
	wxMenuItem* menuItemOptionsDark = menuOptions->Append(ID_DARK, wxString::FromUTF8(optionsDarkMode), "", wxITEM_RADIO);
	wxMenuItem* menuItemOptionsDeleteTemp = menuOptions->Append(ID_DELETETEMP, wxString::FromUTF8(optionsDeleteTemp), "", wxITEM_CHECK);
	wxMenu* menuLanguage = new wxMenu();
	wxMenu* menuHelp = new wxMenu();
	wxMenuItem* menuItemHelpAbout = menuHelp->Append(ID_ABOUT, wxString::FromUTF8(helpAbout + "\tF1"));
	
	//frame
	wxLogWindow* consoleLog = new wxLogWindow(frame, wxString::FromUTF8(logFrameText), false);

	//panel
	wxStaticText* modeText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(ModeText), {10, 10});
	wxChoice* modeChoiceBox = new wxChoice(panel, wxID_ANY);
	
	wxStaticText* bannerText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(BannerText));
	wxTextCtrl* bannerBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString);
	wxButton* bannerBrowse = new wxButton(panel, wxID_ANY, wxString::FromUTF8(Browse));
	wxStaticText* bannerError = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(ErrorText + ' ' + ImageInfoError + " (0) " + SeeLog));
	
	wxStaticText* iconText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(IconText));
	wxTextCtrl* iconBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString);
	wxButton* iconBrowse = new wxButton(panel, wxID_ANY, wxString::FromUTF8(Browse));
	wxStaticText* iconError = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(ErrorText + ' ' + ImageInfoError + " (0) " + SeeLog));
	
	wxStaticText* shortnameText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(ShortNameText));
	wxTextCtrl* shortnameBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	wxStaticText* shortnameError = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (0/64)"));

	wxStaticText* longnameText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(LongNameText));
	wxTextCtrl* longnameBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	wxStaticText* longnameError = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (0/128)"));

	wxStaticText* publisherText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(PublisherText));
	wxTextCtrl* publisherBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	wxStaticText* publisherError = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(ErrorText + ' ' + TextTooLongError + " (0/64)"));

	wxTextCtrl* copyBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, {300, 200}, wxTE_MULTILINE | wxTE_CENTRE);
	wxCheckBox* copyCheck = new wxCheckBox(panel, wxID_ANY, wxString::FromUTF8(CopyrightCheckText));
	
	wxStaticBitmap* bannerPreview = new wxStaticBitmap(panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, {264, 154}, wxBORDER_NONE);
	wxStaticText* bannerCustomText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(BannerCustomText));
	wxStaticText* bannerPreviewText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(BannerPreviewText));
	
	wxButton* iconPreview = new wxButton(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, {48, 48}, wxBORDER_NONE);
	
	wxCheckBox* ffRewindCheck = new wxCheckBox(panel, wxID_ANY, wxString::FromUTF8(FFrewindCheckText));
	
	wxCheckBox* dimCheck = new wxCheckBox(panel, wxID_ANY, wxString::FromUTF8(DimCheckText));
	
	wxButton* multiBannerPreview = new wxButton(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, {264, 154}, wxBORDER_NONE);
	
	wxStaticText* multiBannerPreviewIndex = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8("/"));
	wxButton* multiBannerPreviewLeft = new wxButton(panel, wxID_ANY, wxString::FromUTF8("←"));
	wxButton* multiBannerPreviewRight = new wxButton(panel, wxID_ANY, wxString::FromUTF8("→"));
	
	wxStaticText* playerTitleText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(PlayerTitleText));
	wxStaticText* moflexFileText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(MoflexFileText));
	wxStaticText* menuBannerText = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(MenuBannerText));
	
	//mediaPanel
	wxPanel* mediaPanel = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxBORDER_THEME);
	wxScrolled<wxPanel>* scrolledPanel = new wxScrolled<wxPanel>(mediaPanel);
	
	std::vector<wxTextCtrl*> PlayerTitles;
	std::vector<wxTextCtrl*> MoflexFiles;
	std::vector<wxTextCtrl*> MenuBanners;
	
	std::vector<wxButton*> MultiUp;
	std::vector<wxButton*> MultiDown;
	
	wxButton* moflexBrowse = new wxButton(scrolledPanel, wxID_ANY, wxString::FromUTF8(Browse));
	wxButton* multiBannerBrowse = new wxButton(scrolledPanel, wxID_ANY, wxString::FromUTF8(Browse));
	
	wxButton* removeRow = new wxButton(scrolledPanel, wxID_ANY, wxString::FromUTF8("-"));
	wxButton* appendRow = new wxButton(scrolledPanel, wxID_ANY, wxString::FromUTF8("+"));
	
	wxToggleButton* splitPatchButton = new wxToggleButton(scrolledPanel, wxID_ANY, wxString::FromUTF8(SplitIntoAPatch));
	wxColouredLine* splitPatchLine = new wxColouredLine(scrolledPanel, {0, 0, 0});
	wxButton* splitPatchUp = new wxButton(scrolledPanel, wxID_ANY, wxString::FromUTF8("↑"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	wxButton* splitPatchDown = new wxButton(scrolledPanel, wxID_ANY, wxString::FromUTF8("↓"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	
	wxStaticText* rowText = new wxStaticText(scrolledPanel, wxID_ANY, wxString::FromUTF8("1/27"));
	
	wxFrame* buildframe = new wxFrame(frame, wxID_ANY, wxString::FromUTF8(buildFrameText), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxCLOSE_BOX|wxRESIZE_BORDER|wxFRAME_FLOAT_ON_PARENT|wxCLIP_CHILDREN);
	wxPanel* buildpanel = new wxPanel(buildframe);
	
	wxStaticText* titleIDText = new wxStaticText(buildpanel, wxID_ANY, wxString::FromUTF8(TitleIDText));
	wxTextCtrl* titleIDBox = new wxTextCtrl(buildpanel, wxID_ANY, wxEmptyString);
	wxStaticText* zerozero = new wxStaticText(buildpanel, wxID_ANY, wxString::FromUTF8("00"));
	wxButton* titleIDButton = new wxButton(buildpanel, wxID_ANY, wxString::FromUTF8("⚄"));
	
	wxStaticText* applicationTitleText = new wxStaticText(buildpanel, wxID_ANY, wxString::FromUTF8(ApplicationTitleText));
	wxTextCtrl* applicationTitleBox = new wxTextCtrl(buildpanel, wxID_ANY, wxString::FromUTF8("video"));

	wxStaticText* productCodeText = new wxStaticText(buildpanel, wxID_ANY, wxString::FromUTF8(ProductCodeText));
	wxTextCtrl* productCodeBox = new wxTextCtrl(buildpanel, wxID_ANY, wxString::FromUTF8("VDIJ"));
	
	wxStaticText* statusText = new wxStaticText(buildpanel, wxID_ANY, "   ");
	
	wxGauge* buildBar = new wxGauge(buildpanel, wxID_ANY, 1, wxDefaultPosition, {-1, 25});
	wxProcess* exportArchive = new wxProcess(frame);
	wxTimer* barPulser = new wxTimer();
	wxTimer* exportLogger = new wxTimer();
	
	wxButton* buildButton = new wxButton(buildpanel, wxID_ANY, wxString::FromUTF8(Build));
	wxButton* cancelButton = new wxButton(buildpanel, wxID_ANY, wxString::FromUTF8(Cancel));
	
	wxProgressDialog* extractDialog = new wxProgressDialog(wxEmptyString, wxEmptyString);
	wxProcess* extractArchive = new wxProcess(frame);
	wxTimer* extractPulser = new wxTimer();
	wxTimer* extractLogger = new wxTimer();
	
	wxFrame* aboutframe = new wxFrame(frame, wxID_ANY, wxString::FromUTF8(aboutFrameText), wxDefaultPosition, wxDefaultSize, wxRESIZE_BORDER|wxCAPTION|wxCLOSE_BOX|wxFRAME_FLOAT_ON_PARENT|wxCLIP_CHILDREN);
	wxPanel* aboutpanel = new wxPanel(aboutframe);
	wxStaticBitmap* titleLogo = new wxStaticBitmap(aboutpanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	wxStaticText* byMeText = new wxStaticText(aboutpanel, wxID_ANY, wxString::FromUTF8(ByMeText));
	wxHyperlinkCtrl* gitHubLinker = new wxHyperlinkCtrl(aboutpanel, wxID_ANY, wxString::FromUTF8(GitHubLinker), wxString::FromUTF8(githubRepoLink));
	wxStaticText* versionText = new wxStaticText(aboutpanel, wxID_ANY, wxString::FromUTF8(version));
};

void doAddRows(InitWidgets* wid, int rows);

void initLanguage(InitWidgets* wid);
void initAllWidgets(InitWidgets* initwidgets);
void setFonts(InitWidgets* wid);
void EnableBannerLeftRight(InitWidgets* wid);
void ShowMultiUpDown(InitWidgets* wid);
void ShowPatchUpDown(InitWidgets* wid, VI9Pparameters* parameters);
void setToolTips(InitWidgets* initwidgets);
void setCursors(InitWidgets* wid);

void positionWidgets(InitWidgets* wid, VI9Pparameters* parameters);

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
	static wxColour bannerCustomText;
	static wxColour bannerPreviewText;
	static wxColour iconPreview;
	static wxColour ffRewindCheck;
	static wxColour dimCheck;
	static wxColour multiBannerPreview;
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
	static wxColour splitPatchUp;
	static wxColour splitPatchDown;
	static wxColour buildpanel;
	static wxColour titleIDText;
	static wxColour titleIDBox;
	static wxColour zerozero;
	static wxColour titleIDButton;
	static wxColour applicationTitleText;
	static wxColour applicationTitleBox;
	static wxColour productCodeText;
	static wxColour productCodeBox;
	static wxColour statusText;
	//static wxColour buildBar;
	static wxColour buildButton;
	static wxColour cancelButton;
	static wxColour aboutpanel;
	static wxColour byMeText;
	static wxColour versionText;
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
	static wxColour bannerCustomText;
	static wxColour bannerPreviewText;
	static wxColour iconPreview;
	static wxColour ffRewindCheck;
	static wxColour dimCheck;
	static wxColour multiBannerPreview;
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
	static wxColour splitPatchUp;
	static wxColour splitPatchDown;
	static wxColour buildpanel;
	static wxColour titleIDText;
	static wxColour titleIDBox;
	static wxColour zerozero;
	static wxColour titleIDButton;
	static wxColour applicationTitleText;
	static wxColour applicationTitleBox;
	static wxColour productCodeText;
	static wxColour productCodeBox;
	static wxColour statusText;
	//static wxColour buildBar;
	static wxColour buildButton;
	static wxColour cancelButton;
	static wxColour aboutpanel;
	static wxColour byMeText;
	static wxColour versionText;
};

void getAppearance(InitWidgets* wid);

/*
set all widgets on panel to color mode
0 light 1 dark 2 system
wait a minute... why does the wxStaticText change automatically? oh well, i'll do it anyway
*/
void setAppearance(InitWidgets* wid, int Mode);

//send VI9P::WorkingFile to cli -pp to get parameters from the file
int loadParameters(InitWidgets* wid, VI9Pparameters* parameters);

//enables or disables stuff based on parameters->mode
void applyMode(InitWidgets* wid, VI9Pparameters* parameters);

void applyParameters(InitWidgets* wid, VI9Pparameters* parameters);

/*count: how many rows to add
note that this also called -ar to the CLI to add rows to VI9P::WorkingFile
27 is maximum because if there are more then your 3ds will crash when loading it
hopefully this doesnt leak memory*/
void addRows(InitWidgets* wid, VI9Pparameters* parameters, uint8_t count = 1);

//similar to addRows, this will delete the last element of the wid vectors and pop_back from the wid and parameters vectors
void removeRows(InitWidgets* wid, VI9Pparameters* parameters, uint8_t count = 1);

#include "events.hpp"
