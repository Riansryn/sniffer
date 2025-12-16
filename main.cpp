#include "mainwindow.h"
#include "packetmodel.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    
    QApplication app(argc, argv);
    
    app.setApplicationName("Network Sniffer");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("NetworkTools");
    
    QQmlApplicationEngine engine;
    
    // Create controller
    MainWindow controller;
    
    // Make controller available to QML
    engine.rootContext()->setContextProperty("controller", &controller);
    
    // Load QML
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    return app.exec();
}
