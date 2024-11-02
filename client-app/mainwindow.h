// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "addresswidget.h"

#include <QMainWindow>
#include "telemetry.h"

//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void updateActions(const QItemSelection &selection);
    void openFile();
    void saveFile();

private:
    Telemetry *telemetry;
    int addCounter;

    void createMenus();

    AddressWidget *addressWidget;
    QAction *editAct;
    QAction *removeAct;

    QLabel *entryCountLabel;
};
//! [0]

#endif // MAINWINDOW_H
