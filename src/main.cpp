#include <QApplication>
#include "core/dbmanager.h"
#include "ui/logindialog.h"
#include "ui/mainwindow.h"
#include "models/user.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    const QString globalStyle = R"(
        QMainWindow {
            background-color: #1E1E2E;
        }
        QWidget {
            background-color: #1E1E2E;
            color: #FFFFFF;
        }
        QPushButton {
            background-color: #2E2E3E;
            color: #FFFFFF;
            border: none;
            padding: 10px;
            text-align: left;
        }
        QPushButton:hover {
            background-color: #3E3E4E;
        }
        QPushButton:pressed {
            background-color: #4E4E5E;
        }
        QLineEdit {
            background-color: #2E2E3E;
            color: #FFFFFF;
            border: 1px solid #444444;
            padding: 5px;
        }
        QGroupBox {
            border: 1px solid #444444;
            margin-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            color: #FFFFFF;
        }
        QTableWidget {
            background-color: #2E2E3E;
            color: #FFFFFF;
            border: 1px solid #444444;
        }
        QHeaderView::section {
            background-color: #3E3E4E;
            color: #FFFFFF;
            padding: 4px;
            border: 1px solid #444444;
        }
        QCheckBox {
            color: #FFFFFF;
        }
        QSlider::groove:horizontal {
            border: 1px solid #444444;
            height: 8px;
            background: #3E3E4E;
            margin: 2px 0;
            border-radius: 4px;
        }
        QSlider::handle:horizontal {
            background: #5E5E6E;
            border: 1px solid #444444;
            width: 18px;
            margin: -5px 0;
            border-radius: 3px;
        }
        QSpinBox {
            background-color: #2E2E3E;
            color: #FFFFFF;
            border: 1px solid #444444;
            padding: 5px;
        }
    )";
    a.setStyleSheet(globalStyle);

    const QString host = "localhost";
    const QString dbName = "new_password_manager";
    const QString dbUser = "password_manager";

    if (const QString dbPassword = "password"; !DBManager::instance().
        openConnection(host, dbName, dbUser, dbPassword)) {
        return -1;
    }

    LoginDialog loginDialog;

    if (const int result = loginDialog.exec(); result != QDialog::Accepted) {
        return 0;
    }

    User *user = loginDialog.getLoggedInUser();
    const QString passwordUsed = loginDialog.getLoggedInPassword();

    if (!user) {
        return 0;
    }

    MainWindow w;
    w.setCurrentUser(user, passwordUsed);
    w.show();

    return a.exec();
}
