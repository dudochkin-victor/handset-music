/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEARCHRESULTS_H
#define SEARCHRESULTS_H

#include <QDebug>
#include <MButton>
#include <MStylableWidget>
#include <MWidgetStyle>
#include <MList>
#include <MImageWidget>

#include "appwindow.h"
#include "artistitemcreator.h"

class QGraphicsLinearLayout;

class SearchItem:public MButton {
    Q_OBJECT;

public:
    SearchItem(QGraphicsItem * parent = 0);
    ~SearchItem() {
        qDebug() << "SearchItem::~SearchItem";
    };

    QString nmmArtistName() const {
        return m_nmmArtistName;
    };
    QString nmmPerformer() const {
        return m_nmmPerformer;
    };

    void setDisplayName(const QString & displayName) {
        label->setText(displayName);
    };
    void setIcon(const QString & type) {
        if (type == "Artist") {
            icon->setImage("icon-m-content-artist");
        } else if (type == "Album") {
            icon->setImage("icon-m-content-album");
        } else if (type == "Song") {
            icon->setImage("icon-m-content-audio");
        }
    };

public slots:

private:
    QStringList m_uris;
    QStringList m_mimeTypes;
    QString m_nmmArtistName;
    QString m_nmmPerformer;
    MLabel *label;
    MImageWidget *icon;
};

class AllSearchListModel:public QAbstractTableModel {
    Q_OBJECT

public:
    AllSearchListModel(QString & query);
    virtual ~ AllSearchListModel();

    virtual int rowCount(const QModelIndex & parent = QModelIndex())const;
    virtual int columnCount(const QModelIndex & parent =
                            QModelIndex())const;
    virtual QVariant data(const QModelIndex & index, int role =
                          Qt::DisplayRole) const;

private:
    QVector < QStringList > results;
};

class ArtistSearchListModel:public QAbstractTableModel {
    Q_OBJECT

public:
    ArtistSearchListModel(QString & query);
    virtual ~ ArtistSearchListModel();

    virtual int rowCount(const QModelIndex & parent = QModelIndex())const;
    virtual int columnCount(const QModelIndex & parent =
                            QModelIndex())const;
    virtual QVariant data(const QModelIndex & index, int role =
                          Qt::DisplayRole) const;

private:
    QVector < QStringList > results;
};

class AlbumSearchListModel:public QAbstractTableModel {
    Q_OBJECT

public:
    AlbumSearchListModel(QString & query);
    virtual ~ AlbumSearchListModel();

    virtual int rowCount(const QModelIndex & parent = QModelIndex())const;
    virtual int columnCount(const QModelIndex & parent =
                            QModelIndex())const;
    virtual QVariant data(const QModelIndex & index, int role =
                          Qt::DisplayRole) const;

private:
    QVector < QStringList > results;
};

class SongSearchListModel:public QAbstractTableModel {
    Q_OBJECT

public:
    SongSearchListModel(QString & query);
    virtual ~ SongSearchListModel();

    virtual int rowCount(const QModelIndex & parent = QModelIndex())const;
    virtual int columnCount(const QModelIndex & parent =
                            QModelIndex())const;
    virtual QVariant data(const QModelIndex & index, int role =
                          Qt::DisplayRole) const;

private:
    QVector < QStringList > results;
};


class SearchItemCreator:public MAbstractCellCreator<SearchItem> {
public:
    SearchItemCreator() {
        setCellViewType("ArtistsItemType");
        setCellObjectName("ArtistsItem");

        SearchItem *cell = new SearchItem();
        cell->setViewType("ArtistsItemType");
        cell->setObjectName("ArtistsItem");

        size = cell->effectiveSizeHint(Qt::PreferredSize);
        delete cell;
    }
    virtual void updateCell(const QModelIndex & index,
                            MWidget * cell) const;

    virtual QSizeF cellSize() const {
        return size;
    }
private:
    QSizeF size;
};

class SearchResults:public MStylableWidget {
    Q_OBJECT;

public:
    SearchResults(QGraphicsItem * parent = 0);
    ~SearchResults();

    void setSearchString(QString search);
    void addObjectMenuActions(MList *list);

public slots:
    void showAll(QString query);
    void showArtist(QString query);
    void showAlbums(QString query);
    void showSongs(QString query);

    void playResult();
    void addToPlayqueue();
    void addToPlaylist();
    void itemLongTapped(const QModelIndex & index);

private:
    void getResults(QVector <QStringList> &result);
    void clearList();
    void searchArtist(QString query);
    void searchAlbums(QString query);
    void searchSongs(QString query);

    QString searchString;
    QGraphicsLinearLayout *listLayout;
    MList *allList;
    MList *artistList;
    MList *albumList;
    MList *songList;
    QModelIndex longTappedIndex;
//    M_STYLABLE_WIDGET(MWidgetStyle);
};

#endif                          // SEARCHRESULTS_H
