// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef ADDRESSWIDGET_H
#define ADDRESSWIDGET_H
\

#include "newaddresstab.h"
#include "tablemodel.h"
#include <QItemSelection>
#include <QTabWidget>
#include <QStandardPaths>
#include <QTabWidget>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QSortFilterProxyModel;
class QItemSelectionModel;
QT_END_NAMESPACE

class Telemetry; // Forward declaration
//! [0]
class AddressWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit AddressWidget(QWidget *parent = nullptr); // Constructor updated to accept telemetry
    void readFromFile();
    void writeToFile();

public slots:
    void showAddEntryDialog();
    void addEntry(const QString &name, const QString &address);
    void editEntry();
    void removeEntry();
    void openDialog();

signals:
    void selectionChanged(const QItemSelection &selected);
    void entryAdded();

private:
    void setupTabs();

    Telemetry *telemetry; // Declare telemetry object

    inline static QString fileName =
        QStandardPaths::standardLocations(QStandardPaths::TempLocation).value(0)
        + QStringLiteral("/addressbook.dat");
    TableModel *table;
    NewAddressTab *newAddressTab;
};
//! [0]

#endif // ADDRESSWIDGET_H
