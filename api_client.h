#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>

/**
 * @class ApiClient
 * @brief Klasa odpowiedzialna za komunikację z API GIOŚ.
 *
 * ApiClient wykonuje zapytania HTTP do API GIOŚ, pobiera dane o stacjach, czujnikach i wartościach pomiarowych.
 */
class ApiClient : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Konstruktor klasy ApiClient.
     * @param parent Wskaźnik na obiekt nadrzędny (domyślnie nullptr).
     */
    explicit ApiClient(QObject *parent = nullptr);

    /**
     * @brief Pobiera listę stacji pomiarowych z API.
     */
    Q_INVOKABLE void fetchStations();

    /**
     * @brief Pobiera listę czujników dla wybranej stacji.
     * @param stationId Identyfikator stacji pomiarowej.
     */
    Q_INVOKABLE void fetchSensors(int stationId);

    /**
     * @brief Pobiera dane z wybranego czujnika.
     * @param sensorId Identyfikator czujnika.
     */
    Q_INVOKABLE void fetchSensorData(int sensorId);

signals:
    /// @brief Sygnał emitowany po pomyślnym pobraniu listy stacji.
    /// @param stations Tablica JSON z danymi stacji.
    void stationsReady(QJsonArray stations);

    /// @brief Sygnał emitowany po pomyślnym pobraniu listy czujników.
    /// @param sensors Tablica JSON z danymi czujników.
    void sensorsReady(QJsonArray sensors);

    /// @brief Sygnał emitowany po pomyślnym pobraniu danych z czujnika.
    /// @param values Tablica JSON z wartościami pomiarowymi.
    void sensorDataReady(QJsonArray values);

    /// @brief Sygnał emitowany w przypadku błędu podczas komunikacji z API.
    /// @param message Komunikat błędu.
    void errorOccurred(QString message);

private:
    QNetworkAccessManager manager; ///< Obiekt zarządzający zapytaniami sieciowymi.
};

#endif
