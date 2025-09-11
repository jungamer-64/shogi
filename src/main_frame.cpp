#include "main_frame.h"
#include "board_panel.h"
#include "game_logic.h"
#include <wx/msgdlg.h>

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MainFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
    EVT_MENU(ID_NEW_GAME, MainFrame::OnNewGame)
wxEND_EVENT_TABLE()

MainFrame::MainFrame() 
    : wxFrame(nullptr, wxID_ANY, "Shogi Game", wxDefaultPosition, wxSize(800, 900))
{
    // Create game logic
    m_game_logic = std::make_unique<GameLogic>();
    
    // Create menu bar
    CreateMenuBar();
    
    // Create status bar
    CreateStatusBar();
    SetStatusText("Welcome to Shogi!");
    
    // Create main panel
    wxPanel* main_panel = new wxPanel(this);
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Create board panel
    m_board_panel = new BoardPanel(main_panel, m_game_logic.get());
    main_sizer->Add(m_board_panel, 1, wxEXPAND | wxALL, 5);
    
    main_panel->SetSizer(main_sizer);
    
    // Center the frame
    Center();
}

void MainFrame::CreateMenuBar() {
    wxMenuBar* menu_bar = new wxMenuBar;
    
    // Game menu
    wxMenu* game_menu = new wxMenu;
    game_menu->Append(ID_NEW_GAME, "&New Game\tCtrl+N", "Start a new game");
    game_menu->AppendSeparator();
    game_menu->Append(wxID_EXIT, "E&xit\tAlt+X", "Quit this program");
    
    // Help menu
    wxMenu* help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT, "&About\tF1", "Show about dialog");
    
    menu_bar->Append(game_menu, "&Game");
    menu_bar->Append(help_menu, "&Help");
    
    SetMenuBar(menu_bar);
}

void MainFrame::CreateStatusBar() {
    wxFrame::CreateStatusBar(2);
    SetStatusText("Ready", 0);
    SetStatusText("First Player Turn", 1);
}

void MainFrame::OnExit(wxCommandEvent& WXUNUSED(event)) {
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
    wxMessageBox(
        std::format("Shogi Game v1.0\n\nBuilt with C++23 and wxWidgets\n\n"
                   "A full-featured Japanese Chess game with:\n"
                   "• Local multiplayer\n"
                   "• AI opponents (USI)\n"
                   "• Online play\n"
                   "• Game record support").c_str(),
        "About Shogi Game",
        wxOK | wxICON_INFORMATION,
        this
    );
}

void MainFrame::OnNewGame(wxCommandEvent& WXUNUSED(event)) {
    if (m_game_logic) {
        m_game_logic->reset_game();
        if (m_board_panel) {
            m_board_panel->Refresh();
        }
        SetStatusText("New game started", 0);
        SetStatusText("First Player Turn", 1);
    }
}