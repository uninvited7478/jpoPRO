#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>

class ApiClient : public QObject {
    Q_OBJECT
public:
    explicit ApiClient(QObject *parent = nullptr);

    Q_INVOKABLE void fetchStations();
    Q_INVOKABLE void fetchSensors(int stationId);
    Q_INVOKABLE void fetchSensorData(int sensorId);

signals:
    void stationsReady(QJsonArray stations);
    void sensorsReady(QJsonArray sensors);
    void sensorDataReady(QJsonArray values);
    void errorOccurred(QString message);

private:
    QNetworkAccessManager manager;
};

#endif
