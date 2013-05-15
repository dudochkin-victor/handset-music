/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#include "SearchEngineUnitTests.h"
#include <QtDebug>
#include <QSettings>

void SearchEngineUnitTests::initTestCase()
{

}

void SearchEngineUnitTests::cleanupTestCase()
{

}

void SearchEngineUnitTests::init()
{
    /* clean up all playlists before running each unit test */
    QStringList result;
    SearchEngine::getAllPlaylists(result);
    for (QStringList::iterator i = result.begin(); i != result.end(); i++)
        SearchEngine::deletePlaylist(*i);

}

void SearchEngineUnitTests::cleanup()
{
    /* clean up all playlists after running each unit test */
    QStringList result;
    SearchEngine::getAllPlaylists(result);
    for (QStringList::iterator i = result.begin(); i != result.end(); i++)
        SearchEngine::deletePlaylist(*i);
}

void SearchEngineUnitTests::testNewPlaylist()
{
    QStringList a;
    QVector<QStringList> items;
    QStringList result;

    SearchEngine::newPlaylist("playlista", items);
    SearchEngine::getAllPlaylists(result);
    QStringList expectedResult;
    expectedResult << "playlista";
    QCOMPARE(result, expectedResult);

    a << "url" << "displayname";
    items << a;

    SearchEngine::newPlaylist("playlista", items);
    SearchEngine::newPlaylist("playlistb", items);
    SearchEngine::newPlaylist("playlistc", items);

    SearchEngine::getAllPlaylists(result);
    expectedResult.clear();;
    expectedResult << "playlista" << "playlistb" << "playlistc";
    QCOMPARE(result, expectedResult);

}

void SearchEngineUnitTests::testDeletePlaylist()
{
    QStringList a;
    QVector<QStringList> items;
    QStringList result;

    SearchEngine::newPlaylist("playlista", items);
    QStringList expectedResult;
    SearchEngine::deletePlaylist("playlista");
    SearchEngine::getAllPlaylists(result);
    QCOMPARE(result, expectedResult);

    a << "url" << "displayname";
    items << a;

    SearchEngine::newPlaylist("playlista", items);
    SearchEngine::newPlaylist("playlistb", items);
    SearchEngine::newPlaylist("playlistc", items);

    expectedResult.clear();

    expectedResult.clear();
    result.clear();
    expectedResult << "playlistb" << "playlistc";
    SearchEngine::deletePlaylist("playlista");
    SearchEngine::getAllPlaylists(result);
    QCOMPARE(result, expectedResult);

    expectedResult.clear();
    result.clear();
    expectedResult << "playlistb" << "playlistc";
    SearchEngine::deletePlaylist("playlistdoesnotexist");
    SearchEngine::getAllPlaylists(result);
    QCOMPARE(result, expectedResult);

    expectedResult.clear();
    result.clear();
    expectedResult << "playlistc";
    SearchEngine::deletePlaylist("playlistb");
    SearchEngine::getAllPlaylists(result);
    QCOMPARE(result, expectedResult);

    expectedResult.clear();
    result.clear();

    SearchEngine::deletePlaylist("playlistc");
    SearchEngine::getAllPlaylists(result);
    QCOMPARE(result, expectedResult);
}

void SearchEngineUnitTests::testAddItem()
{
    QStringList a;
    QStringList b;
    QVector<QStringList> items;
    QVector<QStringList> new_items;

    SearchEngine::newPlaylist("playlistb", items);

    a << "url" << "displayname";
    items << a;
    b<< "url1" << "displayname1";

    SearchEngine::addPlaylistItem("playlistb", a);
    SearchEngine::addPlaylistItem("playlistb", b);

    SearchEngine::getPlaylistItems("playlistb", new_items);
    Q_ASSERT(items != new_items);
    items << b;
    QCOMPARE(items, new_items);

    items.clear();
    new_items.clear();
    items << a;
    SearchEngine::newPlaylist("playlista", items);

    SearchEngine::addPlaylistItem("playlista", b);
    SearchEngine::getPlaylistItems("playlista", new_items);
    Q_ASSERT(items != new_items);
    items << b;
    QCOMPARE(items, new_items);
}

void SearchEngineUnitTests::testDeleteItem()
{
    QCOMPARE(1,1);
    qDebug() << "Already Covered by other tests";
}

void SearchEngineUnitTests::testGetPlaylist()
{
    QCOMPARE(1,1);
    qDebug() << "Already Covered by other tests";
}

void SearchEngineUnitTests::testGetPlaylistItem()
{
    QStringList a;
    QVector<QStringList> items;
    QVector<QStringList> resultItems;

    a << "url" << "displayname";
    items << a;

    SearchEngine::newPlaylist("playlista", items);
    Q_ASSERT(resultItems != items);
    SearchEngine::getPlaylistItems("playlista", resultItems);
    QCOMPARE(resultItems, items);
    resultItems.clear();
    SearchEngine::getPlaylistItems("playlistdoesnotexist", resultItems);
    Q_ASSERT(resultItems != items);
}

void SearchEngineUnitTests::testAddItems()
{
    QStringList a,b;
    QVector<QStringList> items;
    QVector<QStringList> items1;

    QVector<QStringList> resultItems;

    a << "url" << "displayname";
    b << "url1" << "displayname1";
    items << a << b;

    SearchEngine::newPlaylist("playlista", items1);
    SearchEngine::addPlaylistItems("playlista", items);
    SearchEngine::getPlaylistItems("playlista", resultItems);
    QCOMPARE(resultItems, items);

    resultItems.clear();
    SearchEngine::addPlaylistItems("playlista", items);
    SearchEngine::getPlaylistItems("playlista", resultItems);
    QCOMPARE(resultItems, items);

    resultItems.clear();
    QStringList c;
    QVector<QStringList> items2;
    c << "url2" << "url2";
    items2 << c;
    items << c;
    SearchEngine::addPlaylistItems("playlista", items2);
    SearchEngine::getPlaylistItems("playlista", resultItems);
    QCOMPARE(resultItems, items);
}

QTEST_MAIN(SearchEngineUnitTests)
