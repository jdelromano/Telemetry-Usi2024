#ifndef TELEMETRY_H
#define TELEMETRY_H


#include "kuserfeedback/provider.h"
#include <qdatetime.h>
#include <qelapsedtimer.h>
#include <qnetworkreply.h>
#include <qobject.h>
#include <qtimer.h>

class MainWindow; // Forward declaration

class Telemetry : public QObject
{
    Q_OBJECT

public:
    void DebugJSON(const QVariantMap &dataMap);
    // Constructor for Telemetry, initializing counters and setting up provider
    explicit Telemetry(QObject *parent = nullptr);

   ~Telemetry();

    // Function to store or update a value in the telemetry data map
    // In Telemetry.h
    void incCount(QString field);

    int total_time;
    //int totalTimeElapsed;

    void checkAndUpdate( QString &key,  QString &value);
    void checkAndUpdate(const QString &key,const  QString &value);
    void checkAndUpdateBool(const QString &key, bool value);
    void checkAndUpdateInt(const QString &key, int value);

    // send telemetry data to the server
    void sendTelemetryData();

    //convert data into json
    QJsonObject MapToJSON();
    QJsonObject createCustomDataJson(const QString& key, const QVariant& value, const QString& valueType);

    QElapsedTimer elapsedTimer;
    MainWindow *mainWindow; // Reference to MainWindow
    QNetworkAccessManager *manager;

     QMap<QString, QVariant> *db;

    KUserFeedback::Provider *provider;  // Feedback provider
    QTimer feedbackTimer;  // Timer for periodic feedback
    int totalTimeElapsed;  // Total elapsed time
    int retryCount;  // Retry count for sending telemetry

    // Function to send periodic feedback
    void sendPeriodicFeedback();

    // Utility function to create a JSON object for a count value
    QJsonObject createCountJson(int value);

    // Function to retry sending telemetry data if an error occurs
    void retrySendingData(QNetworkReply *reply);

};

#endif // TELEMETRY_H
