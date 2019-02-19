#include <QtQuickTest>
#include <QQmlEngine>
#include <QQmlContext>

class Setup : public QObject
{
    Q_OBJECT

public:
    Setup() {}

public slots:
    void qmlEngineAvailable(QQmlEngine *engine)
    {
        
    }
};

QUICK_TEST_MAIN_WITH_SETUP(test_main, Setup)

#include "test_main.moc"
