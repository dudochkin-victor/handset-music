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
#include <algorithm>
#include <MTheme>

#include "albumslistmodel.h"
#include "searchengine.h"

static int compare(const QStringList & a, const QStringList & b)
{
    if (a[0] < b[0])
        return 1;
    else
        return 0;
}

AlbumsListModel::AlbumsListModel(const QString & artist)
:  
nmmArtist(artist),
defaultThumbnail(*MTheme::pixmap("albumart-generic"))
{
    //get all albums
    albums = new QVector < QStringList > ();
    SearchEngine::getMusicArtistAlbums(artist, *albums);

    //Assuming empty one is the first, since we use SORT inside Sparql
    if (((albums->value(0))[1]).isEmpty()) {    // we have an unknown album need to be inserted
        QStringList unknownAlbum;
        unknownAlbum.append("Unknown");
        unknownAlbum.append("Unknown");
        unknownAlbum.append("Unknown");

        //remove the first empty one, and add unknown one
        albums->remove(0);
        albums->append(unknownAlbum);

        //sort it
        std::sort(albums->begin(), albums->end(), compare);
    }
    //create albumArtss

    albumArts =
        new QVector < AlbumArt > (albums->size(), defaultThumbnail);
}

AlbumsListModel::~AlbumsListModel()
{
    delete albums;
    delete albumArts;
    tasks.clear();
}

QString AlbumsListModel::getAlbumArtLocation(const QString & album,
                                             int index) const
{
    if ((albumArts->value(index)).albumArtLocation.isEmpty()) {
        QString albumInfo = album.toLower();
        QString artistInfo = nmmArtist.toLower();

        albumInfo.replace(QString("urn:album:"), QString(""));
        albumInfo.replace(QString("%20"), QString(" "));

        artistInfo.replace(QString("urn:artist:"), QString(""));
        artistInfo.replace(QString("%20"), QString(" "));

        QByteArray album_md5 =
            QCryptographicHash::hash(albumInfo.toAscii(),
                                     QCryptographicHash::Md5);
        QByteArray artist_md5 =
            QCryptographicHash::hash(artistInfo.toAscii(),
                                     QCryptographicHash::Md5);

        QString path = QDir::toNativeSeparators(QDir::homePath()) +
            QDir::separator() + QString(".cache/media-art/album-") +
            artist_md5.toHex() + "-" + album_md5.toHex() + ".jpeg";
        qDebug() << "Album art thumbnail of artist " << nmmArtist <<
            "album " << album << " is: " << path;

        ((*albumArts)[index]).albumArtLocation = path;
    }

    return (albumArts->value(index)).albumArtLocation;
}

QVariant AlbumsListModel::data(const QModelIndex & index, int role) const
{
    // This function will be called many times during fast panning, lets
    // check boundaries and validnes only in debug mode
    Q_ASSERT(index.isValid());
    Q_ASSERT(index.row() < artists->size());

    if (role == Qt::DisplayRole) {      //return the display name for album
        return QVariant::fromValue((albums->value(index.row()))[0]);
    } else if (role == Qt::UserRole) {  //return the string that could be used for query
        return QVariant::fromValue((albums->value(index.row()))[1]);
    } else if (role == Qt::DecorationRole) {
        return QVariant::fromValue < QPixmap >
            (albumArts->value(index.row()).albumArtPixmap);
    }

    return QVariant();
}

int AlbumsListModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return 1;
}

int AlbumsListModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return albums->size();
}

void AlbumsListModel::addTask(const QModelIndex & index)
{
    qDebug() << "AlbumsListModel::addTask(), row is " << index.row();
    tasks << index;
}

void AlbumsListModel::startTaskQueue()
{
    if (tasks.isEmpty())
        return;

    const QModelIndex & index = tasks.takeFirst();
    processTask(index);
    if (!tasks.isEmpty())
        QTimer::singleShot(0, this, SLOT(startTaskQueue()));
}

void AlbumsListModel::stopTaskQueue()
{
    qDebug() << "AlbumsListModel::stopTaskQueue" << tasks.count();
    tasks.clear();
}

void AlbumsListModel::processTask(const QModelIndex & index)
{
    int row = index.row();
    qDebug() << "AlbumsListModel::processTask()" << row;
    AlbumArt *arts = albumArts->data();

    if (arts[row].loaded)
        return;

    const QString & pixmapURI =
        getAlbumArtLocation((albums->value(row))[1], row);

    if (QFile(pixmapURI).exists()) {
        QImage a = QImage(pixmapURI);
        int w = a.width();
        int h = a.height();
        int edge = w;
        if (edge > h)
            edge = h;
        QImage b =
            a.scaled(edge, edge, Qt::KeepAspectRatioByExpanding,
                     Qt::SmoothTransformation);
        QImage c = b.copy(0, 0, edge, edge);
        arts[row].albumArtPixmap = QPixmap::fromImage(c);

        arts[row].loaded = true;

        qDebug() << "QFile(pixmapURI).exists" << row;
        emit dataChanged(index, index);
    }
}
