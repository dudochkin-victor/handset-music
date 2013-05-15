/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QApplication>
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
#include <MList>

#include "appwindow.h"
#include "artistlist.h"
#include "albumitem.h"
#include "albumslistmodel.h"
#include "searchengine.h"

#define SONGS_QUERY "SELECT nie:url(?uri) WHERE { ?uri a nmm:MusicPiece . ?uri nmm:performer '%1' . ?uri nmm:musicAlbum '%2' } ORDER BY nmm:trackNumber"

ArtistList::ArtistList(const QString nmmArtistName,
                       const QString nmmPerformer,
                       QGraphicsItem * parent):MStylableWidget(parent),
m_nmmArtistName(nmmArtistName), m_nmmPerformer(nmmPerformer),
itemCreator(NULL), model(NULL), clickWait(false)
{
    qDebug() << "ArtistList::ArtistList";
    doubleTapTimer = new QTimer(this);
    doubleTimeoutInterval = QApplication::doubleClickInterval();

    setObjectName("ArtistList");
    setContentsMargins(0, 0, 0, 0);

    QGraphicsLinearLayout *layout =
        new QGraphicsLinearLayout(Qt::Vertical, this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    MPannableViewport *viewport = new MPannableViewport(this);
    MWidgetController *container = new MWidgetController;
    container->setViewType("background");
    container->setObjectName("ArtistListContainer");
    container->setContentsMargins(0, 0, 0, 0);
    viewport->setObjectName("ArtistListing");

    QGraphicsLinearLayout *listLayout =
        new QGraphicsLinearLayout(Qt::Vertical, container);
    listLayout->setSpacing(0);
    listLayout->setContentsMargins(0, 0, 0, 0);

    viewport->setWidget(container);
    layout->addItem(viewport);

    list = new MList(this);

    if (AppWindow::instance()->orientation() == M::Landscape)
        list->setColumns(2);
    else if (AppWindow::instance()->orientation() == M::Portrait)
        list->setColumns(1);

    itemCreator = new AlbumItemCreator();
    list->setCellCreator(itemCreator);
    model = new AlbumsListModel(nmmPerformer);
    list->setItemModel(model);

    MAction *action = new MAction("Play", list);
    action->setLocation(MAction::ObjectMenuLocation);
    list->addAction(action);
    connect(action, SIGNAL(triggered()), this,
            SLOT(playAlbumFromObjectMenu()));

    action = new MAction("Open", list);
    action->setLocation(MAction::ObjectMenuLocation);
    list->addAction(action);
    connect(action, SIGNAL(triggered()), this,
            SLOT(showAlbumFromObjectMenu()));

    action = new MAction("Add to play queue", list);
    action->setLocation(MAction::ObjectMenuLocation);
    list->addAction(action);
    connect(action, SIGNAL(triggered()), this,
            SLOT(addToQueueFromObjectMenu()));

    action = new MAction("Add to a playlist", list);
    action->setLocation(MAction::ObjectMenuLocation);
    list->addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(addToPlaylist()));

    /* support window rotation */
    connect(AppWindow::instance(), SIGNAL(orientationChanged(M::Orientation)),
            this, SLOT(onOrientationChanged(M::Orientation)));

    connect(list, SIGNAL(itemClicked(QModelIndex)), this,
            SLOT(listClicked(QModelIndex)));
    connect(list, SIGNAL(itemLongTapped(QModelIndex)), this,
            SLOT(itemLongTapped(QModelIndex)));
    connect(doubleTapTimer, SIGNAL(timeout()), this, SLOT(waitTimeout()));
    connect(list, SIGNAL(panningStopped()), this, SLOT(queueTasks()));
    connect(list, SIGNAL(panningStarted()), this, SLOT(dequeueTasks()));

    QTimer::singleShot(0, this, SLOT(queueTasks()));

    listLayout->addItem(list);
}

ArtistList::~ArtistList()
{
    qDebug() << "ArtistList::~ArtistList";
//    delete itemCreator;
    delete model;
}

void ArtistList::itemLongTapped(const QModelIndex & index)
{
    qDebug() << "ArtistList::itemLongTapped" << index.row();

    longTappedIndex = index;
}

void ArtistList::waitTimeout()
{
    qDebug() << "ArtistList::waitTimeout:single-click";
    doubleTapTimer->stop();
    if (clickWaitIndex.isValid())
        showAlbum(clickWaitIndex);
    clickWait = false;
}

