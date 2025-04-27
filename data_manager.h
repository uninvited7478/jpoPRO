#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <QObject>
#include <QJsonArray>

/**
 * @class DataManager
 * @brief Klasa odpowiedzialna za zarządzanie zapisywaniem i wczytywaniem danych w formacie JSON.
 *
 * DataManager umożliwia zapisywanie tablic JSON do plików oraz wczytywanie ich z powrotem.
 * Emituje sygnały w przypadku sukcesu lub błędu, które są używane w interfejsie QML.
 */
class DataManager : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Konstruktor klasy DataManager.
     * @param parent Wskaźnik na obiekt nadrzędny (domyślnie nullptr).
     */
    explicit DataManager(QObject *parent = nullptr);

    /**
     * @brief Zapisuje tablicę JSON do pliku.
     * @param data Tablica JSON do zapisania.
     * @param filename Ścieżka do pliku, w którym dane zostaną zapisane.
     */
    Q_INVOKABLE void saveToFile(QJsonArray data, const QString &filename);

    /**
     * @brief Wczytuje tablicę JSON z pliku.
     * @param filename Ścieżka do pliku, z którego dane zostaną wczytane.
     * @return Wczytana tablica JSON lub pusta tablica w przypadku błędu.
     */
    Q_INVOKABLE QJsonArray loadFromFile(const QString &filename);

signals:
    /// @brief Sygnał emitowany po pomyślnym zapisaniu danych do pliku.
    void saveSuccess();

    /// @brief Sygnał emitowany w przypadku błędu podczas zapisu.
    /// @param message Komunikat błędu.
    void saveError(QString message);

    /// @brief Sygnał emitowany po pomyślnym wczytaniu danych z pliku.
    void loadSuccess();

    /// @brief Sygnał emitowany w przypadku błędu podczas wczytywania.
    /// @param message Komunikat błędu.
    void loadError(QString message);
};

#endif
