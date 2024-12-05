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
    if (!MyQApp::DB.contains("usageTime")) {
        MyQApp::DB["usageTime"] = ""; // Placeholder, initialize as needed
    }

    // Add button actions
    if (!MyQApp::DB.contains("addMainPage")) {
        MyQApp::DB["addMainPage"] = "0"; // "Add" button counter main page menu
    }
    if (!MyQApp::DB.contains("addToolMenu")) {
        MyQApp::DB["addToolMenu"] = "0"; // "Add" button counter in tool menu
    }

    // Dialog window actions
    if (!MyQApp::DB.contains("getIntButton")) {
        MyQApp::DB["getIntButton"] = "0"; // getInt() button
    }
    if (!MyQApp::DB.contains("getDoubleButton")) {
        MyQApp::DB["getDoubleButton"] = "0"; // getDouble() button
    }
    if (!MyQApp::DB.contains("getIttemButton")) {
        MyQApp::DB["getIttemButton"] = "0"; // getItem() button
    }
    if (!MyQApp::DB.contains("getTextButton")) {
        MyQApp::DB["getTextButton"] = "0"; // getText() button
    }
    if (!MyQApp::DB.contains("getMultiLineButton")) {
        MyQApp::DB["getMultiLineButton"] = "0"; // getMultiline() button
    }

    // Color dialog options
    if (!MyQApp::DB.contains("dialogBox1Flag")) {
        MyQApp::DB["dialogBox1Flag"] = "false"; // First checkbox
    }
    if (!MyQApp::DB.contains("dialogBox2Flag")) {
        MyQApp::DB["dialogBox2Flag"] = "false"; // Second checkbox
    }
    if (!MyQApp::DB.contains("dialogBox3Flag")) {
        MyQApp::DB["dialogBox3Flag"] = "false"; // Third checkbox
    }
    // Debug output to verify initialization
    qDebug() << "Database initialized:";
    for (auto it = MyQApp::DB.cbegin(); it != MyQApp::DB.cend(); ++it) {
        qDebug() << it.key() << "a: " << it.value();
    }
}

//getters
//get the whole MyQApp::DB
 QMap<QString, QVariant>& MainWindow::getdb()  {
    return MyQApp::DB;
}

 QVariant MainWindow::getDBValue(const QString &key) const {
     // Check if the key exists in the database
     if (MyQApp::DB.contains(key)) {
         return MyQApp::DB.value(key);
     } else {
         // Return an empty QVariant (Invalid) if the key is not found
         qDebug() << "Key not found in DB:" << key;
         return QVariant(); // Return Invalid QVariant
     }
 }



//setter
void MainWindow::setDB(const QString &key, const QString &value)
{
    MyQApp::DB.insert(key, value); // Insert or update the value in the MyQApp::DB
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

