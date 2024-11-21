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
    explicit Telemetry(MainWindow *mainWindow, QObject *parent = nullptr);

   ~Telemetry();

    // Function to store or update a value in the telemetry data map
    // In Telemetry.h
    void incCount(const QString field);

    int total_time;
    //int totalTimeElapsed;

    void checkAndUpdate(const QString &key, const QString &value);

    //map used to store temporarly data to be converted in JSON and send
    //data should not be the same as the one in the server
    //emptied after send,
    QMap<QString, QString> toSendDB;

    // send telemetry data to the server
    void sendTelemetryData();

    //convert data into json
    QJsonObject MapToJSON();

private:
    QElapsedTimer elapsedTimer;
    MainWindow *mainWindow; // Reference to MainWindow
    QNetworkAccessManager *manager;

    const QMap<QString, QString> *db;

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
