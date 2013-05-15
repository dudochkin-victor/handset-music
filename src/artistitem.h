/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef ARTISTITEM_H
#define ARTISTITEM_H

#include <MButton>

class ArtistItem:public MButton {
    Q_OBJECT
public:
    ArtistItem(QString nmmArtistName, QString nmmPerformer, QGraphicsItem * parent = 0):
    MButton(parent), m_nmmArtistName(nmmArtistName), m_nmmPerformer(nmmPerformer) {
        setText(m_nmmArtistName);
        setObjectName("AllMusicItem");
    };
    ~ArtistItem() {
    };

    QString nmmPerformer() const {
        return m_nmmPerformer;
    };
    QString nmmArtistName() const {
        return m_nmmArtistName;
    };

private:
    QString m_nmmArtistName;
    QString m_nmmPerformer;
};

#endif                          // ARTISTITEM_H
