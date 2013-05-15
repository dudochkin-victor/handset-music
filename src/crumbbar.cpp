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
#include <QGraphicsLinearLayout>

#include <MButton>
#include <MImageWidget>
#include <MStylableWidget>
#include <MLayout>
#include <MLinearLayoutPolicy>

#include "appwindow.h"
#include "crumbbar.h"
#include "musiclayoutanimation.h"

CrumbBar::CrumbBar(const QString nmmArtistName,
                   const QString nmmPerformer,
                   const QString nmmMusicAlbum,
                   const QString nmmAlbumTitle,
                   QGraphicsItem * parent):MStylableWidget(parent),
m_nmmArtistName(nmmArtistName),
m_nmmPerformer(nmmPerformer),
m_nmmMusicAlbum(nmmMusicAlbum), m_nmmAlbumTitle(nmmAlbumTitle)
{
    qDebug() << "CrumbBar::CrumbBar : " << nmmArtistName << nmmPerformer <<
        nmmMusicAlbum << nmmAlbumTitle;

    setObjectName("CrumbBar");
    setViewType("background");
    setContentsMargins(0, 0, 0, 0);

    MLayout *layout = new MLayout(this);
//    MusicLayoutAnimation *animation = new MusicLayoutAnimation(layout);

    policy = new MLinearLayoutPolicy(layout, Qt::Horizontal);
    policy->setSpacing(0);
    policy->setContentsMargins(0, 0, 0, 0);

    //Now Playing page crumb bar
    if (nmmArtistName == "Now Playing") {
        //% "Play list"
        titleButton = new MButton(qtTrId("meego_music_crumbbar_play_queue"));
        titleButton->setObjectName("CrumbButton");
        policy->addItem(titleButton);

        policy->addStretch();

        return;
    }

    titleButton = new MButton(nmmArtistName);
    titleButton->setObjectName("CrumbButton");
//    b->setViewType(MButton::iconType);
//    connect(b, SIGNAL(clicked()), this, SLOT(showAllArtist()));
    policy->addItem(titleButton);

    policy->addStretch();

    qDebug() << "CrumbBar::CrumbBar" << policy->count();
}

CrumbBar::~CrumbBar()
{
    qDebug() << "CrumbBar::~CrumbBar";
}

void CrumbBar::addAlbumTitle(QString & AlbumTitle)
{
    if (AlbumTitle.isEmpty() == false) {
        QString text = AlbumTitle;

        text.append(", ");
        text.append(titleButton->text());
        titleButton->setText(text);
    }
}

void CrumbBar::removeAlbumTitle()
{
    qDebug() << "CrumbBar::removeItem";

    titleButton->setText(m_nmmArtistName);
}

void CrumbBar::setText(const QString & text)
{
    qDebug() << "CrumbBar::setText" << text;

    titleButton->setText(text);
}

void CrumbBar::retranslateUi()
{
    if (titleButton->text() == "Now Playing") {
        //% "Play list"
        titleButton->setText(qtTrId("meego_music_crumbbar_play_queue"));
    }
}

