#include "telemetry.h"
#include "mainwindow.h"
#include "myQApp.h"

// TODO: debug
#include <QJsonObject>
#include <QJsonDocument>
#include <QVariant>
#include <QDebug>
// TODO: debug
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

Telemetry::Telemetry(QObject *parent)
    : QObject(parent),
    provider(new KUserFeedback::Provider(this)),
    retryCount(0)//,
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

//Setters
// Check and update toSendDB
//if the value is already in the database, dosen't update the array to be send
/*
 * void Telemetry::checkAndUpdate( QString &key,  QString &value) {
     QMap<QString, QVariant> &db = mainWindow->getdb();

    // Synchronize: Check if the key needs to be staged
    // if it is staged
    if (MyQApp::toSendDB.contains(key)) {
        // Only update if toSendDB is different from both db and the new value
        if (MyQApp::toSendDB[key] != db.value(key) && MyQApp::toSendDB[key] != value) {
            MyQApp::toSendDB[key] = value;
        }
    } else {
        // If not staged, check if the value differs from db
        if (!db.contains(key) || db.value(key) != value) {
            MyQApp::toSendDB[key] = value; // it is a new value
        }
    }
}
*/

void Telemetry::checkAndUpdate(const QString &key, const QString &value) {
    qDebug()<< "appena dentro checkandupdate" << value;
    // Access db through MyQApp singleton
    QMap<QString, QVariant>& db = MyQApp::getDb();
    QMap<QString, QVariant>& toSendDB = MyQApp::getToSendDB();

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
    qDebug()<<"dentro toSendDB" << toSendDB[key];
}
// Change db and toSendDB to hold QVariant to support different types
void Telemetry::checkAndUpdateInt(const QString &key, int value) {
    // Access db and toSendDB through MyQApp singleton
    QMap<QString, QVariant>& db = MyQApp::getDb();  // Get db through singleton
    QMap<QString, QVariant>& toSendDB = MyQApp::getToSendDB();  // Get toSendDB through singleton

    // Synchronize: Check if the key needs to be staged
    if (toSendDB.contains(key)) {
        // Only update if toSendDB is different from both db and the new value
        if (toSendDB[key].toInt() != db.value(key).toInt() && toSendDB[key].toInt() != value) {
            qDebug() << "Updating int value for key" << key << "from"
                     << toSendDB[key].toInt() << "to" << value;
            toSendDB[key] = value;  // Update value in toSendDB
        }
    } else {
        // If not staged, check if the value differs from db
        if (!db.contains(key) || db.value(key).toInt() != value) {
            qDebug() << "New int key-value pair added: " << key << "->" << value;
            toSendDB[key] = value;  // It is a new value
        }
    }
}

