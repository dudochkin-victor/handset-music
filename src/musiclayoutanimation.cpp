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
#include "musiclayoutanimation.h"

#define DURATION 500

MusicLayoutAnimation::MusicLayoutAnimation(MLayout * layout):
MLayoutAnimation(layout)
{
    delayedAnimation = new QParallelAnimationGroup;
    positionAnimation = new QPropertyAnimation;
    opacityAnimation = new QPropertyAnimation;

    delayedAnimation->addAnimation(positionAnimation);

}

MusicLayoutAnimation::~MusicLayoutAnimation()
{
    delete positionAnimation;
    delete opacityAnimation;
    delete delayedAnimation;
}

void MusicLayoutAnimation::itemAddedToLayout(int index)
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

    qreal x;

    if (index != 0) {
        qDebug() << "index == " << index;

        if (positionAnimation->state() == QAbstractAnimation::Running) {
            layout()->takeAt(index);
            return;
        }


        showItemNow((QGraphicsWidget *) layout()->itemAt(index));

//        delayedAnimation = new QParallelAnimationGroup;
//        positionAnimation = new QPropertyAnimation;
//        delayedAnimation->addAnimation(positionAnimation);
        positionAnimation->setPropertyName("pos");
        positionAnimation->
            setTargetObject(((MWidgetController *) layout()->
                             itemAt(index)->graphicsItem()));

        x = layout()->itemAt(index - 1)->graphicsItem()->pos().x() +
            layout()->itemAt(index -
                             1)->graphicsItem()->boundingRect().width();
        qDebug() << "x == " << x;
        positionAnimation->setStartValue(QPointF(1024, 0));
        positionAnimation->setEndValue(QPointF(x, 0));
        positionAnimation->setEasingCurve(QEasingCurve::InOutExpo);

        if (index == 2)
            positionAnimation->setDuration(DURATION);
        else
            positionAnimation->setDuration(0);

//        addAnimation(delayedAnimation);

        delayedAnimation->start();
        return;
    }
//        return;
    showItemNow((QGraphicsWidget *) layout()->itemAt(index));
}

void MusicLayoutAnimation::itemRemovedFromLayout(int index)
{
    qDebug() << "MusicLayoutAnimation::itemRemovedFromLayout" << index;

}

void MusicLayoutAnimation::setItemGeometry(int index,
                                           const QRectF & geometry)
{

    qDebug() << "MusicLayoutAnimation::setItemGeometry" << index <<
        geometry;
}

void MusicLayoutAnimation::hideItem(int index)
{
    qDebug() << "MusicLayoutAnimation::hideItem" << index;

//    QPropertyAnimation *animation = new QPropertyAnimation;

    opacityAnimation->setPropertyName("opacity");
    opacityAnimation->
        setTargetObject(((MWidgetController *) layout()->itemAt(index)->
                         graphicsItem()));
    opacityAnimation->setStartValue(1.0);
    opacityAnimation->setEndValue(0.0);
    opacityAnimation->setDuration(DURATION);

    opacityAnimation->start();

    layout()->takeAt(index);
}

void MusicLayoutAnimation::animatedDeleteItem(int index)
{
    qDebug() << "MusicLayoutAnimation::animatedDeleteItem" << index;

}
