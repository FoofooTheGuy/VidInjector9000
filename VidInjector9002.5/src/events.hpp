#pragma once

#include "settings.hpp"
#include "strings.hpp"
#include "widgets.hpp"
#include "vi9p.hpp"

void frame_wxEVT_WEBREQUEST_STATE(InitWidgets* wid, wxWebRequestEvent* event);
void frame_wxEVT_CLOSE_WINDOW(wxCloseEvent* event);
void panel_wxEVT_SIZE(InitWidgets* wid, VI9Pparameters* parameters);
void modeChoiceBox_wxEVT_CHOICE(InitWidgets* wid, VI9Pparameters* parameters);
void bannerBox_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters);
void bannerBrowse_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters);
void iconBox_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters);
void iconPreview_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters);
void iconBrowse_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters);
void shortnameBox_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters);
void longnameBox_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters);
void publisherBox_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters);
void copyBox_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters);
void copyCheck_wxEVT_CHECKBOX(InitWidgets* wid, VI9Pparameters* parameters);
void ffRewindCheck_wxEVT_CHECKBOX(InitWidgets* wid, VI9Pparameters* parameters);
void dimCheck_wxEVT_CHECKBOX(InitWidgets* wid, VI9Pparameters* parameters);
void multiBannerPreview_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters);
void multiBannerPreviewLeft_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters);
void multiBannerPreviewRight_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters);
void PlayerTitles_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters, wxTextCtrl* row);
void MoflexFiles_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters, wxTextCtrl* row);
void MenuBanners_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters, wxTextCtrl* row);
void MenuBanners_EVT_TEXT_ENTER(InitWidgets* wid, VI9Pparameters* parameters, wxTextCtrl* row);
void MultiUp_wxEVT_BUTTON(InitWidgets* wid, wxButton* row);
void MultiDown_wxEVT_BUTTON(InitWidgets* wid, wxButton* row);
void moflexBrowse_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters);
void multiBannerBrowse_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters);
void removeRow_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters);
void splitPatchButton_wxEVT_TOGGLEBUTTON(InitWidgets* wid, VI9Pparameters* parameters);
void splitPatchUp_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters);
void splitPatchDown_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters);
void buildframe_wxEVT_CLOSE_WINDOW(InitWidgets* wid, wxCloseEvent* event);
void buildpanel_wxEVT_SIZE(InitWidgets* wid, VI9Pparameters* parameters);
void titleIDBox_wxEVT_TEXT(InitWidgets* wid);
void titleIDButton_wxEVT_BUTTON(InitWidgets* wid);
void exportArchive_wxEVT_END_PROCESS(InitWidgets* wid, wxProcessEvent* event);
void buildButton_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters);
void cancelButton_wxEVT_BUTTON(InitWidgets* wid);
void barPulser_wxEVT_TIMER(InitWidgets* wid);
void exportLogger_wxEVT_TIMER(InitWidgets* wid);
void extractDialog_wxEVT_CLOSE_WINDOW(InitWidgets* wid, wxCloseEvent* event);
void extractArchive_wxEVT_END_PROCESS(InitWidgets* wid, wxProcessEvent* event);
void extractPulser_wxEVT_TIMER(InitWidgets* wid);
void extractLogger_wxEVT_TIMER(InitWidgets* wid);
void aboutframe_wxEVT_CLOSE_WINDOW(InitWidgets* wid, wxCloseEvent* event);
void aboutpanel_wxEVT_SIZE(InitWidgets* wid, VI9Pparameters* parameters);


//binds are in these binds so it cant be done outside of main
/*

void buildButt_wxEVT_BUTTON(InitWidgets* wid);
void appendRow_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters);
void mainMenu_wxEVT_MENU(InitWidgets* wid, VI9Pparameters* parameters);
*/
