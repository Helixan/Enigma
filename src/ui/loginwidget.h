#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>

class QLineEdit;
class QPushButton;
class QGroupBox;
class QVBoxLayout;
class QHBoxLayout;

class User;

class LoginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

    signals:
        void loginSuccessful(User* user, const QString& password);
    void registerSuccessful();

    private slots:
        void handleLogin();
    void handleRegister();

private:
    void setupUI();

    QGroupBox*  loginGroupBox;
    QLineEdit*  usernameLineEdit;
    QLineEdit*  passwordLineEdit;
    QPushButton* loginButton;
    QPushButton* registerButton;
};

#endif // LOGINWIDGET_H
