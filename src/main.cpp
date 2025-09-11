#include <QApplication>
#include "ui/board/main_window.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("Shogi Game");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ShogiDev");
    app.setOrganizationDomain("shogidev.com");
    
    // Create and show main window
    shogi::ui::MainWindow window;
    window.show();
    
    return app.exec();
}