/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#include "searchengine.h"
#include <QtTracker/Tracker>
#include <QSettings>

static const QString unknown = "unknown";
static const QString SqlGetAllMusicArtists =
    "SELECT nmm:artistName(nmm:performer(?uri)) nmm:performer(?uri) "
    " WHERE { ?uri a nmm:MusicPiece } "
    " GROUP BY nmm:performer(?uri) ORDER BY nmm:artistName(nmm:performer(?uri))";

static const QString SqlGetMusicArtistAlbumWithArtistName =
    "SELECT nmm:albumTitle(nmm:musicAlbum(?uri)) nmm:musicAlbum(?uri) nie:url(nie:isStoredAs(?uri)) "
    " WHERE {?uri a nmm:MusicPiece . OPTIONAL { ?uri nmm:musicAlbum ?album  .} "
    "  { SELECT ?uri WHERE { ?uri a nmm:MusicPiece ; nmm:performer '%1' . } } } "
    " GROUP by nmm:musicAlbum(?uri) ORDER by nmm:musicAlbum(?uri)";
static const QString SqlGetMusicArtistAlbumWithoutArtistName =
    "SELECT nmm:albumTitle(nmm:musicAlbum(?uri)) nmm:musicAlbum(?uri) nie:url(nie:isStoredAs(?uri)) "
    " WHERE {?uri a nmm:MusicPiece . OPTIONAL { ?uri nmm:musicAlbum ?album  .} "
    "  { SELECT ?uri WHERE { ?uri a nmm:MusicPiece . OPTIONAL {?uri nmm:performer ?performer} FILTER(! BOUND(?performer))} } } "
    " GROUP by nmm:musicAlbum(?uri) ORDER by nmm:musicAlbum(?uri)";

static const QString SqlGetMusicAlbumSongWithArtistWithAlbum =
    "SELECT nie:url(?uri) nie:mimeType(?uri) nie:title(?uri) nfo:duration(?uri) "
    " WHERE {?uri a nmm:MusicPiece . ?uri nmm:performer '%1' .  ?uri nmm:musicAlbum '%2'} "
    " ORDER by nmm:trackNumber(?uri)";
static const QString SqlGetMusicAlbumSongWithArtistWithoutAlbum =
    "SELECT nie:url(?uri) nie:mimeType(?uri) nie:title(?uri) nfo:duration(?uri) "
    " WHERE {?uri a nmm:MusicPiece . OPTIONAL { ?uri nmm:musicAlbum ?album  .} FILTER(! BOUND(?album)) "
    "  { SELECT ?uri WHERE { ?uri a nmm:MusicPiece ; nmm:performer '%1' . } } } "
    " ORDER by nmm:trackNumber(?uri)";
static const QString SqlGetMusicAlbumSongWithoutArtistWithAlbum =
    "SELECT nie:url(?uri) nie:mimeType(?uri) nie:title(?uri) nfo:duration(?uri) "
    " WHERE {?uri a nmm:MusicPiece . ?uri nmm:musicAlbum '%1' .  "
    "  { SELECT ?uri WHERE { ?uri a nmm:MusicPiece . OPTIONAL {?uri nmm:performer ?performer} FILTER(! BOUND(?performer))} } } "
    " ORDER by nmm:trackNumber(?uri)";
static const QString SqlGetMusicAlbumSongWithoutArtistWithoutAlbum =
    "SELECT nie:url(?uri) nie:mimeType(?uri) nie:title(?uri) nfo:duration(?uri) "
    " WHERE {?uri a nmm:MusicPiece . OPTIONAL { ?uri nmm:musicAlbum ?album } FILTER(! BOUND(?album)) "
    "  { SELECT ?uri WHERE { ?uri a nmm:MusicPiece . OPTIONAL {?uri nmm:performer ?performer} FILTER(! BOUND(?performer))} } } "
    " ORDER by nmm:trackNumber(?uri)";

static const QString SqlGetSongTitle =
    "SELECT nie:title(?uri) WHERE {  ?uri nie:url '%1' }";
static const QString SqlGetSongTitleArtistAlbum =
    "SELECT nie:title(?song) nmm:artistName(nmm:performer(?song)) nmm:albumTitle(nmm:musicAlbum(?song)) nie:mimeType(?song) nfo:duration(?song)"
    " WHERE {?song nie:url '%1' }";
static const QString SqlGetAllSong =
    "SELECT nie:url(?uri) nie:mimeType(?uri) "
    " WHERE { ?uri a nmm:MusicPiece } ORDER BY nmm:musicAlbum(?uri) nmm:trackNumber(?uri)";

