#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <QObject>
#include <QMap>
#include <QVariant>


class FileParser : public QObject
{
    Q_OBJECT
public:
    explicit FileParser(QObject *parent = nullptr);
    Q_INVOKABLE QVariantMap parseFile(QString filePath);

signals:
    void signalEmitted(QString msg);

};

#endif // FILEPARSER_H
