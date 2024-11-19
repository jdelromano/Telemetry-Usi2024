#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <QObject>
#include <QMap>
#include <QElapsedTimer>
#include <QPair>
#include <QVariant>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <KUserFeedback/Provider>
#include <KUserFeedback/UsageTimeSource>
#include <KUserFeedback/PlatformInfoSource>
#include <KUserFeedback/CompilerInfoSource>
#include <QTimer>
#include <QTime>

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

    QJsonObject MapToJSON();


private:

    QMap<QString, QString> tempDB;
    QElapsedTimer elapsedTimer;
    MainWindow *mainWindow; // Reference to MainWindow
    QNetworkAccessManager *manager;
    //

    const QMap<QString, QString> *db;



    KUserFeedback::Provider *provider;  // Feedback provider
    QTimer feedbackTimer;  // Timer for periodic feedback
    int totalTimeElapsed;  // Total elapsed time
    int retryCount;  // Retry count for sending telemetry


    // Function to send telemetry data to the server
    //to server
    void sendTelemetryData();

    // Function to send periodic feedback
    void sendPeriodicFeedback();

    // Function to prepare telemetry data into a JSON object
    //QJsonObject prepareFeedbackJson(const QString &feedbackMessage, int elapsedTime);

    // Utility function to create a JSON object for a count value
    QJsonObject createCountJson(int value);

    // Function to retry sending telemetry data if an error occurs
    void retrySendingData(QNetworkReply *reply);

};

#endif // TELEMETRY_H
