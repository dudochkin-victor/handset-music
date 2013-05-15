/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QCryptographicHash>
#include <QDebug>
#include <QFile>
#include <QGraphicsLinearLayout>
#include <QPixmap>
#include <QtTracker/Tracker>

#include <MAction>
#include <MButton>
#include <MContentItem>
#include <MImageWidget>
#include <MPannableViewport>
#include <MStylableWidget>
#include <MTheme>

#include "appwindow.h"
#include "album.h"
#include "songitem.h"

Album::Album(QString nmmArtistName, QString nmmPerformer, QString nmmMusicAlbum, QString nmmAlbumTitle, QGraphicsItem * parent):
MStylableWidget(parent),
m_nmmPerformer(nmmPerformer),
m_nmmArtistName(nmmArtistName),
m_nmmMusicAlbum(nmmMusicAlbum),
m_nmmAlbumTitle(nmmAlbumTitle), itemCreator(NULL), model(NULL)
{
    qDebug() << "Album::Album";

    setObjectName("Album");
    setContentsMargins(0, 0, 0, 0);

    QGraphicsLinearLayout *layout =
        new QGraphicsLinearLayout(Qt::Horizontal, this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    QString artist = nmmArtistName.toLower();
    artist.replace(QString("urn:artist:"), QString(""));
    artist.replace(QString("%20"), QString(" "));

    QString album = nmmMusicAlbum.toLower();
    album.replace(QString("urn:album:"), QString(""));
    album.replace(QString("%20"), QString(" "));

    QByteArray album_md5 =
        QCryptographicHash::hash(album.toAscii(), QCryptographicHash::Md5);
    QByteArray artist_md5 =
        QCryptographicHash::hash(artist.toAscii(),
                                 QCryptographicHash::Md5);

    QString path = QDir::toNativeSeparators(QDir::homePath()) +
        QDir::separator() + QString(".cache/media-art/album-") +
        artist_md5.toHex() + "-" + album_md5.toHex() + ".jpeg";

    MContentItem *image = new MContentItem(MContentItem::SingleIcon);
//    image->setViewType("background");
    image->setContentsMargins(0, 0, 0, 0);
    image->setObjectName("AlbumArt");

    if (QFile(path).exists()) {
        QImage a = QImage(path);
        int w = a.width();
        int h = a.height();
        int edge = w;
        if (edge > h)
            edge = h;
        QImage b =
            a.scaled(edge, edge, Qt::KeepAspectRatioByExpanding,
                     Qt::SmoothTransformation);
        QImage c = b.copy(0, 0, edge, edge);

        image->setPixmap(QPixmap::fromImage(c));
    } else
        image->setPixmap(*MTheme::pixmap("albumart-generic"));

    layout->addItem(image);

    MPannableViewport *viewport = new MPannableViewport(this);
    MWidgetController *container = new MWidgetController;
    container->setViewType("background");
    container->setObjectName("AlbumContainer");
    container->setContentsMargins(0, 0, 0, 0);
    viewport->setObjectName("AlbumViewport");

    QGraphicsLinearLayout *listLayout =
        new QGraphicsLinearLayout(Qt::Vertical, container);
    listLayout->setSpacing(0);
    listLayout->setContentsMargins(0, 0, 0, 0);

    viewport->setWidget(container);
    layout->addItem(viewport);

    list = new MList(this);
    itemCreator = new SongItemCreator();
    list->setCellCreator(itemCreator);
    model = new SongsListModel(nmmPerformer, nmmMusicAlbum);
    list->setItemModel(model);

    MAction *action = new MAction("Play", list);
    action->setLocation(MAction::ObjectMenuLocation);
    list->addAction(action);
    connect(action, SIGNAL(triggered()), this,
            SLOT(playSongFromObjectMenu()));

    action = new MAction("Add to play queue", list);
    action->setLocation(MAction::ObjectMenuLocation);
    list->addAction(action);
    connect(action, SIGNAL(triggered()), this,
            SLOT(addToQueueFromObjectMenu()));

    action = new MAction("Add to a playlist", list);
    action->setLocation(MAction::ObjectMenuLocation);
    list->addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(addToPlaylist()));

    connect(list, SIGNAL(itemClicked(QModelIndex)), this,
            SLOT(play(QModelIndex)));
    connect(list, SIGNAL(itemLongTapped(QModelIndex)), this,
            SLOT(itemLongTapped(QModelIndex)));

    listLayout->addItem(list);

    //TODO: get current playing index from backend data model and scrollTo()
}

Album::~Album()
{
    qDebug() << "Album::~Album";
//    delete itemCreator;
    delete model;
}

void Album::itemLongTapped(const QModelIndex & index)
{
    qDebug() << "Album::itemLongTapped" << index.row();

    longTappedIndex = index;
}

void Album::addToPlaylist()
{
    qDebug() << "Album::addToPlaylist";
    QVector < QStringList > vector;

    vector.append(QStringList() << longTappedIndex.data(Qt::UserRole).
                  toString());
    AppWindow::instance()->addToPlaylist(vector);
}

void Album::play(QModelIndex index)
{
    AppWindow *window = AppWindow::instance();

    window->play(index.data(Qt::UserRole).toString(),
                 index.data(Qt::UserRole + 1).toString());
}

void Album::addToQueueFromObjectMenu()
{
    qDebug() << "Album::addToQueueFromObjectMenu";
    AppWindow::instance()->addToQueue(QStringList() << longTappedIndex.
                                      data(Qt::UserRole).toString(),
                                      QStringList() << longTappedIndex.
                                      data(Qt::UserRole + 1).toString());
}

void Album::playSongFromObjectMenu()
{
    qDebug() << "Album::playSongFromObjectMenu" << longTappedIndex.
        data(Qt::UserRole).toString() << longTappedIndex.
        data(Qt::UserRole + 1).toString();
    play(longTappedIndex);
}
