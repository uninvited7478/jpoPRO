#include "api_client.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

ApiClient::ApiClient(QObject *parent) : QObject(parent) {}

void ApiClient::fetchStations() {
    QUrl url("https://api.gios.gov.pl/pjp-api/rest/station/findAll");
    QNetworkRequest request(url);
    QNetworkReply* reply = manager.get(request);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            emit errorOccurred(reply->errorString());
            reply->deleteLater();
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        emit stationsReady(doc.array());
        reply->deleteLater();
    });
}

void ApiClient::fetchSensors(int stationId) {
    QUrl url(QString("https://api.gios.gov.pl/pjp-api/rest/station/sensors/%1").arg(stationId));
    QNetworkRequest request(url);
    QNetworkReply* reply = manager.get(request);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            emit errorOccurred(reply->errorString());
            reply->deleteLater();
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        emit sensorsReady(doc.array());
        reply->deleteLater();
    });
}

void ApiClient::fetchSensorData(int sensorId) {
    QUrl url(QString("https://api.gios.gov.pl/pjp-api/rest/data/getData/%1").arg(sensorId));
    QNetworkRequest request(url);
    QNetworkReply* reply = manager.get(request);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            emit errorOccurred(reply->errorString());
            reply->deleteLater();
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonArray values = doc.object().value("values").toArray();
        emit sensorDataReady(values);
        reply->deleteLater();
    });
}
