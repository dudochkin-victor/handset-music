/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "listwithuri.h"

class Playlist:public ListWithUri {
    Q_OBJECT;

public:
    Playlist(const QStringList & songurls, CrumbBar *crumbbar, QGraphicsItem * parent = 0);
    virtual ~Playlist();

private slots:
    void playFromObjectMenu();
    void loadFromObjectMenu();
    void itemLongTapped(const QModelIndex & index);
private:
    QModelIndex longTappedIndex;
};


#endif  // PLAYLIST_H
