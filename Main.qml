/**
 * @file Main.qml
 * @brief Główny plik QML aplikacji do monitorowania jakości powietrza.
 *
 * Ten plik definiuje interfejs użytkownika aplikacji, w tym wybór stacji i czujników,
 * wyświetlanie danych na wykresach oraz analizę danych.
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtCharts 2.15
import QtQuick.Dialogs

/**
 * @brief Główny komponent okna aplikacji.
 */
ApplicationWindow {
    id: appWindow
    visible: true
    width: 800
    height: 700
    minimumWidth: 600
    title: "Monitor Jakości Powietrza"

    property var latestSensorData: [] ///< Najnowsze dane z czujnika.
    property var loadedSensorData: [] ///< Wczytane dane z pliku.
    property int selectedStationId: -1 ///< Identyfikator wybranej stacji.
    property int selectedSensorId: -1 ///< Identyfikator wybranego czujnika.
    property string selectedSensorParam: "Wybierz czujnik" ///< Nazwa wybranego parametru czujnika.
    property bool internetAvailable: true ///< Flaga dostępności internetu.
    property bool wasDisconnected: false ///< Flaga wskazująca, czy wcześniej utracono połączenie.
    property string selectedStationName: ""

    /**
     * @brief Pasek stanu sieci.
     *
     * Wyświetlany, gdy brak połączenia z internetem.
     */
    Rectangle {
        id: networkStatusBar
        visible: !internetAvailable
        color: "red"
        height: 30
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        z: 1
        Text {
            anchors.centerIn: parent
            text: "Brak połączenia z internetem!"
            color: "white"
            font.bold: true
        }
    }

    /**
     * @brief Główny układ elementów interfejsu.
     */
    Column {
        id: mainColumn
        anchors.centerIn: parent
        spacing: 15
        anchors.topMargin: internetAvailable ? 0 : 30

        Component.onCompleted: {
            console.log("Main Column parent:", parent)
            console.log("Main Column width:", width)
        }

        Button {
            text: "Pobierz stacje"
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: apiClient.fetchStations()
        }

        Row {
            spacing: 20

            Column {
                width: appWindow.width * 0.45
                spacing: 5

                Text {
                    text: selectedStationId === -1 ? "Wybierz stację" : "Wybrana stacja: " + selectedStationName
                    font.bold: true
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width
                }

                ListView {
                    id: stationList
                    height: 100
                    model: stationModel
                    clip: true
                    Binding {
                        target: stationList
                        property: "width"
                        value: appWindow.width * 0.45
                        when: appWindow.width > 0
                    }
                    delegate: ItemDelegate {
                        text: model.name
                        width: ListView.view.width
                        onClicked: {
                            selectedStationId = model.id
                            selectedStationName = model.name
                            apiClient.fetchSensors(selectedStationId)
                        }
                    }
                    Component.onCompleted: {
                        console.log("Station ListView parent:", parent)
                        console.log("Station ListView parent.width:", parent ? parent.width : "null")
                    }
                }
            }

            Column {
                width: appWindow.width * 0.45
                spacing: 5

                Text {
                    text: "Wybierz czynnik"
                    font.bold: true
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width
                }

                ListView {
                    id: sensorList
                    height: 100
                    model: sensorModel
                    clip: true
                    Binding {
                        target: sensorList
                        property: "width"
                        value: appWindow.width * 0.45
                        when: appWindow.width > 0
                    }
                    delegate: ItemDelegate {
                        text: model.param
                        width: parent.width
                        onClicked: {
                            selectedSensorId = model.id
                            selectedSensorParam = model.param
                            apiClient.fetchSensorData(model.id)
                        }
                    }
                    Component.onCompleted: {
                        console.log("Sensor ListView parent:", parent)
                        console.log("Sensor ListView parent.width:", parent ? parent.width : "null")
                    }
                }
            }
        }

        Row {
            spacing: 20

            Column {
                width: appWindow.width * 0.45
                spacing: 5

                Text {
                    text: selectedSensorParam
                    font.bold: true
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width
                }

                Canvas {
                    id: customChart
                    height: 350
                    width: parent.width
                    property var dataPoints: []

                    onPaint: {
                        if (dataPoints.length < 2 || width === 0 || height === 0)
                            return

                        var ctx = getContext("2d")
                        ctx.clearRect(0, 0, width, height)

                        var maxVal = Math.max.apply(Math, dataPoints)
                        var minVal = Math.min.apply(Math, dataPoints)
                        var range = maxVal - minVal || 1

                        // Rysuj siatkę
                        ctx.strokeStyle = "#cccccc"
                        ctx.lineWidth = 1
                        var xStep = width / 10
                        for (var x = 0; x <= width; x += xStep) {
                            ctx.beginPath()
                            ctx.moveTo(x, 0)
                            ctx.lineTo(x, height)
                            ctx.stroke()
                        }
                        var yStep = height / 5
                        for (var y = 0; y <= height; y += yStep) {
                            ctx.beginPath()
                            ctx.moveTo(0, y)
                            ctx.lineTo(width, y)
                            ctx.stroke()
                            // Etykiety osi Y
                            ctx.fillStyle = "black"
                            ctx.font = "12px Arial"
                            var val = maxVal - (y / height) * range
                            ctx.fillText(val.toFixed(1), 10, y + 12)
                        }

                        // Rysuj dane
                        ctx.strokeStyle = "blue"
                        ctx.lineWidth = 2
                        ctx.beginPath()
                        for (var i = 0; i < dataPoints.length; i++) {
                            var x = i * (width / (dataPoints.length - 1))
                            var y = height - ((dataPoints[i] - minVal) * height) / range
                            if (i === 0)
                                ctx.moveTo(x, y)
                            else
                                ctx.lineTo(x, y)
                        }
                        ctx.stroke()
                    }

                    /**
                     * @brief Aktualizuje wykres na podstawie nowych danych.
                     * @param values Tablica wartości do wyświetlenia na wykresie.
                     */
                    function updateChart(values) {
                        if (width === 0 || height === 0) {
                            console.log("Canvas not ready yet")
                            Qt.callLater(() => updateChart(values))
                            return
                        }
                        if (values.length === 0) {
                            console.log("No data to display")
                            dataPoints = []
                            requestPaint()
                            return
                        }
                        dataPoints = values
                        requestPaint()
                    }

                    Component.onCompleted: {
                        console.log("Canvas parent:", parent)
                        console.log("Canvas parent.width:", parent ? parent.width : "null")
                        requestPaint()
                    }
                }
            }

            Column {
                width: appWindow.width * 0.45
                spacing: 5

                Text {
                    text: "Wczytane dane"
                    font.bold: true
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width
                }

                Canvas {
                    id: loadedChart
                    height: 350
                    width: parent.width
                    property var dataPoints: []

                    onPaint: {
                        if (dataPoints.length < 2 || width === 0 || height === 0)
                            return

                        var ctx = getContext("2d")
                        ctx.clearRect(0, 0, width, height)

                        var maxVal = Math.max.apply(Math, dataPoints)
                        var minVal = Math.min.apply(Math, dataPoints)
                        var range = maxVal - minVal || 1

                        // Rysuj siatkę
                        ctx.strokeStyle = "#cccccc"
                        ctx.lineWidth = 1
                        var xStep = width / 10
                        for (var x = 0; x <= width; x += xStep) {
                            ctx.beginPath()
                            ctx.moveTo(x, 0)
                            ctx.lineTo(x, height)
                            ctx.stroke()
                        }
                        var yStep = height / 5
                        for (var y = 0; y <= height; y += yStep) {
                            ctx.beginPath()
                            ctx.moveTo(0, y)
                            ctx.lineTo(width, y)
                            ctx.stroke()
                            // Etykiety osi Y
                            ctx.fillStyle = "black"
                            ctx.font = "12px Arial"
                            var val = maxVal - (y / height) * range
                            ctx.fillText(val.toFixed(1), 10, y + 12)
                        }

                        // Rysuj dane
                        ctx.strokeStyle = "red"
                        ctx.lineWidth = 2
                        ctx.beginPath()
                        for (var i = 0; i < dataPoints.length; i++) {
                            var x = i * (width / (dataPoints.length - 1))
                            var y = height - ((dataPoints[i] - minVal) * height) / range
                            if (i === 0)
                                ctx.moveTo(x, y)
                            else
                                ctx.lineTo(x, y)
                        }
                        ctx.stroke()
                    }

                    /**
                     * @brief Aktualizuje wykres na podstawie wczytanych danych.
                     * @param values Tablica wartości do wyświetlenia na wykresie.
                     */
                    function updateChart(values) {
                        if (width === 0 || height === 0) {
                            console.log("Loaded Canvas not ready yet")
                            Qt.callLater(() => updateChart(values))
                            return
                        }
                        if (values.length === 0) {
                            console.log("No loaded data to display")
                            dataPoints = []
                            requestPaint()
                            return
                        }
                        dataPoints = values
                        requestPaint()
                    }

                    Component.onCompleted: {
                        console.log("Loaded Canvas parent:", parent)
                        console.log("Loaded Canvas parent.width:", parent ? parent.width : "null")
                        requestPaint()
                    }
                }
            }
        }

        Row {
            spacing: 10
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                text: "Zapisz dane"
                onClicked: {
                    if (selectedStationId === -1 || selectedSensorId === -1) {
                        resultDialog.title = "Błąd"
                        resultDialog.text = "Najpierw wybierz stację i czujnik!"
                        resultDialog.open()
                    } else {
                        dataManager.saveToFile(latestSensorData, "dane.json")
                    }
                }
            }

            Button {
                text: "Wczytaj dane"
                onClicked: {
                    let json = dataManager.loadFromFile("dane.json")
                    displayLoadedData(json)
                }
            }

            Button {
                text: "Analiza"
                onClicked: analyzeData(latestSensorData, loadedSensorData)
            }
        }
    }

    ListModel { id: stationModel } ///< Model przechowujący listę stacji.
    ListModel { id: sensorModel } ///< Model przechowujący listę czujników.

    MessageDialog {
        id: resultDialog
        title: "Informacja"
        text: ""
        visible: false
        onAccepted: visible = false
    }

    Connections {
        target: networkStatus
        function onInternetStatusChanged(connected) {
            internetAvailable = connected
            if (!connected && !wasDisconnected) {
                resultDialog.title = "Błąd połączenia"
                resultDialog.text = "Brak internetu"
                resultDialog.open()
                wasDisconnected = true
            } else if (connected) {
                wasDisconnected = false
            }
        }
    }

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
            displaySensorData(values)
        }

        function onErrorOccurred(msg) {
            resultDialog.title = "Błąd"
            resultDialog.text = "Wystąpił błąd: " + msg
            resultDialog.open()
        }
    }

    Connections {
        target: dataManager
        function onSaveSuccess() {
            var sensorParam = selectedSensorParam !== "Wybierz czujnik" ? selectedSensorParam : "Nieznany parametr"
            resultDialog.title = "Sukces"
            resultDialog.text = "Zapisano dane dla stacji ID: " + selectedStationId + ", parametr: " + sensorParam
            resultDialog.open()
        }

        function onSaveError(msg) {
            resultDialog.title = "Błąd zapisu"
            resultDialog.text = msg
            resultDialog.open()
        }

        function onLoadSuccess() {
            resultDialog.title = "Sukces"
            resultDialog.text = "Wczytano dane z pliku dane.json"
            resultDialog.open()
        }

        function onLoadError(msg) {
            resultDialog.title = "Błąd odczytu"
            resultDialog.text = msg
            resultDialog.open()
        }
    }

    /**
     * @brief Analizuje bieżące i wczytane dane, wyświetlając wyniki w oknie dialogowym.
     * @param currentData Bieżące dane z czujnika.
     * @param loadedData Wczytane dane z pliku.
     */
    function analyzeData(currentData, loadedData) {
        var resultText = ""

        try {
            // Logowanie dla debugowania
            console.log("analyzeData: currentData length:", currentData.length)
            console.log("analyzeData: loadedData length:", loadedData.length)
            for (var i = 0; i < currentData.length; i++) {
                console.log("currentData[" + i + "].value:", currentData[i].value)
            }
            for (var j = 0; j < loadedData.length; j++) {
                console.log("loadedData[" + j + "].value:", loadedData[j].value)
            }

            // Analiza bieżących danych
            var currentMin = Number.MAX_VALUE
            var currentMax = Number.MIN_VALUE
            var currentSum = 0
            var currentCount = 0
            var validValues = []

            try {
                for (var i = 0; i < currentData.length; i++) {
                    var v = currentData[i].value
                    if (v !== null && v !== undefined) {
                        validValues.push(v)
                        currentSum += v
                        if (v < currentMin) currentMin = v
                        if (v > currentMax) currentMax = v
                        currentCount++
                    }
                }
            } catch (error) {
                console.log("analyzeData: Error processing currentData:", error.message)
                resultText += "Bieżące dane: Błąd przetwarzania danych (" + error.message + ")\n"
                currentMin = Number.MAX_VALUE
                currentMax = Number.MIN_VALUE
                currentAvg = "brak danych"
                currentTrend = "brak danych"
            }

            console.log("analyzeData: currentCount:", currentCount)
            console.log("analyzeData: validValues:", validValues)

            var currentAvg = (currentCount > 0) ? (currentSum / currentCount).toFixed(2) : "brak danych"
            var currentTrend = (validValues.length >= 2)
                ? (validValues[0] < validValues[validValues.length - 1] ? "wzrost" : "spadek")
                : "brak danych"

            if (!resultText.includes("Błąd przetwarzania danych")) {
                resultText += "Bieżące dane:\n"
                resultText += "Min: " + (currentMin === Number.MAX_VALUE ? "brak danych" : currentMin) + "\n"
                resultText += "Max: " + (currentMax === Number.MIN_VALUE ? "brak danych" : currentMax) + "\n"
                resultText += "Średnia: " + currentAvg + "\n"
                resultText += "Trend: " + currentTrend + "\n"
            }

            // Analiza wczytanych danych (jeśli istnieją)
            if (loadedData.length > 0) {
                var loadedMin = Number.MAX_VALUE
                var loadedMax = Number.MIN_VALUE
                var loadedSum = 0
                var loadedCount = 0
                var loadedValidValues = []

                try {
                    for (var j = 0; j < loadedData.length; j++) {
                        var w = loadedData[j].value
                        if (w !== null && w !== undefined) {
                            loadedValidValues.push(w)
                            loadedSum += w
                            if (w < loadedMin) loadedMin = w
                            if (w > loadedMax) loadedMax = w
                            loadedCount++
                        }
                    }
                } catch (error) {
                    console.log("analyzeData: Error processing loadedData:", error.message)
                    resultText += "\nWczytane dane: Błąd przetwarzania danych (" + error.message + ")"
                    loadedMin = Number.MAX_VALUE
                    loadedMax = Number.MIN_VALUE
                    loadedAvg = "brak danych"
                    loadedTrend = "brak danych"
                }

                console.log("analyzeData: loadedCount:", loadedCount)
                console.log("analyzeData: loadedValidValues:", loadedValidValues)

                var loadedAvg = (loadedCount > 0) ? (loadedSum / loadedCount).toFixed(2) : "brak danych"
                var loadedTrend = (loadedValidValues.length >= 2)
                    ? (loadedValidValues[0] < loadedValidValues[loadedValidValues.length - 1] ? "wzrost" : "spadek")
                    : "brak danych"

                if (!resultText.includes("Wczytane dane: Błąd przetwarzania danych")) {
                    resultText += "\nWczytane dane:\n"
                    resultText += "Min: " + (loadedMin === Number.MAX_VALUE ? "brak danych" : loadedMin) + "\n"
                    resultText += "Max: " + (loadedMax === Number.MIN_VALUE ? "brak danych" : loadedMax) + "\n"
                    resultText += "Średnia: " + loadedAvg + "\n"
                    resultText += "Trend: " + loadedTrend
                }
            } else {
                resultText += "\nWczytane dane: brak"
            }
        } catch (error) {
            console.log("analyzeData: General error:", error.message)
            resultText = "Błąd analizy danych: " + error.message
        }

        resultDialog.title = "Analiza danych"
        resultDialog.text = resultText
        resultDialog.open()
    }

    /**
     * @brief Wyświetla bieżące dane z czujnika na wykresie.
     * @param values Tablica wartości z czujnika.
     */
    function displaySensorData(values) {
        try {
            latestSensorData = values
            var cleanData = []

            console.log("displaySensorData: values length:", values.length)
            for (var i = 0; i < values.length; i++) {
                console.log("displaySensorData: values[" + i + "].value:", values[i].value)
                var val = values[i].value
                if (val !== null && val !== undefined) {
                    cleanData.push(val)
                }
            }
            console.log("displaySensorData: cleanData:", cleanData)

            customChart.updateChart(cleanData)
        } catch (error) {
            console.log("displaySensorData: Error:", error.message)
            resultDialog.title = "Błąd wyświetlania danych"
            resultDialog.text = "Nie udało się wyświetlić bieżących danych: " + error.message
            resultDialog.open()
            customChart.updateChart([])
        }
    }

    /**
     * @brief Wyświetla wczytane dane na wykresie.
     * @param values Tablica wczytanych wartości.
     */
    function displayLoadedData(values) {
        try {
            loadedSensorData = values
            var cleanData = []

            console.log("displayLoadedData: values length:", values.length)
            for (var i = 0; i < values.length; i++) {
                console.log("displayLoadedData: values[" + i + "].value:", values[i].value)
                var val = values[i].value
                if (val !== null && val !== undefined) {
                    cleanData.push(val)
                }
            }
            console.log("displayLoadedData: cleanData:", cleanData)

            loadedChart.updateChart(cleanData)
        } catch (error) {
            console.log("displayLoadedData: Error:", error.message)
            resultDialog.title = "Błąd wyświetlania danych"
            resultDialog.text = "Nie udało się wyświetlić wczytanych danych: " + error.message
            resultDialog.open()
            loadedChart.updateChart([])
        }
    }
}
