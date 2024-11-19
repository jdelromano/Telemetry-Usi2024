// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <qitemselectionmodel.h>

class Telemetry; // Forward declaration
class AddressWidget;


//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    //QMap<QString, QString> db;

    void updateDB(const QString &key, const QString &value);
        QString getDBValue(const QString &key) const;


private slots:
    void updateActions(const QItemSelection &selection);
    void openFile();
    void saveFile();
    void openDialog();  // Slot for opening the new dialog

private:
    //QMap<QString, QJsonValue> db;
    QMap<QString, QString> db;
    Telemetry *telemetry;
    int addCounter;

    void createDB();
    void initializeSystemInfo();

    QAction *openDialogAct;  // Action to open the dialog

public:
    const QMap<QString, QString>& getdb() const;


    void createMenus();

    AddressWidget *addressWidget;
    QAction *editAct;
    QAction *removeAct;

    QLabel *entryCountLabel;
};
//! [0]

#endif // MAINWINDOW_H
