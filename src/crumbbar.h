/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef CRUMBBAR_H
#define CRUMBBAR_H

#include <QGraphicsLayoutItem>

#include <MStylableWidget>
#include <MWidgetStyle>
#include <MLinearLayoutPolicy>
#include <MButton>

class CrumbBar:public MStylableWidget {
    Q_OBJECT;

public:
    CrumbBar(const QString nmmArtistName, const QString nmmPerformer,
             const QString nmmMusicAlbum =
             QString(), const QString nmmAlbumTitle =
             QString(), QGraphicsItem * parent = 0);
    ~CrumbBar();

    void addAlbumTitle(QString & AlbumTitle);
    void removeAlbumTitle();
    void setText(const QString & text);
    virtual void retranslateUi();

private slots:

private:
//    M_STYLABLE_WIDGET(MWidgetStyle);
    QString m_nmmArtistName;
    QString m_nmmPerformer;
    QString m_nmmMusicAlbum;
    QString m_nmmAlbumTitle;

    MLinearLayoutPolicy *policy;

    MButton *titleButton;
};

#endif                          // CRUMBBAR_H
