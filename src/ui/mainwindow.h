#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QPushButton;
class QStackedWidget;
class User;
class Encryption;
class PasswordManager;
class NoteManager;

class PasswordManagerWidget;
class PasswordGeneratorWidget;
class NotepadWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setCurrentUser(User* user, const QString& password);

    private slots:
        void switchFeature();

private:
    void setupUI();

    User* currentUser;
    Encryption* encryption;
    PasswordManager* passwordManager;
    NoteManager* noteManager;

    QWidget* centralWidget;
    QWidget* sidebar;
    QStackedWidget* stackedWidget;

    QPushButton* passwordManagerButton;
    QPushButton* passwordGeneratorButton;
    QPushButton* notepadButton;

    PasswordManagerWidget* passwordManagerWidget;
    PasswordGeneratorWidget* passwordGeneratorWidget;
    NotepadWidget* notepadWidget;
};

#endif // MAINWINDOW_H
