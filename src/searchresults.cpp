/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QGraphicsLinearLayout>
#include <QtTracker/Tracker>

#include <MAction>
#include <MButton>
#include <MImageWidget>
#include <MLabel>
#include <MPannableViewport>
#include <MStylableWidget>

#include "appwindow.h"
#include "searchresults.h"


#define SONGS_QUERY "SELECT nie:url(?uri) nie:mimeType(?uri) nie:title(?uri) WHERE { ?uri a nmm:MusicPiece . FILTER regex(nie:title(?uri), '%1', 'i') } ORDER BY nmm:trackNumber"
#define ALBUMS_QUERY "SELECT nmm:artistName(nmm:performer(?uri)) nmm:performer(?uri) nmm:musicAlbum(?uri) nmm:albumTitle(nmm:musicAlbum(?uri)) WHERE { ?uri a nmm:MusicPiece . FILTER regex(nmm:albumTitle(nmm:musicAlbum(?uri)), '%1', 'i') } GROUP BY nmm:musicAlbum(?uri) ORDER BY nmm:performer(?uri) nmm:musicAlbum(?uri)"
#define ARTIST_QUERY "SELECT nmm:artistName(nmm:performer(?uri)) nmm:performer(?uri) " \
    "WHERE { ?uri a nmm:MusicPiece . FILTER regex(nmm:artistName(nmm:performer(?uri)), '%1', 'i') } GROUP BY nmm:performer(?uri) ORDER BY nmm:artistName(nmm:performer(?uri))"

#define ARTIST_SONGS_QUERY "SELECT nie:url(?uri) WHERE { ?uri a nmm:MusicPiece . ?uri nmm:performer '%1' } ORDER BY nmm:musicAlbum(?uri) nmm:trackNumber"
#define ALBUM_SONGS_QUERY "SELECT nie:url(?uri) WHERE { ?uri a nmm:MusicPiece . ?uri nmm:performer '%1' . ?uri nmm:musicAlbum '%2' } ORDER BY nmm:trackNumber"

SearchItem::SearchItem(QGraphicsItem * parent):
MButton(parent)
{
    setObjectName("SearchItem");
    setContentsMargins(0, 0, 0, 0);

    QGraphicsLinearLayout *layout =
        new QGraphicsLinearLayout(Qt::Horizontal, this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    icon = new MImageWidget();
    icon->setObjectName("SearchItemIcon");
    layout->addItem(icon);

    label = new MLabel();
    label->setObjectName("SearchItemLabel");
    layout->addItem(label);
    layout->addStretch();
}

AllSearchListModel::AllSearchListModel(QString & query)
{
    QVector < QStringList > tmp;

    results =::tracker()->rawSparqlQuery(QString(ARTIST_QUERY).arg(query));
    results += ::tracker()->rawSparqlQuery(QString(ALBUMS_QUERY).arg(query));
    results += ::tracker()->rawSparqlQuery(QString(SONGS_QUERY).arg(query));
}

AllSearchListModel::~AllSearchListModel()
{
    results.clear();
}

int AllSearchListModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return results.size();
}

int AllSearchListModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return 1;
}

QVariant AllSearchListModel::data(const QModelIndex & index, int role) const
{
    Q_ASSERT(index.isValid());

    switch (results.value(index.row()).size()) {
    case 2: // Artist
        if (role == Qt::DisplayRole) {
            return QVariant::fromValue(results.value(index.row())[0]);
        } else if (role == Qt::UserRole) {
            return QVariant::fromValue(QString("Artist"));
        } else if (role == Qt::UserRole + 1) { //nmm:performer
            return QVariant::fromValue(results.value(index.row())[1]);
        }

        return QVariant();
    case 4: // Album
        if (role == Qt::DisplayRole) {
            return QVariant::fromValue(results.value(index.row())[3]);
        } else if (role == Qt::UserRole) {
            return QVariant::fromValue(QString("Album"));
        } else if (role == Qt::UserRole + 1) { //nmm:performer
             return QVariant::fromValue(results.value(index.row())[1]);
        } else if (role == Qt::UserRole + 2) { //nmm:musicAlbum
             return QVariant::fromValue(results.value(index.row())[2]);
        }

        return QVariant();
    case 3: //Song
        if (role == Qt::DisplayRole) {
            return QVariant::fromValue(results.value(index.row())[2]);
        } else if (role == Qt::UserRole) {
            return QVariant::fromValue(QString("Song"));
        } else if (role == Qt::UserRole + 1) { //nie:url
            return QVariant::fromValue(results.value(index.row())[0]);
        }

        return QVariant();
    default:
        return QVariant();
    }
}

ArtistSearchListModel::ArtistSearchListModel(QString & query)
{
    results =::tracker()->rawSparqlQuery(QString(ARTIST_QUERY).arg(query));
}

