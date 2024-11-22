// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "addresswidget.h"
#include "adddialog.h"
#include <qtableview.h>
#include "dialog.h"
//#include "MyQApp.h"

#include <QtWidgets>

#include "myQApp.h"

AddressWidget::AddressWidget(QWidget *parent)
    : QTabWidget(parent),
    table(new TableModel(this)),
    newAddressTab(new NewAddressTab(this))
{
    // Access the global telemetry instance from MyQApp
    connect(newAddressTab, &NewAddressTab::entryAdded,
            this, []() {
                Telemetry *telemetry = MyQApp::telemetry();
                //if (telemetry) {
                    // Update or increment the value in telemetry
                   // telemetry->storeOrUpdateValue("addMenuCounter", "1");
               // }
            });
//}


    // Connect sendDetails signal to the addEntry method
    connect(newAddressTab, &NewAddressTab::sendDetails,
            this, &AddressWidget::addEntry);

    // Add the NewAddressTab to the QTabWidget
    addTab(newAddressTab, tr("Address Book"));

    // Initialize tabs (additional setup)
    setupTabs();
}

//! [0]

//! [2]
void AddressWidget::showAddEntryDialog()
{
        telemetry->incCount("addToolMenu");

    AddDialog aDialog;

    // If the dialog was accepted, add the entry
    if (aDialog.exec()) {
        // Store the dialog's results in non- variables
        QString name = aDialog.name();
        QString address = aDialog.address();

        // Now pass these non- lvalues to addEntry
        addEntry(name, address);
    }
}


//! [2]

//! [3]
void AddressWidget::addEntry(const QString &name, const QString &address)
{
    if (!name.front().isLetter()) {
        QMessageBox::information(this, tr("Invalid name"),
            tr("The name must start with a letter."));
    } else if (!table->getContacts().contains({ name, address })) {
        table->insertRows(0, 1, QModelIndex());

        QModelIndex index = table->index(0, 0, QModelIndex());
        table->setData(index, name, Qt::EditRole);
        index = table->index(0, 1, QModelIndex());
        table->setData(index, address, Qt::EditRole);
        removeTab(indexOf(newAddressTab));
    } else {
        QMessageBox::information(this, tr("Duplicate Name"),
            tr("The name \"%1\" already exists.").arg(name));
    }
}
//! [3]

//! [4a]
void AddressWidget::editEntry()
{
    QTableView *temp = static_cast<QTableView*>(currentWidget());
    QSortFilterProxyModel *proxy = static_cast<QSortFilterProxyModel*>(temp->model());
    QItemSelectionModel *selectionModel = temp->selectionModel();

     QModelIndexList indexes = selectionModel->selectedRows();
    QString name;
    QString address;
    int row = -1;

    for ( QModelIndex &index : indexes) {
        row = proxy->mapToSource(index).row();
        QModelIndex nameIndex = table->index(row, 0, QModelIndex());
        QVariant varName = table->data(nameIndex, Qt::DisplayRole);
        name = varName.toString();

        QModelIndex addressIndex = table->index(row, 1, QModelIndex());
        QVariant varAddr = table->data(addressIndex, Qt::DisplayRole);
        address = varAddr.toString();
    }
//! [4a]

//! [4b]
    AddDialog aDialog;
    aDialog.setWindowTitle(tr("Edit a Contact"));
    aDialog.editAddress(name, address);

    if (aDialog.exec()) {
         QString newAddress = aDialog.address();
        if (newAddress != address) {
             QModelIndex index = table->index(row, 1, QModelIndex());
            table->setData(index, newAddress, Qt::EditRole);
        }
    }
}
//! [4b]

//! [5]
void AddressWidget::removeEntry()
{
    QTableView *temp = static_cast<QTableView*>(currentWidget());
    QSortFilterProxyModel *proxy = static_cast<QSortFilterProxyModel*>(temp->model());
    QItemSelectionModel *selectionModel = temp->selectionModel();

     QModelIndexList indexes = selectionModel->selectedRows();

    for (QModelIndex index : indexes) {
        int row = proxy->mapToSource(index).row();
        table->removeRows(row, 1, QModelIndex());
    }

    if (table->rowCount(QModelIndex()) == 0)
        insertTab(0, newAddressTab, tr("Address Book"));
}
//! [5]

//! [1]
void AddressWidget::setupTabs()
{
    using namespace Qt::StringLiterals;
     auto groups = { "ABC"_L1, "DEF"_L1, "GHI"_L1, "JKL"_L1, "MNO"_L1, "PQR"_L1,
                          "STU"_L1, "VW"_L1, "XYZ"_L1 };

    for (QLatin1StringView str : groups) {
         auto regExp = QRegularExpression(QLatin1StringView("^[%1].*").arg(str),
                                               QRegularExpression::CaseInsensitiveOption);

        auto proxyModel = new QSortFilterProxyModel(this);
        proxyModel->setSourceModel(table);
        proxyModel->setFilterRegularExpression(regExp);
        proxyModel->setFilterKeyColumn(0);

        QTableView *tableView = new QTableView;
        tableView->setModel(proxyModel);
        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableView->horizontalHeader()->setStretchLastSection(true);
        tableView->verticalHeader()->hide();
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        tableView->setSortingEnabled(true);

        connect(tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
                this, &AddressWidget::selectionChanged);

        connect(this, &QTabWidget::currentChanged, this, [this, tableView](int tabIndex) {
            if (widget(tabIndex) == tableView)
                emit selectionChanged(tableView->selectionModel()->selection());
        });

        addTab(tableView, str);
    }
}
//! [1]

//! [7]
void AddressWidget::readFromFile()
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("Unable to open file"),
            file.errorString());
        return;
    }

    QList<Contact> contacts;
    QDataStream in(&file);
    in >> contacts;

    if (contacts.isEmpty()) {
        QMessageBox::information(this, tr("No contacts in file"),
                                 tr("The file you are attempting to open contains no contacts."));
    } else {
        for ( auto &contact: std::as_const(contacts))
            addEntry(contact.name, contact.address);
    }
}
//! [7]

//! [6]
void AddressWidget::writeToFile()
{
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Unable to open file"), file.errorString());
        return;
    }

    QDataStream out(&file);
    out << table->getContacts();
}
//! [6]

void AddressWidget::openDialog()
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
