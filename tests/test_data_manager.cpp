#include <QtTest/QtTest>
#include "../data_manager.h" // Ścieżka do data_manager.h w głównym katalogu
#include <QTemporaryFile>
#include <QJsonArray>
#include <QJsonDocument>

class TestDataManager : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // Inicjalizacja przed wszystkimi testami
    }

    void cleanupTestCase() {
        // Sprzątanie po wszystkich testach
    }

    void testSaveToFile() {
        DataManager dm;
        QSignalSpy saveSuccessSpy(&dm, &DataManager::saveSuccess);
        QSignalSpy saveErrorSpy(&dm, &DataManager::saveError);

        // Przygotuj dane testowe
        QJsonArray data = { QJsonObject{{"key", "value"}} };
        QTemporaryFile tempFile;
        QVERIFY(tempFile.open());

        // Test zapisu
        dm.saveToFile(data, tempFile.fileName());

        // Sprawdź, czy sygnał sukcesu został wyemitowany
        QCOMPARE(saveSuccessSpy.count(), 1);
        QCOMPARE(saveErrorSpy.count(), 0);

        // Sprawdź zawartość pliku
        tempFile.seek(0);
        QByteArray fileContent = tempFile.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(fileContent);
        QVERIFY(!doc.isNull());
        QCOMPARE(doc.array(), data);
    }

    void testLoadFromFile() {
        DataManager dm;
        QSignalSpy loadSuccessSpy(&dm, &DataManager::loadSuccess);
        QSignalSpy loadErrorSpy(&dm, &DataManager::loadError);

        // Przygotuj plik z danymi
        QJsonArray data = { QJsonObject{{"key", "value"}} };
        QTemporaryFile tempFile;
        QVERIFY(tempFile.open());
        QJsonDocument doc(data);
        tempFile.write(doc.toJson());
        tempFile.close();

        // Test wczytywania
        QJsonArray loadedData = dm.loadFromFile(tempFile.fileName());

        // Sprawdź, czy sygnał sukcesu został wyemitowany
        QCOMPARE(loadSuccessSpy.count(), 1);
        QCOMPARE(loadErrorSpy.count(), 0);

        // Sprawdź wczytane dane
        QCOMPARE(loadedData, data);
    }

    void testLoadFromNonExistentFile() {
        DataManager dm;
        QSignalSpy loadSuccessSpy(&dm, &DataManager::loadSuccess);
        QSignalSpy loadErrorSpy(&dm, &DataManager::loadError);

        // Test wczytywania z nieistniejącego pliku
        QJsonArray loadedData = dm.loadFromFile("non_existent_file.json");

        // Sprawdź, czy sygnał błędu został wyemitowany
        QCOMPARE(loadSuccessSpy.count(), 0);
        QCOMPARE(loadErrorSpy.count(), 1);
        QVERIFY(loadedData.isEmpty());
    }

    void testSaveToReadOnlyFile() {
        DataManager dm;
        QSignalSpy saveSuccessSpy(&dm, &DataManager::saveSuccess);
        QSignalSpy saveErrorSpy(&dm, &DataManager::saveError);

        // Przygotuj dane testowe
        QJsonArray data = { QJsonObject{{"key", "value"}} };
        QTemporaryFile tempFile;
        QVERIFY(tempFile.open());
        tempFile.setPermissions(QFileDevice::ReadOwner); // Ustaw plik jako tylko do odczytu
        tempFile.close();

        // Test zapisu
        dm.saveToFile(data, tempFile.fileName());

        // Sprawdź, czy sygnał błędu został wyemitowany
        QCOMPARE(saveSuccessSpy.count(), 0);
        QCOMPARE(saveErrorSpy.count(), 1);
    }
};

QTEST_MAIN(TestDataManager)
#include "test_data_manager.moc" // Ten plik powinien być generowany automatycznie przez AUTOMOC
