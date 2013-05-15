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
#include <MButton>
#include <MImageWidget>
#include <MStylableWidget>

#include "appwindow.h"
#include "albumitem.h"
#include "albumart.h"

AlbumItem::AlbumItem(QString nmmArtistName, QString nmmPerformer, QString nmmMusicAlbum, QString nmmAlbumTitle, QGraphicsItem * parent):
MStylableWidget(parent),
m_nmmArtistName(nmmArtistName),
m_nmmPerformer(nmmPerformer),
m_nmmMusicAlbum(nmmMusicAlbum), m_nmmAlbumTitle(nmmAlbumTitle)
{
    qDebug() << "AlbumItem::AlbumItem : " << nmmArtistName <<
        nmmMusicAlbum;

    setObjectName("AlbumItem");
    setContentsMargins(0, 0, 0, 0);

    QGraphicsLinearLayout *layout =
        new QGraphicsLinearLayout(Qt::Horizontal, this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    AlbumArt *image = new AlbumArt;
    image->setObjectName("AlbumItemImage");
    image->setData(nmmArtistName, nmmPerformer, nmmMusicAlbum,
                   nmmAlbumTitle);
    layout->addItem(image);

    MButton *b = new MButton(nmmAlbumTitle);
    b->setObjectName("AlbumItemButton");
    connect(b, SIGNAL(clicked()), this, SLOT(showAlbum()));
    layout->addItem(b);
}

AlbumItem::~AlbumItem()
{
    qDebug() << "AlbumItem::~AlbumItem";
}

void AlbumItem::showAlbum()
{
    qDebug() << "AlbumItem::showAlbum()";
    emit clicked();
}
