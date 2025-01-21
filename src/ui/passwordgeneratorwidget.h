#ifndef PASSWORDGENERATORWIDGET_H
#define PASSWORDGENERATORWIDGET_H

#include <QWidget>

class QCheckBox;
class QLineEdit;
class QSlider;
class QSpinBox;
class QPushButton;
class QLabel;

class PasswordGeneratorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PasswordGeneratorWidget(QWidget *parent = nullptr);
    ~PasswordGeneratorWidget();

    private slots:
        void generatePassword();
    void copyPassword();

private:
    void setupUI();

    QCheckBox* includeUppercaseCheckBox;
    QCheckBox* includeLowercaseCheckBox;
    QCheckBox* includeNumbersCheckBox;
    QCheckBox* includeSymbolsCheckBox;
    QCheckBox* includeCustomCheckBox;
    QLineEdit* customCharsLineEdit;
    QSlider* lengthSlider;
    QSpinBox* lengthSpinBox;
    QPushButton* generateButton;
    QLineEdit* generatedPasswordLineEdit;
    QPushButton* copyButton;
    QLabel* strengthLabel;
};

#endif // PASSWORDGENERATORWIDGET_H
