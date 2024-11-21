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
#include <KUserFeedback/ApplicationVersionSource>
#include <KUserFeedback/CpuInfoSource>
#include <KUserFeedback/LocaleInfoSource>
#include <QSysInfo>
#include <QLocale>
#include <QThread>
#include <QTimeZone>
#include <qcheckbox.h>
#include <qjsonarray.h>

// Constructor for Telemetry, initializing counters and setting up provider
Telemetry::Telemetry(MainWindow* mainWindow,  QObject *parent)
    : QObject(parent),
    provider(new KUserFeedback::Provider(this)),
    retryCount(0),
    mainWindow(mainWindow)
{
    // Basic settings
    int delay = 1; //seconds delay
    int periodicFeedback_timeInterval = delay*1000;  // ms*1000= seconds

    // Load old total time
    QSettings settings;
    totalTimeElapsed = settings.value("total_usage_time", 0).toInt();
    qDebug() << "[Telemetry] Initial total usage time (ms):" << totalTimeElapsed;


    // Set up provider data sources
    provider->addDataSource(new KUserFeedback::UsageTimeSource);
    provider->addDataSource(new KUserFeedback::PlatformInfoSource);
    provider->addDataSource(new KUserFeedback::CompilerInfoSource);
    provider->setSurveyInterval(delay*1000); // ms*1000= seconds

    // Start the elapsed timer
    elapsedTimer.start();


    // Start periodic feedback timer
    connect(&feedbackTimer, &QTimer::timeout, this, &Telemetry::sendTelemetryData);
    feedbackTimer.start(periodicFeedback_timeInterval);

    // Save the current total usage time
    settings.setValue("total_usage_time", totalTimeElapsed);
    qDebug() << "[Telemetry] Initial total usage time (ms):" << totalTimeElapsed;

    // Send initial telemetry data
    sendTelemetryData();
    qDebug() << "Telemetry initialized and first data sent.";
}

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

