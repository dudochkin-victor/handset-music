/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef SONGITEM_H
#define SONGITEM_H

#include <QDebug>
#include <MButton>

#include "appwindow.h"

class SongItem:public MButton {
    Q_OBJECT;

public:
    SongItem(const QString url, const QString mimeType, const QString title, const QString nmmArtistName = QString(), const QString nmmPerformer = QString(), const QString nmmMusicAlbum = QString(), const QString nmmAlbumTitle = QString(), QGraphicsItem * parent = 0):
        MButton(parent),
        m_url(url),
        m_mimeType(mimeType),
        m_title(title),
        m_nmmArtistName(nmmArtistName),
        m_nmmPerformer(nmmPerformer),
        m_nmmMusicAlbum(nmmMusicAlbum), m_nmmAlbumTitle(nmmAlbumTitle) {
        setText(m_title);
        setObjectName("SongItem");
        connect(this, SIGNAL(clicked()), this, SLOT(play()));
    };
    ~SongItem() {
        qDebug() << "SongItem::~SongItem";
    };

    QString url() const {
        return m_url;
    };
    QString mimeType() const {
        return m_mimeType;
    };
    QString title() const {
        return m_title;
    };
    QString nmmArtistName() const {
        return m_nmmArtistName;
    };
    QString nmmPerformer() const {
        return m_nmmPerformer;
    };
    QString nmmMusicAlbum() const {
        return m_nmmMusicAlbum;
    };
    QString nmmAlbumTitle() const {
        return m_nmmAlbumTitle;
    };


public slots:
    void play() {
        AppWindow::instance()->play(m_url, m_mimeType);
    };

private:
    QString m_url;
    QString m_mimeType;
    QString m_title;
    QString m_nmmArtistName;
    QString m_nmmPerformer;
    QString m_nmmMusicAlbum;
    QString m_nmmAlbumTitle;
};

#endif                          // SONGITEM_H
