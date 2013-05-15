/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef SONGSLISTMODEL_H
#define SONGSLISTMODEL_H

#include <QAbstractTableModel>

class SongsListModel:public QAbstractTableModel {
    Q_OBJECT

public:
    SongsListModel(const QString & artist, const QString & album);
    virtual ~ SongsListModel();

    virtual int rowCount(const QModelIndex & parent = QModelIndex())const;
    virtual int columnCount(const QModelIndex & parent =
                            QModelIndex())const;
    virtual QVariant data(const QModelIndex & index, int role =
                          Qt::DisplayRole) const;

private:
    SongsListModel();

    QVector < QStringList > *songs;
    QString nmmArtist;
    QString nmmAlbum;
    int currentPlaying;

private slots:
    void changeState(int index);
};
#endif                          // SONGSLISTMODEL_H
