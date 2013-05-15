/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef ALBUM_H
#define ALBUM_H

#include <MStylableWidget>
#include <MWidgetStyle>
#include <MList>

#include "songitemcreator.h"
#include "songslistmodel.h"

class Album:public MStylableWidget {
    Q_OBJECT;

  public:
    Album(const QString nmmArtistName, const QString nmmPerformer,
          const QString nmmMusicAlbum, const QString nmmAlbumTitle,
          QGraphicsItem * parent = 0);
    ~Album();

    QString nmmArtistName() const {
        return m_nmmArtistName;
    };
    QString nmmPerformer() const {
        return m_nmmPerformer;
    };
    QString nmmMusicAlbum() const {
        return m_nmmMusicAlbum;
    };
    QString nmmAlbumTitle() const {
        return m_nmmAlbumTitle;
    };
    QAbstractTableModel *getModel() const {
        return model;
    };

public slots:
    void addToPlaylist();
    void play(QModelIndex index);
    void itemLongTapped(const QModelIndex & index);
    void addToQueueFromObjectMenu();
    void playSongFromObjectMenu();

private:
    QString m_nmmPerformer;
    QString m_nmmArtistName;
    QString m_nmmMusicAlbum;
    QString m_nmmAlbumTitle;
//    M_STYLABLE_WIDGET(MWidgetStyle);

    MList *list;
    SongItemCreator *itemCreator;
    SongsListModel *model;
    QModelIndex longTappedIndex;
};

#endif                          // ALBUM_H
