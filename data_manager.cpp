#include "data_manager.h"
#include <QFile>
#include <QJsonDocument>
#include <QDebug>

DataManager::DataManager(QObject *parent) : QObject(parent) {}

void DataManager::saveToFile(QJsonArray data, const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Nie można zapisać pliku";
        return;
    }

    QJsonDocument doc(data);
    file.write(doc.toJson());
    file.close();
}

QJsonArray DataManager::loadFromFile(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Nie można odczytać pliku";
        return QJsonArray();
    }

    QByteArray rawData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(rawData);
    return doc.array();
}
