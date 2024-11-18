#include "telemetry.h"
#include "mainwindow.h"

//test TODO
/*
 * QSettings
 * stores data across application sessions.
 * uses the platform's native format for storing configuration
 *  settings (e.g., registry on Windows, .ini files on Linux/macOS).
*/

#include <QSettings>
#include <QVariant>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <KUserFeedback/Provider>
#include <KUserFeedback/UsageTimeSource>
#include <KUserFeedback/PlatformInfoSource>
#include <KUserFeedback/CompilerInfoSource>
#include <QSysInfo>
#include <QLocale>
#include <QThread>
#include <QTimeZone>
#include <QUuid>
#include <QTimer>
#include <QElapsedTimer>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <KUserFeedback/Provider>
#include <KUserFeedback/UsageTimeSource>
#include <KUserFeedback/PlatformInfoSource>
#include <KUserFeedback/CompilerInfoSource>
#include <QSysInfo>
#include <QLocale>
#include <QThread>
#include <QTimeZone>

// Constructor for Telemetry, initializing counters and setting up provider
Telemetry::Telemetry(MainWindow* mainWindow, QObject *parent)
    : QObject(parent),
    provider(new KUserFeedback::Provider(this)),
    retryCount(0),
    mainWindow(mainWindow)
{
    // Basic settings
    int periodicFeedback_timeInterval = 1000;  // ms
    int retryCount;

    // Load old total time
    QSettings settings;
    totalTimeElapsed = settings.value("total_usage_time", 0).toInt();  // Load saved time, 0 if not present

    // Set up provider data sources
    provider->addDataSource(new KUserFeedback::UsageTimeSource);
    provider->addDataSource(new KUserFeedback::PlatformInfoSource);
    provider->addDataSource(new KUserFeedback::CompilerInfoSource);

    provider->setSurveyInterval(100000);
    provider->setTelemetryMode(KUserFeedback::Provider::BasicUsageStatistics);

    // Start the elapsed timer
    elapsedTimer.start();  // Start the timer to track usage time

    // Start periodic feedback timer
    connect(&feedbackTimer, &QTimer::timeout, this, &Telemetry::sendTelemetryData);
    feedbackTimer.start(periodicFeedback_timeInterval);

    settings.setValue("total_usage_time", totalTimeElapsed);

    // Send initial telemetry data
    sendTelemetryData();
    qDebug() << "Done";
}

//deconstruct
/*
Telemetry::~Telemetry()
{
    // Save total elapsed time on app close
    int session_time = elapsedTimer.elapsed() / 1000;
    totalTimeElapsed += session_time;

    //TODO: celete or save?
    if(!tempDB.isEmpty()){
        sendTelemetryData();
    }

    //QSettings settings;
    //settings.setValue("total_usage_time", totalTimeElapsed);
};
*/

//data
// go in
void Telemetry::checkAndUpdate(const QString &key, const QString &value) {
    // Access db from MainWindow
    const QMap<QString, QString> &db = mainWindow->getdb();

    // Initialize tempDB entry with db's value or default to "0"
    if (!tempDB.contains(key)) {
        tempDB[key] = db.contains(key) ? db.value(key) : "0";
    }

    int currentValue = tempDB[key].toInt();
    int newValue = value.toInt();

    if (currentValue >= newValue) {
        qDebug() << "No update needed for key:" << key << "Current value:" << currentValue << "New value:" << newValue;
        return;
    }

    // Update tempDB with the new value
    tempDB[key] = value;
    qDebug() << "Updated tempDB. Key:" << key << ", Value:" << value;
}


void Telemetry::incCount(const QString field){
    // Get value from  tempDB ("0" if not found)
    int value = tempDB.value(field, "0").toInt();
    value++;

    //update
    tempDB[field] = QString::number(value);

    qDebug() << "Incremented count for" << field << ":" << value;
    checkAndUpdate(field, QString::number(value));
}

