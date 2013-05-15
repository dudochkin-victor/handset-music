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
#include <QCryptographicHash>
#include "listwithurimodel.h"
#include "searchengine.h"
#include "appwindow.h"

ListWithUriModel::
ListWithUriModel(const QStringList & songurls):urls(songurls),
currentPlaying(-1)
{
    metadata = new QVector < SongMetaData > (urls.size());

    QVector < SongMetaData >::iterator j = metadata->begin();
    for (QStringList::const_iterator i = urls.begin(); i != urls.end();
         i++, j++) {
        QVector < QStringList > result;

        SearchEngine::getSongTitleArtistAlbum(*i, result);
        /* result only contains one result, with three column */
        j->songName = result.value(0).value(0);
        j->artistName = result.value(0).value(1);
        j->albumName = result.value(0).value(2);
        j->mimeType = result.value(0).value(3);
        j->duration = result.value(0).value(4);
    }

    connect(AppWindow::instance(), SIGNAL(queueUriRemoved(int)), this, SLOT(removeUri(int)));
}

ListWithUriModel::~ListWithUriModel()
{
    delete metadata;
}

QString ListWithUriModel::getAlbumArtLocation(const QModelIndex & index) const
{
    if ((metadata->value(index.row())).albumArt.isEmpty()) {
        QString albumInfo =
            metadata->value(index.row()).albumName.toLower();
        QString artistInfo =
            metadata->value(index.row()).artistName.toLower();

        QByteArray album_md5 =
            QCryptographicHash::hash(albumInfo.toAscii(),
                                     QCryptographicHash::Md5);
        QByteArray artist_md5 =
            QCryptographicHash::hash(artistInfo.toAscii(),
                                     QCryptographicHash::Md5);

        QString path = QDir::toNativeSeparators(QDir::homePath()) +
            QDir::separator() + QString(".cache/media-art/album-") +
            artist_md5.toHex() + "-" + album_md5.toHex() + ".jpeg";
        qDebug() << "Album art thumbnail of artist " << artistInfo <<
            "album " << albumInfo << " is: " << path;

        metadata->data()[(index.row())].albumArt = path;
    }

    return metadata->value(index.row()).albumArt;
}

QVariant ListWithUriModel::data(const QModelIndex & index, int role) const
{
    // This function will be called many times during fast panning, lets
    // check boundaries and validnes only in debug mode
    Q_ASSERT(index.isValid());
    Q_ASSERT(index.row() < urls.size());

    if (role == Qt::DisplayRole) {      //return the display name for song
        return QVariant::fromValue(metadata->value(index.row()).songName);
    } else if (role == Qt::UserRole) {
        return QVariant::fromValue(getAlbumArtLocation(index));
    } else if (role == Qt::UserRole + 1) {
        return QVariant::fromValue(metadata->value(index.row()).mimeType);
    } else if (role == (Qt::UserRole + 2)) {
        if (index.row() == currentPlaying) {
            return QVariant::fromValue(true);
        } else {
            return QVariant::fromValue(false);
        }
    } else if (role == (Qt::UserRole + 3)) {
        return QVariant::fromValue(metadata->value(index.row()).duration);
    } else if (role == (Qt::UserRole + 4)) { //Artist name
        return QVariant::fromValue(metadata->value(index.row()).artistName);
    } else if (role == (Qt::UserRole + 5)) {  //Album name
        return QVariant::fromValue(metadata->value(index.row()).albumName);
    }
    return QVariant();
}

bool ListWithUriModel::setData(const QModelIndex & index,
                                  const QVariant & value, int role =
                                  Qt::UserRole + 2)
{
    int prevCurrentPlaying;

    if (role != Qt::UserRole + 2)
        return false;

    qDebug() << "NowplayingListModel::setData" << value.toBool();

    if (currentPlaying == index.row())
        return true;

    prevCurrentPlaying = currentPlaying;
    currentPlaying = index.row();

    if (prevCurrentPlaying != -1) {
        QModelIndex modelIndex =
            QAbstractTableModel::index(prevCurrentPlaying, 0);
        emit dataChanged(modelIndex, modelIndex);
    }

    emit dataChanged(index, index);

    return true;
}

int ListWithUriModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return 1;
}

int ListWithUriModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return urls.size();
}

void ListWithUriModel::removeUri(int index)
{
    qDebug() << "ListWithUriModel::removeUri" << index;

    if (currentPlaying == index)
        currentPlaying = -1;

    if (currentPlaying > index)
        currentPlaying--;

    beginResetModel();
    urls.removeAt(index);
    endResetModel();

}
