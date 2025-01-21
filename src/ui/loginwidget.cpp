#include "loginwidget.h"
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDebug>

#include "models/user.h"

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent) {
    setupUI();
}

LoginWidget::~LoginWidget() {
}

void LoginWidget::setupUI() {
    const auto mainLayout = new QVBoxLayout(this);

    loginGroupBox = new QGroupBox("Login / Register", this);
    const auto groupLayout = new QHBoxLayout(loginGroupBox);

    usernameLineEdit = new QLineEdit(loginGroupBox);
    usernameLineEdit->setPlaceholderText("Username");
    groupLayout->addWidget(usernameLineEdit);

    passwordLineEdit = new QLineEdit(loginGroupBox);
    passwordLineEdit->setPlaceholderText("Password");
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    groupLayout->addWidget(passwordLineEdit);

    loginButton = new QPushButton("Login", loginGroupBox);
    groupLayout->addWidget(loginButton);

    registerButton = new QPushButton("Register", loginGroupBox);
    groupLayout->addWidget(registerButton);

    loginGroupBox->setLayout(groupLayout);

    mainLayout->addWidget(loginGroupBox);
    mainLayout->addStretch();

    connect(loginButton, &QPushButton::clicked, this, &LoginWidget::handleLogin);
    connect(registerButton, &QPushButton::clicked, this, &LoginWidget::handleRegister);
}

void LoginWidget::handleLogin() {
    const QString username = usernameLineEdit->text().trimmed();
    const QString password = passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Username and Password cannot be empty.");
        return;
    }

    if (User *user = User::login(username, password)) {
        QMessageBox::information(this, "Success", "Logged in successfully.");
        emit loginSuccessful(user, password);
    } else {
        QMessageBox::warning(this, "Error", "Login failed. Check your credentials.");
    }
}

void LoginWidget::handleRegister() {
    const QString username = usernameLineEdit->text().trimmed();
    const QString password = passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Username and Password cannot be empty.");
        return;
    }

    if (User::registerUser(username, password)) {
        QMessageBox::information(this, "Success", "User registered successfully.");
        emit registerSuccessful();
    } else {
        QMessageBox::warning(this, "Error", "Registration failed. Username might already exist.");
    }
}
