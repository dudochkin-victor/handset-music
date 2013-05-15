/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef SEARCHENGINEUNITTESTS_H
#define SEARCHENGINEUNITTESTS_H

#include <QObject>
#include <QtTest/QtTest>
#include "searchengine.h"


class SearchEngineUnitTests: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void cleanupTestCase();

    void init();
    void cleanup();

    void testNewPlaylist();
    void testDeletePlaylist();

    void testAddItem();
    void testDeleteItem();

    void testGetPlaylist();
    void testGetPlaylistItem();

    void testAddItems();
};

#endif //SEARCHENGINEUNITTESTS_H

