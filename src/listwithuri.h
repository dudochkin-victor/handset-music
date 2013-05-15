/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef LISTWITHURI_H
#define LISTWITHURI_H

#include <MStylableWidget>
#include <MWidgetStyle>
#include <MList>
#include <MImageWidget>
#include <QModelIndex>

#include "listwithurimodel.h"
#include "songitemcreator.h"
#include "crumbbar.h"

class QGraphicsLinearLayout;
class MPannableViewport;

class ListWithUri:public MStylableWidget {
    Q_OBJECT;

public:
    ListWithUri(const QStringList & songurls, CrumbBar *crumbbar, QGraphicsItem * parent = 0);
    ~ListWithUri();

    QAbstractTableModel *getModel() const {
        return model;
    };
    const QStringList *getUrls() {
        return &m_songurls;
    };

private slots:
    void indexChanged(int row);
    void play(QModelIndex index);
    void firstIndexChanged();

protected:
    MList *list;
    const QStringList & m_songurls;

private:

    MImageWidget * image;
    QGraphicsLinearLayout *listLayout;
    MPannableViewport *viewport;
//    M_STYLABLE_WIDGET(MWidgetStyle);

    QAbstractTableModel *model;
    SongItemCreator *itemCreator;

    CrumbBar *m_crumbbar;
};

#endif                          // LISTWITHURI_H
