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
    Q_INVOKABLE void parseFile(QString filePath);
    Q_INVOKABLE void parseFileInSeparateThread(QString filePath);

signals:
    void progressChanged(qreal value);
    void fileParsed(QVariantMap resultMap);
};

#endif // FILEPARSER_H
