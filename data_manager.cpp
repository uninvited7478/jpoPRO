#include "data_manager.h"
#include <QFile>
#include <QJsonDocument>
#include <QDebug>

DataManager::DataManager(QObject *parent) : QObject(parent) {}

void DataManager::saveToFile(QJsonArray data, const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        emit saveError("Nie można zapisać pliku: " + file.errorString());
        return;
    }

    QJsonDocument doc(data);
    file.write(doc.toJson());
    file.close();
    emit saveSuccess();
}

QJsonArray DataManager::loadFromFile(const QString &filename) {
    QFile file(filename);
    qDebug() << file.fileName();
    if (!file.open(QIODevice::ReadOnly)) {
        emit loadError("Nie można odczytać pliku: " + file.errorString());
        return QJsonArray();
    }

    QByteArray rawData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(rawData);
    if (doc.isNull() || !doc.isArray()) {
        emit loadError("Nieprawidłowy format JSON");
        return QJsonArray();
    }
    emit loadSuccess();
    return doc.array();
}
