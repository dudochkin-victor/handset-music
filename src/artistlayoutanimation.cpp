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
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <MWidgetController>

#include "artistlayoutanimation.h"
#include "appwindow.h"

#define DURATION 500

ArtistLayoutAnimation::ArtistLayoutAnimation(MLayout * layout):
MLayoutAnimation(layout)
{
    delayedAnimation = new QParallelAnimationGroup;
    positionAnimation = new QPropertyAnimation;
    removeAnimation = new QPropertyAnimation;

    delayedAnimation->addAnimation(positionAnimation);
}

ArtistLayoutAnimation::~ArtistLayoutAnimation()
{
    delete positionAnimation;
    delete removeAnimation;
    delete delayedAnimation;
}

void ArtistLayoutAnimation::itemAddedToLayout(int index)
{
//    ((MWidgetController *)(layout()->itemAt(index)))->show();
    qDebug() << "itemAddedToLayout " << index << layout()->itemAt(index)->
        graphicsItem()->pos();

//    QSequentialAnimationGroup

//    if (index == 1)
//    {
//        qDebug() << "itemAddedToLayout " << layout()->itemAt(index-1)->graphicsItem()->boundingRect().width();
//        return;
//    }

    qreal y;

    if (index != 0) {
        qDebug() << "index == " << index;

        if (positionAnimation->state() == QAbstractAnimation::Running) {
            layout()->takeAt(index);
            return;
        }


        showItemNow((QGraphicsWidget *) layout()->itemAt(index));

        positionAnimation->setPropertyName("pos");
        positionAnimation->
            setTargetObject(((MWidgetController *) layout()->
                             itemAt(index)->graphicsItem()));

        y = layout()->itemAt(index - 1)->graphicsItem()->pos().y() +
            layout()->itemAt(index -
                             1)->graphicsItem()->boundingRect().height();
        qDebug() << "y == " << y;
        positionAnimation->setStartValue(QPointF(1024, y));
        positionAnimation->setEndValue(QPointF(0, y));
        positionAnimation->setEasingCurve(QEasingCurve::InOutExpo);

        if (AppWindow::instance()->state() == AppWindow::StateAllArtist) {
            positionAnimation->setDuration(0);
        } else {
            if (index == 1)
                positionAnimation->setDuration(DURATION);
            else
                positionAnimation->setDuration(0);
        }

//        addAnimation(delayedAnimation);

        delayedAnimation->start();
        return;
    }
//        return;
    showItemNow((QGraphicsWidget *) layout()->itemAt(index));
}

void ArtistLayoutAnimation::itemRemovedFromLayout(int index)
{
    qDebug() << "ArtistLayoutAnimation::itemRemovedFromLayout" << index;

}

void ArtistLayoutAnimation::setItemGeometry(int index,
                                            const QRectF & geometry)
{

    qDebug() << "MusicLayoutAnimation::setItemGeometry" << index <<
        geometry;
}

void ArtistLayoutAnimation::hideItem(int index)
{
    qDebug() << "ArtistLayoutAnimation::hideItem" << index << layout()->
        itemAt(index)->graphicsItem()->pos().y();

    removeAnimation->setPropertyName("pos");
    removeAnimation->
        setTargetObject(((MWidgetController *) layout()->itemAt(index)->
                         graphicsItem()));
    removeAnimation->
        setStartValue(QPointF
                      (0,
                       layout()->itemAt(index)->graphicsItem()->pos().
                       y()));
    removeAnimation->
        setEndValue(QPointF
                    (-1024,
                     layout()->itemAt(index)->graphicsItem()->pos().y()));
    removeAnimation->setDuration(DURATION + 100);
    removeAnimation->setEasingCurve(QEasingCurve::InOutExpo);

    removeAnimation->start();

    layout()->takeAt(index);
}

void ArtistLayoutAnimation::animatedDeleteItem(int index)
{
    qDebug() << "MusicLayoutAnimation::animatedDeleteItem" << index;

}