ArtistSearchListModel::~ArtistSearchListModel()
{
    results.clear();
}

int ArtistSearchListModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return results.size();
}

int ArtistSearchListModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return 1;
}

QVariant ArtistSearchListModel::data(const QModelIndex & index, int role) const
{
    Q_ASSERT(index.isValid());

    if (role == Qt::DisplayRole) {
        return QVariant::fromValue(results.value(index.row())[0]);
    } else if (role == Qt::UserRole) {
        return QVariant::fromValue(QString("Artist"));
    } else if (role == Qt::UserRole + 1) { //nmm:performer
        return QVariant::fromValue(results.value(index.row())[1]);
    }

    return QVariant();
}

AlbumSearchListModel::AlbumSearchListModel(QString & query)
{
    results =::tracker()->rawSparqlQuery(QString(ALBUMS_QUERY).arg(query));
}

AlbumSearchListModel::~AlbumSearchListModel()
{
    results.clear();
}

int AlbumSearchListModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return results.size();
}

int AlbumSearchListModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return 1;
}

QVariant AlbumSearchListModel::data(const QModelIndex & index, int role) const
{
    Q_ASSERT(index.isValid());

    if (role == Qt::DisplayRole) {
        return QVariant::fromValue(results.value(index.row())[3]);
    } else if (role == Qt::UserRole) {
        return QVariant::fromValue(QString("Album"));
    } else if (role == Qt::UserRole + 1) { //nmm:performer
        return QVariant::fromValue(results.value(index.row())[1]);
    } else if (role == Qt::UserRole + 2) { //nmm:musicAlbum
        return QVariant::fromValue(results.value(index.row())[2]);
    }

    return QVariant();
}

SongSearchListModel::SongSearchListModel(QString & query)
{
    results =::tracker()->rawSparqlQuery(QString(SONGS_QUERY).arg(query));
}

SongSearchListModel::~SongSearchListModel()
{
    results.clear();
}

int SongSearchListModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return results.size();
}

int SongSearchListModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return 1;
}

QVariant SongSearchListModel::data(const QModelIndex & index, int role) const
{
    Q_ASSERT(index.isValid());

    if (role == Qt::DisplayRole) {
        return QVariant::fromValue(results.value(index.row())[2]);
    } else if (role == Qt::UserRole) {
        return QVariant::fromValue(QString("Song"));
    } else if (role == Qt::UserRole + 1) { //nie:url
        return QVariant::fromValue(results.value(index.row())[0]);
    }

    return QVariant();
}

void SearchItemCreator::updateCell(const QModelIndex & index, MWidget * cell) const
{
    SearchItem *contentItem = qobject_cast < SearchItem * >(cell);

    contentItem->setDisplayName(index.data(Qt::DisplayRole).toString());
    contentItem->setIcon(index.data(Qt::UserRole).toString());
}

