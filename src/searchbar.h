/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEARCHBAR_H
#define SEARCHBAR_H

#include <MStylableWidget>
#include <MWidgetStyle>

#include "appwindow.h"

class MButton;

class SearchBar:public MStylableWidget {
    Q_OBJECT;

public:
    SearchBar(QGraphicsItem * parent = 0);
    ~SearchBar();
    virtual void retranslateUi();

signals:
    void filter(int type);

private slots:
    void showAll();
    void showArtist();
    void showAlbums();
    void showSongs();

private:
    MButton * allButton;
    MButton *artistButton;
    MButton *albumsButton;
    MButton *songsButton;
//    M_STYLABLE_WIDGET(MWidgetStyle);
};

#endif                          // SEARCHBAR_H
