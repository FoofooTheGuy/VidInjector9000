/*
  Copyright (c) 2019 [Gammasoft](https://gammasoft71.wixsite.com/gammasoft).
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#pragma once
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/sysopt.h>

//stolen from https://github.com/gammasoft71/Examples_wxWidgets/blob/adbd395081bf25c9034f2b64eee62608a943441f/src/Application/ApplicationAndMain2/AppInitializer.h
struct AppInitializer {
  AppInitializer() : AppInitializer(nullptr, true) {}
  AppInitializer(wxApp* app) : AppInitializer(app, true) {}
  AppInitializer(bool exit_on_last_frame_closed) : AppInitializer(nullptr, exit_on_last_frame_closed) {}
  AppInitializer(wxApp* app, bool exit_on_last_frame_closed) {
    wxDISABLE_DEBUG_SUPPORT();
    wxSystemOptions::SetOption("osx.openfiledialog.always-show-types", 1);
    wxApp::SetInstance(app ? app : new wxApp());
    int argc = 0;
    wxEntryStart(argc, (wxChar**)NULL);
    wxTheApp->CallOnInit();
    wxTheApp->SetExitOnFrameDelete(exit_on_last_frame_closed);
    wxInitAllImageHandlers();
#if __WXOSX__
    wxMenuBar* menubar = new wxMenuBar();
    menubar->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {
      if (event.GetId() == wxID_EXIT) {
        auto can_quit = true;
        for (auto window : wxTopLevelWindows)
          if (!(can_quit = window->Close())) break;
        if (can_quit) wxTheApp->ExitMainLoop();
      } else event.Skip();
    });
    wxMenuBar::MacSetCommonMenuBar(menubar);
#endif
  }
  
  ~AppInitializer() {
    if (!wxTheApp) return;
    wxImage::CleanUpHandlers();
    wxTheApp->OnExit();
    wxEntryCleanup();
    delete wxTheApp;
    wxApp::SetInstance(nullptr);
  }
};

