/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <algorithm>

#include <QDebug>

#include "playlistsmodel.h"
#include "searchengine.h"

static int compare(const QString & a, const QString & b)
{
    int z = QString::compare(a, b, Qt::CaseInsensitive);
    if (z < 0)
        return 1;
    else
        return 0;
}

PlaylistsModel::PlaylistsModel()
{
    SearchEngine::getAllPlaylists(playlists);

    std::sort(playlists.begin(), playlists.end(), compare);

    qDebug() << "PlaylistsModel::PlaylistsModel" << playlists;
}

QVariant PlaylistsModel::data(const QModelIndex & index, int role) const
{
    if (role == Qt::DisplayRole)
        return QVariant::fromValue(playlists.value(index.row()));
    else
        return QVariant();
}

int PlaylistsModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return 1;
}

int PlaylistsModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    if (parent.isValid() == true)
        return 0;

    return playlists.size();
}

void PlaylistsModel::newPlaylist(const QString & playlistName)
{
    QVector < QStringList > items;
    SearchEngine::newPlaylist(playlistName, items);

    beginResetModel();

    playlists.clear();

    SearchEngine::getAllPlaylists(playlists);
    std::sort(playlists.begin(), playlists.end(), compare);

    endResetModel();
}

void PlaylistsModel::deletePlaylist(const QModelIndex & index)
{
    SearchEngine::deletePlaylist(playlists.value(index.row()));

    beginResetModel();

    playlists.clear();

    SearchEngine::getAllPlaylists(playlists);
    std::sort(playlists.begin(), playlists.end(), compare);

    endResetModel();
}
