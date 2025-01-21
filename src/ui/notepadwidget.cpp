#include "notepadwidget.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <QGroupBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>

#include "models/notemanager.h"

NotepadWidget::NotepadWidget(QWidget *parent)
    : QWidget(parent)
    , noteManager(nullptr)
    , isAddingNew(false)
    , selectedNoteId(-1)
{
    setupUI();
}

NotepadWidget::~NotepadWidget()
{
}

void NotepadWidget::setupUI()
{
    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    leftPanel = new QWidget(this);
    QVBoxLayout* leftPanelLayout = new QVBoxLayout(leftPanel);

    addButton = new QPushButton("Add Note", this);
    deleteButton = new QPushButton("Delete Note", this);

    QHBoxLayout* topRowLayout = new QHBoxLayout();
    topRowLayout->addWidget(addButton);
    topRowLayout->addWidget(deleteButton);

    leftPanelLayout->addLayout(topRowLayout);

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

    QGroupBox* detailGroup = new QGroupBox("Note Details", rightPanel);
    QVBoxLayout* detailLayout = new QVBoxLayout(detailGroup);

    {
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* lbl = new QLabel("Title:");
        titleEdit = new QLineEdit();
        row->addWidget(lbl);
        row->addWidget(titleEdit);
        detailLayout->addLayout(row);
    }

    {
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* lbl = new QLabel("Content:");
        contentEdit = new QPlainTextEdit();
        row->addWidget(lbl, 0);
        row->addWidget(contentEdit, 1);
        detailLayout->addLayout(row);
    }

    saveButton = new QPushButton("Save");
    detailLayout->addWidget(saveButton);

    detailGroup->setLayout(detailLayout);
    rightPanelLayout->addWidget(detailGroup);
    rightPanelLayout->addStretch();

    rightPanel->setLayout(rightPanelLayout);
    mainLayout->addWidget(rightPanel, 2);

    connect(addButton, &QPushButton::clicked, this, &NotepadWidget::onAddClicked);
    connect(deleteButton, &QPushButton::clicked, this, &NotepadWidget::onDeleteClicked);
    connect(saveButton, &QPushButton::clicked, this, &NotepadWidget::onSaveClicked);

    setLayout(mainLayout);
}

void NotepadWidget::setNoteManager(NoteManager* manager)
{
    noteManager = manager;
}

void NotepadWidget::loadNotes()
{
    if (!noteManager) {
        return;
    }

    cachedNotes.clear();
    QLayoutItem* child;
    while ((child = scrollAreaLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }

    cachedNotes = noteManager->getNotes();

    for (const NoteEntry &note : cachedNotes) {
        QPushButton* noteButton = new QPushButton(note.title, this);

        connect(noteButton, &QPushButton::clicked, this, [=]() {
            onNoteClicked(note.id);
        });

        scrollAreaLayout->addWidget(noteButton);
    }

    scrollAreaLayout->addStretch();

    if (!cachedNotes.isEmpty()) {
        onNoteClicked(cachedNotes.first().id);
    }
}

void NotepadWidget::onAddClicked()
{
    clearFields();
    isAddingNew = true;
    selectedNoteId = -1;
}

void NotepadWidget::onSaveClicked()
{
    if (!noteManager) {
        QMessageBox::warning(this, "Error", "No NoteManager available.");
        return;
    }

    NoteEntry entry = gatherFields();
    if (entry.title.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Title cannot be empty.");
        return;
    }

    if (isAddingNew) {
        if (noteManager->addNote(entry)) {
            QMessageBox::information(this, "Success", "Note added successfully.");
            loadNotes();
            isAddingNew = false;
        } else {
            QMessageBox::warning(this, "Error", "Failed to add note.");
        }
    } else {
        int id = currentSelectedId();
        if (id < 0) {
            if (noteManager->addNote(entry)) {
                QMessageBox::information(this, "Success", "Note added successfully.");
                loadNotes();
            }
            return;
        }
        if (noteManager->updateNote(id, entry)) {
            QMessageBox::information(this, "Success", "Note updated successfully.");
            loadNotes();
        } else {
            QMessageBox::warning(this, "Error", "Failed to update note.");
        }
    }
}

void NotepadWidget::onDeleteClicked()
{
    int id = currentSelectedId();
    if (id < 0) {
        QMessageBox::warning(this, "Delete", "No note is selected.");
        return;
    }

    if (!noteManager) {
        return;
    }

    auto reply = QMessageBox::question(this, "Confirm Delete",
                                       "Are you sure you want to delete this note?");
    if (reply == QMessageBox::Yes) {
        if (noteManager->deleteNote(id)) {
            QMessageBox::information(this, "Deleted", "Note deleted successfully.");
            loadNotes();
            clearFields();
            selectedNoteId = -1;
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete note.");
        }
    }
}

void NotepadWidget::onNoteClicked(int id)
{
    selectedNoteId = id;
    isAddingNew = false;

    for (auto &note : cachedNotes) {
        if (note.id == id) {
            populateFields(note);
            break;
        }
    }
}

void NotepadWidget::clearFields()
{
    titleEdit->clear();
    contentEdit->clear();
}

void NotepadWidget::populateFields(const NoteEntry &entry)
{
    titleEdit->setText(entry.title);
    contentEdit->setPlainText(entry.content);
}

NoteEntry NotepadWidget::gatherFields() const
{
    NoteEntry e;
    e.title   = titleEdit->text().trimmed();
    e.content = contentEdit->toPlainText().trimmed();
    return e;
}

int NotepadWidget::currentSelectedId() const
{
    return selectedNoteId;
}
