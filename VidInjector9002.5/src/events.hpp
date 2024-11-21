#pragma once

#include "settings.hpp"
#include "strings.hpp"
#include "widgets.hpp"
#include "vi9p.hpp"

void panel_wxEVT_SIZE(InitWidgets* wid, VI9Pparameters* parameters);
void modeChoiceBox_wxEVT_CHOICE(InitWidgets* wid, VI9Pparameters* parameters);
void bannerBox_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters);
void bannerBrowse_wxEVT_BUTTON(InitWidgets* wid);
void iconBox_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters);
void iconPreview_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters);
void iconBrowse_wxEVT_BUTTON(InitWidgets* wid);
void shortnameBox_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters);
void longnameBox_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters);
void publisherBox_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters);
void copyBox_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters);
void copyCheck_wxEVT_CHECKBOX(InitWidgets* wid, VI9Pparameters* parameters);
void ffRewindCheck_wxEVT_CHECKBOX(InitWidgets* wid, VI9Pparameters* parameters);
void dimCheck_wxEVT_CHECKBOX(InitWidgets* wid, VI9Pparameters* parameters);
void multiBannerPreview_wxEVT_BUTTON(InitWidgets* wid);
void multiBannerPreviewLeft_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters);
void multiBannerPreviewRight_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters);
void PlayerTitles_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters, wxTextCtrl* row);
void MoflexFiles_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters, wxTextCtrl* row);
void MenuBanners_wxEVT_TEXT(InitWidgets* wid, VI9Pparameters* parameters, wxTextCtrl* row);
void MultiUp_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters, wxButton* row);
void MultiDown_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters, wxButton* row);
void moflexBrowse_wxEVT_BUTTON(InitWidgets* wid);
void multiBannerBrowse_wxEVT_BUTTON(InitWidgets* wid);
void removeRow_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters);
void splitPatchButton_wxEVT_TOGGLEBUTTON(InitWidgets* wid, VI9Pparameters* parameters);
void splitPatchUp_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters);
void splitPatchDown_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters);

//binds are in these binds so it cant be done outside of main
//void appendRow_wxEVT_BUTTON(InitWidgets* wid, VI9Pparameters* parameters);
//void mainMenu_wxEVT_MENU(InitWidgets* wid, VI9Pparameters* parameters);
