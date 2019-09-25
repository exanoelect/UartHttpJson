#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <qdir.h>
#include <QTextStream>
#include <QtDebug>
#include <qdebug.h>
#include <QDebug>

//Test Commits Broh

class filemanager : public QObject
{
    Q_OBJECT
public:
    explicit filemanager(QObject *parent = nullptr);
    QString fileFirstFound(QString dir);
    bool fileWrite(QString path, QString data);
    QString fileRead(QString path);
    bool fileDelete(QString path);
signals:

public slots:
};

#endif // FILEMANAGER_H
