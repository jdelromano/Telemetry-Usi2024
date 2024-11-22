/*#include "myQApp.h"

// Define the static member
Telemetry* MyQApp::m_telemetry = nullptr;
MainWindow* MyQApp::m_mainWindow = nullptr;

// Constructor definition
MyQApp::MyQApp(int &argc, char **argv)
    : QApplication(argc, argv)
{
    if (m_mainWindow) {
        m_mainWindow = new MainWindow(nullptr);  // Initialize only once
    }
    if (!m_telemetry) {
        m_telemetry = new Telemetry(this);  // Initialize only once
    }
}

// Static method definition
Telemetry* MyQApp::telemetry()
{
    return m_telemetry;
}

MainWindow* MyQApp::getMainWindow() {
    return MainWindow;
}*/
#include "myQApp.h"
#include "mainwindow.h"

// Initialize static members
MyQApp* MyQApp::instance = nullptr;
Telemetry* MyQApp::m_telemetry = nullptr;
MainWindow* MyQApp::m_mainWindow = nullptr;
QMap<QString, QString> MyQApp::toSendDB;  // Static toSendDB map
QMap<QString, QString> MyQApp::db;  // Static db map

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

QMap<QString, QString>& MyQApp::getDb() {
    return db;  // Return the reference to db
}

QMap<QString, QString>& MyQApp::getToSendDB() {
    return toSendDB;  // Return the reference to toSendDB
}

