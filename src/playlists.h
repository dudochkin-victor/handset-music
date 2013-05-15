/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef PLAYLISTS_H
#define PLAYLISTS_H

#include <MStylableWidget>
#include <QSettings>
#include <MTextEdit>

#include "playlistsmodel.h"

class QGraphicsLinearLayout;
class MList;

class Playlists:public MStylableWidget {
    Q_OBJECT;

public:
    Playlists(MStylableWidget * parent = 0);
    ~Playlists();

    QAbstractTableModel *getModel() const {
        return model;
    };

    MList *getList() {
        return list;
    };

    QStringList uris;

public slots:
    void newPlaylistFromObjectMenu();

private slots:
    void openPlaylist(const QModelIndex & index);
    void deletePlaylist();
    void playPlaylistFromObjectMenu();
    void loadPlaylistFromObjectMenu();
    void deletePlaylistFromObjectMenu();
    void showDialog();
    void newPlaylist();
    void itemLongTapped(const QModelIndex & index);

private:
    QSettings settings;
    QGraphicsLinearLayout *listLayout;
    MList *list;
    MTextEdit *playlistEdit;
    PlaylistsModel *model;
    QModelIndex longTappedIndex;
};

#endif                          // PLAYLISTS_H
