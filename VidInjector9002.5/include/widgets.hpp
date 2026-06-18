// Pretty much all of this is taken from https://github.com/gammasoft71/Examples_wxWidgets

#pragma once

#include <algorithm>
#include <map>

#include <wx/webrequest.h>
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

#include "programStrings.hpp"
#include "formatting.hpp"
#include "language.hpp"
#include "settings.hpp"
#include "vi9p.hpp"

#include <strings.hpp>

class Borders {
public:
	static int width;
	static int height;
};

// https://github.com/gammasoft71/Examples_wxWidgets/blob/0337d5a59aaf5753d9a2f11bc39198f7875c9677/wxCore/UserControls/Line/Line.cpp#L5
class wxColouredLine : public wxPanel {
public:
	wxColouredLine(wxWindow* parent, const wxColour& colour, const wxPoint& position = wxDefaultPosition, const wxSize& size = wxDefaultSize) : wxPanel {parent, wxID_ANY, position, size} {
		SetColour(colour);
	}
	
	void SetColour(const wxColour& colour) {
		SetBackgroundColour(colour);
	}
	wxColour GetColour() const {
		return GetBackgroundColour();
	}
};

class wxPathCtrl : public wxTextCtrl {
public:
	// attribute inheritance (wxTextCtrl contructor)
	wxPathCtrl(
		wxWindow *parent,
		wxWindowID id,
		const wxString &value = wxEmptyString,
		const wxPoint &pos = wxDefaultPosition,
		const wxSize &size = wxDefaultSize,
		long style = 0,
		const wxValidator &validator = wxDefaultValidator,
		const wxString &name = wxTextCtrlNameStr
	) : wxTextCtrl{parent, id, value, pos, size, style, validator, name} {
		// these binds are actually handled after the other ones in main
		Bind(wxEVT_SIZE, [&](wxSizeEvent& WXUNUSED(event)) {
			DisplayShortPath();
		});
		
		// typing in the box
		Bind(wxEVT_SET_FOCUS, [&](wxFocusEvent& WXUNUSED(event)) {
			DisplayFullPath();
		});

		Bind(wxEVT_KILL_FOCUS, [&](wxFocusEvent& WXUNUSED(event)) {
			DisplayShortPath();
		});

		// hovering cursor
		Bind(wxEVT_ENTER_WINDOW, [&](wxMouseEvent& WXUNUSED(event)) {
			DisplayFullPath();
		});

		Bind(wxEVT_LEAVE_WINDOW, [&](wxMouseEvent& WXUNUSED(event)) {
			if(!HasFocus()) {
				DisplayShortPath();
			}
		});
	}
	
	void SetFullPath(const wxString &value) {
		Full = value;
	}
	
	wxString GetFullPath() {
		return Full;
	}
	
	wxString GetShortPath() {
		// get only the filename
		std::string filename;
		size_t start = Full.find_last_of("\\/");
		if(start == std::string::npos) { // no path
			filename = Full;
		}
		else {
			filename = Full;
			filename.erase(filename.begin(), filename.begin() + start);
			// remove the leading \\/
			removeFirstCharacter(&filename);
		}
		
		// measure stuff
		size_t loop = 0;
		int textwidth, boxwidth;
		wxFont f = GetFont();
		
		GetTextExtent(filename, &textwidth, nullptr, nullptr, nullptr, &f);
		GetSize(&boxwidth, nullptr);
		if(textwidth <= (boxwidth - Borders::width)) {
			Short = wxString::FromUTF8(filename);
			return Short;
		}
		
		// measure it again but shorten more each time
		while(textwidth > (boxwidth - Borders::width)) {
			Short = wxString::FromUTF8(shortenStr(filename, loop));
			++loop;
			
			GetTextExtent(Short, &textwidth, nullptr, nullptr, nullptr, &f);
		}

		return Short;
	}
	
	void DisplayFullPath() {
		ChangeValue(Full);
	}
	
	void DisplayShortPath() {
		ChangeValue(GetShortPath());
	}

private:
	wxString Full = "";
	wxString Short = "";
};

class theWidgets {
public:
	VI9Pparameters parameters;
	
	wxFrame* frame;
	wxPanel* panel;
	
	// main menu
	wxMenuBar* mainMenu;
	
	wxMenu* menuFile;
	wxMenuItem* menuItemFileNew;
	wxMenuItem* menuItemFileOpen;
	wxMenuItem* menuItemFileSave;
	wxMenuItem* menuItemFileSaveAs;
	wxMenuItem* menuItemFileExport;
	wxMenuItem* menuItemFileExtract;
	wxMenuItem* menuItemFileImportSeed;
	wxMenuItem* menuItemFileQuit;
	wxMenu* menuOptions;
	wxMenuItem* menuItemOptionsSystem;
	wxMenuItem* menuItemOptionsLight;
	wxMenuItem* menuItemOptionsDark;
	wxMenuItem* menuItemOptionsLog;
	wxMenuItem* menuItemOptionsDeleteTemp;
	wxMenuItem* menuItemOptionsUpdateCheck;
	wxMenu* menuLanguage;
	wxMenu* menuHelp;
	wxMenuItem* menuItemHelpAbout;
	
