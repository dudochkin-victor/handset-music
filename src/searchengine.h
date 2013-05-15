/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H
#include <QtCore>

class SearchEngine {
public:
    static bool getAllMusicArtists(QVector < QStringList > &result);
    static bool getMusicArtistAlbums(const QString & artist,
                                     QVector < QStringList > &result);
    static bool getMusicAlbumSongs(const QString & artist,
                                   const QString & album,
                                   QVector < QStringList > &result);
    static bool getSongTitle(const QString & url,
                             QVector < QStringList > &result);
    static bool getSongTitleArtistAlbum(const QString & url,
                                        QVector < QStringList > &result);
    static bool getAllSongs(QVector < QStringList > &result);
    static bool getAllSongsForArtist(const QString & artist,
                                     QVector < QStringList > &result);

    static void getAllPlaylists(QStringList & result);
    static void getPlaylistItems(const QString & name,
                                 QVector < QStringList > &items);

    static void deletePlaylist(const QString & name);
    static void deletePlaylistItem(const QString & name,
                                   const QString & itemName);

    static void newPlaylist(const QString & name,
                            const QVector < QStringList > &items);
    static void addPlaylistItem(const QString & name,
                                const QStringList & item);
    static void addPlaylistItems(const QString & name,
                                 const QVector < QStringList >
                                 &itemsToAdd);
};

#endif
