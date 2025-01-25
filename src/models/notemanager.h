#ifndef NOTEMANAGER_H
#define NOTEMANAGER_H

#include <QList>
#include "core/encryption.h"

struct NoteEntry {
    int id;
    QByteArray salt;
    QString title;
    QString content;
};

class NoteManager {
public:
    NoteManager(int userId, Encryption *encryption);

    bool addNote(const NoteEntry &entry) const;

    bool updateNote(int id, const NoteEntry &entry) const;

    QList<NoteEntry> getNotes() const;

    bool deleteNote(int id) const;

private:
    int userId;
    Encryption *encryption;

    static QByteArray generateRandomSalt(int length = 16);
};

#endif // NOTEMANAGER_H
