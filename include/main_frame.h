#pragma once

#include <wx/wx.h>
#include <memory>
#include "common.h"

class BoardPanel;
class GameLogic;

class MainFrame : public wxFrame {
public:
    MainFrame();
    
private:
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnNewGame(wxCommandEvent& event);
    void CreateMenuBar();
    void CreateStatusBar();
    
    // UI components
    BoardPanel* m_board_panel{nullptr};
    
    // Game state
    std::unique_ptr<GameLogic> m_game_logic;
    
    // Event IDs
    enum {
        ID_NEW_GAME = 1000
    };
    
    wxDECLARE_EVENT_TABLE();
};