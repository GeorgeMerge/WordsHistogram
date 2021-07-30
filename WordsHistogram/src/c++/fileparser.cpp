#include "fileparser.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

FileParser::FileParser(QObject *parent) : QObject(parent)
{

}

void FileParser::parseFile(QString filePath)
{
    QVariantMap toReturn;
    QList<QPair<int, QString>> tmp;

    QFile inputFile(filePath);
    QElapsedTimer timer;
    //Prepare data from file and fill the map
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);

       timer.start();

       QString data = QString(in.readAll().toLocal8Bit());

       emit progressChanged(10);

       if (data.isEmpty()) {
           emit progressChanged(100);
           return;
       }

       data = replacePatternWithWhitespaceConcurrently(data, QRegExp("[^a-zA-Zа-яА-Я]"), 20).simplified().toLower();

       qDebug()<<"Replace pattern elapsed: "<<timer.restart() << "ms";
       emit progressChanged(50);

       fillWordsCountConcurrently(data, toReturn, 20);

       qDebug()<<"Words counting elapsed: "<<timer.restart() << "ms";
       emit progressChanged(80);

       inputFile.close();
    }
    else {
        qDebug()<<"QFile::open returned false";
        return;
    }

    //Fill second map to sort by value desc
    for (const auto &key : toReturn.keys()) {
        tmp.append(QPair<int, QString>(toReturn.value(key).value<int>(), key));
    }
    std::sort(tmp.begin(), tmp.end(), [](QPair<int, QString> a, QPair<int, QString> b) {
        return a.first > b.first;
    });

    emit progressChanged(90);

    //Rm from original size is 16+
    if (tmp.size() > 15) {
        for (int i = 15; i< tmp.size(); i++) {
            toReturn.remove(tmp[i].second);
        }
    }

    emit progressChanged(100);
    emit fileParsed(toReturn);
}

QString FileParser::replacePatternWithWhitespaceConcurrently(QString &data, QRegExp pattern, int substrings)
{
    if (data.length() < substrings) {
        return data;
    }
    QStringList dataList;
    int substrSize = data.length() / substrings;

    splitString(data, substrSize, dataList);

    std::function<void(QString&)> replaceFunc = [pattern](QString &string) {
        string.replace(pattern, " ");
    };

    QtConcurrent::blockingMap(dataList, replaceFunc);

    return dataList.join("");
}

void FileParser::splitString(const QString &str, int n, QStringList &list)
{
    if (n < 1 || str.length() < n) {
        list.append(str);
        return;
    }

    QString tmp(str);

    list.clear();

    while (!tmp.isEmpty()) {
        list.append(tmp.left(n));
        tmp.remove(0, n);
    }
}

void FileParser::splitStringSavingWords(const QString &str, int n, QStringList &list)
{
    if (n < 1 || str.length() < n) {
        list.append(str);
        return;
    }

    QString tmp(str);

    list.clear();

    int index = 0;
    while (index != -1) {
        index = tmp.indexOf(" ", n);
        list.append(tmp.left(index));
        tmp.remove(0, index + 1);
    }
}

void FileParser::fillWordsCountConcurrently(QString &data, QVariantMap &map, int substrings)
{
    QStringList dataList;
    int substrSize = data.length() / substrings;

    splitStringSavingWords(data, substrSize, dataList);

    std::function<QVariantMap(const QString)> mapFn = [](const QString &str){
        QVariantMap toReturn;
        QStringList words = str.split(" ");

        for (const auto &word : words) {
            if (toReturn.contains(word)) {
                toReturn[word] = toReturn[word].value<int>() + 1;
            }
            else {
                toReturn[word] = 1;
            }
        }

        return toReturn;
    };

    std::function<void(QVariantMap &, const QVariantMap &)> reduceFn = [](QVariantMap &result, const QVariantMap &w){
        QMapIterator<QString, QVariant> i(w);
        while (i.hasNext()) {
            i.next();
            result[i.key()] = result[i.key()].value<int>() + i.value().value<int>();
        }
    };

    map = QtConcurrent::blockingMappedReduced<QVariantMap>(dataList, mapFn, reduceFn);
}

void FileParser::parseFileInSeparateThread(QString filePath)
{
    QtConcurrent::run(this, &FileParser::parseFile, filePath);
}