static const QString SqlGetAllSongForArtistWithArtist =
    "SELECT nie:url(?uri) nie:mimeType(?uri) "
    " WHERE { ?uri a nmm:MusicPiece . ?uri nmm:performer '%1' } "
    " ORDER BY nmm:musicAlbum(?uri) nmm:trackNumber(?uri)";
static const QString SqlGetAllSongForArtistWithoutArtist =
    "SELECT nie:url(?uri) nie:mimeType(?uri) "
    " WHERE { ?uri a nmm:MusicPiece . OPTIONAL {?uri nmm:performer ?performer} FILTER(! BOUND(?performer))} "
    " ORDER BY nmm:musicAlbum(?uri) nmm:trackNumber(?uri)";

static const QString PlayListSettings = "MeeGoMusicPlaylistSettings";

bool SearchEngine::getAllMusicArtists(QVector < QStringList > &result)
{
    qDebug() << "SearchEngine::getAllMusicArtists " <<
        SqlGetAllMusicArtists;
    result =::tracker()->rawSparqlQuery(SqlGetAllMusicArtists);
    return TRUE;
}

bool SearchEngine::getMusicArtistAlbums(const QString & artist,
                                        QVector < QStringList > &result)
{
    QString sparql;
    if (0 == artist.compare(unknown, Qt::CaseInsensitive)
        || artist.isEmpty()) {
        sparql = SqlGetMusicArtistAlbumWithoutArtistName;
    } else {
        sparql = QString(SqlGetMusicArtistAlbumWithArtistName).arg(artist);
    }

    qDebug() << "SearchEngine::getMusicArtistAlbums " << sparql;
    result =::tracker()->rawSparqlQuery(sparql);
    return TRUE;
}

bool SearchEngine::getMusicAlbumSongs(const QString & artist,
                                      const QString & album,
                                      QVector < QStringList > &result)
{
    QString sparql;
    if (0 == artist.compare(unknown, Qt::CaseInsensitive)
        || artist.isEmpty()) {
        if (0 == album.compare(unknown, Qt::CaseInsensitive)
            || album.isEmpty()) {
            sparql = SqlGetMusicAlbumSongWithoutArtistWithoutAlbum;
        } else {
            sparql =
                QString(SqlGetMusicAlbumSongWithoutArtistWithAlbum).
                arg(album);
        }
    } else {
        if (0 == album.compare(unknown, Qt::CaseInsensitive)
            || album.isEmpty()) {
            sparql =
                QString(SqlGetMusicAlbumSongWithArtistWithoutAlbum).
                arg(artist);
        } else {
            sparql =
                QString(SqlGetMusicAlbumSongWithArtistWithAlbum).
                arg(artist).arg(album);
        }
    }

    qDebug() << "SearchEngine::getMusicAlbumSongs " << sparql;
    result =::tracker()->rawSparqlQuery(sparql);
    return TRUE;
}

bool SearchEngine::getSongTitle(const QString & url,
                                QVector < QStringList > &result)
{
    QString sparql;
    sparql = QString(SqlGetSongTitle).arg(url);

    qDebug() << "SearchEngine::getSongTitle " << sparql;
    result =::tracker()->rawSparqlQuery(sparql);
    return TRUE;
}

bool SearchEngine::getSongTitleArtistAlbum(const QString & url,
                                           QVector < QStringList > &result)
{
    QString sparql;
    sparql = QString(SqlGetSongTitleArtistAlbum).arg(url);

    qDebug() << "SearchEngine::getSongTitleArtistAlbum " << sparql;
    result =::tracker()->rawSparqlQuery(sparql);
    return TRUE;
}

bool SearchEngine::getAllSongs(QVector < QStringList > &result)
{
    qDebug() << "SearchEngine::getAllSongs " << SqlGetAllSong;
    result =::tracker()->rawSparqlQuery(SqlGetAllSong);
    return TRUE;
}

bool SearchEngine::getAllSongsForArtist(const QString & artist,
                                        QVector < QStringList > &result)
{
    QString sparql;
    if (0 == artist.compare(unknown, Qt::CaseInsensitive)) {
        sparql = QString(SqlGetAllSongForArtistWithoutArtist);
    } else {
        sparql = QString(SqlGetAllSongForArtistWithArtist).arg(artist);
    }

    qDebug() << "SearchEngine::getAllSongsForArtist " << sparql;
    result =::tracker()->rawSparqlQuery(sparql);
    return TRUE;
}



void SearchEngine::getAllPlaylists(QStringList & result)
{
    QSettings settings;

    settings.beginGroup(PlayListSettings);
    result = settings.childGroups();
    settings.endGroup();
}

