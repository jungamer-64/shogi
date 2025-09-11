#include "app.h"
#include "main_frame.h"

wxIMPLEMENT_APP_NO_MAIN(ShogiApp);

bool ShogiApp::OnInit() {
    // Create and show main frame
    m_main_frame = new MainFrame();
    m_main_frame->Show(true);
    
    return true;
}