void Telemetry::checkAndUpdateBool(const QString &key, bool value) {
    // Access db and toSendDB through MyQApp singleton
    QMap<QString, QVariant>& db = MyQApp::getDb();  // Get db through singleton
    QMap<QString, QVariant>& toSendDB = MyQApp::getToSendDB();  // Get toSendDB through singleton

    // Synchronize: Check if the key needs to be staged
    if (toSendDB.contains(key)) {
        // Only update if toSendDB is different from both db and the new value
        if (toSendDB[key].toBool() != db.value(key).toBool() && toSendDB[key].toBool() != value) {
            qDebug() << "Updating bool value for key" << key << "from"
                     << toSendDB[key].toBool() << "to" << value;
            toSendDB[key] = value;  // Update value in toSendDB
        }
    } else {
        // If not staged, check if the value differs from db
        if (!db.contains(key) || db.value(key).toBool() != value) {
            qDebug() << "New bool key-value pair added: " << key << "->" << value;
            toSendDB[key] = value;  // It is a new value
        }
    }
}
//void Telemetry::incCount( QString field) {
void Telemetry::incCount(QString field) {

    // Retrieve the current value or default to "0"
    int value = MyQApp::toSendDB.value(field, "0").toInt();

    // Increment the value
    value++;

    // Update the map with the new value
    MyQApp::toSendDB[field] = QString::number(value);
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

    //parse and clean map<QString, QVariant> to valid JSon
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

//map<QString, QVariant> to JSon parser + auxiliary

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

QJsonObject Telemetry::MapToJSON() {
    QJsonObject jsonData;
    if (!mainWindow) {
        return jsonData;
    }

    // Fixed data
    jsonData["feedback"] = "message";
    jsonData["version"] = "Banana.ch";
    jsonData["versionNumber"]= "5";
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

    for (auto it = MyQApp::toSendDB.begin(); it != MyQApp::toSendDB.end(); ++it) {
        QString currentKey = it.key();
        QVariant currentValue = it.value();

        qDebug() << "inside map to json " << currentValue;

        QJsonObject customEntry;
        customEntry["key"] = currentKey;

        // Check and handle the type dynamically
        if (currentValue.type() == QVariant::Int) {
            int currentIntValue = currentValue.toInt();
            customEntry["type"] = "counter1";
            customEntry["int"] = currentIntValue;
    } else if (currentValue.type() == QVariant::Double) {
            double currentDoubleValue = currentValue.toDouble();
            customEntry["type"] = "counter2";
            customEntry["double"] = currentDoubleValue;
            } else if (currentValue.type() == QVariant::String) {
            QString currentStringValue = currentValue.toString();
            customEntry["type"] = "text";
            customEntry["string"] = currentStringValue;
        } else {
            customEntry["type"] = "unknown";
            customEntry["other"] = currentValue.toString();
        }
        // Add to JSON structures
        jsonData[currentKey] = customEntry;
        customDataArray.append(customEntry);
    }
    // Add custom data array to JSON
    jsonData["custom_data"] = customDataArray;

    // Convert JSON object to indented JSON string
    QString indentedJson = QJsonDocument(jsonData).toJson(QJsonDocument::Indented);

    // Print each line of the indented JSON
    QStringList jsonLines = indentedJson.split('\n');
    for (const QString& line : jsonLines) {
        qDebug() << line;
    }


    return jsonData;
    // 2. Send the JSON data
    //sendData(jsonData);

    // 3. Store data in mainDB
    //for (auto it = MyQApp::toSendDB.begin(); it != MyQApp::toSendDB.end(); ++it) {
      //  QString currentKey = it.key();
        //QVariant currentValue = it.value();

        // Save into mainDB
       // MyQApp::mainDB[currentKey] = currentValue;
    //}

    // Optionally clear toSendDB after processing
   // MyQApp::toSendDB.clear();
}
/*

//language and region -> note, is the user defined setting
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
        return jsonData;
    }

    // Fixed data
    jsonData["feedback"] = "message";
    jsonData["version"] = "Banana.ch";
    jsonData["versionNumber"]= "5";
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

    for (auto it = MyQApp::toSendDB.begin(); it != MyQApp::toSendDB.end(); ++it) {
        QVariant currentValue = toSendDB[it//mainWindow->getDBValue(it.key());
        qDebug()<<"value in MapToJS" << currentValue;
        QJsonObject customEntry;
        customEntry["key"] = it.key();
        // Check and handle the type dynamically
        if (currentValue.canConvert<int>()) {
            // If it's an integer, handle it as a numeric value
            int currentIntValue = currentValue.toInt();
            customEntry["type"] = "counter";
            customEntry["int"] = currentIntValue; // store as numeric

            jsonData[it.key()] = customEntry;
        }
        else if (currentValue.canConvert<double>()) {
            // If it's a double, handle as a numeric value
            double currentDoubleValue = currentValue.toDouble();
            customEntry["type"] = "counter";
            customEntry["double"] = currentDoubleValue; // store as numeric

            jsonData[it.key()] = customEntry;
        }
        else if (currentValue.canConvert<QString>()) {
            // If it's a string, handle as text
            QString currentStringValue = currentValue.toString();
            customEntry["type"] = "text";
            customEntry["string"] = currentStringValue; // store as string

            jsonData[it.key()] = customEntry;
        }
        else {
            // Handle any other types (e.g., bool, date, etc.)
            customEntry["type"] = "unknown";
            customEntry["other"] = currentValue.toString(); // Store unknown types as string

            jsonData[it.key()] = customEntry;
        }

        // Append to the custom data array
        customDataArray.append(customEntry);
    }

    // Add custom data to the main JSON object
    jsonData["custom_data"] = customDataArray;

    // Convert JSON object to indented JSON string
    QString indentedJson = QJsonDocument(jsonData).toJson(QJsonDocument::Indented);

    // Print each line of the indented JSON
    QStringList jsonLines = indentedJson.split('\n');
    for (const QString& line : jsonLines) {
        qDebug() << line;
    }

    return jsonData;
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
    QSettings settings;
    int elapsed = elapsedTimer.elapsed();
    jsonData["sessionTime"] =  QJsonObject{{"value", elapsed}};
    // Load old total time
    totalTimeElapsed = settings.value("total_usage_time", 0).toInt();
    jsonData["TotalUsageTime"] = QJsonObject{{"value",  totalTimeElapsed+elapsed}};  // Total time used (in seconds)

    jsonData["timezone"] = createTimeZoneJson();
    jsonData["locale"] = createLocaleJson();

    //custom data
    QJsonArray customDataArray;

    for (auto it = MyQApp::toSendDB.begin(); it != MyQApp::toSendDB.end(); ++it) {
        QVariant currentValue = mainWindow->getDBValue(it.key());

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
            textObject["text"] = QJsonValue::fromVariant(currentValue);
            jsonData.insert(it.key(), textObject);

            // Add to the custom data array
            customEntry["type"] = "text";
            customEntry["value"] = QJsonValue::fromVariant(currentValue);
        }

        // Append to the custom data array
        customDataArray.append(customEntry);
    }

    // Clear the entire toSendDB after processing
    MyQApp::toSendDB.clear();

    // Add custom data to the main JSON object
    jsonData["custom_data"] = customDataArray;

    // Convert JSON object to indented JSON string
    QString indentedJson = QJsonDocument(jsonData).toJson(QJsonDocument::Indented);

    // Split by newline character and print each line separately
    QStringList jsonLines = indentedJson.split('\n');
    for (const QString& line : jsonLines) {
        qDebug() << line;
    }


    return jsonData;
}
*/
QString variantToString(const QVariant &variant)
{
    if (variant.isValid()) {
        return variant.toString();
    }
    return QString("Invalid value");
}


void Telemetry::DebugJSON(const QMap<QString, QVariant> &debugInfo)
{
    // Example of how to create a JSON object from QVariantMap
    QJsonObject debugJson;

    for (auto it = debugInfo.begin(); it != debugInfo.end(); ++it) {
        debugJson[it.key()] = it.value().toString();  // Convert all values to QString
    }

    // TODO: debug;
    // Convert to JSON string (for logging or sending)
    QJsonDocument doc(debugJson);
    QString jsonString = doc.toJson(QJsonDocument::Indented);

    // You can log the output to debug
    qDebug() << jsonString;
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
