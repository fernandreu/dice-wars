#include "hexgrid.h"
#include "hex.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QIcon>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<HexGrid>("Hex", 1, 0, "HexGrid");
    qmlRegisterType<Hex>("Hex", 1, 0, "Hex");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
