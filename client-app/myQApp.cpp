#include "myQApp.h"
#include "mainwindow.h"

// Initialize static members
MyQApp* MyQApp::instance = nullptr;
Telemetry* MyQApp::m_telemetry = nullptr;
MainWindow* MyQApp::m_mainWindow = nullptr;
QMap<QString, QVariant> MyQApp::toSendDB;  // Static toSendDB map
QMap<QString, QVariant> MyQApp::db;  // Static db map

MyQApp::MyQApp(int &argc, char **argv)
    : QApplication(argc, argv)
{
    // Initialize mainWindow with nullptr or appropriate parent
    m_mainWindow = new MainWindow(nullptr);

    // Initialize Telemetry
    m_telemetry = new Telemetry();
}

MyQApp* MyQApp::getInstance(int &argc, char **argv) {
    if (!instance) {
        instance = new MyQApp(argc, argv);  // Create instance if not already created
    }
    return instance;
}

Telemetry* MyQApp::telemetry() {
    return m_telemetry;
}

MainWindow* MyQApp::mainWindow() {
    return m_mainWindow;
}

QVariant MyQApp::getDatabaseValue(const QString &key) {
    return MyQApp::getDb().value(key, QVariant());
}

QMap<QString, QVariant>& MyQApp::getDb() {
    return db;  // Return the reference to db
}

QMap<QString, QVariant>& MyQApp::getToSendDB() {
    return toSendDB;  // Return the reference to toSendDB
}

