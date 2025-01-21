#include "logindialog.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

#include "models/user.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , loggedInUser(nullptr)
{
    setupUI();
    setModal(true);
    setWindowTitle("Login / Register");
}

LoginDialog::~LoginDialog()
{
}

void LoginDialog::setupUI()
{
    setStyleSheet(R"(
        QDialog {
            background-color: #1E1E2E;
        }
        QLabel {
            color: #FFFFFF;
        }
        QLineEdit {
            background-color: #2E2E3E;
            color: #FFFFFF;
            border: 1px solid #444444;
            padding: 5px;
        }
        QPushButton {
            background-color: #2E2E3E;
            color: #FFFFFF;
            border: none;
            padding: 8px;
            margin: 4px;
        }
        QPushButton:hover {
            background-color: #3E3E4E;
        }
        QPushButton:pressed {
            background-color: #4E4E5E;
        }
    )");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    messageLabel = new QLabel("Enter your username and password.");
    mainLayout->addWidget(messageLabel);

    QHBoxLayout* formLayout = new QHBoxLayout();
    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("Username");

    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setEchoMode(QLineEdit::Password);

    formLayout->addWidget(usernameEdit);
    formLayout->addWidget(passwordEdit);
    mainLayout->addLayout(formLayout);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    loginButton = new QPushButton("Login", this);
    registerButton = new QPushButton("Register", this);
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(registerButton);
    mainLayout->addLayout(buttonLayout);

    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &LoginDialog::onRegisterClicked);
}

void LoginDialog::onLoginClicked()
{
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Username and password cannot be empty.");
        return;
    }

    User* user = User::login(username, password);
    if (user) {
        loggedInUser = user;
        loggedInPassword = password;
        QMessageBox::information(this, "Success", "Logged in successfully.");
        accept();
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
    }
}

void LoginDialog::onRegisterClicked()
{
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Username and password cannot be empty.");
        return;
    }

    if (User::registerUser(username, password)) {
        QMessageBox::information(this, "Success", "Registered successfully. You can now log in.");
    } else {
        QMessageBox::warning(this, "Error", "Registration failed. Username might already exist.");
    }
}

User* LoginDialog::getLoggedInUser() const
{
    return loggedInUser;
}

QString LoginDialog::getLoggedInPassword() const
{
    return loggedInPassword;
}
