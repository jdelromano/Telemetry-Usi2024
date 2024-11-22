// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QMap>
#include <QVariant>
#include <qitemselectionmodel.h>
#include <QItemSelectionModel>

class Telemetry; // Forward declaration
class AddressWidget;


//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    // Getters and Setters for the database
    void setDB(const QString &key, const QString &value); // Updated to use const
    QVariant getDBValue(const QString &key) const;         // Updated to const
    QMap<QString, QVariant>& getdb();                     // Return non-const reference

private slots:
    void updateActions(const QItemSelection &selection);
    void openFile();
    void saveFile();
    void openDialog();  // Slot for opening the new dialog


private:

    Telemetry *telemetry;
    int addCounter;
    AddressWidget *addressWidget;
    QLabel *entryCountLabel;

    void createDB();
    void initializeSystemInfo();

    QAction *openDialogAct;  // Action to open the dialog

    void createMenus();

    QAction *editAct;
    QAction *removeAct;

};
//! [0]

#endif // MAINWINDOW_H
