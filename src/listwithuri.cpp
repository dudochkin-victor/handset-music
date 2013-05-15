/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QCryptographicHash>
#include <QDebug>
#include <QFile>
#include <QGraphicsLinearLayout>
#include <QPixmap>
#include <QtTracker/Tracker>

#include <MAction>
#include <MButton>
#include <MContentItem>
#include <MImageWidget>
#include <MPannableViewport>
#include <MStylableWidget>
#include <MTheme>

#include "appwindow.h"
#include "listwithuri.h"
#include "songitem.h"

ListWithUri::ListWithUri(const QStringList & songurls, CrumbBar *crumbbar, QGraphicsItem * parent):
    MStylableWidget(parent), m_songurls(songurls), m_crumbbar(crumbbar)
{
    qDebug() << "NowPlaying::NowPlaying";

    setObjectName("ListWithUri");
    setViewType("background");
    setContentsMargins(0, 0, 0, 0);

    QGraphicsLinearLayout *layout =
        new QGraphicsLinearLayout(Qt::Horizontal, this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    image = new MImageWidget;
    image->setObjectName("AlbumArt");
    layout->addItem(image);

    viewport = new MPannableViewport(this);
    MWidgetController *container = new MWidgetController;
    container->setViewType("background");
    container->setObjectName("AlbumContainer");
    container->setContentsMargins(0, 0, 0, 0);
    viewport->setObjectName("AlbumViewport");

    listLayout = new QGraphicsLinearLayout(Qt::Vertical, container);
    listLayout->setSpacing(0);
    listLayout->setContentsMargins(0, 0, 0, 0);

    viewport->setWidget(container);
    layout->addItem(viewport);

    list = new MList(this);
    itemCreator = new SongItemCreator();
    list->setCellCreator(itemCreator);
    model = new ListWithUriModel(m_songurls);
    list->setItemModel(model);

    connect(list, SIGNAL(itemClicked(QModelIndex)), this,
            SLOT(play(QModelIndex)));

    listLayout->addItem(list);

    connect(AppWindow::instance(), SIGNAL(queueIndexChanged(int)), this,
            SLOT(indexChanged(int)));

    QTimer::singleShot(0, this, SLOT(firstIndexChanged()));
}

ListWithUri::~ListWithUri()
{
    qDebug() << "ListWithUri::~ListWithUri";

    delete model;
//    delete itemCreator;
}

void ListWithUri::firstIndexChanged()
{
    qDebug() << "firstIndexChanged";

    if (AppWindow::instance()->index() == -1)
        return;

    indexChanged(AppWindow::instance()->index());
}

void ListWithUri::indexChanged(int row)
{
    if (row == -1)
        return;

    qDebug() << "ListWithUri::indexChanged : " << row << AppWindow::instance()->queue()[row]
             << m_songurls.indexOf(AppWindow::instance()->queue()[row]);

    int current = m_songurls.indexOf(AppWindow::instance()->queue()[row]);

    if (current == -1)
        return;

    QModelIndex index = model->index(current, 0);
    const QString & pixmapURI = index.data(Qt::UserRole).toString();

    if (QFile(pixmapURI).exists())
        image->setPixmap(QPixmap(pixmapURI));
    else
        image->setPixmap(*MTheme::pixmap("albumart-generic"));

    model->setData(index, QVariant::fromValue(true), Qt::UserRole + 2);

    if (model->rowCount() > 4)
        list->scrollTo(index, MList::PositionAtTopHint);

    QString text = index.data(Qt::UserRole + 5).toString();
    text.append(", ");
    text.append(index.data(Qt::UserRole + 4).toString());

    m_crumbbar->setText(text);
}

void ListWithUri::play(QModelIndex index)
{
    AppWindow *window = AppWindow::instance();

    window->play(m_songurls[index.row()],
                 index.data(Qt::UserRole + 1).toString());
}
