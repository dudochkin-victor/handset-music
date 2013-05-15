/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QPropertyAnimation>

#include <MLayoutAnimation>
#include <MLayout>

#ifndef ARTISTLAYOUTANIMATION_H
#define ARTISTLAYOUTANIMATION_H

class ArtistLayoutAnimation:public MLayoutAnimation {
public:
    ArtistLayoutAnimation(MLayout * layout);

    virtual ~ ArtistLayoutAnimation();

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
        Q_UNUSED(currentTime);
    }

private:
    QParallelAnimationGroup * delayedAnimation;
    QPropertyAnimation *positionAnimation;
    QPropertyAnimation *removeAnimation;
};

#endif