// Check and update tempDB
void Telemetry::checkAndUpdate(const QString &key, const QString &value) {
    const QMap<QString, QString> &db = mainWindow->getdb();

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


void Telemetry::checkAndUpdateString(const QString &key, const QString &value) {
    tempDB[key] = value;  // Simply insert or update the value in the map
    qDebug() << "Set tempDB. Key:" << key << ", Value (Structured):" << value;
}

void Telemetry::switchFlag(const QString field) {

    // Get a reference to the main database
    const QMap<QString, QString> &db = mainWindow->getdb();

    qDebug() << "Processing field:" << field;

    // Use the field name as the key
    QString key = field;

    // Check if the key exists in tempDB
    if (!tempDB.contains(key)) {
        // If it doesn't exist, initialize it with a value from db or a default value
        tempDB[key] = db.contains(key) ? db.value(key) : "0";
    }

    // Debugging output to confirm the result
    qDebug() << "Updated tempDB for key:" << key << "Value:" << tempDB[key];
}

void Telemetry::incCount(const QString field){
    // Get from  tempDB ("0" if not found)
    int value = tempDB.value(field, "0").toInt();
    value++;

    //update
    tempDB[field] = QString::number(value);

    qDebug() << "Incremented count for" << field << ":" << value;
    checkAndUpdate(field, QString::number(value));
}

/*
void Telemetry::checkAndUpdateBoolean(const QString &key, const QString &value) {
    const QMap<QString, QString> &db = mainWindow->getdb();

    if (!tempDB.contains(key)) {
        tempDB[key] = db.contains(key) ? db.value(key) : "false";  // Default to "false"
    }

    QString currentValue = tempDB[key];
    QString newValue = value.toLower();

    // Only update if the current value is different from the new value
    if (currentValue == newValue) {
        qDebug() << "No update needed for key:" << key << "Current value:" << currentValue << "New value:" << newValue;
        return;
    }

    // Update tempDB with the new boolean value
    tempDB[key] = newValue;
    qDebug() << "Updated tempDB (boolean). Key:" << key << ", Value:" << newValue;
}
*/

//to server
void Telemetry::sendTelemetryData()
{
    // Network settings
    QString networkUrl = "http://0.0.0.0:1984/receiver/submit/telemetryapp";
    QString contentType = "application/json";

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request((QUrl(networkUrl)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, contentType);

    // Add checkbox states to telemetry
    QJsonObject jsonData = MapToJSON();
    /*
    if (dialog) {
        QJsonObject checkboxStates = dialog->getCheckboxStates();
        jsonData["checkboxStates"] = checkboxStates;
    }
*/
    // Convert map to JSON
    if (tempDB.isEmpty()) {
        qDebug() << "No telemetry data to send.";
        delete manager;
        return; // Nothing to send
    }
    //QJsonObject jsonData = MapToJSON();
    // Convert QJsonObject to JSON string for POST request
    QJsonDocument jsonDoc(jsonData);
    QByteArray jsonDataBytes = jsonDoc.toJson();

    // Send JSON data via POST
    QNetworkReply *reply = manager->post(request, jsonDataBytes);
    connect(reply, &QNetworkReply::finished, this, [this, reply, manager]() {
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
        delete manager;
    });
}

QJsonObject createTimeZoneJson() {
    QTimeZone timeZone = QTimeZone::systemTimeZone();
    QJsonObject timeZoneObj;

    QDateTime currentDateTime = QDateTime::currentDateTime();

    timeZoneObj["id"] = QString(timeZone.id());  // Timezone ID
    timeZoneObj["offset"] = timeZone.offsetFromUtc(currentDateTime)/3600;  // Timezone offset from UTC
    timeZoneObj["abbreviation"] = timeZone.abbreviation(currentDateTime);  // Timezone abbreviation

    return timeZoneObj;
}

QJsonObject createLocaleJson() {
    QLocale locale;
    QJsonObject localeObj;
    localeObj["language"] = locale.languageToString(locale.language());
    localeObj["region"] = locale.territoryToString(locale.territory());
    return localeObj;
}

QJsonObject Telemetry::MapToJSON() {
    QJsonObject jsonData;

    if (!mainWindow) {
        qDebug() << "MainWindow not available. Cannot prepare telemetry data.";
        return jsonData;
    }

    //fixed data
    jsonData["feedback"] = "message";

     //use a custom method for retrieve the banana app version
    jsonData["version"] = "Banana.ch";
    jsonData["versionNumber"]= "5";

    KUserFeedback::ApplicationVersionSource versionSource;
    jsonData["applicationVersion"] = QJsonObject{{"value", versionSource.data().toString()}};

    QSettings settings;
    int elapsed = elapsedTimer.elapsed();
    jsonData["sessionTime"] = elapsed;
    // Load old total time
    totalTimeElapsed = settings.value("total_usage_time", 0).toInt();
    jsonData["TotalUsageTime"] = totalTimeElapsed+elapsed;
    jsonData["timezone"] = createTimeZoneJson();
    jsonData["locale"] = createLocaleJson();

    jsonData["usageTime"] = QJsonObject{{"value", totalTimeElapsed}};  // Total time used (in seconds)
    jsonData["time_used"] = totalTimeElapsed;

    //custom data
    QJsonArray customDataArray;
    for (auto it = tempDB.begin(); it != tempDB.end();) {
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
            mainWindow->updateDB(it.key(), QString::number(newValue));
        }
        else {
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

        // Remove the entry from tempDB
        it = tempDB.erase(it);
    }
    // Loop through all checked checkboxes from the DialogOptionsWidget
/*
else{
        for (const CheckBoxEntry &checkboxEntry : std::as_const(checkBoxEntries)) {
            QCheckBox* checkBox = checkboxEntry.first;


        // Prepare JSON for each checkbox
        QJsonObject checkboxEntry;
        checkboxEntry["key"] = checkBox->text();  // Use checkbox label as key
        checkboxEntry["value"] = checkBox->isChecked() ? "true" : "false";  // Use the checked state as the value

        // Map to custom data array
        customDataArray.append(checkboxEntry);
    }
*/


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
