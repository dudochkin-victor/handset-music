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
#include <MAction>

#include "nowplaying.h"
#include "appwindow.h"

NowPlaying::NowPlaying(const QStringList & songurls, CrumbBar *crumbbar, QGraphicsItem * parent):
                  ListWithUri(songurls, crumbbar, parent)
{
    qDebug() << "NowPlaying::NowPlaying()";

    MAction *action = new MAction("Play", list);
    action->setLocation(MAction::ObjectMenuLocation);
    list->addAction(action);
    connect(action, SIGNAL(triggered()), this,
            SLOT(playFromObjectMenu()));

    action = new MAction("Add to a playlist", list);
    action->setLocation(MAction::ObjectMenuLocation);
    list->addAction(action);
    connect(action, SIGNAL(triggered()), this,
            SLOT(addToPlaylist()));

    action = new MAction("Remove from play queue", list);
    action->setLocation(MAction::ObjectMenuLocation);
    list->addAction(action);
    connect(action, SIGNAL(triggered()), this,
            SLOT(unloadFromObjectMenu()));
    connect(list, SIGNAL(itemLongTapped(QModelIndex)), this,
            SLOT(itemLongTapped(QModelIndex)));

}

NowPlaying::~NowPlaying()
{

}

void NowPlaying::playFromObjectMenu()
{
    qDebug() << "NowPlaying::playFromObjectMenu()" << m_songurls.at(longTappedIndex.row()) << longTappedIndex.data(Qt::UserRole+1).toString();
    AppWindow::instance()->play(m_songurls.at(longTappedIndex.row()), longTappedIndex.data(Qt::UserRole+1).toString());
}

void NowPlaying::unloadFromObjectMenu()
{
    qDebug() << "NowPlaying::unloadFromObjectMenu()";

    AppWindow::instance()->removeFromQueue(longTappedIndex.row(), 1);
}

void NowPlaying::addToPlaylist()
{
    qDebug() << "NowPlaying::addToPlaylist()";
    QVector < QStringList > vector;

    vector.append(QStringList() << m_songurls.at(longTappedIndex.row()));
    AppWindow::instance()->addToPlaylist(vector);
}

void NowPlaying::itemLongTapped(const QModelIndex & index)
{
    qDebug() << "Playlist::itemLongTapped" << index.row();

    longTappedIndex = index;
}

