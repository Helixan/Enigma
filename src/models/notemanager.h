#ifndef NOTEMANAGER_H
#define NOTEMANAGER_H

#include <QList>
#include "core/encryption.h"

struct NoteEntry {
    int id;
    QString title;
    QString content;
};

class NoteManager
{
public:
    NoteManager(int userId, Encryption* encryption);

    bool addNote(const NoteEntry& entry);
    bool updateNote(int id, const NoteEntry& entry);
    QList<NoteEntry> getNotes();
    bool deleteNote(int id);

private:
    int userId;
    Encryption* encryption;
};

#endif // NOTEMANAGER_H
