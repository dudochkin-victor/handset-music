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
#include <QModelIndex>
#include <MWidget>
#include <QFile>
#include <QPixmap>

#include "albumitemcreator.h"

void AlbumItemCreator::updateCell(const QModelIndex & index,
                                  MWidget * cell) const
{
    qDebug() << "AlbumItemCreator::updateCell" << index.
        data(Qt::DisplayRole).toString()
        << index.row();

    if (!cell)
        return;

    MContentItem *contentItem = qobject_cast < MContentItem * >(cell);
    const QString & displayName = index.data(Qt::DisplayRole).toString();
    const QPixmap & pixmap =
        index.data(Qt::DecorationRole).value < QPixmap > ();

    contentItem->setPixmap(pixmap);
    contentItem->setTitle(displayName);
    contentItem->boundingRect();
}

MWidget *AlbumItemCreator::createCell(const QModelIndex & index,
                                      MWidgetRecycler & recycler) const
{
    MContentItem *cell =
        dynamic_cast <
        MContentItem *
        >(recycler.take(MContentItem::staticMetaObject.className()));

    if (cell == NULL) {
        cell = new MContentItem(MContentItem::IconAndSingleTextLabel);
        cell->setObjectName("AlbumsItem");
        cell->setViewType("AlbumsItemType");
    }

    updateCell(index, cell);
    return cell;
}
