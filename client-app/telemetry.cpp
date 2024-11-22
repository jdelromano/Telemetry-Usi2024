#include "telemetry.h"
#include "mainwindow.h"
#include "myQApp.h"

/*
 * QSettings
 * stores data across application sessions.
 * uses the platform's native format for storing configuration
 *  settings (e.g., registry on Windows, .ini files on Linux/macOS).
*/

#include "telemetry.h"
#include "kuserfeedback/applicationversionsource.h"
#include "kuserfeedback/compilerinfosource.h"
#include "kuserfeedback/platforminfosource.h"
#include "kuserfeedback/usagetimesource.h"
#include "mainwindow.h"
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qmutex.h>
#include <qsettings.h>
#include <qtimezone.h>

Telemetry::Telemetry(QObject *parent)
    : QObject(parent),
    provider(new KUserFeedback::Provider(this)),
    retryCount(0)//,
    //toSendDB(QMap<QString, QString>()) // Proper initialization of toSendDB
{
    // Basic settings
    int delay = 10; // Seconds delay
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
/*
void Telemetry::checkAndUpdate(const QString &key, const QString &value) {
    QMap<QString, QString> &db = mainWindow->getdb();

    // Synchronize: Check if the key needs to be staged
    // If it is staged
    if (toSendDB.contains(key)) {
        // Only update if toSendDB is different from both db and the new value
        if (toSendDB[key] != db.value(key) && toSendDB[key] != value) {
            toSendDB[key] = value;
        }
    } else {
        // If not staged, check if the value differs from db
        if (!db.contains(key) || db.value(key) != value) {
            //toSendDB[key] = value; // it is a new value
        }
    }
}*/


//Setters
// Check and update toSendDB
//if the value is already in the database, dosen't update the array to be send
/*void Telemetry::checkAndUpdate( QString &key,  QString &value) {
     QMap<QString, QString> &db = mainWindow->getdb();

    // Synchronize: Check if the key needs to be staged
    // if it is staged
    if (toSendDB.contains(key)) {
        // Only update if toSendDB is different from both db and the new value
        if (toSendDB[key] != db.value(key) && toSendDB[key] != value) {
            toSendDB[key] = value;
        }
    } else {
        // If not staged, check if the value differs from db
        if (!db.contains(key) || db.value(key) != value) {
            toSendDB[key] = value; // it is a new value
        }
    }
}
*/
QMutex mutex;

void Telemetry::incCount( QString field) {
    //if (!toSendDB.contains(field)) {
      //  toSendDB[field] = "0";
    //}
    //if (!toSendDB.isEmpty() && toSendDB.contains(field)) {
        // Proceed with operations
   // } else {
     //   qDebug() << "toSendDB is either empty or uninitialized";
    //}
    mutex.lock();
    //int value = toSendDB.value(field, "0").toInt();
    //value++;
    //toSendDB[field] = QString::number(value);
    mutex.unlock();

    //toSendDB[field] = QString::number(value);

    // Call checkAndUpdate (ensure no recursive issues occur here)
    //checkAndUpdate(field, QString::number(value));
}

#include "telemetry.h"
#include "myQApp.h"  // Include MyQApp to access the singleton

void Telemetry::checkAndUpdate(const QString &key, const QString &value) {
    // Access db through MyQApp singleton
    QMap<QString, QString>& db = MyQApp::getDb();  // Get db through singleton
    QMap<QString, QString>& toSendDB = MyQApp::getToSendDB();  // Get toSendDB through singleton

    // Synchronize: Check if the key needs to be staged
    if (toSendDB.contains(key)) {
        // Only update if toSendDB is different from both db and the new value
        if (toSendDB[key] != db.value(key) && toSendDB[key] != value) {
            toSendDB[key] = value;
        }
    } else {
        // If not staged, check if the value differs from db
        if (!db.contains(key) || db.value(key) != value) {
            toSendDB[key] = value;  // It is a new value
        }
    }
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


    //sender empty == no new things
    if (MyQApp::toSendDB.isEmpty()) {
        qDebug() << "No telemetry data to send.";
        delete manager;
        return; // Nothing to send
    }

    //parse and clean map<QString, QString> to valid JSon
    QJsonObject jsonData = MapToJSON();

    // Convert QJsonObject to JSON string for POST request
    QJsonDocument jsonDoc(jsonData);
    QByteArray jsonDataBytes = jsonDoc.toJson();

    // Send JSON data via POST
    QNetworkReply *reply = manager->post(request, jsonDataBytes);
    connect(reply, &QNetworkReply::finished, this, [this, reply, manager]() {
        if (reply->error() == QNetworkReply::NoError) { //success
            QByteArray responseData = reply->readAll();

            // Read server's response
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
        delete manager;
    });
}

//map<QString, QString> to JSon parser + auxiliary

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

//language and region -> note, is the user defined setting
QJsonObject createLocaleJson() {
    QLocale locale;
    QJsonObject localeObj;
    localeObj["language"] = locale.languageToString(locale.language());
    localeObj["region"] = locale.territoryToString(locale.territory());
    return localeObj;
}

//the actual parser
QJsonObject Telemetry::MapToJSON() {

    QJsonObject jsonData;
    if (!mainWindow) {
        return jsonData;
    }

    //fixed data
    jsonData["feedback"] = "message";

     //use a custom method for retrieve the banana app version
    //these below can be used even for custom extra messages
    jsonData["version"] = "Banana.ch";
    jsonData["versionNumber"]= "5";
    KUserFeedback::ApplicationVersionSource versionSource;
    jsonData["applicationVersion"] = QJsonObject{{"value", versionSource.data().toString()}};

    //time
    //QSettings settings;
    //int elapsed = elapsedTimer.elapsed();
    //jsonData["sessionTime"] =  QJsonObject{{"value", elapsed}};
    // Load old total time
    //totalTimeElapsed = settings.value("total_usage_time", 0).toInt();
    //jsonData["TotalUsageTime"] = QJsonObject{{"value",  totalTimeElapsed+elapsed}};  // Total time used (in seconds)

    jsonData["timezone"] = createTimeZoneJson();
    jsonData["locale"] = createLocaleJson();

    //custom data
    QJsonArray customDataArray;

    for (auto it = MyQApp::toSendDB.begin(); it != MyQApp::toSendDB.end(); ++it) {
        QString currentValue = mainWindow->getDBValue(it.key());

        // Check if the value is numeric
        bool isNumber = false;
        int currentIntValue = currentValue.toInt(&isNumber);

        QJsonObject customEntry;
        customEntry["key"] = it.key();

        if (isNumber) {
            // Add numerical value to the counter
            int newValue = currentIntValue + it.value().toInt();

            // JSON entry as "counter"
            QJsonObject counterObject;
            counterObject["counter"] = QString::number(newValue);
            jsonData.insert(it.key(), counterObject);

            // Add to the custom data array
            customEntry["type"] = "counter";
            customEntry["value"] = newValue;

            // Update mainDB
            mainWindow->setDB(it.key(), QString::number(newValue));
        } else {
            // JSON entry as "text"
            QJsonObject textObject;
            textObject["text"] = currentValue;
            jsonData.insert(it.key(), textObject);

            // Add to the custom data array
            customEntry["type"] = "text";
            customEntry["value"] = currentValue;
        }

        // Append to the custom data array
        customDataArray.append(customEntry);
    }

    // Clear the entire toSendDB after processing
    //toSendDB.clear();

    // Add custom data to the main JSON object
    jsonData["custom_data"] = customDataArray;

    // Log the JSON data for debugging
    qDebug() << "Converted map to JSON:" << QJsonDocument(jsonData).toJson(QJsonDocument::Indented);

    return jsonData;
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
