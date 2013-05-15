/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef ARTISTLIST_H
#define ARTISTLIST_H

#include <MStylableWidget>
#include <MWidgetStyle>
#include <MList>

#include "albumitemcreator.h"
#include "albumslistmodel.h"

class ArtistList:public MStylableWidget {
    Q_OBJECT;

public:
    ArtistList(const QString nmmArtistName, const QString nmmPerformer,
               QGraphicsItem * parent = 0);
    ~ArtistList();

    QString nmmArtistName() const {
        return m_nmmArtistName;
    };
    QString nmmPerformer() const {
        return m_nmmPerformer;
    };

public slots:
    void itemLongTapped(const QModelIndex & index);
    void showAlbum(const QModelIndex & index);
    void showAndPlayAlbum();
    void addToPlaylist();
    void showAlbumFromObjectMenu();
    void playAlbumFromObjectMenu();
    void addToQueueFromObjectMenu();
    void queueTasks();
    void dequeueTasks();

private:
    QString m_nmmArtistName;
    QString m_nmmPerformer;
//    M_STYLABLE_WIDGET(MWidgetStyle);

    MList *list;
    AlbumItemCreator *itemCreator;
    AlbumsListModel *model;
    QModelIndex longTappedIndex;
    QModelIndex clickWaitIndex;
    QTimer *doubleTapTimer;
    bool clickWait;
    int doubleTimeoutInterval;
private slots:
    void getSongs(QString artist, QString album, QStringList *uris, QStringList *mimeTypes);
    void listClicked(const QModelIndex & index);
    void waitTimeout();
    void onOrientationChanged(M::Orientation orientation);
};

#endif                          // ARTISTLIST_H
