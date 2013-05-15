/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef ALBUMITEMCREATOR_H
#define ALBUMITEMCREATOR_H

#include <MWidget>
#include <QModelIndex>
#include <MContentItem>
#include <MAbstractCellCreator>
#include <MWidgetRecycler>

class AlbumItemCreator:public MAbstractCellCreator < MContentItem > {

public:
    AlbumItemCreator() {
        MContentItem *cell = new MContentItem();
        cell->setViewType("AlbumsItemType");
        cell->setObjectName("AlbumsItem");

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
