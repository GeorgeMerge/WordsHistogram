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
       qDebug()<<"in.readAll().toLocal8Bit() took"<<timer.elapsed() / 1000;
       data = data.replace(QRegExp("[^a-zA-Zа-яА-Я]"), " ");
       qDebug()<<"data.replace(QRegExp([^a-zA-Zа-яА-Я]), " ")"<<timer.elapsed() / 1000;
       data = data.simplified();
       qDebug()<<"data.simplified() took"<<timer.elapsed() / 1000;
       data = data.toLower();
       qDebug()<<"data.toLower() took"<<timer.elapsed() / 1000;
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

    std::sort(temp.begin(), temp.end(), [](QPair<int, QString> a, QPair<int, QString> b) {
        return a.first > b.first;
    });

    emit progressChanged(85);
    if (temp.size() > 15) {
        for (int i = 15; i< temp.size(); i++) {
            toReturn.remove(temp[i].second);
        }
    }

    emit progressChanged(100);
    emit fileParsed(toReturn);
    qDebug()<<"Everything" << timer.elapsed() / 1000;
}

void FileParser::parseFileInSeparateThread(QString filePath)
{
    QtConcurrent::run(this, &FileParser::parseFile, filePath);
}
