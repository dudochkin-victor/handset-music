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
#include <algorithm>
#include "artistslistmodel.h"
#include "searchengine.h"
#include "utils.h"

static int compare(const QStringList & a, const QStringList & b)
{
    int z = QString::compare(a[0], b[0], Qt::CaseInsensitive);
    if (z < 0)
        return 1;
    else
        return 0;
}

ArtistsListModel::ArtistsListModel():
root(NULL)
{
    getArtists();

    if (sortArtists())
        indexArtists();
}

void ArtistsListModel::getArtists()
{
    //get all artists urls from tracker
    if (postThread != NULL) {
        //We must wait all items loaded, since we should handle "unknown" and sorting before show.
        while (!(postThread->loadFinished)) {
            usleep(50000);
        }
        artists = postThread->items;

        delete postThread;
        postThread = NULL;
    } else {
        artists = new QVector < QStringList > ();
        SearchEngine::getAllMusicArtists(*artists);
    }
}

bool ArtistsListModel::sortArtists()
{
    if (artists->size() == 0)
        return false;

    //Assuming empty one is the first, since we use SORT inside Sparql
    if (((artists->value(0))[1]).isEmpty()) {   // we have an unknown artist need to be inserted
        QStringList unknownArtist;
        unknownArtist.append("Unknown");
        unknownArtist.append("Unknown");

        //remove the first empty one, and add unknown one
        artists->remove(0);
        artists->append(unknownArtist);
    }

    for (QVector < QStringList >::iterator i = artists->begin();
         i != artists->end(); i++) {
        if ((*i)[0].isEmpty()) {
            (*i)[0] = (*i)[1];
            (*i)[0].remove(QString("urn:artist:"));
        }
    }
    //sort it
    std::sort(artists->begin(), artists->end(), compare);
    return true;
}

void ArtistsListModel::indexArtists()
{
    //build the tree-structured data model
    root = new QVector < struct Element *>();

    QChar pre, current;
    struct Element *element, *level1Element;
    QVector < struct Element *>*level1 = NULL;

    for (int j = 0; j < artists->size(); j++) {
        //       qDebug() << (*i)[0].at(0).isLetter();
        if (!(artists->value(j))[0].at(0).isLetter())
            continue;

        current = (artists->value(j))[0].at(0).toUpper();
        if (current > 'Z') {
            qDebug() << current;
            element->content.level0.count++;

            level1Element = new(struct Element);
            level1Element->level = 1;
            level1Element->content.level1.parent = element;
            level1->append(level1Element);
            continue;
        }

        if (current != pre) {
            element = new(struct Element);

            level1 = new QVector < struct Element *>();

            element->content.level0.child = level1;
            element->level = 0;
            element->content.level0.count = 0;
            element->content.level0.offset = j;
            element->content.level0.header = current.toAscii();

            root->append(element);
//            qDebug() << "a new struct Element is created";
//            qDebug() << current;
        }
        element->content.level0.count++;
        level1Element = new(struct Element);
        level1Element->level = 1;
        level1Element->content.level1.parent = element;
        level1->append(level1Element);

        pre = current;
    }

    qDebug() << "count of root is " << root->size();

    for (int j = 0; j < root->size(); j++) {
//        qDebug() << (*i)->content.level0.count << (*i)->content.level0.header << (*i)->content.level0.count++;
        qDebug() << root->value(j)->content.level0.count << root->
            value(j)->content.level0.header << root->value(j)->content.
            level0.offset;

        element = root->value(j);
        for (int k = 0; k < element->content.level0.child->size(); k++) {
            qDebug() << "--" << element << "--" << element->content.level0.
                child->value(k)->content.level1.parent;
        }
    }
}

ArtistsListModel::~ArtistsListModel()
{
    delete artists;
}

QModelIndex ArtistsListModel::index(int row, int column,
                                    const QModelIndex & parent) const
{
    qDebug() << "ArtistsListModel::index";

    if (!parent.isValid()) {
        if (root == NULL)
            return QModelIndex();
        qDebug() << "1 createIndex" << root->value(row);
        return createIndex(row, column, root->value(row));
    } else {
//        qDebug() << "2 createIndex" << static_cast<struct Element *>(parent.internalPointer())->content.level0.child->value(row);

        return createIndex(row, column,
                           static_cast <
                           struct Element *
                           >(parent.internalPointer())->content.level0.
                           child->value(row));
    }
}

QModelIndex ArtistsListModel::parent(const QModelIndex & child) const
{
    if (!child.isValid())
        return QModelIndex();

    qDebug() << "ArtistsListModel::parent";

    struct Element *element =
        static_cast < struct Element *>(child.internalPointer());

    //first level, i.e. level 0
    if (element->level == 0) {
        return QModelIndex();
    }
    //second level, i.e. level 1
    struct Element *parent = element->content.level1.parent;

//    qDebug() << "3 createIndex" << parent;
    return createIndex(root->indexOf(parent), 0, parent);
}

QVariant ArtistsListModel::data(const QModelIndex & index, int role) const
{
    qDebug() << "ArtistsListModel::data";
    // This function will be called many times during fast panning, lets
    // check boundaries and validnes only in debug mode
    Q_ASSERT(index.isValid());
//    Q_ASSERT(index.row() < artists->size());

    struct Element *element =
        static_cast < struct Element *>(index.internalPointer());

    //first level, i.e. level 0
    if (element->level == 0) {
        if (role == Qt::DisplayRole)
            return QVariant::fromValue(element->content.level0.header);
        else
            return QVariant();
    }
    //second level, i.e. level 1
    if (role == Qt::DisplayRole) {
        return QVariant::fromValue(artists->
                                   value(element->content.level1.parent->
                                         content.level0.offset +
                                         index.row())[0]);
    } else if (role == Qt::UserRole) {
        return QVariant::fromValue(artists->
                                   value(element->content.level1.parent->
                                         content.level0.offset +
                                         index.row())[1]);
    }

    return QVariant();
}

int ArtistsListModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return 1;
}

int ArtistsListModel::rowCount(const QModelIndex & parent) const
{
//    qDebug() << "ArtistsListModel::rowCount";
    //first level, i.e. level 0
    if (!parent.isValid()) {
        if (root == NULL)
            return 0;

        qDebug() << "rowCount 1" << root->size();
        return root->size();
    }

    qDebug() << "rowCount 2:";

    //second level, i.e. level 1
    struct Element *element =
        static_cast < struct Element *>(parent.internalPointer());

    qDebug() << "rowCount 2:" << parent.row() << element->content.level0.
        count;
    return element->content.level0.count;
}
