#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

class QLineEdit;
class QPushButton;
class QLabel;
class User;

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    User* getLoggedInUser() const;

    QString getLoggedInPassword() const;

    private slots:
        void onLoginClicked();
    void onRegisterClicked();

private:
    void setupUI();

    QLineEdit* usernameEdit;
    QLineEdit* passwordEdit;
    QPushButton* loginButton;
    QPushButton* registerButton;
    QLabel* messageLabel;

    User* loggedInUser;
    QString loggedInPassword;
};

#endif // LOGINDIALOG_H
