#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "api_client.h"
#include "data_manager.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    ApiClient apiClient;
    engine.rootContext()->setContextProperty("apiClient", &apiClient);

    DataManager dataManager;
    engine.rootContext()->setContextProperty("dataManager", &dataManager);

    engine.loadFromModule("jpoPRO", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
