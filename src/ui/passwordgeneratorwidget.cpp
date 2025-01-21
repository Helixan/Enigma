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
#include <random>

PasswordGeneratorWidget::PasswordGeneratorWidget(QWidget *parent)
    : QWidget(parent) {
    setupUI();
}

PasswordGeneratorWidget::~PasswordGeneratorWidget() {
}

void PasswordGeneratorWidget::setupUI() {
    const auto mainLayout = new QVBoxLayout(this);

    const auto passwordGenGroupBox = new QGroupBox("Password Generator", this);
    const auto pgGroupLayout = new QVBoxLayout(passwordGenGroupBox);

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

    const auto customCharsLayout = new QHBoxLayout();
    const auto customCharsLabel = new QLabel("Custom Characters:", passwordGenGroupBox);
    customCharsLineEdit = new QLineEdit(passwordGenGroupBox);
    customCharsLineEdit->setPlaceholderText("e.g., @#$%");
    customCharsLineEdit->setEnabled(false);
    customCharsLayout->addWidget(customCharsLabel);
    customCharsLayout->addWidget(customCharsLineEdit);
    pgGroupLayout->addLayout(customCharsLayout);

    connect(includeCustomCheckBox, &QCheckBox::toggled, customCharsLineEdit, &QLineEdit::setEnabled);

    const auto lengthLayout = new QHBoxLayout();
    const auto lengthLabel = new QLabel("Length:", passwordGenGroupBox);
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

    const auto generatedPasswordLayout = new QHBoxLayout();
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

void PasswordGeneratorWidget::generatePassword() {
    const QString uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const QString lowercase = "abcdefghijklmnopqrstuvwxyz";
    const QString numbers = "0123456789";
    const QString symbols = "!@#$%^&*()-_=+[]{}|;:,.<>?";

    QString characterPool;
    QString requiredCharacters;

    if (includeUppercaseCheckBox->isChecked()) {
        characterPool += uppercase;
        requiredCharacters += uppercase.at(QRandomGenerator::global()->bounded(uppercase.size()));
    }
    if (includeLowercaseCheckBox->isChecked()) {
        characterPool += lowercase;
        requiredCharacters += lowercase.at(QRandomGenerator::global()->bounded(lowercase.size()));
    }
    if (includeNumbersCheckBox->isChecked()) {
        characterPool += numbers;
        requiredCharacters += numbers.at(QRandomGenerator::global()->bounded(numbers.size()));
    }
    if (includeSymbolsCheckBox->isChecked()) {
        characterPool += symbols;
        requiredCharacters += symbols.at(QRandomGenerator::global()->bounded(symbols.size()));
    }
    if (includeCustomCheckBox->isChecked()) {
        if (const QString custom = customCharsLineEdit->text(); !custom.isEmpty()) {
            characterPool += custom;
            requiredCharacters += custom.at(QRandomGenerator::global()->bounded(custom.size()));
        }
    }

    if (characterPool.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please select at least one character set.");
        return;
    }

    const int length = lengthSpinBox->value();

    if (length < requiredCharacters.length()) {
        QMessageBox::warning(this, "Input Error",
                             QString("Password length must be at least %1 to include all selected character sets.")
                             .arg(requiredCharacters.length()));
        return;
    }

    QString password = requiredCharacters;

    const int remainingLength = length - requiredCharacters.length();

    QByteArray buffer;
    buffer.resize(remainingLength);
    if (RAND_bytes(reinterpret_cast<unsigned char *>(buffer.data()), remainingLength) != 1) {
        QMessageBox::critical(this, "Error", "Failed to generate secure random bytes.");
        return;
    }

    for (int i = 0; i < remainingLength; ++i) {
        const int index = static_cast<unsigned char>(buffer[i]) % characterPool.size();
        password.append(characterPool.at(index));
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::ranges::shuffle(password, g);

    generatedPasswordLineEdit->setText(password);

    const double entropy = length * std::log2(static_cast<double>(characterPool.size()));
    QString strength;

    if (entropy >= 80) {
        strength = "Very Strong";
    } else if (entropy >= 60) {
        strength = "Strong";
    } else if (entropy >= 40) {
        strength = "Moderate";
    } else if (entropy >= 20) {
        strength = "Weak";
    } else {
        strength = "Very Weak";
    }

    strengthLabel->setText("Strength: " + strength);
}

void PasswordGeneratorWidget::copyPassword() {
    const QString password = generatedPasswordLineEdit->text();
    if (password.isEmpty()) {
        QMessageBox::warning(this, "Copy Error", "No password to copy.");
        return;
    }

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(password);
    QMessageBox::information(this, "Copied", "Password copied to clipboard.");
}
