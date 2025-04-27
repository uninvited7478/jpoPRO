#include "network_status.h"
#include <QDebug>
#include <Qthread>

NetworkStatus::NetworkStatus(QObject *parent) : QObject(parent) {}

void NetworkStatus::onInternetStatusChanged(bool connected) {
    qDebug() << "NetworkStatus: Internet status changed to" << (connected ? "connected" : "disconnected") << "in main thread:" << QThread::currentThreadId();
    emit internetStatusChanged(connected);
}
