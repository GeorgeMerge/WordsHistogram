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
    QList<QPair<int, QString>> temp;

    //Fill first map
    //Contains all the entries with number of occurrences
    QFile inputFile(filePath);
    QElapsedTimer timer;
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);      
       timer.start();
       QString data = QString(in.readAll().toLocal8Bit());
       data = replacePatternWithWhitespaceConcurrently(data, QRegExp("[^a-zA-Zа-яА-Я]"), 20);
       qDebug()<<"replacePatternWithWhitespaceConcurrently took"<<timer.elapsed();
//       data = data.replace(QRegExp("[^a-zA-Zа-яА-Я]"), " ");
       data = data.simplified();
       qDebug()<<"data.simplified() took"<<timer.elapsed();
       data = data.toLower();
       qDebug()<<"data.toLower() took"<<timer.elapsed();
       emit progressChanged(50);
       QStringList words = data.split(" ");
       int progress_60_words = words.size() / 3;
       int progress_70_words = (words.size() / 3) * 2;
       int currentIteration = 0;
       for (auto word : words) {
          if (toReturn.contains(word)) {
              toReturn[word] = toReturn[word].value<int>() + 1;
          }
          else {
              toReturn[word] = 1;
          }
          currentIteration++;
          if (currentIteration == progress_60_words) {
              emit progressChanged(60);
          }
          else if (currentIteration == progress_70_words) {
              emit progressChanged(70);
          }
       }
       qDebug()<<"map forming took"<<timer.elapsed();
       emit progressChanged(80);
       inputFile.close();
    }
    else {
        //Handle
    }

    //Fill temporary list with swapped entries from prev map
    //Needed to apply sort based on number of occurrences
    for (auto key : toReturn.keys()) {
        temp.append(QPair<int, QString>(toReturn.value(key).value<int>(), key));
    }
    qDebug()<<"temp map forming took"<<timer.elapsed();
    std::sort(temp.begin(), temp.end(), [](QPair<int, QString> a, QPair<int, QString> b) {
        return a.first > b.first;
    });

    qDebug()<<"sort took"<<timer.elapsed();
    emit progressChanged(85);
    if (temp.size() > 15) {
        for (int i = 15; i< temp.size(); i++) {
            toReturn.remove(temp[i].second);
        }
    }
    qDebug()<<"key removing took"<<timer.elapsed();

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

    if ((list.last().length() / list.first().length()) < 0.5 && list.size() >= 2) {
        list[list.length() - 2].append(list.last());
        list.removeLast();
    }
}

void FileParser::parseFileInSeparateThread(QString filePath)
{
    QtConcurrent::run(this, &FileParser::parseFile, filePath);
}