//to server
void Telemetry::sendTelemetryData()
{
    // Network settings
    QString networkUrl = "http://0.0.0.0:1984/receiver/submit/telemetryapp";
    QString contentType = "application/json";

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request((QUrl(networkUrl)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, contentType);

    // Convert map to JSON
    if (tempDB.isEmpty()) {
        qDebug() << "No telemetry data to send.";
        return; // Nothing to send
    }
    QJsonObject jsonData = JSONToMap();
    // Convert QJsonObject to JSON string for POST request
    QJsonDocument jsonDoc(jsonData);
    QByteArray jsonDataBytes = jsonDoc.toJson();

    // Send JSON data via POST
    QNetworkReply *reply = manager->post(request, jsonDataBytes);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            // Successful request, handle response
            QByteArray responseData = reply->readAll();  // Read server's response
            qDebug() << "Telemetry data sent successfully. Server response:" << responseData;

            retryCount = 0;  // Reset retry count on success
        } else {
            // Handle error, display error message
            qDebug() << "Error sending telemetry data:" << reply->errorString();
            qDebug() << "Server response code:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            qDebug() << "Server response body:" << reply->readAll();

            retrySendingData(reply);
        }
        reply->deleteLater();
    });
}

QJsonObject Telemetry::JSONToMap() {
    QJsonObject jsonData;

    if (!mainWindow) {
        qDebug() << "MainWindow not available. Cannot update mainDB.";
        return jsonData;
    }

    for (auto it = tempDB.begin(); it != tempDB.end();) {
        // Fetch current value from mainDB
        QString currentValue = mainWindow->getDBValue(it.key());
        int currentIntValue = currentValue.toInt();

        // Add the value from tempDB
        int newValue = currentIntValue + it.value().toInt();

        // Create JSON entry
        QJsonObject counterObject;
        counterObject["counter"] = QString::number(newValue);

        // Insert into the JSON object
        jsonData.insert(it.key(), counterObject);

        // Update mainDB with the new value
        mainWindow->updateDB(it.key(), QString::number(newValue));

        // Remove the entry from tempDB
        it = tempDB.erase(it);
    }

    // Log the JSON data for debugging
    qDebug() << "Converted map to JSON:" << QJsonDocument(jsonData).toJson(QJsonDocument::Indented);

    return jsonData;
}


void Telemetry::retrySendingData(QNetworkReply *reply)
{
    if (retryCount < 3) {
        retryCount++;
        qDebug() << "Retrying to send telemetry data... Retry count:" << retryCount;
        //QTimer::singleShot(5000, this, &Telemetry::sendTelemetryData);  // Retry after 5 seconds
    } else {
        qDebug() << "Max retries reached. Abandoning telemetry data sending.";
    }
}
/*
void Telemetry::sendPeriodicFeedback()

    sendTelemetryData();
    QJsonObject changedData;

    auto checkAndUpdate(const QString &key, int &lastValue, int currentValue) {
        if (lastValue != currentValue) {
            QJsonObject counterObject;
            counterObject["counter"] = QString::number(currentValue);
            changedData[key] = counterObject;
            lastValue = currentValue;
        }
    };
}*/


//test
// Function to send telemetry data
/*
void Telemetry::sendTelemetryData()
{
    // Network settings
    QString networkUrl = "http://localhost:1984/receiver/submit/telemetryapp";
    QString contentType = "application/json";

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request((QUrl(networkUrl)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, contentType);

    // Convert QMap to QJsonObject
    QJsonObject jsonData;
    for (auto it = tempDB.begin(); it != tempDB.end(); ++it) {
        jsonData.insert(it.key(), QJsonValue::fromVariant(it.value().first));
        if (it.value().second) {  // Only add if 'bool' value is true
            jsonData.insert(it.key(), QJsonValue::fromVariant(it.value().first));
        }
    }

    // Send JSON data
    QNetworkReply *reply = manager->post(request, QJsonDocument(jsonData).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "Telemetry tempDB sent successfully.";
            retryCount = 0;  // Reset retry count on success
        } else {
            qDebug() << "Error sending telemetry data:" << reply->errorString();
            retrySendingData(reply);
        }
        reply->deleteLater();
    });
}
*/
