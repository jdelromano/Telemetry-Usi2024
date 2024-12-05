#include "telemetry.h"
#include "mainwindow.h"
#include "myQApp.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QVariant>
#include <QDebug>

/*
 * QSettings
 * stores data across application sessions.
 * uses the platform's native format for storing configuration
 *  settings (e.g., registry on Windows, .ini files on Linux/macOS).
*/

#include "kuserfeedback/applicationversionsource.h"
#include "kuserfeedback/compilerinfosource.h"
#include "kuserfeedback/platforminfosource.h"
#include "kuserfeedback/usagetimesource.h"

#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qmutex.h>
#include <qsettings.h>
#include <qtimezone.h>

#include <QObject>
#include <QString>

#include "telemetry.h"
#include "mainwindow.h" // Include if necessary


class MyQApp;
class MainWindow;

Telemetry::Telemetry(QObject *parent)
    : QObject(parent),
    provider(new KUserFeedback::Provider(this)),
    retryCount(0)//,
{
    // Basic settings
    int delay = 1; // Seconds delay
    int periodicFeedback_timeInterval = delay * 1000; // ms*1000 = seconds

    // Load old total time
    QSettings settings;
    totalTimeElapsed = settings.value("total_usage_time", 0).toInt();

    // Set up provider data sources
    provider->addDataSource(new KUserFeedback::UsageTimeSource);
    provider->addDataSource(new KUserFeedback::PlatformInfoSource);
    provider->addDataSource(new KUserFeedback::CompilerInfoSource);
    provider->setSurveyInterval(periodicFeedback_timeInterval);

    // Start the elapsed timer
    elapsedTimer.start();

    // Start periodic feedback timer
    connect(&feedbackTimer, &QTimer::timeout, this, &Telemetry::sendTelemetryData);
    feedbackTimer.start(periodicFeedback_timeInterval);

    // Save the current total usage time
    settings.setValue("total_usage_time", totalTimeElapsed);

    // Send initial telemetry data
    sendTelemetryData();
}

//custom deconstructor for save time
Telemetry::~Telemetry()
{
    // Stop the feedback timer
    feedbackTimer.stop();

    // Update total time elapsed with the remaining time
    int elapsed = elapsedTimer.elapsed();
    totalTimeElapsed += elapsed;

    // Save the total usage time to settings
    QSettings settings;
    settings.setValue("total_usage_time", totalTimeElapsed);
}

void Telemetry::checkAndUpdate(const QString &key, const QString &value) {
    qDebug()<< "appena dentro checkandupdate" << value;
    // Access DB through MyQApp singleton
    QMap<QString, QVariant>& DB = MyQApp::getDb();
    QMap<QString, QVariant>& toSendDB = MyQApp::getToSendDB();

    // Synchronize: Check if the key needs to be staged
    if (toSendDB.contains(key)) {
        // Only update if toSendDB is different from both DB and the new value
        if (toSendDB[key] != DB.value(key) && toSendDB[key] != value) {
            toSendDB[key] = value;
        }
    } else {
        // If not staged, check if the value differs from DB
        if (!DB.contains(key) || DB.value(key) != value) {
            toSendDB[key] = value;  // It is a new value
        }
    }
    qDebug()<<"dentro toSendDB" << toSendDB[key];
}
// Change DB and toSendDB to hold QVariant to support different types
void Telemetry::checkAndUpdateInt(const QString &key, int value) {
    // Access DB and toSendDB through MyQApp singleton
    QMap<QString, QVariant>& DB = MyQApp::getDb();  // Get DB through singleton
    QMap<QString, QVariant>& toSendDB = MyQApp::getToSendDB();  // Get toSendDB through singleton

    // Synchronize: Check if the key needs to be staged
    if (toSendDB.contains(key)) {
        // Only update if toSendDB is different from both DB and the new value
        if (toSendDB[key].toInt() != DB.value(key).toInt() && toSendDB[key].toInt() != value) {
            qDebug() << "Updating int value for key" << key << "from"
                     << toSendDB[key].toInt() << "to" << value;
            toSendDB[key] = value;  // Update value in toSendDB
        }
    } else {
        // If not staged, check if the value differs from DB
        if (!DB.contains(key) || DB.value(key).toInt() != value) {
            qDebug() << "New int key-value pair added: " << key << "->" << value;
            toSendDB[key] = value;  // It is a new value
        }
    }
}

