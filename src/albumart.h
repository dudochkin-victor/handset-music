/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef ALBUMART_H
#define ALBUMART_H

#include <QByteArray>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QPixmap>
#include <MImageWidget>

#include "appwindow.h"

class AlbumArt:public MImageWidget {
    Q_OBJECT;

public:
    AlbumArt(QGraphicsItem * parent = 0):
    MImageWidget(parent) {
        connect(this, SIGNAL(clicked()), this, SLOT(showAlbum()));
    };
    ~AlbumArt() {
        qDebug() << "AlbumArt::~AlbumArt";
    };

    void setData(const QString nmmArtistName,
                 const QString nmmPerformer,
                 const QString nmmMusicAlbum,
                 const QString nmmAlbumTitle) {
        m_nmmArtistName = nmmArtistName;
        m_nmmPerformer = nmmPerformer;
        m_nmmMusicAlbum = nmmMusicAlbum;
        m_nmmAlbumTitle = nmmAlbumTitle;

        QString artist = nmmArtistName.toLower();
        artist.replace(QString("urn:artist:"), QString(""));
        artist.replace(QString("%20"), QString(" "));

        QString album = nmmMusicAlbum.toLower();
        album.replace(QString("urn:album:"), QString(""));
        album.replace(QString("%20"), QString(" "));

        QByteArray album_md5 =
            QCryptographicHash::hash(album.toAscii(),
                                     QCryptographicHash::Md5);
        QByteArray artist_md5 =
            QCryptographicHash::hash(artist.toAscii(),
                                     QCryptographicHash::Md5);

        QString path = QDir::toNativeSeparators(QDir::homePath()) +
            QDir::separator() + QString(".cache/media-art/album-") +
            artist_md5.toHex() + "-" + album_md5.toHex() + ".jpeg";

        if (QFile(path).exists())
            setPixmap(QPixmap(path));
        else
            setPixmap(*MTheme::pixmap("albumart-generic"));
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


    public slots:void showAlbum() {
        AppWindow::instance()->showAlbum(m_nmmArtistName,
                                         m_nmmPerformer,
                                         m_nmmMusicAlbum, m_nmmAlbumTitle);
    };

signals:
    void clicked();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *) {
        qDebug() << objectName() << " clicked()";
        emit clicked();
    };

private:
    QString m_nmmArtistName;
    QString m_nmmPerformer;
    QString m_nmmMusicAlbum;
    QString m_nmmAlbumTitle;
};

#endif                          // ALBUMART_H
