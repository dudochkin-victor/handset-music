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
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QtTracker/Tracker>

#include <MAction>
#include <MPannableViewport>
#include <MWidgetController>

#include "artistitem.h"
#include "allartist.h"
#include "appwindow.h"
#include "searchengine.h"

#define SONGS_FOR_ARTIST "SELECT nie:url(?uri) WHERE { ?uri a nmm:MusicPiece . ?uri nmm:performer '%1' } ORDER BY nmm:musicAlbum(?uri) nmm:trackNumber(?uri)"
#define URL_MTYPE_FOR_ARTIST "SELECT nie:url(?uri) nie:mimeType(?uri) WHERE { ?uri a nmm:MusicPiece . ?uri nmm:performer '%1' } ORDER BY nmm:musicAlbum(?uri) nmm:trackNumber(?uri)"

AllArtist::AllArtist(MStylableWidget * parent):MStylableWidget(parent),
clickWait(false)
{
    qDebug() << "AllArtist::AllArtist";
    doubleTapTimer = new QTimer(this);
    doubleTimeoutInterval = QApplication::doubleClickInterval();

    setObjectName("AllArtist");
    setViewType("background");
    setContentsMargins(0, 0, 0, 0);

    QGraphicsLinearLayout *layout =
        new QGraphicsLinearLayout(Qt::Vertical, this);
    layout->setSpacing(0);
//    layout->setContentsMargins(0, 0, 0, 0);

    MPannableViewport *viewport = new MPannableViewport(this);
    MWidgetController *container = new MWidgetController;
    container->setViewType("background");
    container->setObjectName("AllArtistContainer");
//    container->setContentsMargins(0, 0, 0, 0);
    viewport->setObjectName("AllArtistListing");

    QGraphicsLinearLayout *listLayout =
        new QGraphicsLinearLayout(Qt::Vertical, container);
    listLayout->setSpacing(0);
    listLayout->setContentsMargins(0, 0, 0, 0);

    viewport->setWidget(container);
    layout->addItem(viewport);

    list = new MList(this);
    itemCreator = new ArtistItemCreator();
    list->setCellCreator(itemCreator);
    model = new ArtistsListModel();
    list->setItemModel(model);

    list->setObjectName("AllArtistList");
    list->setShowGroups(true);

//    viewport->setWidget(list);

    MAction *action = new MAction("Play", list);
    action->setLocation(MAction::ObjectMenuLocation);
    list->addAction(action);
    connect(action, SIGNAL(triggered()), this,
            SLOT(playArtistFromObjectMenu()));

    action = new MAction("Open", list);
    action->setLocation(MAction::ObjectMenuLocation);
    list->addAction(action);
    connect(action, SIGNAL(triggered()), this,
            SLOT(showArtistFromObjectMenu()));

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
            SLOT(listClicked(QModelIndex)));
    connect(list, SIGNAL(itemLongTapped(QModelIndex)), this,
            SLOT(itemLongTapped(QModelIndex)));
    connect(doubleTapTimer, SIGNAL(timeout()), this, SLOT(waitTimeout()));

    listLayout->addItem(list);
}

AllArtist::~AllArtist()
{
    qDebug() << "AllArtist::~AllArtist";
//    delete itemCreator;
    delete model;
}

void AllArtist::clicked()
{
    qDebug() << "AllArtist::clicked()";
}

void AllArtist::itemLongTapped(const QModelIndex & index)
{
    qDebug() << "AllArtist::itemLongTapped" << index.row();

    longTappedIndex = index;
}

void AllArtist::waitTimeout()
{
    qDebug() << "AllArtist::waitTimeout:single-click";
    doubleTapTimer->stop();
    if (clickWaitIndex.isValid())
        showArtist(clickWaitIndex);
    clickWait = false;
}

void AllArtist::listClicked(const QModelIndex & index)
{
    if(clickWait) {
        qDebug() << "AllArtist::double-click";
        QStringList uris;
        QStringList mimeTypes;
        if (clickWaitIndex.isValid()) {
            getSongs(clickWaitIndex.data(Qt::UserRole).toString(),
                     &uris, &mimeTypes);
            AppWindow::instance()->playSongs(uris, mimeTypes);
            qDebug() << "AllArtist::dbl click" << uris;
        }
        doubleTapTimer->stop();
        clickWait = false;
    } else {
        clickWaitIndex = index;
        doubleTapTimer->start(doubleTimeoutInterval);
        clickWait = true;
    }
}

void AllArtist::getSongs(QString artist, QStringList *uris, QStringList *mimeTypes)
{
    QVector < QStringList > results;
    results =::tracker()->rawSparqlQuery(QString(URL_MTYPE_FOR_ARTIST).
                                         arg(artist));
    for (QVector < QStringList >::iterator iter = results.begin();
         iter != results.end(); iter++) {
        QStringList data = *iter;
        *uris << data[0];
        *mimeTypes << data[1];
    }
}

void AllArtist::showArtist(const QModelIndex & index)
{
    qDebug() << "AllArtist::showArtist";

    AppWindow *w = AppWindow::instance();
    w->showArtist(index.data(Qt::DisplayRole).toString(),
                  index.data(Qt::UserRole).toString());
}

void AllArtist::showArtistFromObjectMenu()
{
    qDebug() << "AllArtist::showArtistFromObjectMenu";

    AppWindow *w = AppWindow::instance();

    if (longTappedIndex.isValid())
        w->showArtist(longTappedIndex.data(Qt::DisplayRole).toString(),
                      longTappedIndex.data(Qt::UserRole).toString());
}

void AllArtist::playArtistFromObjectMenu()
{
    qDebug() << "AllArtist::playArtistFromObjectMenu" << longTappedIndex.
        data(Qt::UserRole).toString();

    if (longTappedIndex.isValid()) {
        QStringList uris;
        QStringList mimeTypes;
        getSongs(longTappedIndex.data(Qt::UserRole).toString(),
                 &uris, &mimeTypes);
        AppWindow::instance()->playSongs(uris, mimeTypes);
    }
}

void AllArtist::addToQueueFromObjectMenu()
{
    qDebug() << "AllArtist::addToQueueFromObjectMenu" << longTappedIndex.
        data(Qt::UserRole).toString();

    if (longTappedIndex.isValid()) {
        QStringList uris;
        QStringList mimeTypes;
        getSongs(longTappedIndex.data(Qt::UserRole).toString(),
                 &uris, &mimeTypes);
        AppWindow::instance()->addToQueue(uris, mimeTypes);
    }
}

void AllArtist::addToPlaylist()
{
    qDebug() << "AllArtist::addToPlaylist";

    QVector < QStringList >
        results =::tracker()->rawSparqlQuery(QString(SONGS_FOR_ARTIST).
                                             arg(longTappedIndex.
                                                 data(Qt::UserRole).
                                                 toString()));

    AppWindow::instance()->addToPlaylist(results);
}
