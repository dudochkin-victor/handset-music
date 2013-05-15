/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef UTILS_H
#define UTILS_H

#include <QThread>
#include <QPixmap>

class postLoadItems:public QThread {
public:
    QVector < QStringList > *items;
    bool loadFinished;

    postLoadItems();
    void run();
};

extern postLoadItems *postThread;

#endif                          // UTILS_H
