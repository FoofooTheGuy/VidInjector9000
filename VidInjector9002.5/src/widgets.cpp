#include "widgets.hpp"

void initAllWidgets(InitWidgets* wid) {
	//main menu
	wid->menuItemFileNew->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW, wxART_MENU));
	wid->menuItemFileOpen->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_MENU));
	wid->menuItemFileSave->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_MENU));
	wid->menuItemFileSaveAs->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS, wxART_MENU));
	wid->mainMenu->Append(wid->menuFile, wxString::FromUTF8(file));
	
	wid->mainMenu->Append(wid->menuOptions, wxString::FromUTF8(options));
	
	wid->frame->SetMenuBar(wid->mainMenu);
	
	//panel
	//set fonts
	{//modeText
		int w, h;
		wxFont f;
		
		wid->modeText->SetFont(wid->modeText->GetFont().Scale(1.5F));
		
		f = wid->modeText->GetFont();
		wid->modeText->GetTextExtent(wid->modeText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->modeText->SetSize(w, h);
	}
	
	{//bannerText
		int w, h;
		wxFont f;
		
		wid->bannerText->SetFont(wid->modeText->GetFont());
		
		f = wid->bannerText->GetFont();
		wid->bannerText->GetTextExtent(wid->bannerText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->bannerText->SetSize(w, h);
	}
	{//bannerError
		int w, h;
		wxFont f;
		
		wid->bannerError->SetFont(wid->modeText->GetFont().Scale(0.666F));
		wid->bannerError->SetFont(wid->bannerError->GetFont().Bold());
		
		f = wid->bannerError->GetFont();
		wid->bannerError->GetTextExtent(wid->bannerError->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->bannerError->SetSize(w, h);
		
		//wid->bannerError->Show(false);
	}
	
	{//iconText
		int w, h;
		wxFont f;
		
		wid->iconText->SetFont(wid->modeText->GetFont());
		
		f = wid->iconText->GetFont();
		wid->iconText->GetTextExtent(wid->iconText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->iconText->SetSize(w, h);
	}
	{//iconError
		int w, h;
		wxFont f;
		
		wid->iconError->SetFont(wid->bannerError->GetFont());
		wid->iconError->SetFont(wid->iconError->GetFont().Bold());
		
		f = wid->iconError->GetFont();
		wid->iconError->GetTextExtent(wid->iconError->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->iconError->SetSize(w, h);
	}

	{//shortnameText
		int w, h;
		wxFont f;
		
		wid->shortnameText->SetFont(wid->modeText->GetFont());
		
		f = wid->shortnameText->GetFont();
		wid->shortnameText->GetTextExtent(wid->shortnameText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->shortnameText->SetSize(w, h);
	}
	{//shortnameError
		int w, h;
		wxFont f;
		
		wid->shortnameError->SetFont(wid->bannerError->GetFont());
		wid->shortnameError->SetFont(wid->shortnameError->GetFont().Bold());
		
		f = wid->shortnameError->GetFont();
		wid->shortnameError->GetTextExtent(wid->shortnameError->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->shortnameError->SetSize(w, h);
	}
	
	{//longnameText
		int w, h;
		wxFont f;
		
		wid->longnameText->SetFont(wid->modeText->GetFont());
		
		f = wid->longnameText->GetFont();
		wid->longnameText->GetTextExtent(wid->longnameText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->longnameText->SetSize(w, h);
	}
	{//longnameError
		int w, h;
		wxFont f;
		
		wid->longnameError->SetFont(wid->bannerError->GetFont());
		wid->longnameError->SetFont(wid->longnameError->GetFont().Bold());
		
		f = wid->longnameError->GetFont();
		wid->longnameError->GetTextExtent(wid->longnameError->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->longnameError->SetSize(w, h);
	}
	
	{//publisherText
		int w, h;
		wxFont f;
		
		wid->publisherText->SetFont(wid->modeText->GetFont());
		
		f = wid->publisherText->GetFont();
		wid->publisherText->GetTextExtent(wid->publisherText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->publisherText->SetSize(w, h);
	}
	{//publisherError
		int w, h;
		wxFont f;
		
		wid->publisherError->SetFont(wid->bannerError->GetFont());
		wid->publisherError->SetFont(wid->publisherError->GetFont().Bold());

		f = wid->publisherError->GetFont();
		wid->publisherError->GetTextExtent(wid->publisherError->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->publisherError->SetSize(w, h);
	}
	
	{//bannerPreviewText
		int w, h;
		wxFont f;
		
		wid->bannerPreviewText->SetFont(wid->bannerError->GetFont().GetBaseFont());
		
		f = wid->bannerPreviewText->GetFont();
		wid->bannerPreviewText->GetTextExtent(wid->bannerPreviewText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->bannerPreviewText->SetSize(w, h);
	}

	{//copyCheck
		int w, width, mywidth, boxwidth, h;
		wxFont f;
		
		f = wid->copyCheck->GetFont();
		wid->copyCheck->GetTextExtent(wid->copyCheck->GetLabel(), &width, NULL, nullptr, nullptr, &f);
		wid->copyCheck->GetSize(&mywidth, NULL);
		
		boxwidth = mywidth - width;
		
		wid->copyCheck->SetFont(wid->modeText->GetFont());
		
		f = wid->copyCheck->GetFont();
		wid->copyCheck->GetTextExtent(wid->copyCheck->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->copyCheck->SetSize(w + boxwidth, h);
	}
	{//ffRewindCheck
		int w, width, mywidth, boxwidth, h;
		wxFont f;
		
		f = wid->ffRewindCheck->GetFont();
		wid->ffRewindCheck->GetTextExtent(wid->ffRewindCheck->GetLabel(), &width, NULL, nullptr, nullptr, &f);
		wid->ffRewindCheck->GetSize(&mywidth, NULL);
		
		boxwidth = mywidth - width;
		
		wid->ffRewindCheck->SetFont(wid->modeText->GetFont().Scale(0.9F));
		
		f = wid->ffRewindCheck->GetFont();
		wid->ffRewindCheck->GetTextExtent(wid->ffRewindCheck->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->ffRewindCheck->SetSize(w + boxwidth, h);
	}
	{//dimCheck
		int w, width, mywidth, boxwidth, h;
		wxFont f;
		
		f = wid->dimCheck->GetFont();
		wid->dimCheck->GetTextExtent(wid->dimCheck->GetLabel(), &width, NULL, nullptr, nullptr, &f);
		wid->dimCheck->GetSize(&mywidth, NULL);
		
		boxwidth = mywidth - width;
		
		wid->dimCheck->SetFont(wid->ffRewindCheck->GetFont());
		
		f = wid->dimCheck->GetFont();
		wid->dimCheck->GetTextExtent(wid->dimCheck->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->dimCheck->SetSize(w + boxwidth, h);
	}
	
	{//multiBannerPreviewIndex
		int w, h;
		wxFont f;
		
		wid->multiBannerPreviewIndex->SetFont(wid->bannerPreviewText->GetFont());
		
		f = wid->multiBannerPreviewIndex->GetFont();
		wid->multiBannerPreviewIndex->GetTextExtent(wid->multiBannerPreviewIndex->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->multiBannerPreviewIndex->SetSize(w, h);
	}
	
	{//playerTitleText
		int w, h;
		wxFont f;
		
		wid->playerTitleText->SetFont(wid->modeText->GetFont());
		
		f = wid->playerTitleText->GetFont();
		wid->playerTitleText->GetTextExtent(wid->playerTitleText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->playerTitleText->SetSize(w, h);
	}
	{//moflexFileText
		int w, h;
		wxFont f;
		
		wid->moflexFileText->SetFont(wid->modeText->GetFont());
		
		f = wid->moflexFileText->GetFont();
		wid->moflexFileText->GetTextExtent(wid->moflexFileText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->moflexFileText->SetSize(w, h);
	}
	{//menuBannerText
		int w, h;
		wxFont f;
		
		wid->menuBannerText->SetFont(wid->modeText->GetFont());
		
		f = wid->menuBannerText->GetFont();
		wid->menuBannerText->GetTextExtent(wid->menuBannerText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->menuBannerText->SetSize(w, h);
	}
	
	for (const auto &s : {SingleVideo, MultiVideo})//https://github.com/gammasoft71/Examples_wxWidgets/blob/adbd395081bf25c9034f2b64eee62608a943441f/src/CommonControls/Choice/Choice.cpp#L10
        wid->modeChoiceBox->Append(s);
	//wid->modeChoiceBox->SetSelection(0);
	
	wid->scrolledPanel->SetScrollRate(10, 10);
	
	{//row stuff
		int rows = 1;//yes this is loop executes once. the plan is to reuse this code when adding many rows at once. yes, yes, I know...
		
		for(int i = 0; i < rows; i++) {
			wxTextCtrl* box = new wxTextCtrl(wid->scrolledPanel, wxID_ANY, wxEmptyString);
			wid->PlayerTitles.push_back(box);
		}
		for(int i = 0; i < rows; i++) {
			wxTextCtrl* box = new wxTextCtrl(wid->scrolledPanel, wxID_ANY, wxEmptyString);
			wid->MoflexFiles.push_back(box);
		}
		for(int i = 0; i < rows; i++) {
			wxTextCtrl* box = new wxTextCtrl(wid->scrolledPanel, wxID_ANY, wxEmptyString);
			wid->MenuBanners.push_back(box);
		}

		for(int i = 0; i < rows; i++) {
			wxButton* button = new wxButton(wid->scrolledPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
			int width, height;
			wxFont f;
			
			button->SetLabel(wxString::FromUTF8("↑"));
			
			button->GetTextExtent(button->GetLabel(), &width, &height, nullptr, nullptr, &f);
			button->SetSize(width, height);
			
			wid->MultiUp.push_back(button);
		}
		for(int i = 0; i < rows; i++) {
			wxButton* button = new wxButton(wid->scrolledPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
			int width, height;
			wxFont f;
			
			button->SetLabel(wxString::FromUTF8("↓"));
			
			button->GetTextExtent(button->GetLabel(), &width, &height, nullptr, nullptr, &f);
			button->SetSize(width, height);
			
			wid->MultiDown.push_back(button);
		}
	}
	
	{//removeRow
		int w, buttwidth, h, buttheight;
		wxFont f;
		
		wid->removeRow->GetSize(&buttwidth, &buttheight);
		f = wid->removeRow->GetFont();
		wid->removeRow->GetTextExtent(wid->removeRow->GetLabel(), &w, &h, nullptr, nullptr, &f);
		buttwidth = buttwidth - w;
		buttheight = buttheight - h;
		
		wid->removeRow->SetFont(wid->modeText->GetFont().Scale(1.2F));
		
		f = wid->removeRow->GetFont();
		wid->removeRow->GetTextExtent(wid->removeRow->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->removeRow->SetSize(w + buttwidth, h + buttheight);
	}
	{//appendRow
		int w, buttwidth, h, buttheight;
		wxFont f;
		
		wid->appendRow->GetSize(&buttwidth, &buttheight);
		f = wid->appendRow->GetFont();
		wid->appendRow->GetTextExtent(wid->appendRow->GetLabel(), &w, &h, nullptr, nullptr, &f);
		buttwidth = buttwidth - w;
		buttheight = buttheight - h;
		
		wid->appendRow->SetFont(wid->removeRow->GetFont());
		
		f = wid->appendRow->GetFont();
		wid->appendRow->GetTextExtent(wid->appendRow->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->appendRow->SetSize(w + buttwidth, h + buttheight);
	}

	{//splitPatchButton
		int w, buttwidth, h, buttheight;
		wxFont f;
		
		wid->splitPatchButton->GetSize(&buttwidth, &buttheight);
		f = wid->splitPatchButton->GetFont();
		wid->splitPatchButton->GetTextExtent(wid->splitPatchButton->GetLabel(), &w, &h, nullptr, nullptr, &f);
		buttwidth = buttwidth - w;
		buttheight = buttheight - h;
		
		wid->splitPatchButton->SetFont(wid->ffRewindCheck->GetFont());
		
		f = wid->splitPatchButton->GetFont();
		wid->splitPatchButton->GetTextExtent(wid->splitPatchButton->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->splitPatchButton->SetSize(w + buttwidth, h + buttheight);
	}
	{//rowText
		int w, h;
		wxFont f;
		
		wid->rowText->SetFont(wid->bannerPreviewText->GetFont());
		
		f = wid->rowText->GetFont();
		wid->rowText->GetTextExtent(wid->rowText->GetLabel(), &w, &h, nullptr, nullptr, &f);
		wid->rowText->SetSize(w, h);
	}
}

void setToolTips(InitWidgets* wid) {
	wid->modeChoiceBox->SetToolTip(wxString::FromUTF8(modeChoiceBoxTip));
	wid->bannerBox->SetToolTip(wxString::FromUTF8(bannerBoxTip));
	wid->bannerBrowse->SetToolTip(wxString::FromUTF8(bannerBrowseTip));
	wid->iconBox->SetToolTip(wxString::FromUTF8(iconBoxTip));
	wid->iconBrowse->SetToolTip(wxString::FromUTF8(iconBrowseTip));
	wid->shortnameBox->SetToolTip(wxString::FromUTF8(shortnameBoxTip));
	wid->longnameBox->SetToolTip(wxString::FromUTF8(longnameBoxTip));
	wid->publisherBox->SetToolTip(wxString::FromUTF8(publisherBoxTip));
	wid->copyBox->SetToolTip(wxString::FromUTF8(copyBoxTip));
	wid->copyCheck->SetToolTip(wxString::FromUTF8(copyCheckTip));
	wid->iconPreview->SetToolTip(wxString::FromUTF8(iconPreviewTip));
	wid->ffRewindCheck->SetToolTip(wxString::FromUTF8(ffRewindCheckTip));
	wid->dimCheck->SetToolTip(wxString::FromUTF8(dimCheckTip));
	wid->multiBannerPreview->SetToolTip(wxString::FromUTF8(multiBannerPreviewTip));
	wid->multiBannerPreviewLeft->SetToolTip(wxString::FromUTF8(multiBannerPreviewLeftTip));
	wid->multiBannerPreviewRight->SetToolTip(wxString::FromUTF8(multiBannerPreviewRightTip));
}

void positionWidgets(InitWidgets* wid) {
	{//modeChoiceBox
		int x, y, height;
		wid->modeText->GetPosition(&x, &y);
		wid->modeText->GetSize(NULL, &height);
		
		wid->modeChoiceBox->Move(x, y + height);
	}
	{//bannerText
		int x, y, height;
		wid->modeChoiceBox->GetPosition(&x, &y);
		wid->modeChoiceBox->GetSize(NULL, &height);
		
		wid->bannerText->Move(x, y + (height * 2));
	}
	{//bannerBox
		int x, y, width, height, myheight;
		wid->bannerText->GetPosition(&x, &y);
		wid->bannerText->GetSize(&width, &height);
		wid->bannerBox->GetSize(NULL, &myheight);
		
		wid->bannerBox->Move(x + width + 5, y + ((height - myheight) / 2));
	}
	{//bannerBrowse y
		int myx, y, height, myheight;
		wid->bannerBox->GetPosition(NULL, &y);
		wid->bannerBox->GetSize(NULL, &height);
		wid->bannerBrowse->GetPosition(&myx, NULL);
		wid->bannerBrowse->GetSize(NULL, &myheight);
		
		wid->bannerBrowse->Move(myx, y + ((height - myheight) / 2));
	}
	{//bannerError
		int x, y, height;
		wid->bannerBox->GetPosition(&x, NULL);
		wid->bannerBrowse->GetPosition(NULL, &y);
		wid->bannerBrowse->GetSize(NULL, &height);
		
		wid->bannerError->Move(x, y + height);
	}
	{//iconText
		int x, y, height, myheight, browseheight;
		wid->modeText->GetPosition(&x, NULL);
		wid->bannerError->GetPosition(NULL, &y);
		wid->bannerError->GetSize(NULL, &height);
		wid->iconText->GetSize(NULL, &myheight);
		wid->iconBrowse->GetSize(NULL, &browseheight);
		
		wid->iconText->Move(x, y + height + ((browseheight - myheight) / 2));
	}
	{//iconBox
		int x, y, width, height, myheight;
		wid->iconText->GetPosition(&x, &y);
		wid->iconText->GetSize(&width, &height);
		wid->iconBox->GetSize(NULL, &myheight);
		
		wid->iconBox->Move(x + width + 5, y + ((height - myheight) / 2));
		//wid->iconBox->SetSize(, &myheight);
	}
	{//iconBrowse y
		int myx, y, height;
		wid->iconBrowse->GetPosition(&myx, NULL);
		wid->bannerError->GetPosition(NULL, &y);
		wid->bannerError->GetSize(NULL, &height);
		
		wid->iconBrowse->Move(myx, y + height);
	}
	{//iconError
		int x, y, height;
		wid->iconBox->GetPosition(&x, NULL);
		wid->iconBrowse->GetPosition(NULL, &y);
		wid->iconBrowse->GetSize(NULL, &height);
		
		wid->iconError->Move(x, y + height);
	}
	{//shortnameText
		int x, y, height;
		wid->modeText->GetPosition(&x, NULL);
		wid->iconError->GetPosition(NULL, &y);
		wid->iconError->GetSize(NULL, &height);
		
		wid->shortnameText->Move(x, y + height + 6);
	}
	{//shortnameBox
		int x, y, width;
		wid->shortnameText->GetPosition(&x, &y);
		wid->shortnameText->GetSize(&width, NULL);
		
		wid->shortnameBox->Move(x + width + 5, y);
	}
	{//shortnameError
		int x, y, height;
		wid->shortnameBox->GetPosition(&x, &y);
		wid->shortnameBox->GetSize(NULL, &height);
		
		wid->shortnameError->Move(x, y + height);
	}
	{//longnameText
		int x, y, height;
		wid->modeText->GetPosition(&x, NULL);
		wid->shortnameError->GetPosition(NULL, &y);
		wid->shortnameError->GetSize(NULL, &height);
		
		wid->longnameText->Move(x, y + height);
	}
	{//longnameBox
		int x, y, width;
		wid->longnameText->GetPosition(&x, &y);
		wid->longnameText->GetSize(&width, NULL);
		
		wid->longnameBox->Move(x + width + 5, y);
	}
	{//longnameError
		int x, y, height;
		wid->longnameBox->GetPosition(&x, &y);
		wid->longnameBox->GetSize(NULL, &height);
		
		wid->longnameError->Move(x, y + height);
	}
	{//publisherText
		int x, y, height;
		wid->modeText->GetPosition(&x, NULL);
		wid->longnameError->GetPosition(NULL, &y);
		wid->longnameError->GetSize(NULL, &height);
		
		wid->publisherText->Move(x, y + height);
	}
	{//publisherBox
		int x, y, width;
		wid->publisherText->GetPosition(&x, &y);
		wid->publisherText->GetSize(&width, NULL);
		
		wid->publisherBox->Move(x + width + 5, y);
	}
	{//publisherError
		int x, y, height;
		wid->publisherBox->GetPosition(&x, &y);
		wid->publisherBox->GetSize(NULL, &height);
		
		wid->publisherError->Move(x, y + height);
	}
	{//ffRewindCheck
		int x, y, height;
		wid->publisherError->GetPosition(&x, &y);
		wid->publisherError->GetSize(NULL, &height);
		
		wid->ffRewindCheck->Move(x, y + height);
	}
	{//dimCheck
		int x, y, height;
		wid->ffRewindCheck->GetPosition(&x, &y);
		wid->ffRewindCheck->GetSize(NULL, &height);
		
		wid->dimCheck->Move(x, y + height);
	}
	//stuff that moves or changes size
	{//copyBox
		int x, y, width, mywidth, checkwidth, checkheight;
		wid->modeText->GetPosition(&x, &y);
		wid->panel->GetSize(&width, NULL);
		wid->copyBox->GetSize(&mywidth, NULL);
		wid->copyCheck->GetSize(&checkwidth, NULL);
		wid->copyCheck->GetSize(NULL, &checkheight);
		
		int iconErrorX, iconErrorW, iconPreviewW, bannerPreviewW, bannerPreviewTextW;
		wid->iconError->GetPosition(&iconErrorX, NULL);
		wid->iconError->GetSize(&iconErrorW, NULL);
		wid->iconPreview->GetSize(&iconPreviewW, NULL);
		wid->bannerPreview->GetSize(&bannerPreviewW, NULL);
		wid->bannerPreviewText->GetSize(&bannerPreviewTextW, NULL);
		
		if(width >= iconErrorX + iconErrorW + iconPreviewW + 5 + (bannerPreviewW > bannerPreviewTextW ? bannerPreviewW : bannerPreviewTextW) + 15 + (mywidth > checkwidth ? mywidth : checkwidth) + x) {
			wid->copyBox->Move(width - (mywidth > checkwidth ? mywidth : checkwidth) - x, y + checkheight);
		}
		else {
			wid->copyBox->Move(iconErrorX + iconErrorW + iconPreviewW + 5 + (bannerPreviewW > bannerPreviewTextW ? bannerPreviewW : bannerPreviewTextW) + 15, y + checkheight);
		}
	}
	{//copyCheck
		int x, y, myheight;
		wid->copyBox->GetPosition(&x, &y);
		wid->copyCheck->GetSize(NULL, &myheight);
		
		wid->copyCheck->Move(x, y - myheight);
	}
	{//bannerPreview
		int x, y, mywidth, bannerPreviewTextW;
		wid->copyCheck->GetPosition(&x, &y);
		wid->bannerPreview->GetSize(&mywidth, NULL);
		wid->bannerPreviewText->GetSize(&bannerPreviewTextW, NULL);
		
		wid->bannerPreview->Move(x - (mywidth > bannerPreviewTextW ? mywidth : ((bannerPreviewTextW - mywidth) * 2)) - (mywidth > bannerPreviewTextW ? 15 : 15 * 2), y);
	}
	{//bannerPreviewText
		int x, y, width, mywidth, height;
		wid->bannerPreview->GetPosition(&x, &y);
		wid->bannerPreview->GetSize(&width, &height);
		wid->bannerPreviewText->GetSize(&mywidth, NULL);
		
		wid->bannerPreviewText->Move(x + ((width - mywidth) / 2), y + height);
	}
	{//iconPreview
		int x, bannerPreviewTextx, y, mywidth;
		wid->iconBrowse->GetPosition(NULL, &y);
		wid->bannerPreview->GetPosition(&x, NULL);
		wid->iconPreview->GetSize(&mywidth, NULL);
		wid->bannerPreviewText->GetPosition(&bannerPreviewTextx, NULL);
		
		wid->iconPreview->Move((x < bannerPreviewTextx ? x : bannerPreviewTextx) - mywidth - 5, y);
	}
	{//bannerBrowse x
		int x, bannerPreviewTextx, myy, mywidth;
		wid->bannerPreview->GetPosition(&x, NULL);
		wid->bannerBrowse->GetPosition(NULL, &myy);
		wid->bannerBrowse->GetSize(&mywidth, NULL);
		wid->bannerPreviewText->GetPosition(&bannerPreviewTextx, NULL);
		
		wid->bannerBrowse->Move((x < bannerPreviewTextx ? x : bannerPreviewTextx) - mywidth - 5, myy);
	}
	{//iconBrowse x
		int x, myy, mywidth;
		wid->iconPreview->GetPosition(&x, NULL);
		wid->iconBrowse->GetPosition(NULL, &myy);
		wid->iconBrowse->GetSize(&mywidth, NULL);
		
		wid->iconBrowse->Move(x - mywidth - 2, myy);
	}
	{//bannerBox width
		int myx, browsex, myheight;
		wid->bannerBrowse->GetPosition(&browsex, NULL);
		wid->bannerBox->GetPosition(&myx, NULL);
		wid->bannerBox->GetSize(NULL, &myheight);
		
		wid->bannerBox->SetSize(browsex - myx - 2, myheight);
	}
	{//iconBox width
		int myx, browsex, myheight;
		wid->iconBrowse->GetPosition(&browsex, NULL);
		wid->iconBox->GetPosition(&myx, NULL);
		wid->iconBox->GetSize(NULL, &myheight);
		
		wid->iconBox->SetSize(browsex - myx - 2, myheight);
	}
	{//shortnameBox width
		int myx, x, bannerPreviewTextx, width, bannerPreviewTextW, myheight;
		wid->bannerPreview->GetPosition(&x, NULL);
		wid->bannerPreview->GetSize(&width, NULL);
		wid->bannerPreviewText->GetPosition(&bannerPreviewTextx, NULL);
		wid->bannerPreviewText->GetSize(&bannerPreviewTextW, NULL);
		wid->shortnameBox->GetPosition(&myx, NULL);
		wid->shortnameBox->GetSize(NULL, &myheight);
		
		wid->shortnameBox->SetSize(((x < bannerPreviewTextx ? x : bannerPreviewTextx) + (width > bannerPreviewTextW ? width : bannerPreviewTextW)) - myx, myheight);
	}
	{//longnameBox width
		int myx, x, bannerPreviewTextx, width, bannerPreviewTextW, myheight;
		wid->bannerPreview->GetPosition(&x, NULL);
		wid->bannerPreview->GetSize(&width, NULL);
		wid->bannerPreviewText->GetPosition(&bannerPreviewTextx, NULL);
		wid->bannerPreviewText->GetSize(&bannerPreviewTextW, NULL);
		wid->longnameBox->GetPosition(&myx, NULL);
		wid->longnameBox->GetSize(NULL, &myheight);
		
		wid->longnameBox->SetSize(((x < bannerPreviewTextx ? x : bannerPreviewTextx) + (width > bannerPreviewTextW ? width : bannerPreviewTextW)) - myx, myheight);
	}
	{//publisherBox width
		int myx, x, bannerPreviewTextx, width, bannerPreviewTextW, myheight;
		wid->bannerPreview->GetPosition(&x, NULL);
		wid->bannerPreview->GetSize(&width, NULL);
		wid->bannerPreviewText->GetPosition(&bannerPreviewTextx, NULL);
		wid->bannerPreviewText->GetSize(&bannerPreviewTextW, NULL);
		wid->publisherBox->GetPosition(&myx, NULL);
		wid->publisherBox->GetSize(NULL, &myheight);
		
		wid->publisherBox->SetSize(((x < bannerPreviewTextx ? x : bannerPreviewTextx) + (width > bannerPreviewTextW ? width : bannerPreviewTextW)) - myx, myheight);
	}
	{//multiBannerPreview
		int x, y, width, mywidth, height;
		wid->copyBox->GetPosition(&x, &y);
		wid->copyBox->GetSize(&width, &height);
		wid->multiBannerPreview->GetSize(&mywidth, NULL);
		
		wid->multiBannerPreview->Move(x + ((width - mywidth) / 2), y + height + 10);
	}
	{//multiBannerPreviewIndex
		int x, y, width, mywidth, height;
		wid->multiBannerPreview->GetPosition(&x, &y);
		wid->multiBannerPreview->GetSize(&width, &height);
		wid->multiBannerPreviewIndex->GetSize(&mywidth, NULL);
		
		wid->multiBannerPreviewIndex->Move(x + ((width - mywidth) / 2), y + height);
	}
	{//multiBannerPreviewLeft
		int x, y, height;
		wid->multiBannerPreview->GetPosition(&x, &y);
		wid->multiBannerPreview->GetSize(NULL, &height);
		//wid->multiBannerIndex->GetSize(&mywidth, NULL);
		
		wid->multiBannerPreviewLeft->Move(x, y + height);
	}
	{//multiBannerPreviewRight
		int x, y, width, mywidth, height;
		wid->multiBannerPreview->GetPosition(&x, &y);
		wid->multiBannerPreview->GetSize(&width, &height);
		wid->multiBannerPreviewRight->GetSize(&mywidth, NULL);
		
		wid->multiBannerPreviewRight->Move((x + width) - mywidth, y + height);
	}
	{//playerTitleText y
		int x, y, checky, height, checkheight;
		wid->multiBannerPreviewLeft->GetPosition(NULL, &y);
		wid->dimCheck->GetPosition(NULL, &checky);
		wid->dimCheck->GetSize(NULL, &checkheight);
		wid->multiBannerPreviewLeft->GetSize(NULL, &height);
		wid->playerTitleText->GetPosition(&x, NULL);
		
		wid->playerTitleText->Move(x, (y > checky ? (y + height + 5) : (checky + checkheight + 5)));
	}
	{//moflexFileText y
		int x, y, checky, height, checkheight;
		wid->multiBannerPreviewLeft->GetPosition(NULL, &y);
		wid->dimCheck->GetPosition(NULL, &checky);
		wid->dimCheck->GetSize(NULL, &checkheight);
		wid->multiBannerPreviewLeft->GetSize(NULL, &height);
		wid->moflexFileText->GetPosition(&x, NULL);
		
		wid->moflexFileText->Move(x, (y > checky ? (y + height + 5) : (checky + checkheight + 5)));
	}
	{//menuBannerText y
		int x, y, checky, height, checkheight;
		wid->multiBannerPreviewLeft->GetPosition(NULL, &y);
		wid->multiBannerPreviewLeft->GetSize(NULL, &height);
		wid->dimCheck->GetPosition(NULL, &checky);
		wid->dimCheck->GetSize(NULL, &checkheight);
		wid->menuBannerText->GetPosition(&x, NULL);
		
		wid->menuBannerText->Move(x, (y > checky ? (y + height + 5) : (checky + checkheight + 5)));
	}
	{//mediaPanel
		int x, myx, y, myy, width, height;
		wid->modeText->GetPosition(&x, NULL);
		wid->playerTitleText->GetPosition(NULL, &y);
		wid->playerTitleText->GetSize(NULL, &height);
		
		wid->mediaPanel->Move(x, y + height + 1);
		
		wid->panel->GetSize(&width, &height);
		wid->mediaPanel->GetPosition(&myx, &myy);
		
		wid->mediaPanel->SetSize(width - (myx * 2), height - myy - myx);
	}
	{
		std::vector<int> sizes(5, 0);
		wid->playerTitleText->GetSize(&sizes[0], NULL);
		wid->moflexFileText->GetSize(&sizes[1], NULL);
		wid->menuBannerText->GetSize(&sizes[2], NULL);
		wid->moflexBrowse->GetSize(&sizes[3], NULL);
		wid->multiBannerBrowse->GetSize(&sizes[4], NULL);
		std::vector<int>::iterator widestText = std::max_element(sizes.begin(), sizes.end());

		{//PlayerTitles
			for(int row = 0; row < wid->PlayerTitles.size(); row++) {
				int scrolledx, scrolledy, ppux, ppuy, previousy, width, upwidth, downwidth, height;
				wid->mediaPanel->GetSize(&width, NULL);
				wid->scrolledPanel->GetViewStart(&scrolledx, &scrolledy);
				wid->scrolledPanel->GetScrollPixelsPerUnit(&ppux, &ppuy);
				wid->MultiUp.at(0)->GetSize(&upwidth, NULL);
				wid->MultiDown.at(0)->GetSize(&downwidth, NULL);
				wid->PlayerTitles.at(row)->GetSize(NULL, &height);
				
				if((width - (upwidth + 3 + downwidth + (ppux * 2))) / 3 >= *widestText) {
					wid->PlayerTitles.at(row)->SetSize((width - (upwidth + 3 + downwidth + (ppux * 2))) / 3, height);
				}
				else {
					wid->PlayerTitles.at(row)->SetSize(*widestText, height);
				}
				
				if(row > 0) {
					wid->PlayerTitles.at(row - 1)->GetPosition(NULL, &previousy);
					wid->PlayerTitles.at(row)->Move(0 - (scrolledx * ppux), (previousy + height));
				}
				else {
					wid->PlayerTitles.at(row)->Move(0 - (scrolledx * ppux), (height * row) - (scrolledy * ppuy));
				}
			}
		}
		{//MoflexFiles
			for(int row = 0; row < wid->MoflexFiles.size(); row++) {
				int scrolledx, scrolledy, ppux, ppuy, previousy, width, upwidth, downwidth, height;
				wid->mediaPanel->GetSize(&width, NULL);
				wid->scrolledPanel->GetViewStart(&scrolledx, &scrolledy);
				wid->scrolledPanel->GetScrollPixelsPerUnit(&ppux, &ppuy);
				wid->MultiUp.at(0)->GetSize(&upwidth, NULL);
				wid->MultiDown.at(0)->GetSize(&downwidth, NULL);
				wid->MoflexFiles.at(row)->GetSize(NULL, &height);
				
				if((width - (upwidth + 3 + downwidth + (ppux * 2))) / 3 >= *widestText) {
					wid->MoflexFiles.at(row)->SetSize((width - (upwidth + 3 + downwidth + (ppux * 2))) / 3, height);
				}
				else {
					wid->MoflexFiles.at(row)->SetSize(*widestText, height);
				}
				
				wid->MoflexFiles.at(row)->GetSize(&width, NULL);
				
				//row->Move((width) - (scrolledx * ppux), (height * currentrow) - (scrolledy * ppuy));
				if(row > 0) {
					wid->MoflexFiles.at(row - 1)->GetPosition(NULL, &previousy);
					wid->MoflexFiles.at(row)->Move((width) - (scrolledx * ppux), (previousy + height));
				}
				else {
					wid->MoflexFiles.at(row)->Move((width) - (scrolledx * ppux), (height * row) - (scrolledy * ppuy));
				}
			}
		}
		{//MenuBanners
			for(int row = 0; row < wid->MenuBanners.size(); row++) {
				int scrolledx, scrolledy, ppux, ppuy, previousy, width, upwidth, downwidth, height;
				wid->mediaPanel->GetSize(&width, NULL);
				wid->scrolledPanel->GetViewStart(&scrolledx, &scrolledy);
				wid->scrolledPanel->GetScrollPixelsPerUnit(&ppux, &ppuy);
				wid->MultiUp.at(0)->GetSize(&upwidth, NULL);
				wid->MultiDown.at(0)->GetSize(&downwidth, NULL);
				wid->MenuBanners.at(row)->GetSize(NULL, &height);
				
				if((width - (upwidth + 3 + downwidth + (ppux * 2))) / 3 >= *widestText) {
					wid->MenuBanners.at(row)->SetSize((width - (upwidth + 3 + downwidth + (ppux * 2))) / 3, height);
				}
				else {
					wid->MenuBanners.at(row)->SetSize(*widestText, height);
				}
				
				wid->MenuBanners.at(row)->GetSize(&width, NULL);
				
				if(row > 0) {
					wid->MenuBanners.at(row - 1)->GetPosition(NULL, &previousy);
					wid->MenuBanners.at(row)->Move((width * 2) - (scrolledx * ppux), (previousy + height));
				}
				else {
					wid->MenuBanners.at(row)->Move((width * 2) - (scrolledx * ppux), (height * row) - (scrolledy * ppuy));
				}
			}
		}
	}
	{//playerTitleText x
		int x, boxx, y, width, mywidth;
		wid->mediaPanel->GetPosition(&x, NULL);
		wid->PlayerTitles.at(0)->GetSize(&width, NULL);
		wid->PlayerTitles.at(0)->GetPosition(&boxx, NULL);
		wid->playerTitleText->GetSize(&mywidth, NULL);
		wid->playerTitleText->GetPosition(NULL, &y);
		
		wid->playerTitleText->Move(x + boxx + ((width - mywidth) / 2), y);
	}
	{//moflexFileText x
		int x, boxx, y, width, mywidth;
		wid->mediaPanel->GetPosition(&x, NULL);
		wid->MoflexFiles.at(0)->GetSize(&width, NULL);
		wid->MoflexFiles.at(0)->GetPosition(&boxx, NULL);
		wid->moflexFileText->GetSize(&mywidth, NULL);
		wid->moflexFileText->GetPosition(NULL, &y);
		
		wid->moflexFileText->Move(x + boxx + ((width - mywidth) / 2), y);
	}
	{//menuBannerText x
		int x, boxx, y, width, mywidth;
		wid->mediaPanel->GetPosition(&x, NULL);
		wid->MenuBanners.at(0)->GetSize(&width, NULL);
		wid->MenuBanners.at(0)->GetPosition(&boxx, NULL);
		wid->menuBannerText->GetSize(&mywidth, NULL);
		wid->menuBannerText->GetPosition(NULL, &y);
		
		wid->menuBannerText->Move(x + boxx + ((width - mywidth) / 2), y);
	}
	{//MultiUp
		int x, y, width, height, myheight;
		for(int i = 0; i < wid->MenuBanners.size(); i++) {
			wid->MenuBanners.at(i)->GetSize(&width, &height);
			wid->MenuBanners.at(i)->GetPosition(&x, &y);
			wid->MultiUp.at(i)->GetSize(NULL, &myheight);
			
			wid->MultiUp.at(i)->Move(x + width, y + ((height - myheight) / 2));
		}
	}
	{//MultiDown
		int x, y, width;
		for(int i = 0; i < wid->MultiUp.size(); i++) {
			wid->MultiUp.at(i)->GetSize(&width, NULL);
			wid->MultiUp.at(i)->GetPosition(&x, &y);
			
			wid->MultiDown.at(i)->Move(x + width + 3, y);
		}
	}
	{//moflexBrowse
		int x, y, width, mywidth, height;
		wid->MoflexFiles.back()->GetSize(&width, &height);
		wid->MoflexFiles.back()->GetPosition(&x, &y);
		wid->moflexBrowse->GetSize(&mywidth, NULL);
		
		wid->moflexBrowse->Move(x + ((width - mywidth) / 2), y + height);
	}
	{//multiBannerBrowse
		int x, y, width, mywidth, height;
		wid->MenuBanners.back()->GetSize(&width, &height);
		wid->MenuBanners.back()->GetPosition(&x, &y);
		wid->multiBannerBrowse->GetSize(&mywidth, NULL);
		
		wid->multiBannerBrowse->Move(x + ((width - mywidth) / 2), y + height);
	}
	{//removeRow
		int y, width, mywidth, appendrowwidth, height;
		wid->multiBannerBrowse->GetSize(NULL, &height);
		wid->multiBannerBrowse->GetPosition(NULL, &y);
		wid->mediaPanel->GetSize(&width, NULL);
		wid->removeRow->GetSize(&mywidth, NULL);
		wid->appendRow->GetSize(&appendrowwidth, NULL);
		
		wid->removeRow->Move(((width - (mywidth + 2 + appendrowwidth)) / 2), y + height + 2);
	}
	{//appendRow
		int x, y, width;
		wid->removeRow->GetSize(&width, NULL);
		wid->removeRow->GetPosition(&x, &y);
		
		wid->appendRow->Move(x + width + 2, y);
	}
	{//splitPatchButton
		int x, y, width, mywidth, appendrowwidth, height;
		wid->removeRow->GetSize(&width, &height);
		wid->removeRow->GetPosition(&x, &y);
		wid->appendRow->GetSize(&appendrowwidth, NULL);
		wid->splitPatchButton->GetSize(&mywidth, NULL);
		
		wid->splitPatchButton->Move(x + (((width + 2 + appendrowwidth) - mywidth) / 2), y + height + 2);
	}
	{//rowText
		int x, y, width, mywidth, height;
		wid->splitPatchButton->GetSize(&width, &height);
		wid->splitPatchButton->GetPosition(&x, &y);
		wid->rowText->GetSize(&mywidth, NULL);
		
		wid->rowText->Move(x + ((width - mywidth) / 2), y + height + 2);
	}
	{//scrolledPanel
		int width, boxwidth, upwidth, height, boxheight, browseheight, removeheight, splitpatchheight, rowtextheight;//todo: splitpatch line
		wid->mediaPanel->GetSize(&width, &height);
		wid->PlayerTitles.at(0)->GetSize(&boxwidth, &boxheight);
		wid->MultiUp.at(0)->GetSize(&upwidth, NULL);//up and down are the same size so just use this (or are they...?)
		wid->moflexBrowse->GetSize(NULL, &browseheight);
		wid->removeRow->GetSize(NULL, &removeheight);//- and + are the same size
		wid->splitPatchButton->GetSize(NULL, &splitpatchheight);
		wid->rowText->GetSize(NULL, &rowtextheight);
		//wid->splitPatchButton->GetSize(NULL, &virtheight);

		wid->scrolledPanel->SetSize(width, height);
		wid->scrolledPanel->SetVirtualSize((boxwidth * 3) + upwidth + 3 + upwidth, boxheight * wid->PlayerTitles.size() + browseheight + 2 + removeheight + 2 + splitpatchheight + 2 + rowtextheight);//size to contain all widgets (dont use coordinates)
	}
}

wxColour BackColor::panel;
wxColour BackColor::mainMenu;
wxColour BackColor::modeText;
wxColour BackColor::modeChoiceBox;
wxColour BackColor::bannerText;
wxColour BackColor::bannerBox;
wxColour BackColor::bannerBrowse;
wxColour BackColor::bannerError;
wxColour BackColor::iconText;
wxColour BackColor::iconBox;
wxColour BackColor::iconBrowse;
wxColour BackColor::iconError;
wxColour BackColor::shortnameText;
wxColour BackColor::shortnameBox;
wxColour BackColor::shortnameError;
wxColour BackColor::longnameText;
wxColour BackColor::longnameBox;
wxColour BackColor::longnameError;
wxColour BackColor::publisherText;
wxColour BackColor::publisherBox;
wxColour BackColor::publisherError;
wxColour BackColor::copyBox;
wxColour BackColor::copyCheck;
wxColour BackColor::bannerPreviewText;
wxColour BackColor::iconPreview;
wxColour BackColor::ffRewindCheck;
wxColour BackColor::dimCheck;
wxColour BackColor::multiBannerPreviewIndex;
wxColour BackColor::multiBannerPreviewLeft;
wxColour BackColor::multiBannerPreviewRight;
wxColour BackColor::playerTitleText;
wxColour BackColor::moflexFileText;
wxColour BackColor::menuBannerText;
wxColour BackColor::mediaPanel;
wxColour BackColor::moflexBrowse;
wxColour BackColor::multiBannerBrowse;
wxColour BackColor::removeRow;
wxColour BackColor::appendRow;
wxColour BackColor::splitPatchButton;
wxColour BackColor::rowText;

wxColour ForeColor::panel;
wxColour ForeColor::mainMenu;
wxColour ForeColor::modeText;
wxColour ForeColor::modeChoiceBox;
wxColour ForeColor::bannerText;
wxColour ForeColor::bannerBox;
wxColour ForeColor::bannerBrowse;
wxColour ForeColor::bannerError;
wxColour ForeColor::iconText;
wxColour ForeColor::iconBox;
wxColour ForeColor::iconBrowse;
wxColour ForeColor::iconError;
wxColour ForeColor::shortnameText;
wxColour ForeColor::shortnameBox;
wxColour ForeColor::shortnameError;
wxColour ForeColor::longnameText;
wxColour ForeColor::longnameBox;
wxColour ForeColor::longnameError;
wxColour ForeColor::publisherText;
wxColour ForeColor::publisherBox;
wxColour ForeColor::publisherError;
wxColour ForeColor::copyBox;
wxColour ForeColor::copyCheck;
wxColour ForeColor::bannerPreviewText;
wxColour ForeColor::iconPreview;
wxColour ForeColor::ffRewindCheck;
wxColour ForeColor::dimCheck;
wxColour ForeColor::multiBannerPreviewIndex;
wxColour ForeColor::multiBannerPreviewLeft;
wxColour ForeColor::multiBannerPreviewRight;
wxColour ForeColor::playerTitleText;
wxColour ForeColor::moflexFileText;
wxColour ForeColor::menuBannerText;
wxColour ForeColor::mediaPanel;
wxColour ForeColor::moflexBrowse;
wxColour ForeColor::multiBannerBrowse;
wxColour ForeColor::removeRow;
wxColour ForeColor::appendRow;
wxColour ForeColor::splitPatchButton;
wxColour ForeColor::rowText;

void getAppearance(InitWidgets* wid) {
	BackColor::panel = wid->panel->GetBackgroundColour();
	BackColor::mainMenu = wid->mainMenu->GetBackgroundColour();
	BackColor::modeText = wid->modeText->GetBackgroundColour();
	BackColor::modeChoiceBox = wid->modeChoiceBox->GetBackgroundColour();
	BackColor::bannerText = wid->bannerText->GetBackgroundColour();
	BackColor::bannerBox = wid->bannerBox->GetBackgroundColour();
	BackColor::bannerBrowse = wid->bannerBrowse->GetBackgroundColour();
	BackColor::bannerError = wid->bannerError->GetBackgroundColour();
	BackColor::iconText = wid->iconText->GetBackgroundColour();
	BackColor::iconBox = wid->iconBox->GetBackgroundColour();
	BackColor::iconBrowse = wid->iconBrowse->GetBackgroundColour();
	BackColor::iconError = wid->iconError->GetBackgroundColour();
	BackColor::shortnameText = wid->shortnameText->GetBackgroundColour();
	BackColor::shortnameBox = wid->shortnameBox->GetBackgroundColour();
	BackColor::shortnameError = wid->shortnameError->GetBackgroundColour();
	BackColor::longnameText = wid->longnameText->GetBackgroundColour();
	BackColor::longnameBox = wid->longnameBox->GetBackgroundColour();
	BackColor::longnameError = wid->longnameError->GetBackgroundColour();
	BackColor::publisherText = wid->publisherText->GetBackgroundColour();
	BackColor::publisherBox = wid->publisherBox->GetBackgroundColour();
	BackColor::publisherError = wid->publisherError->GetBackgroundColour();
	BackColor::copyBox = wid->copyBox->GetBackgroundColour();
	BackColor::copyCheck = wid->copyCheck->GetBackgroundColour();
	BackColor::bannerPreviewText = wid->bannerPreviewText->GetBackgroundColour();
	BackColor::iconPreview = wid->iconPreview->GetBackgroundColour();
	BackColor::ffRewindCheck = wid->ffRewindCheck->GetBackgroundColour();
	BackColor::dimCheck = wid->dimCheck->GetBackgroundColour();
	BackColor::multiBannerPreviewIndex = wid->multiBannerPreviewIndex->GetBackgroundColour();
	BackColor::multiBannerPreviewLeft = wid->multiBannerPreviewLeft->GetBackgroundColour();
	BackColor::multiBannerPreviewRight = wid->multiBannerPreviewRight->GetBackgroundColour();
	BackColor::playerTitleText = wid->playerTitleText->GetBackgroundColour();
	BackColor::moflexFileText = wid->moflexFileText->GetBackgroundColour();
	BackColor::menuBannerText = wid->menuBannerText->GetBackgroundColour();
	BackColor::mediaPanel = wid->mediaPanel->GetBackgroundColour();
	BackColor::moflexBrowse = wid->moflexBrowse->GetBackgroundColour();
	BackColor::multiBannerBrowse = wid->multiBannerBrowse->GetBackgroundColour();
	BackColor::removeRow = wid->removeRow->GetBackgroundColour();
	BackColor::appendRow = wid->appendRow->GetBackgroundColour();
	BackColor::splitPatchButton = wid->splitPatchButton->GetBackgroundColour();
	BackColor::rowText = wid->rowText->GetBackgroundColour();
	
	ForeColor::panel = wid->panel->GetForegroundColour();
	ForeColor::mainMenu = wid->mainMenu->GetForegroundColour();
	ForeColor::modeText = wid->modeText->GetForegroundColour();
	ForeColor::modeChoiceBox = wid->modeChoiceBox->GetForegroundColour();
	ForeColor::bannerText = wid->bannerText->GetForegroundColour();
	ForeColor::bannerBox = wid->bannerBox->GetForegroundColour();
	ForeColor::bannerBrowse = wid->bannerBrowse->GetForegroundColour();
	ForeColor::bannerError = wid->bannerError->GetForegroundColour();
	ForeColor::iconText = wid->iconText->GetForegroundColour();
	ForeColor::iconBox = wid->iconBox->GetForegroundColour();
	ForeColor::iconBrowse = wid->iconBrowse->GetForegroundColour();
	ForeColor::iconError = wid->iconError->GetForegroundColour();
	ForeColor::shortnameText = wid->shortnameText->GetForegroundColour();
	ForeColor::shortnameBox = wid->shortnameBox->GetForegroundColour();
	ForeColor::shortnameError = wid->shortnameError->GetForegroundColour();
	ForeColor::longnameText = wid->longnameText->GetForegroundColour();
	ForeColor::longnameBox = wid->longnameBox->GetForegroundColour();
	ForeColor::longnameError = wid->longnameError->GetForegroundColour();
	ForeColor::publisherText = wid->publisherText->GetForegroundColour();
	ForeColor::publisherBox = wid->publisherBox->GetForegroundColour();
	ForeColor::publisherError = wid->publisherError->GetForegroundColour();
	ForeColor::copyBox = wid->copyBox->GetForegroundColour();
	ForeColor::copyCheck = wid->copyCheck->GetForegroundColour();
	ForeColor::bannerPreviewText = wid->bannerPreviewText->GetForegroundColour();
	ForeColor::iconPreview = wid->iconPreview->GetForegroundColour();
	ForeColor::ffRewindCheck = wid->ffRewindCheck->GetForegroundColour();
	ForeColor::dimCheck = wid->dimCheck->GetForegroundColour();
	ForeColor::multiBannerPreviewIndex = wid->multiBannerPreviewIndex->GetForegroundColour();
	ForeColor::multiBannerPreviewLeft = wid->multiBannerPreviewLeft->GetForegroundColour();
	ForeColor::multiBannerPreviewRight = wid->multiBannerPreviewRight->GetForegroundColour();
	ForeColor::playerTitleText = wid->playerTitleText->GetForegroundColour();
	ForeColor::moflexFileText = wid->moflexFileText->GetForegroundColour();
	ForeColor::menuBannerText = wid->menuBannerText->GetForegroundColour();
	ForeColor::mediaPanel = wid->mediaPanel->GetForegroundColour();
	ForeColor::moflexBrowse = wid->moflexBrowse->GetForegroundColour();
	ForeColor::multiBannerBrowse = wid->multiBannerBrowse->GetForegroundColour();
	ForeColor::removeRow = wid->removeRow->GetForegroundColour();
	ForeColor::appendRow = wid->appendRow->GetForegroundColour();
	ForeColor::splitPatchButton = wid->splitPatchButton->GetForegroundColour();
	ForeColor::rowText = wid->rowText->GetForegroundColour();
}

void setAppearance(InitWidgets* wid, int Mode) {
	if(Mode < 2) {
		wid->panel->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->mainMenu->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->modeText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->modeChoiceBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->bannerText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->bannerBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->bannerBrowse->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->bannerError->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->iconText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->iconBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->iconBrowse->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->iconError->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->shortnameText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->shortnameBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->shortnameError->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->longnameText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->longnameBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->longnameError->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->publisherText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->publisherBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->publisherError->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->copyBox->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->copyCheck->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->bannerPreviewText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->iconPreview->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->ffRewindCheck->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->dimCheck->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->multiBannerPreviewIndex->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->multiBannerPreviewLeft->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->multiBannerPreviewRight->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->playerTitleText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->moflexFileText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->menuBannerText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->mediaPanel->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->moflexBrowse->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->multiBannerBrowse->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->removeRow->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->appendRow->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->splitPatchButton->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		wid->rowText->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
		
		wid->panel->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->mainMenu->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->modeText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->modeChoiceBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->bannerText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->bannerBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->bannerBrowse->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->bannerError->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->iconText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->iconBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->iconBrowse->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->iconError->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->shortnameText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->shortnameBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->shortnameError->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->longnameText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->longnameBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->longnameError->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->publisherText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->publisherBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->publisherError->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->copyBox->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->copyCheck->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->bannerPreviewText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->iconPreview->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->ffRewindCheck->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->dimCheck->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->multiBannerPreviewIndex->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->multiBannerPreviewLeft->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->multiBannerPreviewRight->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->playerTitleText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->moflexFileText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->menuBannerText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->mediaPanel->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->moflexBrowse->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->multiBannerBrowse->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->removeRow->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->appendRow->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->splitPatchButton->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		wid->rowText->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		
		{
			wxColor LightBlack = wxBLACK->GetRGB() + 0x141414;
			wxColor DarkWhite = wxWHITE->GetRGB() - 0x141414;
			
			int rows = 0;
			for(auto &row : wid->PlayerTitles) {
				if(rows % 2) {
					row->SetBackgroundColour(Mode ? LightBlack : DarkWhite);
				}
				else {
					row->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
				}
				row->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
				++rows;
			}
			rows = 0;
			for(auto &row : wid->MoflexFiles) {
				if(rows % 2) {
					row->SetBackgroundColour(Mode ? LightBlack : DarkWhite);
				}
				else {
					row->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
				}
				row->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
				++rows;
			}
			rows = 0;
			for(auto &row : wid->MenuBanners) {
				if(rows % 2) {
					row->SetBackgroundColour(Mode ? LightBlack : DarkWhite);
				}
				else {
					row->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
				}
				row->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
				++rows;
			}
		}
		
		for(auto &row : wid->MultiUp) {
			row->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
			row->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		}
		for(auto &row : wid->MultiDown) {
			row->SetBackgroundColour(*(Mode ? wxBLACK : wxWHITE));
			row->SetForegroundColour(*(Mode ? wxWHITE : wxBLACK));
		}
	}
	else if(Mode == 2) {
		wid->panel->SetBackgroundColour(BackColor::panel);
		wid->mainMenu->SetBackgroundColour(BackColor::mainMenu);
		wid->modeText->SetBackgroundColour(BackColor::modeText);
		wid->modeChoiceBox->SetBackgroundColour(BackColor::modeChoiceBox);
		wid->bannerText->SetBackgroundColour(BackColor::bannerText);
		wid->bannerBox->SetBackgroundColour(BackColor::bannerBox);
		wid->bannerBrowse->SetBackgroundColour(BackColor::bannerBrowse);
		wid->bannerError->SetBackgroundColour(BackColor::bannerError);
		wid->iconText->SetBackgroundColour(BackColor::iconText);
		wid->iconBox->SetBackgroundColour(BackColor::iconBox);
		wid->iconBrowse->SetBackgroundColour(BackColor::iconBrowse);
		wid->iconError->SetBackgroundColour(BackColor::iconError);
		wid->shortnameText->SetBackgroundColour(BackColor::shortnameText);
		wid->shortnameBox->SetBackgroundColour(BackColor::shortnameBox);
		wid->shortnameError->SetBackgroundColour(BackColor::shortnameError);
		wid->longnameText->SetBackgroundColour(BackColor::longnameText);
		wid->longnameBox->SetBackgroundColour(BackColor::longnameBox);
		wid->longnameError->SetBackgroundColour(BackColor::longnameError);
		wid->publisherText->SetBackgroundColour(BackColor::publisherText);
		wid->publisherBox->SetBackgroundColour(BackColor::publisherBox);
		wid->publisherError->SetBackgroundColour(BackColor::publisherError);
		wid->copyBox->SetBackgroundColour(BackColor::copyBox);
		wid->copyCheck->SetBackgroundColour(BackColor::copyCheck);
		wid->bannerPreviewText->SetBackgroundColour(BackColor::bannerPreviewText);
		wid->iconPreview->SetBackgroundColour(BackColor::iconPreview);
		wid->ffRewindCheck->SetBackgroundColour(BackColor::ffRewindCheck);
		wid->dimCheck->SetBackgroundColour(BackColor::dimCheck);
		wid->multiBannerPreviewIndex->SetBackgroundColour(BackColor::multiBannerPreviewIndex);
		wid->multiBannerPreviewLeft->SetBackgroundColour(BackColor::multiBannerPreviewLeft);
		wid->multiBannerPreviewRight->SetBackgroundColour(BackColor::multiBannerPreviewRight);
		wid->playerTitleText->SetBackgroundColour(BackColor::playerTitleText);
		wid->moflexFileText->SetBackgroundColour(BackColor::moflexFileText);
		wid->menuBannerText->SetBackgroundColour(BackColor::menuBannerText);
		wid->mediaPanel->SetBackgroundColour(BackColor::mediaPanel);
		wid->moflexBrowse->SetBackgroundColour(BackColor::moflexBrowse);
		wid->multiBannerBrowse->SetBackgroundColour(BackColor::multiBannerBrowse);
		wid->removeRow->SetBackgroundColour(BackColor::removeRow);
		wid->appendRow->SetBackgroundColour(BackColor::appendRow);
		wid->splitPatchButton->SetBackgroundColour(BackColor::splitPatchButton);
		wid->rowText->SetBackgroundColour(BackColor::rowText);
		
		wid->panel->SetForegroundColour(ForeColor::panel);
		wid->mainMenu->SetForegroundColour(ForeColor::mainMenu);
		wid->modeText->SetForegroundColour(ForeColor::modeText);
		wid->modeChoiceBox->SetForegroundColour(ForeColor::modeChoiceBox);
		wid->bannerText->SetForegroundColour(ForeColor::bannerText);
		wid->bannerBox->SetForegroundColour(ForeColor::bannerBox);
		wid->bannerBrowse->SetForegroundColour(ForeColor::bannerBrowse);
		wid->bannerError->SetForegroundColour(ForeColor::bannerError);
		wid->iconText->SetForegroundColour(ForeColor::iconText);
		wid->iconBox->SetForegroundColour(ForeColor::iconBox);
		wid->iconBrowse->SetForegroundColour(ForeColor::iconBrowse);
		wid->iconError->SetForegroundColour(ForeColor::iconError);
		wid->shortnameText->SetForegroundColour(ForeColor::shortnameText);
		wid->shortnameBox->SetForegroundColour(ForeColor::shortnameBox);
		wid->shortnameError->SetForegroundColour(ForeColor::shortnameError);
		wid->longnameText->SetForegroundColour(ForeColor::longnameText);
		wid->longnameBox->SetForegroundColour(ForeColor::longnameBox);
		wid->longnameError->SetForegroundColour(ForeColor::longnameError);
		wid->publisherText->SetForegroundColour(ForeColor::publisherText);
		wid->publisherBox->SetForegroundColour(ForeColor::publisherBox);
		wid->publisherError->SetForegroundColour(ForeColor::publisherError);
		wid->copyBox->SetForegroundColour(ForeColor::copyBox);
		wid->copyCheck->SetForegroundColour(ForeColor::copyCheck);
		wid->bannerPreviewText->SetForegroundColour(ForeColor::bannerPreviewText);
		wid->iconPreview->SetForegroundColour(ForeColor::iconPreview);
		wid->ffRewindCheck->SetForegroundColour(ForeColor::ffRewindCheck);
		wid->dimCheck->SetForegroundColour(ForeColor::dimCheck);
		wid->multiBannerPreviewIndex->SetForegroundColour(ForeColor::multiBannerPreviewIndex);
		wid->multiBannerPreviewLeft->SetForegroundColour(ForeColor::multiBannerPreviewLeft);
		wid->multiBannerPreviewRight->SetForegroundColour(ForeColor::multiBannerPreviewRight);
		wid->playerTitleText->SetForegroundColour(ForeColor::playerTitleText);
		wid->moflexFileText->SetForegroundColour(ForeColor::moflexFileText);
		wid->menuBannerText->SetForegroundColour(ForeColor::menuBannerText);
		wid->mediaPanel->SetForegroundColour(ForeColor::mediaPanel);
		wid->moflexBrowse->SetForegroundColour(ForeColor::moflexBrowse);
		wid->multiBannerBrowse->SetForegroundColour(ForeColor::multiBannerBrowse);
		wid->removeRow->SetForegroundColour(ForeColor::removeRow);
		wid->appendRow->SetForegroundColour(ForeColor::appendRow);
		wid->splitPatchButton->SetForegroundColour(ForeColor::splitPatchButton);
		wid->rowText->SetForegroundColour(ForeColor::rowText);
		
		{
			wxColor BackOdd = (BackColor::bannerBox.GetRGB() < 0x7F7F7F) ? (BackColor::bannerBox.GetRGB() + 0x141414) : (BackColor::bannerBox.GetRGB() - 0x141414);//FF/2=7F
			//wxColor ForeOdd = (ForeColor::bannerBox.GetRGB() < 0x7F7F7F) ? (ForeColor::bannerBox.GetRGB() + 0x141414) : (ForeColor::bannerBox.GetRGB() - 0x141414);
			
			int rows = 0;
			for(auto &row : wid->PlayerTitles) {
				if(rows % 2) {
					row->SetBackgroundColour(BackOdd);
				}
				else {
					row->SetBackgroundColour(BackColor::bannerBox);//not worth getting the color from these actual boxes
				}
				row->SetForegroundColour(ForeColor::bannerBox);
				++rows;
			}
			rows = 0;
			for(auto &row : wid->MoflexFiles) {
				if(rows % 2) {
					row->SetBackgroundColour(BackOdd);
				}
				else {
					row->SetBackgroundColour(BackColor::bannerBox);//not worth getting the color from these actual boxes
				}
				row->SetForegroundColour(ForeColor::bannerBox);
				++rows;
			}
			rows = 0;
			for(auto &row : wid->MenuBanners) {
				if(rows % 2) {
					row->SetBackgroundColour(BackOdd);
				}
				else {
					row->SetBackgroundColour(BackColor::bannerBox);//not worth getting the color from these actual boxes
				}
				row->SetForegroundColour(ForeColor::bannerBox);
				++rows;
			}
		}
		
		for(auto &row : wid->MultiUp) {
			row->SetBackgroundColour(BackColor::bannerBrowse);
			row->SetForegroundColour(ForeColor::bannerBrowse);
		}
		for(auto &row : wid->MultiDown) {
			row->SetBackgroundColour(BackColor::bannerBrowse);
			row->SetForegroundColour(ForeColor::bannerBrowse);
		}
	}
	wid->panel->Refresh();
}
