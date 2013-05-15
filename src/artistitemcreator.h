/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef ARTISTITEMCREATOR
#define ARTISTITEMCREATOR

#include <QDebug>
#include <QModelIndex>
#include <MButton>
#include <MAbstractCellCreator>
#include <MWidget>
#include <MLabel>

#include "artistitem.h"

class ArtistItemCreator:public MAbstractCellCreator < MButton > {
public:
    ArtistItemCreator() {
        setCellViewType("ArtistsItemType");
        setCellObjectName("ArtistsItem");

        MButton *cell = new MButton();
        cell->setViewType("ArtistsItemType");
        cell->setObjectName("ArtistsItem");

        size = cell->effectiveSizeHint(Qt::PreferredSize);
        delete cell;
        qDebug() << "update cell size " << size;
    }
    virtual void updateCell(const QModelIndex & index,
                            MWidget * cell) const;

    virtual QSizeF cellSize() const {
        return size;
    }
private:
     QSizeF size;
};

#endif
