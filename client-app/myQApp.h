#ifndef MYQAPP_H
#define MYQAPP_H

#include <QApplication>
#include "telemetry.h"
#include "mainwindow.h"

class MyQApp : public QApplication
{
private:
//public:
    static MyQApp *instance;  // Singleton instance
    static Telemetry *m_telemetry;  // Telemetry instance
    static MainWindow *m_mainWindow;  // MainWindow instance

public:
    MyQApp(int &argc, char **argv);
    static QMap<QString, QVariant> toSendDB;  // Static toSendDB map
    static QMap<QString, QVariant> db;  // Static db map
    static MyQApp *getInstance(int &argc, char **argv);  // Singleton access
    static Telemetry* telemetry();   // Access Telemetry instance
    static MainWindow* mainWindow();  // Access MainWindow instance
    static QMap<QString, QVariant> &getDb();  // Access db
    static QMap<QString, QVariant>& getToSendDB();  // Access toSendDB
};

#endif // MYQAPP_H
