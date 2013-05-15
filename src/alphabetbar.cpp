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
#include <MImageWidget>
#include <MStylableWidget>

#include "appwindow.h"
#include "alphabetbar.h"

AlphabetBar::AlphabetBar(MList * list, QGraphicsItem * parent):MStylableWidget
    (parent)
{
    qDebug() << "AlphabetBar::AlphabetBar";

    setObjectName("AlphabetBar");
    setContentsMargins(0, 0, 0, 0);

    /** TODO: create landscape and portrait layout policy separately for AlphabetBar **/
    QGraphicsLinearLayout *alphabetLayout =
        new QGraphicsLinearLayout(Qt::Horizontal, this);
    alphabetLayout->setSpacing(0);
    alphabetLayout->setContentsMargins(0, 0, 0, 0);

    MButton *b = new MButton("#");
    b->setObjectName("AlphabetItem");
    connect(b, SIGNAL(clicked()), this, SLOT(clicked()));
    alphabetLayout->addItem(b);

    for (char i = 'A'; i <= 'Z'; i++) {
        MButton *b =
            new MButton(QString("%1 %2").arg((char) 0xc2b7).arg(i));
        b->setObjectName("AlphabetItem");
        connect(b, SIGNAL(clicked()), this, SLOT(clicked()));
        alphabetLayout->addItem(b);
    }
    if (list)
        scrollList = list;
}

AlphabetBar::~AlphabetBar()
{
    qDebug() << "AlphabetBar::~AlphabetBar";
}

void AlphabetBar::clicked()
{
    MButton *btn = static_cast < MButton * >(sender());
    QChar jumpToChar;

    if (!scrollList)
        return;
    QAbstractTableModel *model =
        (QAbstractTableModel *) (scrollList->itemModel());

    /* work around for bug #4815 */
    /*first level*/
    int total_count = 0, first_level_row_count = model->rowCount();
    for (int i = 0; i < first_level_row_count; i++) {
        QModelIndex index = model->index(i, 0);
        if (index.isValid() == false)
            continue;
        total_count += model->rowCount(index);
    }

    total_count += first_level_row_count;
    qDebug() << "total_count is" << total_count;
    if (total_count < 6)
        return;

    QModelIndex idx = model->index(0, 0);

    if (idx.isValid() == false)
        return;

    if (btn->text() == "#") {
        scrollList->scrollTo(idx, MList::PositionAtTopHint);
        qDebug() << "AlphabetBar::clicked()" << " : " << btn->text();
        return;
    } else
        jumpToChar = btn->text().at(2);

    qDebug() << "AlphabetBar::clicked()" << jumpToChar << " : " << btn->
        text();

    int low = 0, high = model->rowCount() - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        QModelIndex idx = model->index(mid, 0);
        QString artistName = model->data(idx, Qt::DisplayRole).toString();
        if (artistName.startsWith(jumpToChar, Qt::CaseInsensitive) == true) {   // found
            while (mid > 0) {
                QModelIndex pre_idx = model->index((--mid), 0);
                artistName =
                    model->data(pre_idx, Qt::DisplayRole).toString();
                if (artistName.
                    startsWith(jumpToChar, Qt::CaseInsensitive) == true) {
                    idx = pre_idx;
                } else {
                    mid++;
                    break;
                }
            }
            scrollList->scrollTo(idx, MList::PositionAtTopHint);
            qDebug() << "AlphabetBar::scrollTo: (exact)" << mid << ":" <<
                model->data(idx,
                            Qt::DisplayRole).
                toString() << ":" << jumpToChar;
            return;
        } else {
            if (artistName[0].toUpper() > jumpToChar) {
                high = mid - 1;
            } else {
                low = mid + 1;
            }
        }
    }
    if (high < 0)
        high = 0;
    qDebug() << "AlphabetBar::scrollTo: (no match)" << high << ":" <<
        model->data(model->index(high, 0),
                    Qt::DisplayRole).toString() << ":" << jumpToChar;
    scrollList->scrollTo(model->index(high, 0), MList::PositionAtTopHint);
}
