#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "applicationcontext.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    ApplicationContext applicationContext;
    engine.rootContext()->setContextProperty("applicationContext", &applicationContext);
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    return app.exec();
}
