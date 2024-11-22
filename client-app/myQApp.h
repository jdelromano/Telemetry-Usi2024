/*#ifndef MYQAPP_H
#define MYQAPP_H

#include <QApplication>
#include "telemetry.h"
#include "mainwindow.h"

class MyQApp : public QApplication
{
private:
    static Telemetry *m_telemetry;  // Declare static member
    static MainWindow *m_mainWindow;

public:
    MyQApp(int &argc, char **argv);  // Declare the constructor
    static Telemetry *telemetry();    // Declare the static method
    static MainWindow* getMainWindow();
};

#endif // MYQAPP_H
*/
#ifndef MYQAPP_H
#define MYQAPP_H

#include <QApplication>
#include "telemetry.h"
#include "mainwindow.h"

class MyQApp : public QApplication
{
//private:
public:
    static MyQApp *instance;  // Singleton instance
    static Telemetry *m_telemetry;  // Telemetry instance
    static MainWindow *m_mainWindow;  // MainWindow instance
    static QMap<QString, QString> toSendDB;  // Static toSendDB map
    static QMap<QString, QString> db;  // Static db map

    // Private constructor to prevent direct instantiation
    MyQApp(int &argc, char **argv);

//public:
    static MyQApp *getInstance(int &argc, char **argv);  // Singleton access
    static Telemetry* telemetry();   // Access Telemetry instance
    static MainWindow* mainWindow();  // Access MainWindow instance
    static QMap<QString, QString>& getDb();  // Access db
    static QMap<QString, QString>& getToSendDB();  // Access toSendDB
};

#endif // MYQAPP_H
