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

#include "artistitemcreator.h"

void ArtistItemCreator::updateCell(const QModelIndex & index,
                                   MWidget * cell) const
{
    qDebug() << "updateCell is called" << index.data(Qt::DisplayRole).
        toString();

    MButton *contentItem = qobject_cast < MButton * >(cell);
    QVariant data = index.data(Qt::DisplayRole);
    QString displayName = data.toString();

    contentItem->setText(displayName);
}
