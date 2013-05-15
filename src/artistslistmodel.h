/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef ARTISTSLISTMODEL_H
#define ARTISTSLISTMODEL_H

#include <QAbstractItemModel>

struct Element {
public:
    Element() {
    };

    char level;

    union {
        struct {
            char header;
            unsigned long count;
            unsigned long offset;
            QVector < struct Element *>*child;
        } level0;

        struct {
            unsigned long index;
            struct Element *parent;
        } level1;
    } content;
};

class ArtistsListModel:public QAbstractItemModel {
    Q_OBJECT

public:
    ArtistsListModel();
    virtual ~ ArtistsListModel();

    virtual int rowCount(const QModelIndex & parent = QModelIndex())const;
    virtual int columnCount(const QModelIndex & parent =
                            QModelIndex())const;
    virtual QVariant data(const QModelIndex & index, int role =
                          Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column,
                              const QModelIndex & parent =
                              QModelIndex())const;
    virtual QModelIndex parent(const QModelIndex & child) const;

private:
    QVector < QStringList > *artists;  //hold all artist from sparql
    QVector < struct Element *>*root;  //root for the tree-structure data model

    void getArtists();
    bool sortArtists();
    void indexArtists();
};
#endif                          // ARTISTSLISTMODEL_H