void SearchEngine::getPlaylistItems(const QString & name,
                                    QVector < QStringList > &items)
{
    QSettings settings;

    settings.beginGroup(PlayListSettings);

#ifdef USE_ARRAY
    int numItems = settings.beginReadArray(name);
    for (int j = 0; j < numItems; j++) {
        QStringList itemInfo;
        settings.setArrayIndex(j);
        int itemColumns = settings.beginReadArray(QString(j));

        for (int k = 0; k < itemColumns; k++) {
            settings.setArrayIndex(k);
            itemInfo << settings.value(QString(k)).toString();
        }

        settings.endArray();
        items << itemInfo;
    }
    settings.endArray();
#else
    settings.beginGroup(name);

    QStringList ItemInfos = settings.childGroups();
    int itemIndex = 0;

    for (QStringList::iterator i = ItemInfos.begin(); i != ItemInfos.end();
         i++) {
        QStringList itemInfo;

        settings.beginGroup(QString(itemIndex++));
        QStringList itemColumns = settings.childKeys();
        int itemColumn = 0;
        for (QStringList::iterator j = itemColumns.begin();
             j != itemColumns.end(); j++) {
            itemInfo << settings.value(QString(itemColumn++)).toString();
        }
        settings.endGroup();

        items << itemInfo;
    }

    settings.endGroup();
#endif
    settings.endGroup();
}

void SearchEngine::deletePlaylist(const QString & name)
{
    QSettings settings;

    settings.beginGroup(PlayListSettings);
    settings.remove(name);
    settings.endGroup();
    settings.sync();
}

void SearchEngine::deletePlaylistItem(const QString & name,
                                      const QString & itemName)
{
    QVector < QStringList > items;
    QVector < QStringList > new_items;

    SearchEngine::getPlaylistItems(name, items);
    for (QVector < QStringList >::iterator i = items.begin();
         i != items.end(); i++) {
        if (i->value(0) != itemName) {
            new_items << *i;
        }
    }
    SearchEngine::deletePlaylist(name);
    SearchEngine::newPlaylist(name, new_items);
}

void SearchEngine::newPlaylist(const QString & name,
                               const QVector < QStringList > &items)
{
    QSettings settings;

    settings.beginGroup(PlayListSettings);

#ifdef USE_ARRAY
    settings.beginWriteArray(name);
    int itemIndex = 0;
    for (QVector < QStringList >::const_iterator i = items.begin();
         i != items.end(); i++) {
        settings.setArrayIndex(itemIndex);
        settings.beginWriteArray(QString(itemIndex));
        itemIndex++;

        int itemColumn = 0;
        for (QStringList::const_iterator j = i->begin(); j != i->end();
             j++) {
            settings.setArrayIndex(itemColumn);
            settings.setValue(QString(itemColumn + 2), *j);
            itemColumn++;
        }
        settings.endArray();
    }
    settings.endArray();
#else
    int itemIndex = 0;
    settings.beginGroup(name);

    if (items.size() == 0) {
        settings.setValue(QString("PlaceHolder"), QString("PlaceHolder"));
    }

    for (QVector < QStringList >::const_iterator i = items.begin();
         i != items.end(); i++) {
        settings.beginGroup(QString(itemIndex++));

        int itemColumn = 0;
        for (QStringList::const_iterator j = i->begin(); j != i->end();
             j++) {
            settings.setValue(QString(itemColumn++), *j);
        }

        settings.endGroup();
    }
    settings.endGroup();
#endif
    settings.endGroup();
    settings.sync();
}

void SearchEngine::addPlaylistItem(const QString & name,
                                   const QStringList & item)
{
    QVector < QStringList > items;
    QVector < QStringList > new_items;
    bool replaced = false;

    SearchEngine::getPlaylistItems(name, items);
    for (QVector < QStringList >::iterator i = items.begin();
         i != items.end(); i++) {
        if (i->value(0) == item[0]) {
            new_items << item;
            replaced = true;
        } else {
            new_items << *i;
        }
    }

    if (replaced == false) {
        new_items << item;
    }
    SearchEngine::deletePlaylist(name);
    SearchEngine::newPlaylist(name, new_items);
}

void SearchEngine::addPlaylistItems(const QString & name,
                                    const QVector < QStringList >
                                    &itemsToAdd)
{
    QVector < QStringList > items;
    //QVector<QStringList> new_items;

    SearchEngine::getPlaylistItems(name, items);
    //new_items = items;

    for (QVector < QStringList >::const_iterator item = itemsToAdd.begin();
         item != itemsToAdd.end(); item++) {
        int j = 0;
        bool replaced = false;

        for (QVector < QStringList >::iterator i = items.begin();
             i != items.end(); i++, j++) {
            if (i->value(0) == item->value(0)) {
                items.replace(j, *item);
                replaced = true;
            }
        }

        if (replaced == false) {
            items.append(*item);
        }
    }
    SearchEngine::deletePlaylist(name);
    SearchEngine::newPlaylist(name, items);
}
