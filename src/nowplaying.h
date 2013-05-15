/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef NOWPLAYING_H
#define NOWPLAYING_H

#include "listwithuri.h"

class NowPlaying:public ListWithUri {
    Q_OBJECT;

public:
    NowPlaying(const QStringList & songurls, CrumbBar *crumbbar, QGraphicsItem * parent = 0);
    virtual ~NowPlaying();

private slots:
    void playFromObjectMenu();
    void unloadFromObjectMenu();
    void addToPlaylist();
    void itemLongTapped(const QModelIndex & index);
private:
    QModelIndex longTappedIndex;
};


#endif  // NOWPLAYING_H
