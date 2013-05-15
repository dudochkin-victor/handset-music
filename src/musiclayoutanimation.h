/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

#include <MLayoutAnimation>
#include <MLayout>

#ifndef MUSICLAYOUTANIMATION_H
#define MUSICLAYOUTANIMATION_H

class MusicLayoutAnimation:public MLayoutAnimation {
public:
    MusicLayoutAnimation(MLayout * layout);

    virtual ~ MusicLayoutAnimation();

    virtual void itemRemovedFromLayout(int index);
    virtual void itemAddedToLayout(int index);
    virtual void animatedDeleteItem(int index);
    virtual void setItemGeometry(int index, const QRectF & geometry);
    virtual void hideItem(int index);

protected:
    virtual int duration() const {
        return 3000;
    }
    virtual void updateCurrentTime(int currentTime) {
    }

private:
    QParallelAnimationGroup * delayedAnimation;
    QPropertyAnimation *positionAnimation;
    QPropertyAnimation *opacityAnimation;
};

#endif
