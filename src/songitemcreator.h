/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef SONGITEMCREATOR
#define SONGITEMCREATOR

#include <QModelIndex>
#include <MContentItem>
#include <MAbstractCellCreator>
#include <MWidget>

class SongItemCreator:public MAbstractCellCreator < MContentItem > {
public:
    SongItemCreator() {
        MContentItem *cell = new MContentItem();
        cell->setObjectName("SongsItem0");
        cell->setViewType("SongsItemType");

        size = cell->effectiveSizeHint(Qt::PreferredSize);
        delete cell;
    }
    virtual void updateCell(const QModelIndex & index,
                            MWidget * cell) const;
    virtual MWidget *createCell(const QModelIndex & index,
                                MWidgetRecycler & recycler) const;

    virtual QSizeF cellSize() const {
        return size;
    }
private:
     QSizeF size;
};
#endif
