#include "passwordmanagerwidget.h"

#include <cmath>
#include <QPushButton>
#include <QMessageBox>
#include <QClipboard>
#include <QApplication>
#include <QTimer>
#include <QLabel>
#include <QGroupBox>
#include <QDateTime>
#include <QDebug>
#include <QLineEdit>
#include <QScrollArea>
#include <QPlainTextEdit>
#include <QHBoxLayout>

#include "models/passwordmanager.h"
#include "core/totpgenerator.h"

PasswordManagerWidget::PasswordManagerWidget(QWidget *parent)
    : QWidget(parent)
    , passwordManager(nullptr)
    , isAddingNew(false)
    , selectedEntryId(-1)
{
    setupUI();

    totpTimer = new QTimer(this);
    connect(totpTimer, &QTimer::timeout, this, &PasswordManagerWidget::updateTOTPDisplay);
    totpTimer->start(100);
}

PasswordManagerWidget::~PasswordManagerWidget()
{
}

void PasswordManagerWidget::setupUI()
{
    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    leftPanel = new QWidget(this);
    QVBoxLayout* leftPanelLayout = new QVBoxLayout(leftPanel);

    addButton = new QPushButton("Add", this);
    deleteButton = new QPushButton("Delete", this);

    {
        QHBoxLayout* topRowLayout = new QHBoxLayout();
        topRowLayout->addWidget(addButton);
        topRowLayout->addWidget(deleteButton);
        leftPanelLayout->addLayout(topRowLayout);
    }

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    QWidget* scrollContainer = new QWidget(this);
    scrollAreaLayout = new QVBoxLayout(scrollContainer);
    scrollContainer->setLayout(scrollAreaLayout);

    scrollArea->setWidget(scrollContainer);

    leftPanelLayout->addWidget(scrollArea);
    leftPanel->setLayout(leftPanelLayout);

    mainLayout->addWidget(leftPanel, 1);

    QWidget* rightPanel = new QWidget(this);
    QVBoxLayout* rightPanelLayout = new QVBoxLayout(rightPanel);

    QGroupBox* detailGroup = new QGroupBox("Details", rightPanel);
    QVBoxLayout* detailLayout = new QVBoxLayout(detailGroup);

    {
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* lbl = new QLabel("Service:");
        serviceEdit = new QLineEdit();
        copyServiceButton = new QPushButton("Copy");
        row->addWidget(lbl);
        row->addWidget(serviceEdit);
        row->addWidget(copyServiceButton);
        detailLayout->addLayout(row);

        connect(copyServiceButton, &QPushButton::clicked, this, &PasswordManagerWidget::copyService);
    }

    {
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* lbl = new QLabel("URL:");
        urlEdit = new QLineEdit();
        copyUrlButton = new QPushButton("Copy");
        row->addWidget(lbl);
        row->addWidget(urlEdit);
        row->addWidget(copyUrlButton);
        detailLayout->addLayout(row);

        connect(copyUrlButton, &QPushButton::clicked, this, &PasswordManagerWidget::copyUrl);
    }

    {
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* lbl = new QLabel("Username:");
        usernameEdit = new QLineEdit();
        copyUsernameButton = new QPushButton("Copy");
        row->addWidget(lbl);
        row->addWidget(usernameEdit);
        row->addWidget(copyUsernameButton);
        detailLayout->addLayout(row);

        connect(copyUsernameButton, &QPushButton::clicked, this, &PasswordManagerWidget::copyUsername);
    }

    {
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* lbl = new QLabel("Email:");
        emailEdit = new QLineEdit();
        copyEmailButton = new QPushButton("Copy");
        row->addWidget(lbl);
        row->addWidget(emailEdit);
        row->addWidget(copyEmailButton);
        detailLayout->addLayout(row);

        connect(copyEmailButton, &QPushButton::clicked, this, &PasswordManagerWidget::copyEmail);
    }

    {
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* lbl = new QLabel("Password:");
        passwordEdit = new QLineEdit();
        passwordEdit->setEchoMode(QLineEdit::Password);
        copyPasswordButton = new QPushButton("Copy");
        row->addWidget(lbl);
        row->addWidget(passwordEdit);
        row->addWidget(copyPasswordButton);
        detailLayout->addLayout(row);

        connect(copyPasswordButton, &QPushButton::clicked, this, &PasswordManagerWidget::copyPassword);
    }

    {
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* lbl = new QLabel("TOTP Secret:");
        totpSecretEdit = new QLineEdit();
        totpSecretEdit->setEchoMode(QLineEdit::Password);
        copyTotpSecretButton = new QPushButton("Copy");
        row->addWidget(lbl);
        row->addWidget(totpSecretEdit);
        row->addWidget(copyTotpSecretButton);
        detailLayout->addLayout(row);

        connect(copyTotpSecretButton, &QPushButton::clicked, this, &PasswordManagerWidget::copyTotpSecret);
    }

    {
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* lbl = new QLabel("Current TOTP:");
        totpCodeEdit = new QLineEdit();
        totpCodeEdit->setReadOnly(true);
        copyTotpCodeButton = new QPushButton("Copy");
        totpTimeLabel = new QLabel("Time Left: 30s");

        row->addWidget(lbl);
        row->addWidget(totpCodeEdit);
        row->addWidget(copyTotpCodeButton);
        row->addWidget(totpTimeLabel);

        detailLayout->addLayout(row);

        connect(copyTotpCodeButton, &QPushButton::clicked, this, &PasswordManagerWidget::copyTotpCode);
    }

    {
        QHBoxLayout* descRow = new QHBoxLayout();
        QLabel* lbl = new QLabel("Description:");
        descRow->addWidget(lbl);

        descriptionEdit = new QPlainTextEdit();
        descRow->addWidget(descriptionEdit);
        detailLayout->addLayout(descRow);
    }

    saveButton = new QPushButton("Save");
    detailLayout->addWidget(saveButton);

    detailGroup->setLayout(detailLayout);
    rightPanelLayout->addWidget(detailGroup);
    rightPanelLayout->addStretch();
    rightPanel->setLayout(rightPanelLayout);

    mainLayout->addWidget(rightPanel, 1);

    connect(addButton, &QPushButton::clicked, this, &PasswordManagerWidget::onAddClicked);
    connect(deleteButton, &QPushButton::clicked, this, &PasswordManagerWidget::onDeleteClicked);
    connect(saveButton, &QPushButton::clicked, this, &PasswordManagerWidget::onSaveClicked);

    setLayout(mainLayout);
}

