#include "telemetry.h"

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

Telemetry::Telemetry(QObject *parent)
    : QObject(parent),
    buttonPressCount(0),
    addMenuCount(0),
    provider(new KUserFeedback::Provider(this))
{
    provider->addDataSource(new KUserFeedback::UsageTimeSource);
    provider->addDataSource(new KUserFeedback::PlatformInfoSource);
    provider->addDataSource(new KUserFeedback::CompilerInfoSource);
    //provider->setSurveyInterval(24 * 60 * 60);  // Collect once per day
    provider->setSurveyInterval(100);
    provider->setTelemetryMode(KUserFeedback::Provider::BasicUsageStatistics);
    elapsedTimer.start(); // Start tracking time when open the app

    connect(&feedbackTimer, &QTimer::timeout, this, &Telemetry::sendPeriodicFeedback);
    feedbackTimer.start(10000);  // 10,000 milliseconds = 10 seconds

    sendInitialTelemetryData();
}

void Telemetry::sendInitialTelemetryData() {
    QJsonObject json = prepareFeedbackJson("Application started", elapsedTimer.elapsed() / 1000);
    // Convert JSON object to QJsonDocument
    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    // Create network manager and send the POST request
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("http://localhost:1984/receiver/submit/telemetryapp"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = manager->post(request, data);
    connect(reply, &QNetworkReply::finished, this, [reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "Initial telemetry data sent successfully.";
        } else {
            qDebug() << "Error sending initial telemetry data:" << reply->errorString();
        }
        reply->deleteLater();
    });
}

//increment counters
void Telemetry::incrementButtonPressCount() {
    ++buttonPressCount;
    qDebug() << "Main menu button pressed. Current count:" << buttonPressCount;
    emit buttonPressCountChanged(buttonPressCount);
    sendButtonPressCountFeedback(buttonPressCount);
}

void Telemetry::incrementAddMenuPressCount() {
    ++addMenuCount;
    qDebug() << "Add menu button pressed. Current count:" << addMenuCount;
    emit addMenuCountChanged(addMenuCount);
    sendUsageFeedback("User added a new entry");
}

void Telemetry::sendButtonPressCountFeedback(int count) {
    QJsonObject json;
    json["feedback"] = "User pressed the main menu button";
    json["startCount"] = createCountJson(count);

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("http://localhost:1984/receiver/submit/telemetryapp"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = manager->post(request, data);
    connect(reply, &QNetworkReply::finished, this, [reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "Button press feedback sent successfully.";
        } else {
            qDebug() << "Error sending button press feedback:" << reply->errorString();
        }
        reply->deleteLater();
    });
}

void Telemetry::sendPeriodicFeedback() {
    if (buttonPressCount != lastButtonPressCount || addMenuCount != lastAddMenuCount) {
        lastButtonPressCount = buttonPressCount;
        lastAddMenuCount = addMenuCount;
        sendUsageFeedback("Periodic feedback");
    } else {
        qDebug() << "No changes detected. Skipping feedback.";
    }
}

void Telemetry::sendUsageFeedback(const QString &feedbackMessage) {
    int usageTime = elapsedTimer.elapsed() / 1000; // Elapsed time in seconds
    qDebug() << "Total usage time (in seconds):" << usageTime;

    // Prepare feedback data in JSON
    QJsonObject json;
    json["feedback"] = "User added a new entry"; // Adjust feedback message

    // Add custom values - now it's the menu button counter
    QJsonObject customData;
    customData["custom1"] = "custom_value_1"; // Test data
    customData["custom2"] = QString::number(addMenuCount);  // Test data
    json["mainCustom"] = customData;

    // Send usage time
    json["usageTime"] = createCountJson(elapsedTimer.elapsed() / 1000); // Elapsed time in seconds

    // Convert JSON object to QJsonDocument
    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    // Create network manager and send the POST request
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("http://localhost:1984/receiver/submit/telemetryapp"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = manager->post(request, data);
    connect(reply, &QNetworkReply::finished, this, [reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "Feedback sent successfully.";
        } else {
            qDebug() << "Error sending feedback:" << reply->errorString();
        }
        reply->deleteLater();
    });
}

QJsonObject Telemetry::prepareFeedbackJson(const QString &feedbackMessage, int usageTime) const {
    QJsonObject json;
    json["feedback"] = feedbackMessage;
    json["startCount"] = createCountJson(buttonPressCount);
    json["usageTime"] = createCountJson(usageTime);
    json["timezone"] = createTimeZoneJson();
    json["locale"] = createLocaleJson();
    json["cpu"] = createCpuJson();
    json["compiler"] = createCompilerJson();

    //custom data
    QJsonObject customData;
    customData["custom1"] = "custom_value_1";
    customData["custom2"] = QString::number(addMenuCount);
    json["mainCustom"] = customData;

    json["uuid"] = "d3b21baa-4a1b-b889-e03c80187f05";
    return json;
}

QJsonObject Telemetry::createCountJson(int count) const {
    QJsonObject countJson;
    countJson["value"] = count;
    return countJson;
}

QJsonObject Telemetry::createTimeZoneJson() const {
    QTimeZone timeZone = QTimeZone::systemTimeZone();
    QJsonObject timeZoneObj;
    timeZoneObj["id"] = QString(timeZone.id());
    timeZoneObj["offset"] = timeZone.offsetFromUtc(QDateTime::currentDateTime());
    timeZoneObj["abbreviation"] = timeZone.abbreviation(QDateTime::currentDateTime());
    return timeZoneObj;
}

QJsonObject Telemetry::createLocaleJson() const {
    QLocale locale;
    QJsonObject localeObj;
    localeObj["language"] = locale.languageToString(locale.language());
    localeObj["region"] = locale.countryToString(locale.country());
    return localeObj;
}

QJsonObject Telemetry::createCpuJson() const {
    QJsonObject cpuObj;
    cpuObj["architecture"] = QSysInfo::currentCpuArchitecture();
    cpuObj["count"] = QThread::idealThreadCount();
    return cpuObj;
}

QJsonObject Telemetry::createCompilerJson() const {
    QJsonObject compilerObj;

#if defined(__clang__)
    compilerObj["name"] = "Clang";
    compilerObj["version"] = QString::number(__clang_major__) + "." +
                             QString::number(__clang_minor__) + "." +
                             QString::number(__clang_patchlevel__);
#elif defined(__GNUC__)
    compilerObj["name"] = "GCC";
    compilerObj["version"] = QString::number(__GNUC__) + "." +
                             QString::number(__GNUC_MINOR__) + "." +
                             QString::number(__GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
    compilerObj["name"] = "MSVC";
    compilerObj["version"] = QString::number(_MSC_VER);
#else
    compilerObj["name"] = "Unknown";
    compilerObj["version"] = "Unknown";
#endif

    return compilerObj;
}

int Telemetry::getButtonPressCount() const {
    return buttonPressCount;
}

int Telemetry::getAddMenuCount() const {
    return addMenuCount;
}
