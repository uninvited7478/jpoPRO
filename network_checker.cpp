#include "network_checker.h"
#include <QNetworkReply>
#include <QDebug>
#include <Qthread>

NetworkChecker::NetworkChecker(QObject *parent) : QObject(parent) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &NetworkChecker::checkInternet);
}

void NetworkChecker::startChecking() {
    qDebug() << "NetworkChecker: Started checking connection in thread:" << QThread::currentThreadId();
    timer->start(10000); // 10 sekund
    checkInternet(); // Natychmiastowe sprawdzenie
}

void NetworkChecker::stopChecking() {
    if (timer->isActive()) {
        timer->stop();
        qDebug() << "NetworkChecker: Stopped checking connection in thread:" << QThread::currentThreadId();
    }
}

void NetworkChecker::checkInternet() {
    QNetworkRequest request(QUrl("https://api.gios.gov.pl/pjp-api/rest/station/findAll"));
    QNetworkReply *reply = manager.get(request);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        bool success = (reply->error() == QNetworkReply::NoError);
        qDebug() << "NetworkChecker: Connection status -" << (success ? "OK" : reply->errorString()) << "in thread:" << QThread::currentThreadId();
        emit internetStatusChanged(success);
        reply->deleteLater();
    });
}