void Telemetry::checkAndUpdateBool(const QString &key, bool value) {
    // Access DB and toSendDB through MyQApp singleton
    QMap<QString, QVariant>& DB = MyQApp::getDb();  // Get DB through singleton
    QMap<QString, QVariant>& toSendDB = MyQApp::getToSendDB();  // Get toSendDB through singleton

    // Synchronize: Check if the key needs to be staged
    if (toSendDB.contains(key)) {
        // Only update if toSendDB is different from both DB and the new value
        if (toSendDB[key].toBool() != DB.value(key).toBool() && toSendDB[key].toBool() != value) {
            qDebug() << "Updating bool value for key" << key << "from"
                     << toSendDB[key].toBool() << "to" << value;
            toSendDB[key] = value;  // Update value in toSendDB
        }
    } else {
        // If not staged, check if the value differs from DB
        if (!DB.contains(key) || DB.value(key).toBool() != value) {
            qDebug() << "New bool key-value pair added: " << key << "->" << value;
            toSendDB[key] = value;  // It is a new value
        }
    }
}

void Telemetry::incCount(QString field) {
    // Retrieve the current value from toSendDB, default to "0" if not present
    int currentValue = MyQApp::toSendDB.value(field, "0").toInt();

    // Retrieve the value from the database
    int dbValue = MyQApp::DB.value(field, "0").toInt(); // Implement or replace with your actual DB retrieval function

    // Update the current value if it is less than the database value
    if (currentValue < dbValue) {
        currentValue = dbValue;
    }

    // Increment the value
    currentValue++;

    // Store the updated value back into toSendDB
    MyQApp::toSendDB[field] = QString::number(currentValue);
}

