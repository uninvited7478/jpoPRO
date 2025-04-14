import QtQuick 2.15
import QtQuick.Controls 2.15
import QtCharts 2.15

ApplicationWindow {
    id: appWindow
    visible: true
    width: 640
    height: 600
    title: "Monitor Jakości Powietrza"

    property var latestSensorData: []
    property int selectedStationId: -1

    Column {
        anchors.centerIn: parent
        spacing: 12

        Button {
            text: "Pobierz stacje"
            onClicked: apiClient.fetchStations()
        }

        Text {
            text: selectedStationId === -1 ? "Wybierz stację" : "Wybrana stacja ID: " + selectedStationId
        }

        ListView {
            width: parent.width * 0.9
            height: 100
            model: stationModel
            delegate: ItemDelegate {
                text: model.name
                width: parent.width
                onClicked: {
                    selectedStationId = model.id
                    apiClient.fetchSensors(selectedStationId)
                }
            }
        }

        ListView {
            width: parent.width * 0.9
            height: 100
            model: sensorModel
            delegate: ItemDelegate {
                text: model.param
                width: parent.width
                onClicked: {
                    apiClient.fetchSensorData(model.id)
                }
            }
        }


        Row {
            spacing: 10

            Button {
                text: "Zapisz dane"
                onClicked: dataManager.saveToFile(latestSensorData, "dane.json")
            }

            Button {
                text: "Wczytaj dane"
                onClicked: {
                    let json = dataManager.loadFromFile("dane.json")
                    onSensorDataReady(json)
                }
            }

            Button {
                text: "Analiza"
                onClicked: analyzeData(latestSensorData)
            }
        }
    }

    ListModel { id: stationModel }
    ListModel { id: sensorModel }

    Connections {
        target: apiClient

        function onStationsReady(stations) {
            stationModel.clear()
            for (var i = 0; i < stations.length; i++) {
                stationModel.append({
                    id: stations[i].id,
                    name: stations[i].stationName
                })
            }
        }

        function onSensorsReady(sensors) {
            sensorModel.clear()
            for (var i = 0; i < sensors.length; i++) {
                var p = sensors[i].param
                sensorModel.append({
                    id: sensors[i].id,
                    param: p.paramName + " (" + p.paramCode + ")"
                })
            }
        }

        function onSensorDataReady(values) {
            latestSensorData = values
            lineSeries.clear()
            var count = 0
            var maxVal = 0
            for (var i = 0; i < values.length; i++) {
                var val = values[i].value
                if (val !== null) {
                    lineSeries.append(count, val)
                    if (val > maxVal) maxVal = val
                    count++
                }
            }
            xAxis.max = count
            yAxis.max = maxVal + 10
        }

        function onErrorOccurred(msg) {
            console.log("Błąd API: " + msg)
        }
    }

    function analyzeData(values) {
        var minVal = Number.MAX_VALUE
        var maxVal = Number.MIN_VALUE
        var sum = 0
        var count = 0

        for (var i = 0; i < values.length; i++) {
            var v = values[i].value
            if (v !== null) {
                sum += v
                if (v < minVal) minVal = v
                if (v > maxVal) maxVal = v
                count++
            }
        }

        var avg = (count > 0) ? (sum / count).toFixed(2) : "brak danych"
        var trend = (values[0].value < values[values.length - 1].value) ? "wzrost" : "spadek"

        Qt.callLater(() => {
            Qt.createQmlObject(`
                import QtQuick 2.15
                import QtQuick.Controls 2.15
                MessageDialog {
                    title: "Analiza danych"
                    text: "Min: ${minVal}, Max: ${maxVal}, Średnia: ${avg}, Trend: ${trend}"
                    visible: true
                }`, appWindow, "dialog")
        })
    }
}
