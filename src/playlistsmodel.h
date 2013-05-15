/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef PLAYLISTSMODEL_H
#define PLAYLISTSMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class PlaylistsModel:public QAbstractTableModel {
    Q_OBJECT

public:
    PlaylistsModel();
    virtual ~ PlaylistsModel() {
    };

    virtual int rowCount(const QModelIndex & parent = QModelIndex())const;
    virtual int columnCount(const QModelIndex & parent =
                            QModelIndex())const;
    virtual QVariant data(const QModelIndex & index, int role =
                          Qt::DisplayRole) const;
    void newPlaylist(const QString & playlistName);
    void deletePlaylist(const QModelIndex & index);

private:
    QStringList playlists;
};

#endif                          // PLAYLISTSMODEL_H
