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

#include "playlist.h"
#include "appwindow.h"

Playlist::Playlist(const QStringList & songurls, CrumbBar *crumbbar, QGraphicsItem * parent):
                  ListWithUri(songurls, crumbbar, parent)
{
    qDebug() << "Playlist::Playlist";

    MAction *action = new MAction("Play", list);
    action->setLocation(MAction::ObjectMenuLocation);
    list->addAction(action);
    connect(action, SIGNAL(triggered()), this,
            SLOT(playFromObjectMenu()));

    action = new MAction("Add to play queue", list);
    action->setLocation(MAction::ObjectMenuLocation);
    list->addAction(action);
    connect(action, SIGNAL(triggered()), this,
            SLOT(loadFromObjectMenu()));
    connect(list, SIGNAL(itemLongTapped(QModelIndex)), this,
            SLOT(itemLongTapped(QModelIndex)));
}

Playlist::~Playlist()
{

}

void Playlist::playFromObjectMenu()
{
    qDebug() << "Playlist::playFromObjectMenu()" << m_songurls.at(longTappedIndex.row()) << longTappedIndex.data(Qt::UserRole+1).toString();
    AppWindow::instance()->play(m_songurls.at(longTappedIndex.row()), longTappedIndex.data(Qt::UserRole+1).toString());
}

void Playlist::loadFromObjectMenu()
{
    qDebug() << "Playlist::loadFromObjectMenu()" << m_songurls.at(longTappedIndex.row()) << longTappedIndex.data(Qt::UserRole+1).toString();
    AppWindow::instance()->addToQueue(QStringList() << m_songurls.at(longTappedIndex.row()), QStringList() << longTappedIndex.data(Qt::UserRole+1).toString());
}

void Playlist::itemLongTapped(const QModelIndex & index)
{
    qDebug() << "Playlist::itemLongTapped" << index.row();

    longTappedIndex = index;
}
