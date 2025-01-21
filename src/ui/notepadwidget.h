#ifndef NOTEPADWIDGET_H
#define NOTEPADWIDGET_H

#include <QWidget>
#include <QList>

class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QVBoxLayout;
class QScrollArea;
class QLabel;
class QTimer;

struct NoteEntry;
class NoteManager;

class NotepadWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NotepadWidget(QWidget *parent = nullptr);
    ~NotepadWidget();

    void setNoteManager(NoteManager* manager);
    void loadNotes();

    private slots:
        void onAddClicked();
    void onSaveClicked();
    void onDeleteClicked();
    void onNoteClicked(int id);

private:
    void setupUI();
    void clearFields();
    void populateFields(const NoteEntry &entry);
    NoteEntry gatherFields() const;
    int  currentSelectedId() const;

    QWidget*     leftPanel;
    QScrollArea* scrollArea;
    QVBoxLayout* scrollAreaLayout;

    QPushButton* addButton;
    QPushButton* deleteButton;

    QLineEdit*       titleEdit;
    QPlainTextEdit*  contentEdit;
    QPushButton*     saveButton;

    NoteManager*     noteManager;
    bool             isAddingNew;
    int              selectedNoteId;
    QList<NoteEntry> cachedNotes;
};

#endif // NOTEPADWIDGET_H