SearchResults::SearchResults(QGraphicsItem * parent):MStylableWidget(parent)
{
    qDebug() << "SearchResults::SearchResults";

    setObjectName("SearchResults");
    setContentsMargins(0, 0, 0, 0);

    QGraphicsLinearLayout *layout =
        new QGraphicsLinearLayout(Qt::Vertical, this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    MPannableViewport *viewport = new MPannableViewport(this);
    MWidgetController *container = new MWidgetController;
    container->setViewType("background");
    container->setObjectName("SearchResultsContainer");
    container->setContentsMargins(0, 0, 0, 0);
    viewport->setObjectName("SearchResultsListing");

    listLayout = new QGraphicsLinearLayout(Qt::Vertical, container);
    listLayout->setSpacing(0);
    listLayout->setContentsMargins(0, 0, 0, 0);

    viewport->setWidget(container);
    layout->addItem(viewport);
}

SearchResults::~SearchResults()
{
    qDebug() << "SearchResults::~SearchResults";
}

void SearchResults::showAll(QString query)
{
    qDebug() << "SearchResults::showAll " << query;
    clearList();

    allList = new MList();
    SearchItemCreator *itemCreator = new SearchItemCreator();
    allList->setCellCreator(itemCreator);
    AllSearchListModel *model = new AllSearchListModel(query);
    allList->setItemModel(model);

    listLayout->addItem(allList);

    addObjectMenuActions(allList);
}

void SearchResults::showArtist(QString query)
{
    qDebug() << "SearchResults::showArtist " << query;
    clearList();
    searchArtist(query);
}

void SearchResults::showAlbums(QString query)
{
    qDebug() << "SearchResults::showAlbums " << query;
    clearList();
    searchAlbums(query);
}

void SearchResults::showSongs(QString query)
{
    qDebug() << "SearchResults::showSongs " << query;
    clearList();
    searchSongs(query);
}

void SearchResults::getResults(QVector <QStringList> &results)
{
    QString type;

    qDebug() << "SearchResults::getResults";
    type = longTappedIndex.data(Qt::UserRole).toString();
    if (type == "Artist") {
        results = ::tracker()->rawSparqlQuery(QString(ARTIST_SONGS_QUERY).
                                              arg(longTappedIndex.data(Qt::UserRole + 1).toString()));
    } else if (type == "Album") {
        results = ::tracker()->rawSparqlQuery(QString(ALBUM_SONGS_QUERY).
                                              arg(longTappedIndex.data(Qt::UserRole + 1).toString()).
                                              arg(longTappedIndex.data(Qt::UserRole + 2).toString()));
    } else if (type == "Song") {
        results.append(QStringList() << longTappedIndex.data(Qt::UserRole + 1).toString());
    }
    qDebug() << "SearchResults::getResults" << results;
}

void SearchResults::clearList()
{
    for (int i = listLayout->count(); i > 0; i--) {
        QGraphicsWidget *item =
            static_cast < QGraphicsWidget * >(listLayout->itemAt(0));
        listLayout->removeItem(item);
        item->deleteLater();
    }
}

void SearchResults::searchArtist(QString query)
{
    artistList = new MList();
    SearchItemCreator *itemCreator = new SearchItemCreator();
    artistList->setCellCreator(itemCreator);
    ArtistSearchListModel *model = new ArtistSearchListModel(query);
    artistList->setItemModel(model);

    listLayout->addItem(artistList);

    addObjectMenuActions(artistList);
}

void SearchResults::searchAlbums(QString query)
{
    albumList = new MList();
    SearchItemCreator *itemCreator = new SearchItemCreator();
    albumList->setCellCreator(itemCreator);
    AlbumSearchListModel *model = new AlbumSearchListModel(query);
    albumList->setItemModel(model);

    listLayout->addItem(albumList);

    addObjectMenuActions(albumList);
}

void SearchResults::searchSongs(QString query)
{
    songList = new MList();
    SearchItemCreator *itemCreator = new SearchItemCreator();
    songList->setCellCreator(itemCreator);
    SongSearchListModel *model = new SongSearchListModel(query);
    songList->setItemModel(model);

    listLayout->addItem(songList);

    addObjectMenuActions(songList);
}

void SearchResults::addObjectMenuActions(MList *list)
{
    MAction *action = new MAction("Play", list);
    action->setLocation(MAction::ObjectMenuLocation);
    list->addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(playResult()));

    action = new MAction("Add to play queue", list);
    action->setLocation(MAction::ObjectMenuLocation);
    list->addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(addToPlayqueue()));

    action = new MAction("Add to a playlist", list);
    action->setLocation(MAction::ObjectMenuLocation);
    list->addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(addToPlaylist()));

    connect(list, SIGNAL(itemLongTapped(QModelIndex)), this,
            SLOT(itemLongTapped(QModelIndex)));
}

void SearchResults::playResult()
{
    QVector < QStringList > results;
    qDebug() << "SearchResults::playResult";

    if (longTappedIndex.isValid() == false)
        return;
    getResults(results);
    qDebug() << "SearchResults::playResult" << results;
    AppWindow::instance()->playSearchResults(results);
}

void SearchResults::addToPlayqueue()
{
    QVector < QStringList > results;
    qDebug() << "SearchResults::addToPlayqueue";

    if (longTappedIndex.isValid() == false)
        return;
    getResults(results);
    qDebug() << "SearchResults::addToPlayqueue" << results;
    AppWindow::instance()->loadSearchResults(results);
}

void SearchResults::addToPlaylist()
{
    qDebug() << "SearchResults::addToPlaylist";
    QString type;
    QVector < QStringList > results;

    if (longTappedIndex.isValid() == false)
        return;

    type = longTappedIndex.data(Qt::UserRole).toString();
    if (type == "Artist") {
        results = ::tracker()->rawSparqlQuery(QString(ARTIST_SONGS_QUERY).
                                              arg(longTappedIndex.data(Qt::UserRole + 1).toString()));
    } else if (type == "Album") {
        results = ::tracker()->rawSparqlQuery(QString(ALBUM_SONGS_QUERY).
                                              arg(longTappedIndex.data(Qt::UserRole + 1).toString()).
                                              arg(longTappedIndex.data(Qt::UserRole + 2).toString()));
    } else if (type == "Song") {
        results.append(QStringList() << longTappedIndex.data(Qt::UserRole + 1).toString());
    }

    qDebug() << results;
    AppWindow::instance()->addToPlaylist(results);
}

void SearchResults::itemLongTapped(const QModelIndex & index)
{
    qDebug() << "SearchResults::itemLongTapped" << index.data(Qt::UserRole).toString();

    longTappedIndex = index;
}
