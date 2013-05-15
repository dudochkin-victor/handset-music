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
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QSettings>

#include <MAction>
#include <MButton>
#include <MPannableViewport>
#include <MWidgetController>
#include <MList>
#include <MTextEdit>
#include <MDialog>

#include "playlists.h"
#include "appwindow.h"
#include "playlistsmodel.h"
#include "artistitemcreator.h"
#include "searchengine.h"

Playlists::Playlists(MStylableWidget * parent):MStylableWidget(parent)
{
    qDebug() << "Playlists::Playlists";

    settings.beginGroup(PLAYLIST_SETTINGS_GROUP);

    setObjectName("Playlists");
    setViewType("background");
    setContentsMargins(0, 0, 0, 0);

    QGraphicsLinearLayout *layout =
        new QGraphicsLinearLayout(Qt::Vertical, this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    MPannableViewport *viewport = new MPannableViewport(this);
    MWidgetController *container = new MWidgetController;
    container->setViewType("background");
    container->setObjectName("PlaylistContainer");
    container->setContentsMargins(0, 0, 0, 0);
    viewport->setObjectName("PlaylistListing");

    listLayout = new QGraphicsLinearLayout(Qt::Vertical, container);
    listLayout->setSpacing(0);
    listLayout->setContentsMargins(0, 0, 0, 0);

    viewport->setWidget(container);
    layout->addItem(viewport);

    list = new MList(this);
    ArtistItemCreator *itemCreator = new ArtistItemCreator();
    list->setCellCreator(itemCreator);
    model = new PlaylistsModel();
    list->setItemModel(model);

    MAction *action = new MAction("Play", list);
    action->setLocation(MAction::ObjectMenuLocation);
    list->addAction(action);
    connect(action, SIGNAL(triggered()), this,
            SLOT(playPlaylistFromObjectMenu()));

    action = new MAction("Add to play queue", list);
    action->setLocation(MAction::ObjectMenuLocation);
    list->addAction(action);
    connect(action, SIGNAL(triggered()), this,
            SLOT(loadPlaylistFromObjectMenu()));

    action = new MAction("New playlist", list);
    action->setLocation(MAction::ObjectMenuLocation);
    list->addAction(action);
    connect(action, SIGNAL(triggered()), this,
            SLOT(newPlaylistFromObjectMenu()));

    action = new MAction("Delete playlist", list);
    action->setLocation(MAction::ObjectMenuLocation);
    list->addAction(action);
    connect(action, SIGNAL(triggered()), this,
            SLOT(deletePlaylistFromObjectMenu()));


    connect(list, SIGNAL(itemClicked(QModelIndex)), this,
            SLOT(openPlaylist(QModelIndex)));
    connect(list, SIGNAL(itemLongTapped(QModelIndex)), this,
            SLOT(itemLongTapped(QModelIndex)));

    listLayout->addItem(list);

/*
    QStringList keys = settings.allKeys();
    for (QStringList::const_iterator iter = keys.begin(); iter != keys.end(); iter++)
    {
	MButton *b = new MButton(*iter);
	b->setObjectName("PlaylistItem");
	listLayout->addItem(b);

	MAction* openAction = new MAction (tr("Open"), b);
	openAction->setLocation(MAction::ObjectMenuLocation);
	connect(openAction, SIGNAL(triggered()), this, SLOT(openPlaylist()));
	b->addAction(openAction);

	MAction* shuffleAction = new MAction (tr("Shuffle"), b);
	shuffleAction->setLocation(MAction::ObjectMenuLocation);
	connect(shuffleAction, SIGNAL(triggered()), this, SLOT(shufflePlaylist()));
	b->addAction(shuffleAction);

	MAction* playAction = new MAction (QString(tr("Play %1")).arg(*iter), b);
	playAction->setLocation(MAction::ObjectMenuLocation);
	connect(playAction, SIGNAL(triggered()), this, SLOT(playPlaylist()));
	b->addAction(playAction);

	MAction* deleteAction = new MAction (QString(tr("Delete %1")).arg(*iter), b);
	deleteAction->setLocation(MAction::ObjectMenuLocation);
	connect(deleteAction, SIGNAL(triggered()), this, SLOT(deletePlaylist()));
	b->addAction(deleteAction);

    }
*/
}

Playlists::~Playlists()
{
    qDebug() << "Playlist::~Playlist";
    settings.endGroup();
}

void Playlists::openPlaylist(const QModelIndex & index)
{
    qDebug() << "Playlist::openPlaylist" << index.data(Qt::DisplayRole).toString();

    QString playlistName = index.data(Qt::DisplayRole).toString();
    QVector < QStringList > items;
    SearchEngine::getPlaylistItems(index.data(Qt::DisplayRole).toString(), items);

    for (QVector < QStringList >::iterator iter = items.begin();
         iter != items.end(); iter++) {
         QStringList data = *iter;
         uris << data[0];
    }

//    qDebug() << items << uris;
    AppWindow::instance()->showPlaylist(playlistName, uris);
}

void Playlists::playPlaylistFromObjectMenu()
{
    qDebug() << "Playlists::playPlaylistFromObjectMenu" << longTappedIndex.data(Qt::DisplayRole);
    AppWindow::instance()->playPlaylist(longTappedIndex.data(Qt::DisplayRole).toString());
}

void Playlists::loadPlaylistFromObjectMenu()
{
    qDebug() << "Playlists::loadPlaylistFromObjectMenu" << longTappedIndex.data(Qt::DisplayRole);
    AppWindow::instance()->loadPlaylist(longTappedIndex.data(Qt::DisplayRole).toString());
}

void Playlists::deletePlaylist()
{
    MButton *b = static_cast < MButton * >(sender()->parent());

    qDebug() << "Playlist::deletePlaylist " << b->text();

    settings.remove(b->text());
    settings.sync();

    listLayout->removeItem(b);
    b->deleteLater();
}

void Playlists::newPlaylistFromObjectMenu()
{
    qDebug() << "Playlist::newPlaylistFromObjectMenu";

    showDialog();
}

void Playlists::deletePlaylistFromObjectMenu()
{
    qDebug() << "Playlist::deletePlaylistFromObjectMenu";

    if (longTappedIndex.isValid())
        model->deletePlaylist(longTappedIndex);
}

void Playlists::showDialog()
{
    MWidget *centralWidget = new MWidget;
    QGraphicsLinearLayout *layout =
        new QGraphicsLinearLayout(Qt::Vertical);

    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    MLabel *label = new MLabel("Playlist Name", centralWidget);
    playlistEdit = new MTextEdit(MTextEditModel::SingleLine,
                                 QString(), centralWidget);
    playlistEdit->setObjectName("playlistEdit");
    centralWidget->setLayout(layout);
    layout->addItem(label);
    layout->addItem(playlistEdit);

    MDialog *dialog =
        new MDialog("Please enter your name",
                    M::OkButton | M::CancelButton);
    dialog->setObjectName("Dialog");
    dialog->setCentralWidget(centralWidget);

    connect(dialog, SIGNAL(accepted()), this, SLOT(newPlaylist()));
//    connect(dialog, SIGNAL(rejected()), this, SLOT(test()));
//
    dialog->appear(MSceneWindow::DestroyWhenDone);
}

void Playlists::newPlaylist()
{
    qDebug() << "Playlist::newPlaylist" << playlistEdit->text();
    QVector < QStringList > items;
    model->newPlaylist(playlistEdit->text());
}

void Playlists::itemLongTapped(const QModelIndex & index)
{
    qDebug() << "Playlist::itemLongTapped" << index.row();

    longTappedIndex = index;
}
