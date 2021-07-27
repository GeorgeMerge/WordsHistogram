#include "fileparser.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

FileParser::FileParser(QObject *parent) : QObject(parent)
{

}

QVariantMap FileParser::parseFile(QString filePath)
{
    QVariantMap toReturn;
    QList<QPair<int, QString>> temp;

    emit signalEmitted("Parsing the input file");
    //Fill first map
    //Contains all the entries with number of occurrences
    QFile inputFile(filePath);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       qDebug()<<Q_FUNC_INFO<<filePath<<" opened";
       QTextStream in(&inputFile);
       qDebug()<<"Text stream established";
       QString data = QString(in.readAll().toLocal8Bit()).replace(QRegExp("[^a-zA-Zа-яА-Я]"), " ").simplified().toLower();
       qDebug()<<"Data retrieved";
       QStringList words = data.split(" ");
       qDebug()<<"Data split";
       for (auto word : words) {
          if (toReturn.contains(word)) {
              toReturn[word] = toReturn[word].value<int>() + 1;
          }
          else {
              toReturn[word] = 1;
          }
       }
       inputFile.close();
    }
    else {
        qDebug()<<Q_FUNC_INFO<<"Couldn't open"<<filePath;
    }

    emit signalEmitted("Filling temporary map");
    //Fill temporary list with swapped entries from prev map
    //Needed to apply sort based on number of occurrences
    for (auto key : toReturn.keys()) {
        temp.append(QPair<int, QString>(toReturn.value(key).value<int>(), key));
    }

    std::sort(temp.begin(), temp.end(), [](QPair<int, QString> a, QPair<int, QString> b){
        return a.first > b.first;
    });


    if (temp.size() > 15) {
        emit signalEmitted("Removing unnecessary keys");
        for (int i = 15; i< temp.size(); i++) {
            toReturn.remove(temp[i].second);
        }
    }

    return toReturn;
}
