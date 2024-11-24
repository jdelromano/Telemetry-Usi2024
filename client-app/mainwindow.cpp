// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "mainwindow.h"
#include "addresswidget.h"
#include "myQApp.h"

#include "mainwindow.h"
#include "dialog.h"
#include "telemetry.h"        // Include only here
#include "myQApp.h"
#include "addresswidget.h"

#include <QAction>
#include <QFileDialog>
#include <QMenuBar>
#include <qapplication.h>
#include <qmenu.h>
#include <qmenubar.h>
#include <qscreen.h>
#include <qstylehints.h>


//! [0]
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    addressWidget(new AddressWidget(this))
{
    setCentralWidget(addressWidget); // Use the properly initialized widget
    createDB();
    createMenus();
    MyQApp::telemetry()->sendTelemetryData(); // Use MyQApp to access telemetry
    setWindowTitle(tr("Address Book"));
}
//! [0]

//DB construction
//Used to keep track of the data send to the server during this session
// avoid waste of resources
void MainWindow::createDB() {

    // Check and initialize basic values
    if (!MyQApp::db.contains("usageTime")) {
        MyQApp::db["usageTime"] = ""; // Placeholder, initialize as needed
    }

    // Add button actions
    if (!MyQApp::db.contains("addMainPage")) {
        MyQApp::db["addMainPage"] = "0"; // "Add" button counter main page menu
    }
    if (!MyQApp::db.contains("addToolMenu")) {
        MyQApp::db["addToolMenu"] = "0"; // "Add" button counter in tool menu
    }

    // Dialog window actions
    if (!MyQApp::db.contains("getIntButton")) {
        MyQApp::db["getIntButton"] = "0"; // getInt() button
    }
    if (!MyQApp::db.contains("getDoubleButton")) {
        MyQApp::db["getDoubleButton"] = "0"; // getDouble() button
    }
    if (!MyQApp::db.contains("getIttemButton")) {
        MyQApp::db["getIttemButton"] = "0"; // getItem() button
    }
    if (!MyQApp::db.contains("getTextButton")) {
        MyQApp::db["getTextButton"] = "0"; // getText() button
    }
    if (!MyQApp::db.contains("getMultiLineButton")) {
        MyQApp::db["getMultiLineButton"] = "0"; // getMultiline() button
    }

    // Color dialog options
    if (!MyQApp::db.contains("dialogBox1Flag")) {
        MyQApp::db["dialogBox1Flag"] = "false"; // First checkbox
    }
    if (!MyQApp::db.contains("dialogBox2Flag")) {
        MyQApp::db["dialogBox2Flag"] = "false"; // Second checkbox
    }
    if (!MyQApp::db.contains("dialogBox3Flag")) {
        MyQApp::db["dialogBox3Flag"] = "false"; // Third checkbox
    }
    // Debug output to verify initialization
    qDebug() << "Database initialized:";
    for (auto it = MyQApp::db.cbegin(); it != MyQApp::db.cend(); ++it) {
        qDebug() << it.key() << "a: " << it.value();
    }
}

//getters
//get the whole MyQApp::db
 QMap<QString, QVariant>& MainWindow::getdb()  {
    return MyQApp::db;
}

 QVariant MainWindow::getDBValue(const QString &key) const {
     // Check if the key exists in the database
     if (MyQApp::db.contains(key)) {
         return MyQApp::db.value(key);
     } else {
         // Return an empty QVariant (Invalid) if the key is not found
         qDebug() << "Key not found in DB:" << key;
         return QVariant(); // Return Invalid QVariant
     }
 }



//setter
void MainWindow::setDB(const QString &key, const QString &value)
{
    MyQApp::db.insert(key, value); // Insert or update the value in the MyQApp::db
}
//! [1a]
void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openAct = new QAction(tr("&Open..."), this);
    fileMenu->addAction(openAct);
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);
    //! [1a]

    QAction *saveAct = new QAction(tr("&Save"), this);
    fileMenu->addAction(saveAct);
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);

    fileMenu->addSeparator();

    QAction *exitAct = new QAction(tr("E&xit"), this);
    fileMenu->addAction(exitAct);
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    QMenu *toolMenu = menuBar()->addMenu(tr("&Tools"));

    QAction *openDialogAct = new QAction(tr("Open &Dialog"), this);  // New action
    toolMenu->addAction(openDialogAct);  // Add to toolMenu instead of fileMenu
    connect(openDialogAct, &QAction::triggered, addressWidget, &AddressWidget::openDialog);

    toolMenu->addSeparator();

    QAction *addAct = new QAction(tr("&Add Entry..."), this);
    toolMenu->addAction(addAct);
    connect(addAct, &QAction::triggered,
            addressWidget, &AddressWidget::showAddEntryDialog);
    //! [1b]
    editAct = new QAction(tr("&Edit Entry..."), this);
    editAct->setEnabled(false);
    toolMenu->addAction(editAct);
    connect(editAct, &QAction::triggered, addressWidget, &AddressWidget::editEntry);

    toolMenu->addSeparator();

    removeAct = new QAction(tr("&Remove Entry"), this);
    removeAct->setEnabled(false);
    toolMenu->addAction(removeAct);
    connect(addressWidget, &AddressWidget::selectionChanged,
            this, &MainWindow::updateActions);
}
//! [1b]

//! [2]
void MainWindow::openFile()
{
    addressWidget->readFromFile();
}
//! [2]

//! [3]
void MainWindow::saveFile()
{
    addressWidget->writeToFile();
}
//! [3]

//! [4]
void MainWindow::updateActions(const QItemSelection &selection)
{
    QModelIndexList indexes = selection.indexes();

    if (!indexes.isEmpty()) {
        removeAct->setEnabled(true);
        editAct->setEnabled(true);
    } else {
        removeAct->setEnabled(false);
        editAct->setEnabled(false);
    }
}
//! [4]
//! // Slot to open the dialog
void MainWindow::openDialog()
{
    Dialog *dialog = new Dialog();

    dialog->setWindowTitle(tr("Standard Dialog"));

    if (!QGuiApplication::styleHints()->showIsFullScreen() && !QGuiApplication::styleHints()->showIsMaximized()) {
        QRect availableGeometry = dialog->screen()->availableGeometry();
        dialog->resize(availableGeometry.width() / 3, availableGeometry.height() * 2 / 3);
        dialog->move((availableGeometry.width() - dialog->width()) / 2,
                     (availableGeometry.height() - dialog->height()) / 2);
    }

    dialog->show();
}

