/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef ALLARTIST_H
#define ALLARTIST_H

#include <QGraphicsWidget>
#include <MList>
#include <QModelIndex>
#include <MStylableWidget>

#include "artistitemcreator.h"
#include "artistslistmodel.h"

class AllArtist:public MStylableWidget {
    Q_OBJECT;

public:
    AllArtist(MStylableWidget * parent = 0);
    ~AllArtist();

public slots:
    virtual void clicked();
    void itemLongTapped(const QModelIndex & index);
    void showArtist(const QModelIndex & index);
    void showArtistFromObjectMenu();
    void playArtistFromObjectMenu();
    void addToQueueFromObjectMenu();
    void addToPlaylist();
    // *** open to public, so alphabetbar can access list.
    MList *getList() {
        return list;
    };

private:
    MList * list;
    ArtistItemCreator *itemCreator;
    ArtistsListModel *model;
    QModelIndex longTappedIndex;
    QModelIndex clickWaitIndex;
    QTimer *doubleTapTimer;
    bool clickWait;
    int doubleTimeoutInterval;
private slots:
    void getSongs(QString artist, QStringList *uris, QStringList *mimeTypes);
    void listClicked(const QModelIndex & index);
    void waitTimeout();
};

#endif                          // ALLARTIST_H