	// frame
	wxLogWindow* consoleLog;
	
	// panel
	wxStaticText* modeText;
	wxChoice* modeChoiceBox;
	
	wxStaticText* bannerText;
	wxPathCtrl* bannerBox;
	wxButton* bannerBrowse;
	wxStaticText* bannerError;
	
	wxStaticText* iconText;
	wxPathCtrl* iconBox;
	wxButton* iconBrowse;
	wxStaticText* iconError;
	
	wxStaticText* shortnameText;
	wxTextCtrl* shortnameBox;
	wxStaticText* shortnameError;
	
	wxStaticText* longnameText;
	wxTextCtrl* longnameBox;
	wxStaticText* longnameError;
	
	wxStaticText* publisherText;
	wxTextCtrl* publisherBox;
	wxStaticText* publisherError;
	
	wxTextCtrl* copyBox;
	wxCheckBox* copyCheck;
	
	wxStaticBitmap* bannerPreview;
	wxStaticText* bannerCustomText;
	wxStaticText* bannerPreviewText;
	
	//wxButton* iconPreview = new wxButton(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, {48, 48}, wxBORDER_NONE);
	wxBitmapButton* iconPreview;
	
	wxCheckBox* ffRewindCheck;
	wxCheckBox* dimCheck;
	
	wxBitmapButton* multiBannerPreview;
	
	wxStaticText* multiBannerPreviewIndex;
	wxButton* multiBannerPreviewLeft;
	wxButton* multiBannerPreviewRight;
	
	wxStaticText* playerTitleText;
	wxStaticText* moflexFileText;
	wxStaticText* menuBannerText;
	
	// mediaPanel
	wxPanel* mediaPanel;
	wxScrolled<wxPanel>* scrolledPanel;
	
	std::vector<wxTextCtrl*> PlayerTitles;
	std::vector<wxPathCtrl*> MoflexFiles;
	std::vector<wxPathCtrl*> MenuBanners;
	
	std::vector<wxButton*> MultiUp;
	std::vector<wxButton*> MultiDown;
	
	wxButton* moflexBrowse;
	wxButton* multiBannerBrowse;
	
	wxButton* removeRow;
	wxButton* appendRow;
	
	wxToggleButton* splitPatchButton;
	wxColouredLine* splitPatchLine;
	wxButton* splitPatchUp;
	wxButton* splitPatchDown;
	
	wxStaticText* rowText;
	
	wxFrame* buildframe;
	wxPanel* buildpanel;
	
	wxStaticText* titleIDText;
	wxTextCtrl* titleIDBox;
	wxStaticText* zerozero;
	wxButton* titleIDButton;
	
	wxStaticText* applicationTitleText;
	wxTextCtrl* applicationTitleBox;
	
	wxStaticText* productCodeText;
	wxTextCtrl* productCodeBox;
	
	wxStaticText* statusText;
	
	wxGauge* buildBar;
	wxProcess* exportArchive;
	wxTimer* barPulser;
	wxTimer* exportLogger;
	
	wxButton* buildButton;
	wxButton* cancelButton;
	
	wxProgressDialog* extractDialog;
	wxProcess* extractArchive;
	wxTimer* extractPulser;
	wxTimer* extractLogger;
	
	wxFrame* aboutframe;
	wxPanel* aboutpanel;
	wxStaticBitmap* titleLogo;
	wxStaticText* byMeText;
	wxHyperlinkCtrl* gitHubLinker;
	wxStaticText* versionText;
	
	// you need curl for this if you're linux
	wxWebRequest* updateCheck;
	
	theWidgets();
	
	// wxExecute and write output to log
	// set Execution::flags to change what flags this uses
	int executeCommand(const wxString &command, wxArrayString* output = nullptr, wxArrayString* errors = nullptr);
	
	void doAddRows(int rows);
	
	void setFonts();
	void EnableBannerLeftRight();
	void ShowMultiUpDown();
	void ShowPatchUpDown();
	void setToolTips();
	void setCursors();
	
	void positionWidgets();
	
	void getAppearance();
	
	/*
	set all widgets on panel to color mode
	0 light 1 dark 2 system
	wait a minute... why does the wxStaticText change automatically? oh well, i'll do it anyway
	*/
	void setAppearance(unsigned int Mode);
	
	// send VI9P::WorkingFile to cli -pp to get parameters from the file
	int loadParameters();
	
	// the main stuff that changes based on what mode youre in
	// this enables or disables stuff based on parameters.mode
	void applyMode();
	
	void applyParameters();
	
