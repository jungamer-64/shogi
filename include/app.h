#pragma once

#include <wx/wx.h>
#include "common.h"

class MainFrame;

class ShogiApp : public wxApp {
public:
    bool OnInit() override;
    
private:
    MainFrame* m_main_frame{nullptr};
};

wxDECLARE_APP(ShogiApp);