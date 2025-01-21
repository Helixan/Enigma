#ifndef PASSWORDMANAGERWIDGET_H
#define PASSWORDMANAGERWIDGET_H

#include <QWidget>
#include <QList>

class QLineEdit;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QTimer;
class QLabel;
class QScrollArea;
class QPlainTextEdit;

class PasswordManager;
struct PasswordEntry;

class PasswordManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PasswordManagerWidget(QWidget *parent = nullptr);
    ~PasswordManagerWidget();

    void setPasswordManager(PasswordManager* pm);
    void loadPasswords();

private slots:
    void onAddClicked();
    void onSaveClicked();
    void onDeleteClicked();

    void onEntryClicked(int id);

    void updateTOTPDisplay();

    void copyService();
    void copyUrl();
    void copyUsername();
    void copyEmail();
    void copyPassword();
    void copyTotpSecret();
    void copyTotpCode();

private:
    void setupUI();
    void clearDetailFields();
    void populateDetailFields(const PasswordEntry &entry);
    PasswordEntry gatherDetailFields() const;
    int  currentSelectedId() const;

    QWidget*    leftPanel;
    QScrollArea* scrollArea;
    QVBoxLayout* scrollAreaLayout;

    QPushButton* addButton;
    QPushButton* deleteButton;

    QLineEdit*       serviceEdit;
    QPushButton*     copyServiceButton;

    QLineEdit*       urlEdit;
    QPushButton*     copyUrlButton;

    QLineEdit*       usernameEdit;
    QPushButton*     copyUsernameButton;

    QLineEdit*       emailEdit;
    QPushButton*     copyEmailButton;

    QLineEdit*       passwordEdit;
    QPushButton*     copyPasswordButton;

    QPlainTextEdit*  descriptionEdit;

    QLineEdit*       totpSecretEdit;
    QPushButton*     copyTotpSecretButton;

    QLineEdit*       totpCodeEdit;
    QPushButton*     copyTotpCodeButton;

    QLabel*          totpTimeLabel;

    QPushButton*     saveButton;

    PasswordManager* passwordManager;
    bool             isAddingNew;
    int              selectedEntryId;

    QTimer* totpTimer;

    QList<PasswordEntry> cachedEntries;
};

#endif // PASSWORDMANAGERWIDGET_H
