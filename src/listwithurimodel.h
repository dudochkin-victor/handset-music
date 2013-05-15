/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef LISTWITHURIMODEL_H
#define LISTWITHURIMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QString>

struct SongMetaData {
    QString songName;
    QString artistName;
    QString albumName;
    QString mimeType;
    QString albumArt;
    QString duration;
};

class ListWithUriModel:public QAbstractTableModel {
    Q_OBJECT

public:
    ListWithUriModel(const QStringList & songurls);
    virtual ~ ListWithUriModel();

    virtual int rowCount(const QModelIndex & parent = QModelIndex())const;
    virtual int columnCount(const QModelIndex & parent =
                            QModelIndex())const;
    virtual QVariant data(const QModelIndex & index, int role =
                          Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex & index, const QVariant & value,
                         int role);
    QString getAlbumArtLocation(const QModelIndex & index) const;

private:
    QVector < SongMetaData > *metadata;
    QStringList urls;
    int currentPlaying;

private slots:
    void removeUri(int index);
};
#endif                          // LISTWITHURIMODEL_H
