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
#include <MButtonGroup>
#include <MStylableWidget>

#include "appwindow.h"
#include "searchbar.h"

SearchBar::SearchBar(QGraphicsItem * parent):MStylableWidget(parent)
{
    qDebug() << "SearchBar::SearchBar";

    setObjectName("SearchBar");
//    setContentsMargins(0, 0, 0, 0);

    QGraphicsLinearLayout *layout =
        new QGraphicsLinearLayout(Qt::Horizontal, this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    MButtonGroup *buttonGroup = new MButtonGroup();

    MButton *allButton = new MButton();
    allButton->setObjectName("SearchBarButtonLeft");
    allButton->setViewType(MButton::toggleType);
    allButton->setCheckable(true);
    allButton->setChecked(true);
    connect(allButton, SIGNAL(clicked()), this, SLOT(showAll()));
    layout->addItem(allButton);
    buttonGroup->addButton(allButton);

    artistButton = new MButton();
    artistButton->setObjectName("SearchBarButtonCenter");
    artistButton->setViewType(MButton::toggleType);
    artistButton->setCheckable(true);
    connect(artistButton, SIGNAL(clicked()), this, SLOT(showArtist()));
    layout->addItem(artistButton);
    buttonGroup->addButton(artistButton);

    albumsButton = new MButton();
    albumsButton->setObjectName("SearchBarButtonCenter");
    albumsButton->setViewType(MButton::toggleType);
    albumsButton->setCheckable(true);
    connect(albumsButton, SIGNAL(clicked()), this, SLOT(showAlbums()));
    layout->addItem(albumsButton);
    buttonGroup->addButton(albumsButton);

    songsButton = new MButton();
    songsButton->setObjectName("SearchBarButtonRight");
    songsButton->setViewType(MButton::toggleType);
    songsButton->setCheckable(true);
    connect(songsButton, SIGNAL(clicked()), this, SLOT(showSongs()));
    layout->addItem(songsButton);
    buttonGroup->addButton(songsButton);
    retranslateUi();
}

SearchBar::~SearchBar()
{
    qDebug() << "SearchBar::~SearchBar";
}

void SearchBar::showAll()
{
    qDebug() << "SearchBar::showAll";
    emit filter((int) AppWindow::SearchAll);
}

void SearchBar::showArtist()
{
    qDebug() << "SearchBar::showAllArtist";
    emit filter((int) AppWindow::SearchArtist);
}

void SearchBar::showAlbums()
{
    qDebug() << "SearchBar::showAllAlbums";
    emit filter((int) AppWindow::SearchAlbums);
}

void SearchBar::showSongs()
{
    qDebug() << "SearchBar::showAllSongs";
    emit filter((int) AppWindow::SearchSongs);
}

void SearchBar::retranslateUi()
{
    //% "All"
    allButton->setText(qtTrId("meego_music_searchbar_all"));
    //% "Artist"
    artistButton->setText(qtTrId("meego_music_search_artist"));
    //% "Albums"
    albumsButton->setText(qtTrId("meego_music_search_album"));
    //% "Songs"
    songsButton->setText(qtTrId("meego_music_search_songs"));
}
