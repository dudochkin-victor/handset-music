/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#include <QDebug>
#include <QFileInfo>
#include <QUrl>
#include "songslistmodel.h"
#include "searchengine.h"
#include "appwindow.h"

SongsListModel::SongsListModel(const QString & artist, const QString & album):nmmArtist(artist), nmmAlbum(album),
currentPlaying
(-1)
{
    //get all songs
    songs = new QVector < QStringList > ();
    SearchEngine::getMusicAlbumSongs(artist, album, *songs);

    //connect queueIndexChanged signal to change the state of song item,
    //that is to change the color of currently playing song
    connect(AppWindow::instance(), SIGNAL(queueIndexChanged(int)), this,
            SLOT(changeState(int)));

    if (AppWindow::instance()->index() != -1)
        changeState(AppWindow::instance()->index());
}

SongsListModel::~SongsListModel()
{
    delete songs;

    qDebug() << "SongsListModel::~SongsListModel";
}

QVariant SongsListModel::data(const QModelIndex & index, int role) const
{
    // This function will be called many times during fast panning, lets
    // check boundaries and validnes only in debug mode
    Q_ASSERT(index.isValid());
    Q_ASSERT(index.row() < songs->size());

    if (role == Qt::DisplayRole) {      //return the display name for song
        if (!(songs->value(index.row()))[2].isEmpty())
            return QVariant::fromValue((songs->value(index.row()))[2]);
        else {
            QUrl url =
                QUrl::fromEncoded((songs->value(index.row()))[0].
                                  toAscii());
            QString localPath = url.toLocalFile();
            QFileInfo fileInfo = QFileInfo(localPath);
            QString nameToShow = fileInfo.fileName();
            nameToShow.remove(nameToShow.lastIndexOf("."),
                              nameToShow.size() -
                              nameToShow.lastIndexOf("."));
            return QVariant::fromValue(nameToShow);
        }
    } else if (role == Qt::UserRole) {
        return QVariant::fromValue((songs->value(index.row()))[0]);
    } else if (role == (Qt::UserRole + 1)) {
        return QVariant::fromValue((songs->value(index.row()))[1]);
    } else if (role == (Qt::UserRole + 2)) {
        if (index.row() == currentPlaying) {
            return QVariant::fromValue(true);
        } else {
            return QVariant::fromValue(false);
        }
    } else if (role == (Qt::UserRole + 3)) {
        return QVariant::fromValue((songs->value(index.row()))[3]);
    }
    return QVariant();
}

int SongsListModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return 1;
}

int SongsListModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return songs->size();
}

void SongsListModel::changeState(int index)
{
    qDebug() << "SongsListModel::changeState" << (AppWindow::instance())->
        queue()[index];
    int size = songs->size();

    for (int i = 0; i < size; i++) {
        if ((songs->value(i))[0] ==
            (AppWindow::instance())->queue()[index]) {
            qDebug() << "SongsListModel::changeState match" << i;

            if (i != currentPlaying) {  //emit dataChanged signal
                int prevCurrentPlaying = currentPlaying;
                currentPlaying = i;

                if (prevCurrentPlaying != -1) {
                    QModelIndex modelIndex =
                        QAbstractTableModel::index(prevCurrentPlaying, 0);
                    emit dataChanged(modelIndex, modelIndex);
                }

                QModelIndex modelIndex =
                    QAbstractTableModel::index(currentPlaying, 0);
                emit dataChanged(modelIndex, modelIndex);
            }
            break;
        }
    }
}