//to server
void Telemetry::sendTelemetryData()
{
    // Network settings
    QString networkUrl = "http://0.0.0.1:1984/receiver/submit/telemetryapp";
    QString contentType = "application/json";

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request((QUrl(networkUrl)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, contentType);

    // Check if there is data to send
    if (MyQApp::toSendDB.isEmpty()) {
        qDebug() << "No telemetry data to send.";
        delete manager;
        return;
    }

    // Parse and clean map<QString, QVariant> to valid JSON
    QJsonObject jsonData = MapToJSON();

    // Convert QJsonObject to JSON string for POST request
    QJsonDocument jsonDoc(jsonData);
    QByteArray jsonDataBytes = jsonDoc.toJson();

    // Send JSON data via POST
    QNetworkReply *reply = manager->post(request, jsonDataBytes);
    connect(reply, &QNetworkReply::finished, this, [this, reply, manager]() {

        if (reply->error() == QNetworkReply::NoError) { // Success
            QByteArray responseData = reply->readAll();

            // Log success
            qDebug() << "Telemetry data sent successfully. Server response:" << responseData;

            for (auto it = MyQApp::toSendDB.begin(); it != MyQApp::toSendDB.end(); ++it) {
                MyQApp::DB[it.key()] = it.value(); // Replace or insert values in DB
            }

            // Clean up sent data from the database
            MyQApp::toSendDB.clear(); // Clear all elements after successful send
            retryCount = 0;  // Reset retry count on success
        } else {
            // Handle error, display error message
            qDebug() << "Error sending telemetry data:" << reply->errorString();
            qDebug() << "Server response code:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            qDebug() << "Server response body:" << reply->readAll();

            retrySendingData(reply);
        }
        reply->deleteLater();
        delete manager;
    });
}

//language and region -> note, is the user defined setting
QJsonObject createLocaleJson() {
    QLocale locale;
    QJsonObject localeObj;
    localeObj["language"] = locale.languageToString(locale.language());
    localeObj["region"] = locale.territoryToString(locale.territory());
    return localeObj;
}

//fix fields
//local time -> note, is the user defined setting
QJsonObject createTimeZoneJson() {
    QTimeZone timeZone = QTimeZone::systemTimeZone();
    QJsonObject timeZoneObj;

    QDateTime currentDateTime = QDateTime::currentDateTime();

    timeZoneObj["id"] = QString(timeZone.id());  // Timezone ID
    timeZoneObj["offset"] = timeZone.offsetFromUtc(currentDateTime)/3600;  // Timezone offset from UTC
    timeZoneObj["abbreviation"] = timeZone.abbreviation(currentDateTime);  // Timezone abbreviation

    return timeZoneObj;
}

QJsonObject Telemetry::MapToJSON() {
    QJsonObject jsonData;
    if (!mainWindow) {
        return jsonData;
    }

    // Fixed data
    jsonData["feedback"] = "message";
    jsonData["version"] = "Banana.ch";
    jsonData["versionNumber"] = "5";
    KUserFeedback::ApplicationVersionSource versionSource;
    jsonData["applicationVersion"] = QJsonObject{{"value", versionSource.data().toString()}};

    // Time data
    QSettings settings;
    int elapsed = elapsedTimer.elapsed();
    jsonData["sessionTime"] = QJsonObject{{"value", elapsed}};
    totalTimeElapsed = settings.value("total_usage_time", 0).toInt();
    jsonData["TotalUsageTime"] = QJsonObject{{"value", totalTimeElapsed + elapsed}};

    jsonData["timezone"] = createTimeZoneJson();
    jsonData["locale"] = createLocaleJson();

    // Custom data
    QJsonArray customDataArray;

    // Iterate through the data and add entries to the customDataArray
    for (auto it = MyQApp::toSendDB.begin(); it != MyQApp::toSendDB.end(); ++it) {
        QVariant currentValue = it.value();

        QString valueType = it.key();

        // Call createCustomDataJson with the value type (adjust based on your data)
        QJsonObject customEntry = createCustomDataJson(it.key(), currentValue, valueType);
        customDataArray.append(customEntry);
        jsonData[it.key()] = customEntry; // Add the custom entry to main JSON
    }

    jsonData["custom_data"] = customDataArray;

    // Print the indented JSON for debugging
    QString indentedJson = QJsonDocument(jsonData).toJson(QJsonDocument::Indented);
    QStringList jsonLines = indentedJson.split('\n');
    for (const QString& line : jsonLines) {
    }

    return jsonData;
}

// Helper function to handle key-value pairs
QJsonObject Telemetry::createCustomDataJson(const QString& key, const QVariant& value, const QString& valueType) {
    QJsonObject customEntry;
    customEntry["key"] = key;

    // Use the passed valueType instead of hardcoded "unknown"
    if (valueType == "counter" || value.typeId() == QMetaType::Int || key.contains("count", Qt::CaseInsensitive))  {
        customEntry["type"] = "counter";
        customEntry["int"] = value.toInt();
    } else if (valueType == "value" || value.typeId() == QMetaType::Double) {
        customEntry["type"] = "value";
        customEntry["double"] = value.toDouble();
    } else if (valueType == "text" || value.typeId() == QMetaType::QString) {
        customEntry["type"] = "text";
        customEntry["string"] = value.toString();
    } else if ((valueType == "boolean" || value.typeId() == QMetaType::Bool) || key.contains("flag", Qt::CaseInsensitive)) {
        customEntry["type"] = "flag";
        customEntry["bool"] = value.toBool();
    } else {
        customEntry["type"] = valueType;
        customEntry["string"] = value.toString();
    }

    return customEntry;
}

QJsonObject MapToJSON() {
    QJsonObject jsonData;

    // Check if mainWindow is available
    if (!&MyQApp::mainWindow) {
        return jsonData;
    }

    // Fixed data
    jsonData["feedback"] = "message";
    jsonData["version"] = "Banana.ch";
    jsonData["versionNumber"] = "5";

    KUserFeedback::ApplicationVersionSource versionSource;
    jsonData["applicationVersion"] = QJsonObject{{"value", versionSource.data().toString()}};

    QJsonArray customDataArray;

    for (auto it = MyQApp::toSendDB.begin(); it != MyQApp::toSendDB.end(); ++it) {
        QVariant currentValue = it.value();

        QJsonObject customEntry;
        customEntry["key"] = it.key();

        // Check and handle the type dynamically
        if (currentValue.canConvert<int>()) {
            int currentIntValue = currentValue.toInt();
            customEntry["type"] = "counter";
            customEntry["int"] = currentIntValue;
        }
        else if (currentValue.canConvert<double>()) {
            double currentDoubleValue = currentValue.toDouble();
            customEntry["type"] = "counter";
            customEntry["double"] = currentDoubleValue;
        }
        else if (currentValue.canConvert<QString>()) {
            QString currentStringValue = currentValue.toString();
            customEntry["type"] = "text";
            customEntry["string"] = currentStringValue;
        }
        else {
            customEntry["type"] = "unknown";
            customEntry["other"] = currentValue.toString();
        }

        // Add custom entry to the main JSON object
        jsonData[it.key()] = customEntry;
        customDataArray.append(customEntry);
    }

    // Add custom data to the main JSON object
    jsonData["custom_data"] = customDataArray;

    // Convert to indented JSON string
    QString indentedJson = QJsonDocument(jsonData).toJson(QJsonDocument::Indented);

    // Print out the formatted JSON data for debugging
    QStringList jsonLines = indentedJson.split('\n');
    for (const QString& line : jsonLines) {
    }

    return jsonData;
}

QString variantToString(const QVariant &variant)
{
    if (variant.isValid()) {
        return variant.toString();
    }
    return QString("Invalid value");
}


void Telemetry::retrySendingData(QNetworkReply *reply)
{
    if (retryCount < 3) {
        retryCount++;
        qDebug() << "Retrying to send telemetry data... Retry count:" << retryCount;
        QTimer::singleShot(5000, this, &Telemetry::sendTelemetryData);  // Retry after 5 seconds
    } else {
        qDebug() << "Max retries reached. Abandoning telemetry data sending.";
    }
}
