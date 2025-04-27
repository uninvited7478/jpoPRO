#ifndef NETWORK_CHECKER_H
#define NETWORK_CHECKER_H

#include <QObject>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>

/**
 * @class NetworkChecker
 * @brief Klasa odpowiedzialna za cykliczne sprawdzanie dostępności internetu.
 *
 * NetworkChecker co określony czas sprawdza połączenie z internetem i emituje sygnały informujące o zmianie stanu.
 */
class NetworkChecker : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Konstruktor klasy NetworkChecker.
     * @param parent Wskaźnik na obiekt nadrzędny (domyślnie nullptr).
     */
    explicit NetworkChecker(QObject *parent = nullptr);

public slots:
    /**
     * @brief Rozpoczyna cykliczne sprawdzanie dostępności internetu.
     */
    void startChecking();

    /**
     * @brief Zatrzymuje sprawdzanie dostępności internetu.
     */
    void stopChecking();

    /**
     * @brief Wykonuje jednorazowe sprawdzenie dostępności internetu.
     */
    void checkInternet();

signals:
    /// @brief Sygnał emitowany, gdy zmienia się stan połączenia internetowego.
    /// @param connected True, jeśli internet jest dostępny, false w przeciwnym razie.
    void internetStatusChanged(bool connected);

private:
    QTimer *timer; ///< Timer do cyklicznego sprawdzania połączenia.
    QNetworkAccessManager manager; ///< Obiekt zarządzający zapytaniami sieciowymi.
};

#endif // NETWORK_CHECKER_H
