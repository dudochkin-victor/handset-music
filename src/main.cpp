/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#include <MApplication>
#include <MApplicationWindow>
#include <QDBusConnection>

#include "appwindow.h"
#include "utils.h"
int main(int argc, char *argv[])
{
    MApplication app(argc, argv);
    app.setOrganizationName("/com/meego/meegomusic");

    postThread = new postLoadItems();
    postThread->start(QThread::HighPriority);

    AppWindow w;
    w.show();

    QDBusConnection::sessionBus().registerService("com.meego.music");
    QDBusConnection::sessionBus().registerObject("/", &w);

    return app.exec();
}
