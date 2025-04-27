#include "api_client.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>

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

        QByteArray responseData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(responseData);

        // Debugowanie: sprawdź poprawność danych
        if (doc.isNull()) {
            qDebug() << "Błąd parsowania JSON. Otrzymane dane:" << responseData;
            emit errorOccurred("Nieprawidłowy format danych stacji");
            reply->deleteLater();
            return;
        }

        // Dodaj logowanie przed emisją sygnału
        qDebug() << "Emitting stationsReady, data size:" << doc.array().size();
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
        if (doc.isNull()) {
            emit errorOccurred("Nieprawidłowy format danych sensorów");
            reply->deleteLater();
            return;
        }

        emit sensorsReady(doc.array());  // Emitowanie danych sensorów
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
        if (doc.isNull() || !doc.isObject()) {
            emit errorOccurred("Nieprawidłowy format danych pomiarowych");
            reply->deleteLater();
            return;
        }

        QJsonArray values = doc.object().value("values").toArray();
        emit sensorDataReady(values);  // Emitowanie danych pomiarowych
        reply->deleteLater();
    });
}
