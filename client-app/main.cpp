// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "mainwindow.h"

#include <QApplication>

//! [0]
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mw;
    mw.show();
    return app.exec();
}
//! [0]

/*
#include "mainwindow.h"
#include "telemetry.h"
#include <QApplication>
#include <QScreen>
#include <QStyleHints>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QVariant>
#include <QPair>
#include <QTimeZone>
#include <QSysInfo>

#include "telemetry.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

#if QT_CONFIG(translation)
    QTranslator translator;
    if (translator.load(QLocale::system(), QString::fromUtf16(u"qtbase"), QString::fromUtf16(u"_"),
                        QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        app.installTranslator(&translator);
    }
#endif

    QGuiApplication::setApplicationDisplayName(MainWindow::tr("Standard Dialogs"));

    Telemetry telemetry;

    // at open, data should be send -> complete dataset
    //TODO -> for now all strings with second type counter, maybe latter exploit database
    QList<QPair<QString, QString>> starting_data = {

    //suppose variable
        //should not trigger send, only this one to init
        //{"usageTime, ..."} TODO + change db

        //add button
        {"addMainPage", "0"},             //add main page menu
        {"addToolMenu", "0"},                           //add menu

        //dialog window
            //input dialog
        {"getIntButton", "0"},                                // getInt() button
        {"getDoubleButton", "0"},                       // getDouble() button
        {"getIttemButton", "0"},                          // get Item() button
        {"getTextButton", "0"},                          // get Text() button
        {"getMultiLineButton", "0"},               // get MultLine() button
        //TODO fields

        //Color Dialog
        //TODO: get color
            // options TODO change db to bool
        {"dialogBox1Flag", "false"},                //first checkbox
        {"dialogBox2Flag", "false"},                //second checkbox
        {"dialogBox3Flag", "false"}//,             //third checkbox

        // suppose not variable
        //{"OS", QSysInfo::prettyProductName()},                      //OS
        //{"Locale", QLocale::system().name()},                         //system language
        //{"Timezone", QTimeZone::systemTimeZone().id()}  // system timezone, not the true one
    };
    for (const auto& entry : starting_data) {
        // Store initial data
        telemetry.checkAndUpdate(entry.first, entry.second);
    }
    telemetry.checkAndUpdate("TEST", "4");

    MainWindow mw(&telemetry);

    mw.show();


    return app.exec();
}
*/
