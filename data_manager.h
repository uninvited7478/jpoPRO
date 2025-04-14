#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <QObject>
#include <QJsonArray>

class DataManager : public QObject {
    Q_OBJECT
public:
    explicit DataManager(QObject *parent = nullptr);

    Q_INVOKABLE void saveToFile(QJsonArray data, const QString &filename);
    Q_INVOKABLE QJsonArray loadFromFile(const QString &filename);
};

#endif
