#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <QObject>
#include <QTimer>
#include <KUserFeedback/Provider>
#include <QElapsedTimer>
#include <QJsonObject>

class Telemetry : public QObject
{
    Q_OBJECT

public:
    explicit Telemetry(QObject *parent = nullptr);

    void incrementButtonPressCount();
    void incrementAddMenuPressCount();
    void sendUsageFeedback(const QString &feedbackMessage);
    void sendPeriodicFeedback();
    void sendButtonPressCountFeedback(int count);

    int getButtonPressCount() const;
    int getAddMenuCount() const;

    void sendInitialTelemetryData();

signals:
    void buttonPressCountChanged(int newCount);
    void addMenuCountChanged(int newCount);

private:
    int buttonPressCount;
    int addMenuCount;

    int lastButtonPressCount = 0;
    int lastAddMenuCount = 0;

    KUserFeedback::Provider *provider;
    QElapsedTimer elapsedTimer;
    QTimer feedbackTimer;

    // Private methods for structuring feedback data
    QJsonObject prepareFeedbackJson(const QString &feedbackMessage, int usageTime) const;
    QJsonObject createCountJson(int count) const; 
    QJsonObject createUsageTimeJson(int usageTime) const;
    QJsonObject createTimeZoneJson() const;
    QJsonObject createLocaleJson() const;
    QJsonObject createCpuJson() const;
    QJsonObject createCompilerJson() const;
};

#endif // TELEMETRY_H