void PasswordManagerWidget::setPasswordManager(PasswordManager* pm)
{
    passwordManager = pm;
}

void PasswordManagerWidget::loadPasswords()
{
    if (!passwordManager)
        return;

    cachedEntries.clear();

    QLayoutItem* child;
    while ((child = scrollAreaLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }

    cachedEntries = passwordManager->getPasswords();

    for (const PasswordEntry &entry : cachedEntries) {
        QString btnText = QString("%1\n%2")
                              .arg(entry.service)
                              .arg(entry.username);

        QPushButton* entryButton = new QPushButton(btnText, this);
        scrollAreaLayout->addWidget(entryButton);

        connect(entryButton, &QPushButton::clicked, this, [=]() {
            onEntryClicked(entry.id);
        });
    }

    scrollAreaLayout->addStretch();

    if (!cachedEntries.isEmpty()) {
        onEntryClicked(cachedEntries.first().id);
    }
}

void PasswordManagerWidget::onAddClicked()
{
    clearDetailFields();
    isAddingNew = true;
    selectedEntryId = -1;
}

void PasswordManagerWidget::onDeleteClicked()
{
    int id = currentSelectedId();
    if (id < 0) {
        QMessageBox::warning(this, "Delete", "No password entry is selected.");
        return;
    }

    if (!passwordManager) {
        return;
    }

    auto reply = QMessageBox::question(this, "Confirm Delete",
                                       "Are you sure you want to delete this entry?");
    if (reply == QMessageBox::Yes) {
        if (passwordManager->deletePassword(id)) {
            QMessageBox::information(this, "Deleted", "Password entry deleted successfully.");
            loadPasswords();
            clearDetailFields();
            selectedEntryId = -1;
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete password entry.");
        }
    }
}

void PasswordManagerWidget::onSaveClicked()
{
    if (!passwordManager) {
        QMessageBox::warning(this, "Error", "No password manager available.");
        return;
    }

    PasswordEntry entry = gatherDetailFields();

    if (entry.service.isEmpty() || entry.password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Service and Password cannot be empty.");
        return;
    }

    if (isAddingNew) {
        if (passwordManager->addPassword(entry)) {
            QMessageBox::information(this, "Success", "Password added successfully.");
            loadPasswords();
            isAddingNew = false;
        } else {
            QMessageBox::warning(this, "Error", "Failed to add new password entry.");
        }
    } else {
        int id = currentSelectedId();
        if (id < 0) {
            if (passwordManager->addPassword(entry)) {
                QMessageBox::information(this, "Success", "Password added successfully.");
                loadPasswords();
            }
            return;
        }
        if (passwordManager->updatePassword(id, entry)) {
            QMessageBox::information(this, "Success", "Password entry updated successfully.");
            loadPasswords();
        } else {
            QMessageBox::warning(this, "Error", "Failed to update password entry.");
        }
    }
}

void PasswordManagerWidget::onEntryClicked(int id)
{
    selectedEntryId = id;
    isAddingNew = false;

    for (auto &e : cachedEntries) {
        if (e.id == id) {
            populateDetailFields(e);
            break;
        }
    }
}

void PasswordManagerWidget::updateTOTPDisplay()
{
    int id = currentSelectedId();
    if (id < 0) {
        totpCodeEdit->clear();
        totpTimeLabel->setText("30.0s");
        return;
    }

    qint64 currentTime = QDateTime::currentSecsSinceEpoch();
    qint64 currentMSecs = QDateTime::currentDateTimeUtc().time().msec();
    double currentTotalSeconds = currentTime + currentMSecs / 1000.0;

    double elapsed = fmod(currentTotalSeconds, 30.0);
    double remaining = 30.0 - elapsed;

    QString timeLeftStr = QString("%1s").arg(QString::number(remaining, 'f', 1));
    totpTimeLabel->setText(timeLeftStr);

    QString secret = totpSecretEdit->text().trimmed();
    if (!secret.isEmpty()) {
        static double lastElapsed = -1.0;
        if (static_cast<int>(elapsed) != static_cast<int>(lastElapsed)) {
            QString code = TOTPGenerator::generateTOTP(secret, 6, 30, 0);
            totpCodeEdit->setText(code);
            lastElapsed = elapsed;
        }
    } else {
        totpCodeEdit->clear();
    }
}

void PasswordManagerWidget::clearDetailFields()
{
    serviceEdit->clear();
    urlEdit->clear();
    usernameEdit->clear();
    emailEdit->clear();
    passwordEdit->clear();
    descriptionEdit->clear();
    totpSecretEdit->clear();
    totpCodeEdit->clear();
    totpTimeLabel->setText("30.0s");
}

void PasswordManagerWidget::populateDetailFields(const PasswordEntry &entry)
{
    serviceEdit->setText(entry.service);
    urlEdit->setText(entry.url);
    usernameEdit->setText(entry.username);
    emailEdit->setText(entry.email);
    passwordEdit->setText(entry.password);
    descriptionEdit->setPlainText(entry.description);
    totpSecretEdit->setText(entry.totpSecret);
}

PasswordEntry PasswordManagerWidget::gatherDetailFields() const
{
    PasswordEntry e;
    e.service     = serviceEdit->text().trimmed();
    e.url         = urlEdit->text().trimmed();
    e.username    = usernameEdit->text().trimmed();
    e.email       = emailEdit->text().trimmed();
    e.password    = passwordEdit->text();
    e.description = descriptionEdit->toPlainText().trimmed();
    e.totpSecret  = totpSecretEdit->text().trimmed();
    return e;
}

int PasswordManagerWidget::currentSelectedId() const
{
    return selectedEntryId;
}

void PasswordManagerWidget::copyService()
{
    QApplication::clipboard()->setText(serviceEdit->text());
}

void PasswordManagerWidget::copyUrl()
{
    QApplication::clipboard()->setText(urlEdit->text());
}

void PasswordManagerWidget::copyUsername()
{
    QApplication::clipboard()->setText(usernameEdit->text());
}

void PasswordManagerWidget::copyEmail()
{
    QApplication::clipboard()->setText(emailEdit->text());
}

void PasswordManagerWidget::copyPassword()
{
    QApplication::clipboard()->setText(passwordEdit->text());
}

void PasswordManagerWidget::copyTotpSecret()
{
    QApplication::clipboard()->setText(totpSecretEdit->text());
}

void PasswordManagerWidget::copyTotpCode()
{
    QApplication::clipboard()->setText(totpCodeEdit->text());
}
