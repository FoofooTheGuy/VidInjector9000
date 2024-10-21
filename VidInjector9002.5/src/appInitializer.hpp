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
