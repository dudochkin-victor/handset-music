/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef ALBUMITEM_H
#define ALBUMITEM_H

#include <MStylableWidget>
#include <MWidgetStyle>

class AlbumItem:public MStylableWidget {
    Q_OBJECT;

public:
    AlbumItem(const QString nmmArtistName, const QString nmmPerformer,
              const QString nmmMusicAlbum, const QString nmmAlbumTitle,
              QGraphicsItem * parent = 0);
    ~AlbumItem();

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

    public slots:void showAlbum();

signals:
    void clicked();

private:
    QString m_nmmArtistName;
    QString m_nmmPerformer;
    QString m_nmmMusicAlbum;
    QString m_nmmAlbumTitle;
//    M_STYLABLE_WIDGET(MWidgetStyle);
};

#endif                          // ALBUMITEM_H
