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
#include <MContentItem>
#include <MAbstractCellCreator>
#include <MWidget>

#include "songitemcreator.h"

void SongItemCreator::updateCell(const QModelIndex & index, MWidget * cell) const
{
    MContentItem *contentItem = qobject_cast < MContentItem * >(cell);
    QVariant data = index.data(Qt::DisplayRole);
    QString displayName = data.toString();
    int duration = index.data(Qt::UserRole + 3).toString().toInt();

    qDebug() << "SongItemCreator::updateCell" << index.row() << index.
        data(Qt::UserRole + 2).toBool();

    if (index.data(Qt::UserRole + 2).toBool() == true)
        cell->setObjectName("SongsItemActive");
    else {
        if (index.row() % 2 == 0)
            cell->setObjectName("SongsItem0");
        else
            cell->setObjectName("SongsItem1");
    }

    contentItem->
        setTitle(QString
                 ("<table border=\"0\" width=\"70%\"> <tr> <td width=\"80%\"> %1 %2 </td> <width> <td width=\"20%\"> %3 </td> </tr> </table>").
                 arg(index.row() +
                     1).arg(index.data(Qt::DisplayRole).toString()).
                 arg(QString().
                     sprintf("%02d:%02d", duration / 60, duration % 60)));

}

MWidget *SongItemCreator::createCell(const QModelIndex & index,
                                     MWidgetRecycler & recycler) const
{
    MContentItem *cell =
        dynamic_cast <
        MContentItem *
        >(recycler.take(MContentItem::staticMetaObject.className()));

    if (cell == NULL) {
        cell = new MContentItem(MContentItem::SingleTextLabel);
        if (index.row() % 2 == 0)
            cell->setObjectName("SongsItem0");
        else
            cell->setObjectName("SongsItem1");

        cell->setViewType("SongsItemType");
    }

    updateCell(index, cell);
    return cell;
}
