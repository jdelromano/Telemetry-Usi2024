// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "mainwindow.h"
#include "dialog.h"
#include "telemetry.h"        // Include only here
#include "addresswidget.h"    // Include the full definition here

#include <QAction>
#include <QFileDialog>
#include <QMenuBar>
#include <qapplication.h>
#include <qscreen.h>
#include <qstylehints.h>


//! [0]
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      telemetry(new Telemetry(this, this)),
    addressWidget(new AddressWidget(telemetry, this))
{
    setCentralWidget(addressWidget);
    createDB();
    createMenus();

    telemetry->sendTelemetryData();
    setWindowTitle(tr("Address Book"));
}
//! [0]
void MainWindow::createDB() {
    // Check and initialize each key only if it's not present
    if (!db.contains("usageTime")) {
        db["usageTime"] = ""; // Placeholder, initialize as needed
    }

    // Add button actions
    if (!db.contains("addMainPage")) {
        db["addMainPage"] = "0"; // Add main page menu
    }
    if (!db.contains("addToolMenu")) {
        db["addToolMenu"] = "0"; // Add tool menu
    }

    // Dialog window actions
    if (!db.contains("getIntButton")) {
        db["getIntButton"] = "0"; // getInt() button
    }
    if (!db.contains("getDoubleButton")) {
        db["getDoubleButton"] = "0"; // getDouble() button
    }
    if (!db.contains("getIttemButton")) {
        db["getIttemButton"] = "0"; // getItem() button
    }
    if (!db.contains("getTextButton")) {
        db["getTextButton"] = "0"; // getText() button
    }
    if (!db.contains("getMultiLineButton")) {
        db["getMultiLineButton"] = "0"; // getMultiline() button
    }

    // Color dialog options
    if (!db.contains("dialogBox1Flag")) {
        db["dialogBox1Flag"] = "false"; // First checkbox
    }
    if (!db.contains("dialogBox2Flag")) {
        db["dialogBox2Flag"] = "false"; // Second checkbox
    }
    if (!db.contains("dialogBox3Flag")) {
        db["dialogBox3Flag"] = "false"; // Third checkbox
    }
    // Debug output to verify initialization
    qDebug() << "Database initialized:";
    for (auto it = db.cbegin(); it != db.cend(); ++it) {
        qDebug() << it.key() << ": " << it.value();
    }
}
const QMap<QString, QString>& MainWindow::getdb() const {
    return db;
}

QString MainWindow::getDBValue(const QString &key) const  {
    return db.value(key, "0"); // Return "0" if the key doesn't exist
}

void MainWindow::updateDB(const QString &key, const QString &value)
{
    db.insert(key, value); // Insert or update the value in the db
    qDebug() << "Updated db with key:" << key << "value:" << value;
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
    Telemetry* telemetry = this->telemetry;
    QDialog *dialog = new Dialog(telemetry, this);
    dialog->setWindowTitle(tr("Standard Dialog"));

    if (!QGuiApplication::styleHints()->showIsFullScreen() && !QGuiApplication::styleHints()->showIsMaximized()) {
        QRect availableGeometry = dialog->screen()->availableGeometry();
        dialog->resize(availableGeometry.width() / 3, availableGeometry.height() * 2 / 3);
        dialog->move((availableGeometry.width() - dialog->width()) / 2,
                     (availableGeometry.height() - dialog->height()) / 2);
    }

    dialog->show();
}

