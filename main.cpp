/**
 * @file main.cpp
 * @brief Główny plik aplikacji Qt/QML do monitorowania jakości powietrza.
 *
 * Ten plik zawiera punkt wejścia aplikacji, konfigurację QML oraz zarządzanie wątkami dla NetworkChecker.
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "api_client.h"
#include "data_manager.h"
#include "network_checker.h"
#include "network_status.h"
#include <QThread>

/**
 * @brief Funkcja główna aplikacji.
 *
 * Inicjalizuje aplikację Qt/QML, rejestruje obiekty w kontekście QML, konfiguruje wątek dla NetworkChecker
 * i ładuje główny plik QML.
 *
 * @param argc Liczba argumentów wiersza poleceń.
 * @param argv Tablica argumentów wiersza poleceń.
 * @return Kod wyjścia aplikacji (0 w przypadku sukcesu, -1 w przypadku błędu).
 */
int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // Rejestracja ApiClient
    ApiClient apiClient;
    engine.rootContext()->setContextProperty("apiClient", &apiClient);

    // Rejestracja DataManager
    DataManager dataManager;
    engine.rootContext()->setContextProperty("dataManager", &dataManager);

    // Rejestracja NetworkStatus w głównym wątku
    NetworkStatus *networkStatus = new NetworkStatus();
    engine.rootContext()->setContextProperty("networkStatus", networkStatus);

    // Rejestracja NetworkChecker w osobnym wątku
    QThread networkThread;
    NetworkChecker *networkChecker = new NetworkChecker();
    networkChecker->moveToThread(&networkThread);
    networkThread.start();
    QObject::connect(networkChecker, &NetworkChecker::internetStatusChanged, networkStatus, &NetworkStatus::onInternetStatusChanged, Qt::QueuedConnection);
    QMetaObject::invokeMethod(networkChecker, "startChecking");

    // Ładowanie Main.qml jako modułu
    engine.loadFromModule("jpoPRO", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    int result = app.exec();

    // Bezpieczne zatrzymanie wątku i sprzątanie
    QMetaObject::invokeMethod(networkChecker, "stopChecking", Qt::BlockingQueuedConnection);
    networkThread.quit();
    networkThread.wait();
    delete networkChecker;
    delete networkStatus;

    return result;
}
