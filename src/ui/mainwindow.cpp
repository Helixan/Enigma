#include "mainwindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QStackedWidget>

#include "models/user.h"
#include "core/encryption.h"
#include "models/passwordmanager.h"
#include "models/notemanager.h"
#include "ui/passwordmanagerwidget.h"
#include "ui/passwordgeneratorwidget.h"
#include "ui/notepadwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , currentUser(nullptr)
      , encryption(nullptr)
      , passwordManager(nullptr)
      , noteManager(nullptr) {
    setupUI();
}

MainWindow::~MainWindow() {
    delete currentUser;
    delete encryption;
    delete passwordManager;
    delete noteManager;
}

void MainWindow::setupUI() {
    setWindowTitle("Enigma");
    resize(1024, 768);

    setStyleSheet(R"(
        QMainWindow {
            background-color: #1E1E2E;
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
        QPlainTextEdit {
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
        QScrollArea {
            border: 1px solid #444444;
        }
    )");

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    const auto mainLayout = new QHBoxLayout(centralWidget);

    sidebar = new QWidget(centralWidget);
    const auto sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);
    sidebar->setFixedWidth(200);
    sidebar->setStyleSheet("background-color: #2E2E3E;");

    passwordManagerButton = new QPushButton("Password Manager", sidebar);
    passwordManagerButton->setCheckable(true);
    sidebarLayout->addWidget(passwordManagerButton);

    passwordGeneratorButton = new QPushButton("Password Generator", sidebar);
    passwordGeneratorButton->setCheckable(true);
    sidebarLayout->addWidget(passwordGeneratorButton);

    notepadButton = new QPushButton("Notepad", sidebar);
    notepadButton->setCheckable(true);
    sidebarLayout->addWidget(notepadButton);

    sidebarLayout->addStretch();
    mainLayout->addWidget(sidebar);

    stackedWidget = new QStackedWidget(centralWidget);
    mainLayout->addWidget(stackedWidget, 1);

    passwordManagerWidget = new PasswordManagerWidget(this);
    passwordGeneratorWidget = new PasswordGeneratorWidget(this);
    notepadWidget = new NotepadWidget(this); // NEW

    stackedWidget->addWidget(passwordManagerWidget); // index 0
    stackedWidget->addWidget(passwordGeneratorWidget); // index 1
    stackedWidget->addWidget(notepadWidget); // index 2

    stackedWidget->setCurrentIndex(0);
    passwordManagerButton->setChecked(true);

    connect(passwordManagerButton, &QPushButton::clicked, this, &MainWindow::switchFeature);
    connect(passwordGeneratorButton, &QPushButton::clicked, this, &MainWindow::switchFeature);
    connect(notepadButton, &QPushButton::clicked, this, &MainWindow::switchFeature);
}

void MainWindow::setCurrentUser(User *user, const QString &password) {
    delete currentUser;
    delete encryption;
    delete passwordManager;
    delete noteManager;

    currentUser = user;
    encryption = new Encryption(password.toUtf8());
    passwordManager = new PasswordManager(currentUser->getId(), encryption);
    noteManager = new NoteManager(currentUser->getId(), encryption); // NEW

    passwordManagerWidget->setPasswordManager(passwordManager);
    passwordManagerWidget->loadPasswords();

    notepadWidget->setNoteManager(noteManager);
    notepadWidget->loadNotes();
}

void MainWindow::switchFeature() const {
    const auto senderButton = qobject_cast<QPushButton *>(sender());
    if (!senderButton) {
        return;
    }

    passwordManagerButton->setChecked(false);
    passwordGeneratorButton->setChecked(false);
    notepadButton->setChecked(false);

    senderButton->setChecked(true);

    if (senderButton == passwordManagerButton) {
        stackedWidget->setCurrentWidget(passwordManagerWidget);
    } else if (senderButton == passwordGeneratorButton) {
        stackedWidget->setCurrentWidget(passwordGeneratorWidget);
    } else if (senderButton == notepadButton) {
        stackedWidget->setCurrentWidget(notepadWidget);
    }
}
