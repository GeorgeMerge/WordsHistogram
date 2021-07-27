#include "fileparser.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QApplication>

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    },
    Qt::QueuedConnection);

    qmlRegisterType<FileParser>("FileParser", 1, 0, "FileParser");
//    qmlRegisterType<QMap<QString, int>>("FileParser", 1, 0, "StringCounter");
    engine.load(url);

//    QVariantMap words = FileParser().parseFile(QCoreApplication::applicationDirPath() + "/Input.txt");
//    qDebug()<<words;

    return app.exec();
}
