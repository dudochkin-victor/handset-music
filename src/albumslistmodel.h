/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef ALBUMSLISTMODEL_H
#define ALBUMSLISTMODEL_H

#include <QAbstractTableModel>
#include <QPixmap>

struct AlbumArt {
public:
    AlbumArt() {};
    AlbumArt(QPixmap & defaultAlbumArtPixmap) :
    albumArtPixmap(defaultAlbumArtPixmap), loaded(false) {}

    QString albumArtLocation;
    QPixmap albumArtPixmap;
    bool loaded;
};

class AlbumsListModel:public QAbstractTableModel {
    Q_OBJECT

public:
    AlbumsListModel(const QString & artist);
    virtual ~ AlbumsListModel();

    virtual int rowCount(const QModelIndex & parent = QModelIndex())const;
    virtual int columnCount(const QModelIndex & parent =
                            QModelIndex())const;
    virtual QVariant data(const QModelIndex & index, int role =
                          Qt::DisplayRole) const;

    void addTask(const QModelIndex & index);
    void processTask(const QModelIndex & index);

public slots:
    void startTaskQueue();
    void stopTaskQueue();

private:
    AlbumsListModel();
    QString getAlbumArtLocation(const QString & album, int index) const;

    QVector < QStringList > *albums;
    QVector < AlbumArt > *albumArts;

    QString nmmArtist;
    QList < QModelIndex > tasks;

    QPixmap defaultThumbnail;
};
#endif                          // ALBUMSLISTMODEL_H