	/*
	count: how many rows to add
	note that this also called -ar to the CLI to add rows to VI9P::WorkingFile
	MAX_ROWS is maximum because if there are more then your 3ds will crash when loading it
	hopefully this doesnt leak memory
	*/
	void addRows(uint8_t count = 1);
	
	// similar to addRows, this will delete the last element of the wid vectors and pop_back from the wid and parameters vectors
	void removeRows(uint8_t count = 1);
	
	// This will find out what the size of the button borders are for your platform by creating a temporary button (oof)
	// output is stored in Borders::width and Borders::height.
	void getBorders();
	
	// sets the rowText and multiBannerPreviewIndex if mode 1
	void setRowIndex();
		
	// - language -
	void initLanguage();
	void applyLanguage();
	
	// - events -
	void frame_wxEVT_WEBREQUEST_STATE(wxWebRequestEvent* event);
	void frame_wxEVT_CLOSE_WINDOW(wxCloseEvent* event);
	void panel_wxEVT_SIZE();
	void modeChoiceBox_wxEVT_CHOICE();
	void bannerBox_wxEVT_TEXT(bool setValue = false);
	void bannerBrowse_wxEVT_BUTTON();
	void iconBox_wxEVT_TEXT(bool setValue = false);
	void iconPreview_wxEVT_BUTTON();
	void iconBrowse_wxEVT_BUTTON();
	void shortnameBox_wxEVT_TEXT();
	void longnameBox_wxEVT_TEXT();
	void publisherBox_wxEVT_TEXT();
	void copyBox_wxEVT_TEXT();
	void copyCheck_wxEVT_CHECKBOX();
	void ffRewindCheck_wxEVT_CHECKBOX();
	void dimCheck_wxEVT_CHECKBOX();
	void multiBannerPreview_wxEVT_BUTTON();
	void multiBannerPreviewLeft_wxEVT_BUTTON();
	void multiBannerPreviewRight_wxEVT_BUTTON();
	void PlayerTitles_wxEVT_TEXT(wxTextCtrl* row);
	void MoflexFiles_wxEVT_TEXT(wxPathCtrl* row);
	void MenuBanners_wxEVT_TEXT(wxPathCtrl* row, bool setValue = false);
	void MenuBanners_EVT_TEXT_ENTER(wxPathCtrl* row);
	void MultiUp_wxEVT_BUTTON(wxButton* row);
	void MultiDown_wxEVT_BUTTON(wxButton* row);
	void moflexBrowse_wxEVT_BUTTON();
	void multiBannerBrowse_wxEVT_BUTTON();
	void removeRow_wxEVT_BUTTON();
	void splitPatchButton_wxEVT_TOGGLEBUTTON();
	void splitPatchUp_wxEVT_BUTTON();
	void splitPatchDown_wxEVT_BUTTON();
	void buildframe_wxEVT_CLOSE_WINDOW(wxCloseEvent* event);
	void buildpanel_wxEVT_SIZE();
	void titleIDBox_wxEVT_TEXT();
	void titleIDButton_wxEVT_BUTTON();
	void exportArchive_wxEVT_END_PROCESS(wxProcessEvent* event);
	void buildButton_wxEVT_BUTTON();
	void cancelButton_wxEVT_BUTTON();
	void barPulser_wxEVT_TIMER();
	void exportLogger_wxEVT_TIMER();
	void extractDialog_wxEVT_CLOSE_WINDOW(wxCloseEvent* event);
	void extractArchive_wxEVT_END_PROCESS(wxProcessEvent* event);
	void extractPulser_wxEVT_TIMER();
	void extractLogger_wxEVT_TIMER();
	void aboutframe_wxEVT_CLOSE_WINDOW(wxCloseEvent* event);
	void aboutpanel_wxEVT_SIZE();
};

/*
OutCIA: the output .cia that you choose when exporting
OutTAR: the output .tar that you optionally choose when exporting (for splitting into patch)
PID: process ID of the subprocess for exporting it all
*/
class Exports {
public:
	static std::string CIA;
	static std::string TAR;
};

class Extracted {
public:
	static std::string Archive;
};

class Execution {
public:
	static int flags;
};

enum wxOwnedID {
	ID_EXPORT,
	ID_EXTRACT,
	ID_IMPORTSEED,
	ID_QUIT,
	ID_SYSTEM,
	ID_LIGHT,
	ID_DARK,
	ID_LOGBOOL,
	ID_DELETETEMP,
	ID_UPDATECHECK,
	ID_LANG1,
	ID_LANG2,
	ID_LANG3,
	ID_LANG4,
	ID_LANG5,
	ID_LANG6,
	ID_LANG7,
	ID_LANG8,
	ID_LANG9,
	ID_LANG10, // sorry if you speak more than 10 languages
	ID_ABOUT,
};

//dumb?
class ForeColor {
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

class BackColor {
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
