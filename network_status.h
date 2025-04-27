#ifndef NETWORK_STATUS_H
#define NETWORK_STATUS_H

#include <QObject>

/**
 * @class NetworkStatus
 * @brief Klasa zarządzająca stanem połączenia internetowego.
 *
 * NetworkStatus odbiera informacje o stanie połączenia od NetworkChecker i emituje sygnały,
 * które są wykorzystywane w QML do aktualizacji interfejsu użytkownika.
 */
class NetworkStatus : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Konstruktor klasy NetworkStatus.
     * @param parent Wskaźnik na obiekt nadrzędny (domyślnie nullptr).
     */
    explicit NetworkStatus(QObject *parent = nullptr);

signals:
    /// @brief Sygnał emitowany, gdy zmienia się stan połączenia internetowego.
    /// @param connected True, jeśli internet jest dostępny, false w przeciwnym razie.
    void internetStatusChanged(bool connected);

public slots:
    /**
     * @brief Slot wywoływany, gdy zmieni się stan połączenia internetowego.
     * @param connected True, jeśli internet jest dostępny, false w przeciwnym razie.
     */
    void onInternetStatusChanged(bool connected);
};

#endif