void ArtistList::listClicked(const QModelIndex & index)
{
    if(clickWait) {
        qDebug() << "ArtistList::double-click";
        QStringList uris;
        QStringList mimeTypes;
        if (clickWaitIndex.isValid()) {
            getSongs(m_nmmPerformer,
                     clickWaitIndex.data(Qt::UserRole).toString(),
                     &uris, &mimeTypes);
            AppWindow::instance()->playSongs(uris, mimeTypes);
            qDebug() << "ArtistList::dbl click" << uris;
        }
        doubleTapTimer->stop();
        clickWait = false;
    } else {
        clickWaitIndex = index;
        doubleTapTimer->start(doubleTimeoutInterval);
        clickWait = true;
    }
}

void ArtistList::showAlbum(const QModelIndex & index)
{
    qDebug() << "ArtistList::showAlbum";

    AppWindow::instance()->showAlbum(m_nmmArtistName,
                                     m_nmmPerformer,
                                     index.data(Qt::UserRole).toString(),
                                     index.data(Qt::DisplayRole).
                                     toString());
}

void ArtistList::showAndPlayAlbum()
{
    qDebug() << "ArtistList::showAndPlayAlbum";
}

void ArtistList::addToPlaylist()
{
    qDebug() << "ArtistList::addToPlaylist";

    qDebug() << QString(SONGS_QUERY).arg(m_nmmPerformer).arg(longTappedIndex.data(Qt::UserRole).toString());

    QVector < QStringList >
    results =::tracker()->rawSparqlQuery(QString(SONGS_QUERY).
                                         arg(m_nmmPerformer).
                                         arg(longTappedIndex.
                                             data(Qt::UserRole).
                                             toString()));

    AppWindow::instance()->addToPlaylist(results);
}

void ArtistList::showAlbumFromObjectMenu()
{
    qDebug() << "ArtistList::showAlbumFromObjectMenu";

    if (longTappedIndex.isValid())
        AppWindow::instance()->showAlbum(m_nmmArtistName,
                                         m_nmmPerformer,
                                         longTappedIndex.
                                         data(Qt::UserRole).toString(),
                                         longTappedIndex.
                                         data(Qt::DisplayRole).toString());
}

void ArtistList::getSongs(QString artist, QString album, QStringList *uris, QStringList *mimeTypes)
{
    QVector < QStringList > *results;
    results = new QVector < QStringList > ();
    SearchEngine::getMusicAlbumSongs(artist, album, *results);
    for (QVector < QStringList >::iterator iter = results->begin();
         iter != results->end(); iter++) {
        QStringList data = *iter;
        *uris << data[0];
        *mimeTypes << data[1];
    }
}

void ArtistList::playAlbumFromObjectMenu()
{
    qDebug() << "ArtistList::playAlbumFromObjectMenu" << longTappedIndex.
        data(Qt::UserRole).toString() << longTappedIndex.
        data(Qt::DisplayRole).
        toString() << m_nmmArtistName << m_nmmPerformer;
    if (longTappedIndex.isValid()) {
        QStringList uris;
        QStringList mimeTypes;
        getSongs(m_nmmPerformer,
                 longTappedIndex.data(Qt::UserRole).toString(),
                 &uris, &mimeTypes);
        AppWindow::instance()->playSongs(uris, mimeTypes);
    }
}

void ArtistList::addToQueueFromObjectMenu()
{
    qDebug() << "ArtistList::addToQueueFromObjectMenu" << longTappedIndex.
        data(Qt::UserRole).toString() << longTappedIndex.
        data(Qt::DisplayRole).
        toString() << m_nmmArtistName << m_nmmPerformer;
    if (longTappedIndex.isValid()) {
        QStringList uris;
        QStringList mimeTypes;
        getSongs(m_nmmPerformer,
                 longTappedIndex.data(Qt::UserRole).toString(),
                 &uris, &mimeTypes);
        AppWindow::instance()->addToQueue(uris, mimeTypes);
    }
}

void ArtistList::queueTasks()
{
    qDebug() << "ArtistList::queueTasks()";

    QModelIndex firstVisibleIndex = list->firstVisibleItem();
    QModelIndex lastVisibleIndex = list->lastVisibleItem();
    int firstVisibleRow = firstVisibleIndex.row();
    int lastVisibleRow = lastVisibleIndex.row();

    for (int i = firstVisibleRow; i <= lastVisibleRow; i++) {
        const QModelIndex & index = model->index(i, 0);
        if (!index.isValid())
            continue;
        model->addTask(index);
    }

    model->startTaskQueue();
}

void ArtistList::dequeueTasks()
{
    qDebug() << "ArtistList::dequeueTasks()";

    model->stopTaskQueue();
}

void ArtistList::onOrientationChanged(M::Orientation orientation)
{
    qDebug() << "ArtistList::onOrientationChanged" << orientation;

    if (orientation == M::Landscape)
        list->setColumns(2);
    else if (orientation == M::Portrait)
        list->setColumns(1);
}
