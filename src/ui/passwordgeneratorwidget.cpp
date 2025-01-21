#include "passwordgeneratorwidget.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QSlider>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QClipboard>
#include <QApplication>
#include <QGroupBox>

#include <QRandomGenerator>
#include <openssl/rand.h>

PasswordGeneratorWidget::PasswordGeneratorWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

PasswordGeneratorWidget::~PasswordGeneratorWidget()
{
}

void PasswordGeneratorWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QGroupBox* passwordGenGroupBox = new QGroupBox("Password Generator", this);
    QVBoxLayout* pgGroupLayout = new QVBoxLayout(passwordGenGroupBox);

    includeUppercaseCheckBox = new QCheckBox("Include Uppercase");
    includeUppercaseCheckBox->setChecked(true);
    pgGroupLayout->addWidget(includeUppercaseCheckBox);

    includeLowercaseCheckBox = new QCheckBox("Include Lowercase");
    includeLowercaseCheckBox->setChecked(true);
    pgGroupLayout->addWidget(includeLowercaseCheckBox);

    includeNumbersCheckBox = new QCheckBox("Include Numbers");
    includeNumbersCheckBox->setChecked(true);
    pgGroupLayout->addWidget(includeNumbersCheckBox);

    includeSymbolsCheckBox = new QCheckBox("Include Symbols");
    includeSymbolsCheckBox->setChecked(true);
    pgGroupLayout->addWidget(includeSymbolsCheckBox);

    includeCustomCheckBox = new QCheckBox("Include Custom Characters");
    pgGroupLayout->addWidget(includeCustomCheckBox);

    QHBoxLayout* customCharsLayout = new QHBoxLayout();
    QLabel* customCharsLabel = new QLabel("Custom Characters:", passwordGenGroupBox);
    customCharsLineEdit = new QLineEdit(passwordGenGroupBox);
    customCharsLineEdit->setPlaceholderText("e.g., @#$%");
    customCharsLineEdit->setEnabled(false);
    customCharsLayout->addWidget(customCharsLabel);
    customCharsLayout->addWidget(customCharsLineEdit);
    pgGroupLayout->addLayout(customCharsLayout);

    connect(includeCustomCheckBox, &QCheckBox::toggled, customCharsLineEdit, &QLineEdit::setEnabled);

    QHBoxLayout* lengthLayout = new QHBoxLayout();
    QLabel* lengthLabel = new QLabel("Length:", passwordGenGroupBox);
    lengthSlider = new QSlider(Qt::Horizontal, passwordGenGroupBox);
    lengthSlider->setRange(6, 32);
    lengthSlider->setValue(12);
    lengthSpinBox = new QSpinBox(passwordGenGroupBox);
    lengthSpinBox->setRange(6, 32);
    lengthSpinBox->setValue(12);

    lengthLayout->addWidget(lengthLabel);
    lengthLayout->addWidget(lengthSlider);
    lengthLayout->addWidget(lengthSpinBox);
    pgGroupLayout->addLayout(lengthLayout);

    connect(lengthSlider, &QSlider::valueChanged, lengthSpinBox, &QSpinBox::setValue);
    connect(lengthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), lengthSlider, &QSlider::setValue);

    generateButton = new QPushButton("Generate Password", passwordGenGroupBox);
    pgGroupLayout->addWidget(generateButton);

    QHBoxLayout* generatedPasswordLayout = new QHBoxLayout();
    generatedPasswordLineEdit = new QLineEdit(passwordGenGroupBox);
    generatedPasswordLineEdit->setReadOnly(true);
    generatedPasswordLineEdit->setPlaceholderText("Your generated password will appear here");
    copyButton = new QPushButton("Copy", passwordGenGroupBox);
    generatedPasswordLayout->addWidget(generatedPasswordLineEdit);
    generatedPasswordLayout->addWidget(copyButton);
    pgGroupLayout->addLayout(generatedPasswordLayout);

    strengthLabel = new QLabel("Strength: ", passwordGenGroupBox);
    pgGroupLayout->addWidget(strengthLabel);

    passwordGenGroupBox->setLayout(pgGroupLayout);
    mainLayout->addWidget(passwordGenGroupBox);
    mainLayout->addStretch();

    connect(generateButton, &QPushButton::clicked, this, &PasswordGeneratorWidget::generatePassword);
    connect(copyButton, &QPushButton::clicked, this, &PasswordGeneratorWidget::copyPassword);
}

void PasswordGeneratorWidget::generatePassword()
{
    const QString uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const QString lowercase = "abcdefghijklmnopqrstuvwxyz";
    const QString numbers   = "0123456789";
    const QString symbols   = "!@#$%^&*()-_=+[]{}|;:,.<>?";

    QString characterPool;

    if (includeUppercaseCheckBox->isChecked()) {
        characterPool += uppercase;
    }
    if (includeLowercaseCheckBox->isChecked()) {
        characterPool += lowercase;
    }
    if (includeNumbersCheckBox->isChecked()) {
        characterPool += numbers;
    }
    if (includeSymbolsCheckBox->isChecked()) {
        characterPool += symbols;
    }
    if (includeCustomCheckBox->isChecked()) {
        QString custom = customCharsLineEdit->text();
        if (!custom.isEmpty()) {
            characterPool += custom;
        }
    }

    if (characterPool.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please select at least one character set.");
        return;
    }

    int length = lengthSpinBox->value();
    QString password;

    unsigned char* buffer = new unsigned char[length];
    if (RAND_bytes(buffer, length) != 1) {
        QMessageBox::critical(this, "Error", "Failed to generate secure random bytes.");
        delete[] buffer;
        return;
    }

    for (int i = 0; i < length; ++i) {
        int index = buffer[i] % characterPool.size();
        password.append(characterPool.at(index));
    }

    delete[] buffer;

    generatedPasswordLineEdit->setText(password);

    int score = 0;
    if (includeUppercaseCheckBox->isChecked()) {
        score++;
    }
    if (includeLowercaseCheckBox->isChecked()) {
        score++;
    }
    if (includeNumbersCheckBox->isChecked()) {
        score++;
    }
    if (includeSymbolsCheckBox->isChecked()) {
        score++;
    }
    if (includeCustomCheckBox->isChecked() && !customCharsLineEdit->text().isEmpty()) {
        score++;
    }

    QString strength;
    switch(score) {
        case 5: strength = "Very Strong"; break;
        case 4: strength = "Strong"; break;
        case 3: strength = "Moderate"; break;
        case 2: strength = "Weak"; break;
        default: strength = "Very Weak"; break;
    }
    strengthLabel->setText("Strength: " + strength);
}

void PasswordGeneratorWidget::copyPassword()
{
    QString password = generatedPasswordLineEdit->text();
    if (password.isEmpty()) {
        QMessageBox::warning(this, "Copy Error", "No password to copy.");
        return;
    }

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(password);
    QMessageBox::information(this, "Copied", "Password copied to clipboard.");
}
