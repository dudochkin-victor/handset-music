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
#include <QtTracker/Tracker>
#include <QDBusConnection>
#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>
#include <QGraphicsWidget>
#include <QUrl>
#include <MApplication>
#include <MApplicationPage>
#include <MAction>
#include <MButton>
#include <MDialog>
#include <MLabel>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MLocale>
#include <MOverlay>
#include <MTextEdit>
#include <MSeekBar>
#include <algorithm>

#include "appwindow.h"
#include "album.h"
#include "allartist.h"
#include "alphabetbar.h"
#include "artistlist.h"
#include "bognorqueue_interface.h"
#include "nowplaying.h"
#include "playlist.h"
#include "searchbar.h"
#include "searchresults.h"
#include "searchengine.h"
#include "artistlayoutanimation.h"
#include "music_adaptor.h"

#define SONG_TITLE "SELECT nie:title(?uri) WHERE {  ?uri nie:url '%1' }"
#define ALL_MUSIC "SELECT nie:url(?uri) nie:mimeType(?uri) WHERE { ?uri a nmm:MusicPiece } ORDER BY nmm:musicAlbum(?uri) nmm:trackNumber(?uri)"
#define SONGS_FOR_ARTIST "SELECT nie:url(?uri) nie:mimeType(?uri) WHERE { ?uri a nmm:MusicPiece . ?uri nmm:performer '%1' } ORDER BY nmm:musicAlbum(?uri) nmm:trackNumber(?uri)"

AppWindow *AppWindow::mainWindowInstance = NULL;

AppWindow::AppWindow(QWidget * parent):
MApplicationWindow(parent),
//appState(StateNULL),
searchType(AppWindow::SearchAll),
appState(StateNULL),
isPlaying(false),
userInitiatedStop(false),
loopMode(false),
shuffleMode(false),
artistPage(NULL),
albumPage(NULL),
nowplayingPage(NULL),
playlistsPage(NULL),
playlistPage(NULL),
searchPage(NULL),
contentAllArtist(NULL),
contentArtist(NULL),
contentAlbum(NULL),
contentPlaylists(NULL),
contentPlaylist(NULL),
content(NULL),
currentIndex(-1), currentDuration(0), seekBarIsPressed(false)
{
    mainWindowInstance = this;

    new AudiocontrolAdaptor(this);

    connect(MApplication::instance(), SIGNAL(localeSettingsChanged()), this,
            SLOT(localeChanged()));
    // Connect to the remote media player engine's local queue
    localQueue =
        new OrgMoblinBognorRegisQueueInterface("org.moblin.BognorRegis",
                                               "/org/moblin/BognorRegis/Queues/local_queue",
                                               QDBusConnection::
                                               sessionBus(), this);
    // a workaround hack to force initialization of BR daemon
    QDBusReply < QDBusVariant > rep = localQueue->call("GetRepeatMode");

    connect(localQueue, SIGNAL(PositionChanged(double)), this,
            SLOT(positionChanged(double)));
    connect(localQueue, SIGNAL(IndexChanged(int)), this,
            SLOT(indexChanged(int)));
    connect(localQueue, SIGNAL(ItemMoved(int, int, int)), this,
            SLOT(itemMoved(int, int, int)));
    connect(localQueue, SIGNAL(StateChanged(int)), this,
            SLOT(stateChanged(int)));
    connect(localQueue, SIGNAL(UriAdded(const QString &, int)), this,
            SLOT(uriAdded(const QString &, int)));
    connect(localQueue, SIGNAL(UriRemoved(const QString &, int)), this,
            SLOT(uriRemoved(const QString &, int)));

    QDBusPendingReply < QStringList > reply = localQueue->ListUris();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher *)), this,
            SLOT(listUrisFinished(QDBusPendingCallWatcher *)));

    container = new MWidgetController;
    container->setViewType("background");
    container->setObjectName("ContentContainer");
//    container->setContentsMargins(0, 0, 0, 0);

    mainLayout = new MLayout(container);
//    mainLayout->setContentsMargins(0, 0, 0, 0);

    policy = new MLinearLayoutPolicy(mainLayout, Qt::Vertical);
    policy->setContentsMargins(0, 0, 0, 0);
    policy->setSpacing(0);


    contentAllArtist = new AllArtist;
    content = static_cast < QGraphicsLayoutItem * >(contentAllArtist);
    bar =
        static_cast <
        QGraphicsLayoutItem *
        >(new AlphabetBar(contentAllArtist->getList()));

    policy->addItem(bar);
    policy->addItem(contentAllArtist);
    mainLayout->setLandscapePolicy(policy);

    MLinearLayoutPolicy *portraitPolicy = new MLinearLayoutPolicy(mainLayout, Qt::Horizontal);
    portraitPolicy->setContentsMargins(0, 0, 0, 0);
    portraitPolicy->setSpacing(0);
    portraitPolicy->addItem(contentAllArtist);
    portraitPolicy->addItem(bar);
    mainLayout->setPortraitPolicy(portraitPolicy);

    //bottomToolbar MOverlay
    MOverlay *bottomToolbar = new MOverlay;
    bottomToolbar->setObjectName("BottomOverlay");
    bottomToolbar->setContentsMargins(0, 0, 0, 0);

    MWidget *bToolbar = bottomToolbar->widget();
    bottomToolbarLayout = new MLayout(bToolbar);
    bottomToolbarLayout->setContentsMargins(0, 0, 0, 0);
    playcontrolPolicy =
        new MLinearLayoutPolicy(bottomToolbarLayout, Qt::Horizontal);

    backButton = new MButton;
    backButton->setViewType(MButton::toggleType);
    backButton->setCheckable(true);
    backButton->setIconID("icon-m-common-backwards");
    backButton->setToggledIconID("icon-m-toolbar-mediacontrol-backwards-dissabled");
    backButton->setObjectName("BottomToolbarButtonLeft");
    connect(backButton, SIGNAL(clicked()), this, SLOT(playPrevious()));
    playcontrolPolicy->addItem(backButton);

    playButton = new MButton;
    playButton->setCheckable(true);
    playButton->setViewType(MButton::toggleType);
    playButton->setIconID("icon-m-common-play");
    playButton->setToggledIconID("icon-m-common-pause");
    playButton->setObjectName("BottomToolbarButtonCenter");
    connect(playButton, SIGNAL(clicked()), this, SLOT(playAll()));
    playcontrolPolicy->addItem(playButton);

    forwardButton = new MButton;
    forwardButton->setViewType(MButton::toggleType);
    forwardButton->setCheckable(true);
    forwardButton->setIconID("icon-m-common-forward");
    forwardButton->setToggledIconID("icon-m-toolbar-mediacontrol-forward-dissabled");
    forwardButton->setObjectName("BottomToolbarButtonCenter");
    connect(forwardButton, SIGNAL(clicked()), this, SLOT(playNext()));
    playcontrolPolicy->addItem(forwardButton);

    MWidgetController *playbar_panel1 = new MWidgetController;
    playbar_panel1->setViewType("background");
    playbar_panel1->setObjectName("PlaybarPanel1");

    QGraphicsLinearLayout *playbarPanel1VLayout =
        new QGraphicsLinearLayout(Qt::Vertical, playbar_panel1);
    playbarPanel1VLayout->setSpacing(0);
    playbarPanel1VLayout->setContentsMargins(0, 0, 0, 0);

    QGraphicsLinearLayout *nowplayingHLayout =
        new QGraphicsLinearLayout(Qt::Horizontal);
    nowplayingHLayout->setSpacing(0);
    nowplayingHLayout->setContentsMargins(0, 0, 0, 0);

    nowPlayingButton = new MButton();
    nowPlayingButton->setObjectName("NowPlayingButton");

    nowplayingHLayout->addItem(nowPlayingButton);

    timeButton = new MButton();
    timeButton->setObjectName("TimeButton");
    timeButton->setText("00:00");
    nowplayingHLayout->addItem(timeButton);

    playbarPanel1VLayout->addItem(nowplayingHLayout);

    seekBar = new MSeekBar(bottomToolbar);
    seekBar->setObjectName("BottomToolbarSeekBar");
    seekBar->setLoadedContentRange(0, 100);
    seekBar->setValue(0);
    connect(seekBar, SIGNAL(sliderPressed()), this, SLOT(sliderPressed()));
    connect(seekBar, SIGNAL(sliderReleased()), this,
            SLOT(sliderReleased()));
    playbarPanel1VLayout->addItem(seekBar);

    playcontrolPolicy->addItem(playbar_panel1);

    MWidgetController *playbar_panel2 = new MWidgetController;
    playbar_panel2->setViewType("background");
    playbar_panel2->setObjectName("PlaybarPanel2");

    QGraphicsLinearLayout *playbarPanel2VLayout =
        new QGraphicsLinearLayout(Qt::Vertical, playbar_panel2);
    playbarPanel2VLayout->setSpacing(0);
    playbarPanel2VLayout->setContentsMargins(0, 0, 0, 0);

    nextButton = new MButton();
    nextButton->setObjectName("NextButton");
    connect(nextButton, SIGNAL(clicked()), this, SLOT(showNowPlaying()));

    nextPlayingButton = new MButton();
    nextPlayingButton->setObjectName("NextPlayingButton");
    connect(nextPlayingButton, SIGNAL(clicked()), this,
            SLOT(showNowPlaying()));

    playbarPanel2VLayout->addItem(nextButton);
    playbarPanel2VLayout->addItem(nextPlayingButton);

    playcontrolPolicy->addItem(playbar_panel2);

//    bottomToolbarHLayout->addStretch();

    shuffleButton = new MButton;
    shuffleButton->setCheckable(true);
    shuffleButton->setViewType(MButton::toggleType);
    shuffleButton->setIconID("icon-m-music-video-shuffle-off");
    shuffleButton->setToggledIconID("icon-m-music-video-shuffle-on");
    shuffleButton->setObjectName("BottomToolbarButtonShuffle");
    connect(shuffleButton, SIGNAL(clicked()), this,
            SLOT(toggleShuffleMode()));
    playcontrolPolicy->addItem(shuffleButton);

    loopButton = new MButton;
    loopButton->setCheckable(true);
    loopButton->setViewType(MButton::toggleType);
    loopButton->setIconID("icon-m-music-video-repeat-off");
    loopButton->setToggledIconID("icon-m-music-video-repeat-on");
    loopButton->setObjectName("BottomToolbarButtonRepeat");
    connect(loopButton, SIGNAL(clicked()), this, SLOT(toggleLoopMode()));
    playcontrolPolicy->addItem(loopButton);
    localQueue->SetRepeatMode(0);

    bottomToolbarLayout->setPolicy(playcontrolPolicy);
    bottomToolbar->appear();

    //bottomToolbar MOverlay end

    //  LayoutPolicy for search bottom toolbar
    searchPolicy = new MLinearLayoutPolicy(bottomToolbarLayout, Qt::Horizontal);
    searchPolicy->setSpacing(0);
    searchPolicy->setContentsMargins(0, 0, 0, 0);


    searchEdit = new MTextEdit(MTextEditModel::SingleLine, QString());
    searchEdit->setObjectName("searchEdit");
    searchPolicy->addItem(searchEdit);

    searchButton = new MButton();
    searchButton->setObjectName("searchButton");
    connect(searchButton, SIGNAL(clicked()), this, SLOT(doSearch()));
    searchPolicy->addItem(searchButton);

    // 

    mainPage = new MApplicationPage;
    mainPage->setObjectName("MainApplicationPage");
    mainPage->setEscapeMode(MApplicationPageModel::EscapeCloseWindow);
    mainPage->setPannable(false);
    mainPage->setAutoMarginsForComponentsEnabled(false);
    mainPage->setContentsMargins(0, 0, 0, 0);
    mainPage->
        setComponentsDisplayMode(MApplicationPage::AllComponents,
                                 MApplicationPageModel::Show);
    mainPage->setCentralWidget(container);

    //ApplicationMenu
    searchAction = new MAction(mainPage);
    searchAction->setLocation(MAction::ApplicationMenuLocation);
    mainPage->addAction(searchAction);
    connect(searchAction, SIGNAL(triggered()), this, SLOT(showSearchDialog()));

    newplaylistAction = new MAction(mainPage);
    newplaylistAction->setLocation(MAction::ApplicationMenuLocation);
    mainPage->addAction(newplaylistAction);

    mymusicAction = new MAction(mainPage);
    mymusicAction->setLocation(MAction::ApplicationMenuLocation);
    mainPage->addAction(mymusicAction);
    connect(mymusicAction, SIGNAL(triggered()), this, SLOT(showAllArtist()));

    playlistsAction = new MAction(mainPage);
    playlistsAction->setLocation(MAction::ApplicationMenuLocation);
    mainPage->addAction(playlistsAction);
    connect(playlistsAction, SIGNAL(triggered()), this, SLOT(showPlaylists()));

    qDebug() << "AppWindow::localeChanged";
    localeChanged();
    //end ApplicationMenu
    showAllArtist();
}

AppWindow::~AppWindow()
{
    if (contentAllArtist != NULL)
        static_cast < QGraphicsWidget * >(contentAllArtist)->deleteLater();
}

AppWindow *AppWindow::instance()
{
    return mainWindowInstance;
}

void AppWindow::stop()
{
    userInitiatedStop = true;
    localQueue->Stop();
}

void AppWindow::addToQueue(QStringList uris, QStringList mimeTypes)
{
    localQueue->AppendUris(uris.count(), uris, mimeTypes);
}

void AppWindow::removeFromQueue(int index, int count)
{
    localQueue->RemoveRange(index, count);
}

void AppWindow::playSongs(QStringList uris, QStringList mimeTypes)
{
    if (isPlaying == true)
        localQueue->Stop();
    clearQueue();
    localQueue->AppendUris(uris.count(), uris, mimeTypes);
    localQueue->Play();
}

// to be called from 'now playing', playlist, album page
// if the song is in queue, jump to it, play
// if not in queue, append it, play
void AppWindow::play(QString uri, QString mimeType)
{
    if (uri == "") {
        playAll();
        return;
    }

    int index;
    // do not clear queue
    index = loadedUris.indexOf(uri);
    if (index == -1) {
        // something wrong with loadedUris, continue, pretending nothing wrong
        qWarning() << "AppWindow::Play()";
        index = loadedUris.count();
        localQueue->AppendUris(1, QStringList() << uri,
                               QStringList() << mimeType);
    }
    if (index == currentIndex) {
        // continue playing
        if (isPlaying == false)
            localQueue->Play();
        return;
    } else {
        localQueue->SetIndex(index);
        localQueue->Play();
    }
}

int cpr(const QStringList & a, const QStringList & b)
{
    int z = QString::compare(a[0], b[0], Qt::CaseInsensitive);
    if (z < 0)
        return 1;
    else
        return 0;
}

void AppWindow::playAll()
{
    Album *album = NULL;
    QVector < QStringList > results;

    qDebug() << "AppWindow::playAll appState" << appState;

    if (isPlaying == true) {
        stop();
        return;
    } else {
        if(loadedUris.count()>0) {
        // playqueue is not empty, start to play
            localQueue->Play();
            return;
        }
    }

// playqueue is empty, play everything in this view from top.
    switch (appState) {
    case StateAllArtist:
        {
            QVector<QStringList> artists_result;
            SearchEngine::getAllMusicArtists(artists_result);

            // unknown artist will be list 1st, if any
            if(((artists_result.value(0))[1]).isEmpty()) {
                // move it the end of list
                QStringList unknownArtist;
                unknownArtist.append("unknown");
                unknownArtist.append("unknown");

                //remove the first empty one, and add unknown one
                artists_result.remove(0);
                artists_result.append(unknownArtist);
            }
            for (QVector < QStringList >::iterator i = artists_result.begin();
                i != artists_result.end(); i++) {
                // no artist name => use urn name
                if ((*i)[0].isEmpty()) {
                    (*i)[0] = (*i)[1];
                    (*i)[0].remove(QString("urn:artist:"));
                }
            }
            // sort again for CaseInsensitive
            std::sort(artists_result.begin(), artists_result.end(), cpr);

            for (int k = 0; k < artists_result.count(); k++) {
                qDebug() << "AppWindow:artist:" << k << artists_result[k][0] << artists_result[k][1];
                QString artist = artists_result[k][0];
                QString artist_performer = artists_result[k][1];
                QVector<QStringList> tmp_result;
                SearchEngine::getAllSongsForArtist(artist_performer, tmp_result);
                results += tmp_result;
            }
        }
        break;
    case StateArtist:
// load playqueue with all music from current artist
        {
            qDebug() << "AppWindow::artistName" << contentArtist->nmmArtistName();
            results =::tracker()->rawSparqlQuery(QString(SONGS_FOR_ARTIST).
                                                 arg(contentArtist->
                                                     nmmPerformer()));
        }
        break;

    case StateAlbum:
// load it with all music associated with the album
        {
            album = static_cast < Album * >(content);

            qDebug() << "AppWindow::album: " << album->nmmArtistName() <<
                album->nmmAlbumTitle();

            QAbstractTableModel *model = album->getModel();
            int count = model->rowCount();

            QStringList uris;
            QStringList mimeTypes;
            for (int i = 0; i < count; i++) {
                uris << (model->index(i, 0)).data(Qt::UserRole).toString();
                mimeTypes << (model->index(i, 0)).data(Qt::UserRole +
                                                       1).toString();
            }
            localQueue->AppendUris(uris.count(), uris, mimeTypes);
            localQueue->Play();

            return;
        }
    case StateNowPlaying:
        // do nothing
        return;
    case StatePlaylists:
        {
            Playlists *plist = static_cast < Playlists *>(content);
            qDebug() << "AppWindow::Playall:StatePlaylists";
            QAbstractTableModel *model = plist->getModel();
            int count = model->rowCount();
            for (int i = 0; i < count; i++) {
                qDebug() << i << model->index(i, 0).data(Qt::DisplayRole).toString();
                loadPlaylist(model->index(i, 0).data(Qt::DisplayRole).toString());
            }
            localQueue->Play();
            return;
        }
    case StatePlaylist:
        {
        Playlist *plist = static_cast < Playlist *>(content);
        const QStringList *urls = plist->getUrls();
        QAbstractTableModel *p_model = plist->getModel();

        int count = urls->size();
        QStringList uris;
        QStringList mimeTypes;
        for (int i = 0; i < count; i++) {
            qDebug() << "AppWindow::playlist" << i << urls->at(i) << (p_model->index(i, 0)).data(Qt::UserRole+1).toString();
            uris << urls->at(i);
            mimeTypes << (p_model->index(i, 0)).data(Qt::UserRole+1).toString();
        }
        localQueue->AppendUris(count, *urls, mimeTypes);
        localQueue->Play();
        return;
        }
    case StateSearch:
    default:
        localQueue->Play();
        return;
    }

    QStringList uris;
    QStringList mimeTypes;
    for (QVector < QStringList >::iterator iter = results.begin();
         iter != results.end(); iter++) {
        QStringList data = *iter;
        uris << data[0];
        mimeTypes << data[1];
    }

    localQueue->AppendUris(uris.count(), uris, mimeTypes);
    localQueue->Play();
}

void AppWindow::showAlbum(QString nmmArtistName, QString nmmPerformer,
                          QString nmmMusicAlbum, QString nmmAlbumTitle)
{
    qDebug() << "AppWindow::showAlbum : " << nmmArtistName << nmmPerformer
        << nmmMusicAlbum << nmmAlbumTitle;

    if (appState == StateAlbum)
        return;

    //TODO: switch content
    artistPolicy->removeAt(1);

    if (contentAlbum)
        static_cast < QGraphicsWidget * >(contentAlbum)->deleteLater();

    contentAlbum =
        new Album(nmmArtistName, nmmPerformer, nmmMusicAlbum,
                  nmmAlbumTitle);
    artistPolicy->addItem(contentAlbum);
    content = contentAlbum;

    crumbbar->addAlbumTitle(nmmAlbumTitle);

    setState(StateAlbum);

    disconnect(artistPage, SIGNAL(backButtonClicked()), this, SLOT(showAllArtist()));
    connect(artistPage, SIGNAL(backButtonClicked()), this, SLOT(backToArtist()));
    return;

#if 0
    //show albumPage
    if (albumPage == NULL) {
        MWidgetController *albumContainer = new MWidgetController;
        albumContainer->setViewType("background");
        albumContainer->setObjectName("ContentContainer");
        albumContainer->setContentsMargins(0, 0, 0, 0);

        QGraphicsLinearLayout *albumLayout =
            new QGraphicsLinearLayout(Qt::Vertical, albumContainer);
        albumLayout->setContentsMargins(0, 0, 0, 0);

        MWidgetController *topToolbarHolder = createTopToolbarHolder();
        albumLayout->addItem(topToolbarHolder);

        crumbbar =
            new CrumbBar(nmmArtistName, nmmPerformer, nmmMusicAlbum,
                         nmmAlbumTitle);
        albumLayout->addItem(crumbbar);

        contentAlbum =
            new Album(nmmArtistName, nmmPerformer, nmmMusicAlbum,
                      nmmAlbumTitle);
        albumLayout->addItem(contentAlbum);
        content = contentAlbum;
        qDebug() << "AppWindow::showAlbum : " << contentAlbum->
            nmmPerformer();

        albumPage = new MApplicationPage;
        //% "MeeGo Music!"
//        albumPage->setTitle(qtTrId("meego_music_album_meego_music"));
        albumPage->setObjectName("MainApplicationPage");
        albumPage->setPannable(false);
        albumPage->setAutoMarginsForComponentsEnabled(false);
        albumPage->setContentsMargins(0, 0, 0, 0);
        albumPage->
            setComponentsDisplayMode(MApplicationPage::
                                     NavigationBar | MApplicationPage::
                                     EscapeButton,
                                     MApplicationPageModel::Hide);
        albumPage->setCentralWidget(albumContainer);
    }

    albumPage->appear(MSceneWindow::DestroyWhenDismissed);

    setState(StateAlbum);

    return;
    //show albumPage end
#endif
}

void AppWindow::showArtist(QString nmmArtistName, QString nmmPerformer)
{
    qDebug() << "AppWindow::showArtist : " << nmmArtistName <<
        nmmPerformer;

    if (appState == StateArtist)
        return;

    //show artistPage
    if (artistPage == NULL) {
        MWidgetController *artistContainer = new MWidgetController;
        artistContainer->setViewType("background");
        artistContainer->setObjectName("ContentContainer");
        artistContainer->setContentsMargins(0, 0, 0, 0);

        artistLayout = new MLayout(artistContainer);
        ArtistLayoutAnimation *animation =
            new ArtistLayoutAnimation(artistLayout);

        artistPolicy = new MLinearLayoutPolicy(artistLayout, Qt::Vertical);
        artistPolicy->setSpacing(0);
        artistPolicy->setContentsMargins(0, 0, 0, 0);

        crumbbar = new CrumbBar(nmmArtistName, nmmPerformer);
        artistPolicy->addItem(crumbbar);

        contentArtist = new ArtistList(nmmArtistName, nmmPerformer);
        artistPolicy->addItem(contentArtist);
        content = contentArtist;

        artistPage = new MApplicationPage;
        artistPage->setTitle(qtTrId("meego_music_title"));
        artistPage->setObjectName("MainApplicationPage");
        artistPage->setEscapeMode(MApplicationPageModel::EscapeManualBack);
        artistPage->setPannable(false);
        artistPage->setAutoMarginsForComponentsEnabled(false);
        artistPage->setContentsMargins(0, 0, 0, 0);
        artistPage->
            setComponentsDisplayMode(MApplicationPage::AllComponents,
                                     MApplicationPageModel::Show);
        artistPage->setCentralWidget(artistContainer);

//        connect(artistPage, SIGNAL(backButtonClicked()), this, SLOT(showAllArtist()));
    }

    disconnect(artistPage, SIGNAL(backButtonClicked()), this, SLOT(backToArtist()));
    connect(artistPage, SIGNAL(backButtonClicked()), this, SLOT(showAllArtist()));

    if (appState == StateAlbum) {
        qDebug() << "appState == StateAlbum";
        crumbbar->removeAlbumTitle();

        artistPolicy->removeAt(1);
//        static_cast<QGraphicsWidget *>(contentAlbum)->deleteLater();
        artistPolicy->addItem(contentArtist);

        content = contentArtist;
    } else
        artistPage->appear(MSceneWindow::DestroyWhenDismissed);

    setState(StateArtist);

    if (albumPage != NULL) {
        albumPage->dismiss();
        albumPage = NULL;
    }

    return;
    //show artistPage end
}

void AppWindow::backToArtist()
{
    QString empty;

    showArtist(empty, empty);
}

void AppWindow::showNowPlaying()
{
    qDebug() << "AppWindow::showNowPlaying";

    if (appState == StateNowPlaying)
        return;

    if (loadedUris.count() == 0)
        return;

    origState = appState;

    //show nowplayingPage
    if (nowplayingPage == NULL) {
        MWidgetController *nowplayingContainer = new MWidgetController;
        nowplayingContainer->setViewType("background");
        nowplayingContainer->setObjectName("ContentContainer");
        nowplayingContainer->setContentsMargins(0, 0, 0, 0);

        QGraphicsLinearLayout *nowplayingLayout =
            new QGraphicsLinearLayout(Qt::Vertical, nowplayingContainer);
        nowplayingLayout->setContentsMargins(0, 0, 0, 0);

        CrumbBar *crumbbar = new CrumbBar("Now Playing", "");
        nowplayingLayout->addItem(crumbbar);

        NowPlaying *nowContent = new NowPlaying(queue(), crumbbar);
        nowplayingLayout->addItem(nowContent);
        content = nowContent;

        nowplayingPage = new MApplicationPage;
        nowplayingPage->setTitle(qtTrId("meego_music_title"));
        nowplayingPage->setObjectName("MainApplicationPage");
        nowplayingPage->setEscapeMode(MApplicationPageModel::EscapeManualBack);
        nowplayingPage->setPannable(false);
        nowplayingPage->setAutoMarginsForComponentsEnabled(false);
        nowplayingPage->setContentsMargins(0, 0, 0, 0);
        nowplayingPage->
            setComponentsDisplayMode(MApplicationPage::AllComponents,
                                     MApplicationPageModel::Show);
        nowplayingPage->setCentralWidget(nowplayingContainer);

        connect(nowplayingPage, SIGNAL(backButtonClicked()), this, SLOT(showAllMusic()));
    }

    nowplayingPage->appear(MSceneWindow::DestroyWhenDismissed);

    setState(StateNowPlaying);

    return;
    //show nowplayingPage end
}

void AppWindow::playPrevious()
{
    qDebug() << "AppWindow::playPrevious";
    backButton->setChecked(!backButton->isChecked());
    localQueue->Previous();
}

void AppWindow::playNext()
{
    qDebug() << "AppWindow::playNext" << forwardButton->isChecked();
    forwardButton->setChecked(!forwardButton->isChecked());
    localQueue->Next();
}

void AppWindow::updateButton()
{
    qDebug() << "AppWindow::updateButton in" << forwardButton->isChecked() << currentIndex << isPlaying;
    // get next song's metadata
    QDBusPendingReply<QString, QString, QString> nxt_reply = localQueue->GetNextMetadata();
    QDBusPendingCallWatcher *nxt_watcher = new QDBusPendingCallWatcher(nxt_reply);
    connect(nxt_watcher, SIGNAL(finished(QDBusPendingCallWatcher *)), this, SLOT(getNextMetadataFinished(QDBusPendingCallWatcher *)));

    // only one song is a special case
    if ((loadedUris.count() == 1) || (isPlaying ==0)) {
        // disable the 'previous' button
        backButton->setEnabled(false);
        backButton->setChecked(true);
        // disable the 'next' button
        forwardButton->setEnabled(false);
        forwardButton->setChecked(true);
        return;
    }
    if ((currentIndex == 0) && (!loopMode) && (!shuffleMode)) {
        // disable the 'previous' button
        backButton->setEnabled(false);
        backButton->setChecked(true);
    } else {
        // enable the 'previous' button
        backButton->setEnabled(true);
        backButton->setChecked(false);
    }

    if ((currentIndex >= loadedUris.count() - 1) && (!loopMode)
        && (!shuffleMode)) {
        // disable the 'next' button
        forwardButton->setEnabled(false);
        forwardButton->setChecked(true);
    } else {
        // enable the 'next' button
        forwardButton->setEnabled(true);
        forwardButton->setChecked(false);
    }
}

void AppWindow::indexChanged(int index)
{
    qDebug() << "AppWindow::indexChanged" << index;
    QString
        playing_str
        ("<style type=\"text/css\">span.title {color:#ffffff} span.artist {color:#aaaaaa}</style><span class=\"title\">");
    qDebug() << "AppWindow::indexChanged : " << index;
    seekBar->setValue(0);

    if (index < 0) {
        // Using BognorRegis v0.6.6, I sometimes see a current index of -1 when hornsey
        // is also updating the local queue
        qDebug() << "Invalid index: " << index;
        return;
    }

    // get current song's duration
    QDBusPendingReply<int> getModeReply = localQueue->GetDuration();
    QDBusPendingCallWatcher *getModeWatcher = new QDBusPendingCallWatcher(getModeReply);
    connect(getModeWatcher, SIGNAL(finished(QDBusPendingCallWatcher *)), this, SLOT(getDurationFinished(QDBusPendingCallWatcher *)));

    // get current song's metadata
    QDBusPendingReply<QString, QString, QString> idx_reply = localQueue->GetIndexMetadata(index);
    QDBusPendingCallWatcher *idx_watcher = new QDBusPendingCallWatcher(idx_reply);
    connect(idx_watcher, SIGNAL(finished(QDBusPendingCallWatcher *)), this, SLOT(getIndexMetadataFinished(QDBusPendingCallWatcher *)));

    currentIndex = index;
    updateButton();

    if (currentIndex > loadedUris.count() - 1) {
        qDebug() <<
            "WARNING: the loadedUris is being updated after the changed index!";
        return;
    }

    emit queueIndexChanged(index);
#if 0
    if (appState == StateNowPlaying) {
        NowPlaying *nowPlaying = static_cast < NowPlaying * >(content);
        QAbstractTableModel *model = nowPlaying->getModel();
        QString songName =
            (model->index(index, 0)).data(Qt::DisplayRole).toString();

        if (songName.isNull()) {

            QString songname =
                QUrl(loadedUris[currentIndex]).path().split("/").last();
            playing_str.append(songname);
            playing_str.
                append
                ("</span><span class=\"artist\">   Etta James</span>");
            nowPlayingButton->setText(playing_str);
//            nowPlayingButton->setText(QUrl(loadedUris[currentIndex]).path().split("/").last());
        } else {
            playing_str.append(songName);
            playing_str.
                append
                ("</span><span class=\"artist\">   Etta James</span>");
            nowPlayingButton->setText(playing_str);
//            nowPlayingButton->setText(songName);
        }

        qDebug() << "AppWindow::indexChanged : return";
        return;
    }

    QVector < QStringList >
        results =::tracker()->rawSparqlQuery(QString(SONG_TITLE).
                                             arg(loadedUris
                                                 [currentIndex]));
    for (QVector < QStringList >::iterator iter = results.begin();
         iter != results.end(); iter++) {
        QStringList data = *iter;
        playing_str.append(data[0]);
        playing_str.
            append("</span><span class=\"artist\">   Etta James</span>");
        nowPlayingButton->setText(playing_str);
        // nowPlayingButton->setText(data[0]);
        return;
    }

    // If tracker doesn't have a title for us then default to the filename
    QString songname =
        QUrl(loadedUris[currentIndex]).path().split("/").last();
    playing_str.append(songname);
    playing_str.
        append("</span><span class=\"artist\">   Etta James</span>");
    nowPlayingButton->setText(playing_str);
#endif
//    nowPlayingButton->setText(QUrl(loadedUris[currentIndex]).path().split("/").last());
}

void AppWindow::itemMoved(int old_position, int new_position,
                          int updated_index)
{
    qDebug() << "AppWindow::itemMoved : old_position = " << old_position <<
        "; new_position = " << new_position <<
        "; updated_index = " << updated_index;

    if (new_position < loadedUris.count()) {
        loadedUris.move(old_position, new_position);
        currentIndex = updated_index;
    }
}

void AppWindow::stateChanged(int state)
{
    qDebug() << "AppWindow::stateChanged : " << state;
    if (state == 0) {
        // playing -> stopped 
        playButton->setChecked(false);
        backButton->setEnabled(false);
        backButton->setChecked(true);
        forwardButton->setEnabled(false);
        forwardButton->setChecked(true);
        isPlaying = false;
        if ((userInitiatedStop == false) && (loopMode == true)) {
            qDebug() << "AppWindow::stateChanged : loopMode";
            localQueue->Play();
        }
        userInitiatedStop = false;
    } else {
        // Stopped -> playing
        playButton->setChecked(true);
        isPlaying = true;
        updateButton();
    }
}

void AppWindow::uriAdded(const QString & uri, int position)
{
    qDebug() << "AppWindow::uriAdded : uri = " << uri << "; position = " <<
        position;
    loadedUris << uri;
    updateButton();
}

void AppWindow::uriRemoved(const QString & uri, int position)
{
    qDebug() << "AppWindow::uriRemoved : uri = " << uri << "; position = "
        << position;
    if (position < loadedUris.count()) {
        loadedUris.removeAt(position);

        emit queueUriRemoved(position);
    }
    updateButton();
}

void AppWindow::listUrisFinished(QDBusPendingCallWatcher * watcher)
{
    qDebug() << "AppWindow::listUrisFinished : " << watcher;

    QDBusPendingReply < QStringList > reply = *watcher;

    loadedUris = reply.value();

    QDBusPendingReply < int >getIndexReply = localQueue->GetIndex();
    QDBusPendingCallWatcher *getIndexWatcher =
        new QDBusPendingCallWatcher(getIndexReply);
    connect(getIndexWatcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
            this, SLOT(getIndexFinished(QDBusPendingCallWatcher *)));

    QDBusPendingReply < int >getStateReply = localQueue->GetState();
    QDBusPendingCallWatcher *getStateWatcher =
        new QDBusPendingCallWatcher(getStateReply);
    connect(getStateWatcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
            this, SLOT(getStateFinished(QDBusPendingCallWatcher *)));
}

void AppWindow::getIndexFinished(QDBusPendingCallWatcher * watcher)
{
    qDebug() << "AppWindow::getIndexFinished : " << watcher;

    QDBusPendingReply < int >reply = *watcher;
    indexChanged(reply.value());
}

void AppWindow::getStateFinished(QDBusPendingCallWatcher * watcher)
{
    qDebug() << "AppWindow::getStateFinished : " << watcher;

    QDBusPendingReply < int >reply = *watcher;
    stateChanged(reply.value());
}

void AppWindow::getIndexMetadataFinished(QDBusPendingCallWatcher *watcher)
{
    QString playing_str("<style type=\"text/css\">span.title {color:#ffffff} span.artist {color:#aaaaaa}</style><span class=\"title\">");
    qDebug() << "AppWindow::getIndexMetadataFinished : ";

    QDBusPendingReply<QString, QString, QString> reply = *watcher;
    if (!reply.isError()) {
        playing_str.append(reply.argumentAt<0>());
        playing_str.append("</span><span class=\"artist\">   ");
        playing_str.append(reply.argumentAt<1>());
        playing_str.append("</span>");
        nowPlayingButton->setText(playing_str);

        qDebug() << reply.argumentAt<0>() << ":" << reply.argumentAt<1>() << reply.argumentAt<2>();       
    }
}

void AppWindow::getNextMetadataFinished(QDBusPendingCallWatcher *watcher)
{
    qDebug() << "AppWindow::getNextMetadataFinished : ";

    QDBusPendingReply<QString, QString, QString> reply = *watcher;
    if (!reply.isError()) {
        nextPlayingButton->setText(reply.argumentAt<0>());
        qDebug() << reply.argumentAt<0>() << ":" << reply.argumentAt<1>() << reply.argumentAt<2>();       
    } else {
        nextPlayingButton->setText("");
    }
}

void AppWindow::getDurationFinished(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<int> reply = *watcher;

    currentDuration = reply.value();
    if (currentDuration != 0) {
        qDebug() << "AppWindow:: getDuration: " << currentDuration;
        timeButton->setText(QString().sprintf("%2d:%02d", ((currentDuration-1)/60), ((currentDuration-1)%60)));
    } else {
        // gstreamer not ready, call again
        QDBusPendingReply<int> getModeReply = localQueue->GetDuration();
        QDBusPendingCallWatcher *getModeWatcher = new QDBusPendingCallWatcher(getModeReply);
        connect(getModeWatcher, SIGNAL(finished(QDBusPendingCallWatcher *)), this, SLOT(getDurationFinished(QDBusPendingCallWatcher *)));
    }
}

void AppWindow::showPlaylists()
{
    qDebug() << "AppWindow::showPlaylists ";

    if (appState == StatePlaylists)
        return;

    if (playlistsPage == NULL) {
        MWidgetController *playlistContainer = new MWidgetController;
        playlistContainer->setViewType("background");
        playlistContainer->setObjectName("ContentContainer");
        playlistContainer->setContentsMargins(0, 0, 0, 0);

        MLayout *playlistLayout = new MLayout(playlistContainer);
        MLinearLayoutPolicy *playlistPolicy =
            new MLinearLayoutPolicy(playlistLayout, Qt::Vertical);
        playlistPolicy->setSpacing(0);
        playlistPolicy->setContentsMargins(0, 0, 0, 0);

        contentPlaylists = new Playlists();
        content = contentPlaylists;

        AlphabetBar *bar = new AlphabetBar(contentPlaylists->getList());

        playlistPolicy->addItem(bar);
        playlistPolicy->addItem(contentPlaylists);

        playlistsPage = new MApplicationPage;
        playlistsPage->setTitle(qtTrId("meego_music_title"));
        playlistsPage->setObjectName("MainApplicationPage");
        playlistsPage->setEscapeMode(MApplicationPageModel::EscapeCloseWindow);
        playlistsPage->setPannable(false);
        playlistsPage->setAutoMarginsForComponentsEnabled(false);
        playlistsPage->setContentsMargins(0, 0, 0, 0);
        playlistsPage->
            setComponentsDisplayMode(MApplicationPage::AllComponents,
                                     MApplicationPageModel::Show);
        playlistsPage->setCentralWidget(playlistContainer);

        playlistsPage->addAction(searchAction);
        playlistsPage->addAction(newplaylistAction);
        playlistsPage->addAction(mymusicAction);
        playlistsPage->addAction(playlistsAction);

        newplaylistAction->setEnabled(true);
        connect(newplaylistAction, SIGNAL(triggered()),
                contentPlaylists, SLOT(newPlaylistFromObjectMenu()));
    }

    playlistsPage->appear(MSceneWindow::DestroyWhenDismissed);

    if (playlistPage != NULL) {
        playlistPage->dismiss();
        playlistPage = NULL;

        contentPlaylists->uris.clear();
    }

    setState(StatePlaylists);
}

void AppWindow::showPlaylist(const QString & playlistName, const QStringList & uris)
{
    qDebug() << "AppWindow::showPlaylist" << uris;

    if (appState == StatePlaylist)
        return;

    if (playlistPage == NULL) {
        MWidgetController *playlistContainer = new MWidgetController;
        playlistContainer->setViewType("background");
        playlistContainer->setObjectName("ContentContainer");
        playlistContainer->setContentsMargins(0, 0, 0, 0);

        QGraphicsLinearLayout *playlistLayout =
            new QGraphicsLinearLayout(Qt::Vertical, playlistContainer);
        playlistLayout->setContentsMargins(0, 0, 0, 0);

        CrumbBar *crumbbar = new CrumbBar("Now Playing", "");
        crumbbar->setText(playlistName);
        playlistLayout->addItem(crumbbar);

        contentPlaylist = new Playlist(uris, crumbbar);
        playlistLayout->addItem(contentPlaylist);
        content = contentPlaylist;

        playlistPage = new MApplicationPage;
        playlistPage->setTitle(qtTrId("meego_music_title"));
        playlistPage->setObjectName("MainApplicationPage");
        playlistPage->setEscapeMode(MApplicationPageModel::EscapeManualBack);
        playlistPage->setPannable(false);
        playlistPage->setAutoMarginsForComponentsEnabled(false);
        playlistPage->setContentsMargins(0, 0, 0, 0);
        playlistPage->
            setComponentsDisplayMode(MApplicationPage::AllComponents,
                                     MApplicationPageModel::Show);
        playlistPage->setCentralWidget(playlistContainer);

        connect(playlistPage, SIGNAL(backButtonClicked()), this, SLOT(showPlaylists()));

    }

    playlistPage->appear(MSceneWindow::DestroyWhenDismissed);

    setState(StatePlaylist);

    return;
}

void AppWindow::showAllMusic()
{
    if (appState == StatePlaylists) {
        showAllArtist();
    }

    if (appState == StateNowPlaying || appState == StateSearch) {
        if (origState == StateAlbum || origState == StateArtist) {
            qDebug() << "1";
            artistPage->appear(MSceneWindow::DestroyWhenDismissed);
        } else if (origState == StateAllArtist) {
            qDebug() << "2";
            mainPage->appear();
        } else if (origState == StatePlaylists)
            playlistsPage->appear();
        else if (origState == StatePlaylist)
            playlistPage->appear();

        qDebug() << "3";
        if (nowplayingPage != NULL) {
            nowplayingPage->dismiss();
            nowplayingPage = NULL;
        }

        if (searchPage != NULL) {
            searchPage->dismiss();
            searchPage = NULL;

            bottomToolbarLayout->setPolicy(playcontrolPolicy);
        }

        setState(origState);
    }
}

void AppWindow::showAllArtist()
{
    if (appState == StateAllArtist)
        return;

    setState(StateAllArtist);

    mainPage->appear();

    if (artistPage != NULL) {
        artistPage->dismiss();
        artistPage = NULL;
        contentAlbum = NULL;
    }

    if (albumPage != NULL) {
        albumPage->dismiss();
        albumPage = NULL;
    }

    if (nowplayingPage != NULL) {
        nowplayingPage->dismiss();
        nowplayingPage = NULL;
    }

    if (playlistsPage != NULL) {
        playlistsPage->dismiss();
        playlistsPage = NULL;
    }

    return;
}

void AppWindow::showSearch()
{
    qDebug() << "AppWindow::showSearch";

    if (appState == StateSearch)
        return;

    origState = appState;

    if (searchPage == NULL) {
        MWidgetController *searchContainer = new MWidgetController;
        searchContainer->setViewType("background");
        searchContainer->setObjectName("ContentContainer");
        searchContainer->setContentsMargins(0, 0, 0, 0);

        MLayout *searchLayout = new MLayout(searchContainer);
        MLinearLayoutPolicy *searchPolicy =
            new MLinearLayoutPolicy(searchLayout, Qt::Vertical);
        searchPolicy->setSpacing(0);
        searchPolicy->setContentsMargins(0, 0, 0, 0);

        SearchBar *searchBar = new SearchBar;
        connect(searchBar, SIGNAL(filter(int)), this, SLOT(updateSearchType(int)));
        searchPolicy->addItem(searchBar);

        content = static_cast <QGraphicsLayoutItem *>(new SearchResults);
        searchPolicy->addItem(content);

        searchPage = new MApplicationPage;
        searchPage->setTitle(qtTrId("meego_music_title"));
        searchPage->setObjectName("MainApplicationPage");
        searchPage->setEscapeMode(MApplicationPageModel::EscapeManualBack);
        searchPage->setPannable(false);
        searchPage->setAutoMarginsForComponentsEnabled(false);
        searchPage->setContentsMargins(0, 0, 0, 0);
        searchPage->
            setComponentsDisplayMode(MApplicationPage::AllComponents,
                                     MApplicationPageModel::Show);
        searchPage->setCentralWidget(searchContainer);

        connect(searchPage, SIGNAL(backButtonClicked()), this, SLOT(showAllMusic()));
    }
    searchPage->appear(MSceneWindow::DestroyWhenDismissed);

    setState(StateSearch);
//    doSearch();
}

void AppWindow::positionChanged(double position)
{
    if (seekBarIsPressed == false) {
        seekBar->setValue((int) (position * 100));
    }
    int i = currentDuration - (currentDuration * position);
    timeButton->setText(QString().sprintf("%2d:%02d", (i/60), (i%60)));
}

void AppWindow::sliderPressed()
{
    qDebug() << "AppWindow::sliderReleased : " << seekBar->value();
    seekBarIsPressed = true;
}

void AppWindow::sliderReleased()
{
    qDebug() << "AppWindow::sliderReleased : " << seekBar->value();

    double p = seekBar->value()? (double) (seekBar->value()) / 100 : 0.0;
    localQueue->SetPosition(p);
    seekBarIsPressed = false;
}

void AppWindow::clearQueue()
{
    localQueue->RemoveRange(0, loadedUris.count());
    nowPlayingButton->setText(" ");
    seekBar->setValue(0);
}

void AppWindow::toggleShuffleMode()
{
    int mode = 0;
    if (loopMode) {
        mode = BOGNOR_QUEUE_MODE_REPEATING;
    } else {
        mode = BOGNOR_QUEUE_MODE_NORMAL;
    }
    if (shuffleMode) {
        shuffleMode = false;
    } else {
        shuffleMode = true;
        mode |= BOGNOR_QUEUE_MODE_SHUFFLE;
    }
    qDebug() << "AppWindow::set_play_mode: " << mode << currentIndex;
    localQueue->SetRepeatMode(mode);
    updateButton();
}

void AppWindow::toggleLoopMode()
{
    int mode = 0;
    if (loopMode) {
        loopMode = false;
        mode = BOGNOR_QUEUE_MODE_NORMAL;
    } else {
        loopMode = true;
        mode = BOGNOR_QUEUE_MODE_REPEATING;
    }
    if (shuffleMode) {
        mode |= BOGNOR_QUEUE_MODE_SHUFFLE;
    } else {
    }

    qDebug() << "AppWindow::set_play_mode: " << mode << currentIndex;
    localQueue->SetRepeatMode(mode);
    updateButton();
}

void AppWindow::setState(AppState state)
{
    QString boldText;

    if (state == StateAllArtist) {
        boldText = "<b>" + mymusicAction->text().remove("<b>").remove("</b>") + "</b>";
        mymusicAction->setText(boldText);

        playlistsAction->setText(playlistsAction->text().remove("<b>").remove("</b>"));

        newplaylistAction->setEnabled(false);
    } else if (state == StatePlaylists) {
        boldText = "<b>" + playlistsAction->text().remove("<b>").remove("</b>") + "</b>";
        playlistsAction->setText(boldText);

        mymusicAction->setText(mymusicAction->text().remove("<b>").remove("</b>"));

        newplaylistAction->setEnabled(true);
    }

    appState = state;
}

void AppWindow::addToPlaylist(QVector < QStringList > &items)
{
    qDebug() << "AppWindow::addToPlaylist : " << items;

    //% "Select a Playlist"
    dialog = new MDialog(qtTrId("meego_music_select_a_playlist"), 0);
    dialog->setObjectName("DialogChoosePlaylist");
    MList *list = new MList();
    ArtistItemCreator *itemCreator = new ArtistItemCreator();
    list->setCellCreator(itemCreator);
    PlaylistsModel *model = new PlaylistsModel();
    list->setItemModel(model);

    connect(list, SIGNAL(itemClicked(QModelIndex)), this,
            SLOT(playlistSelected(QModelIndex)));

    dialog->setCentralWidget(list);


    if (QMessageBox::Cancel != dialog->exec()) {
        qDebug() << "Adding " << items << " to " << requestedPlaylist;

        SearchEngine::addPlaylistItems(requestedPlaylist, items);
    }

}

void AppWindow::playlistSelected(QModelIndex index)
{
    requestedPlaylist = index.data(Qt::DisplayRole).toString();
    qDebug() << "AppWindow::playlistSelected" << requestedPlaylist;

    dialog->accept();
}

void AppWindow::playSearchResults(QVector <QStringList> results)
{
    qDebug() << "AppWindow::playSearchResults" << ":" << results;
    if (isPlaying == true)
        localQueue->Stop();
    clearQueue();
    loadSearchResults(results);
    localQueue->Play();
}

void AppWindow::loadSearchResults(QVector <QStringList> results)
{
    qDebug() << "AppWindow::loadSearchResults" << ":" << results;

    QStringList uris;
    QStringList mimeTypes;
    for (QVector < QStringList >::iterator iter = results.begin();
        iter != results.end(); iter++) {
        QStringList data = *iter;
        uris << data[0];
        mimeTypes << QString();  // add fake/empty mimetypes
    }
    qDebug() << "AppWindow::loadsearchResults URIs : " << uris;
    localQueue->AppendUris(uris.count(), uris, mimeTypes);
}

void AppWindow::playPlaylist(QString playlistName)
{
    qDebug() << "AppWindow::playPlaylist " << playlistName;
    if (isPlaying == true)
        localQueue->Stop();
    clearQueue();
    loadPlaylist(playlistName);
    localQueue->Play();
}

void AppWindow::loadPlaylist(QString playlistName)
{
    QVector < QStringList > items;
    SearchEngine::getPlaylistItems(playlistName, items);
    qDebug() << "AppWindow::loadPlaylist " << playlistName << " : " << items;

    QStringList uris;
    QStringList mimeTypes;
    for (QVector < QStringList >::iterator iter = items.begin();
        iter != items.end(); iter++) {
        QStringList data = *iter;
        uris << data[0];
        mimeTypes << QString();  // add fake/empty mimetypes
    }
    qDebug() << "AppWindow::loadPlaylist URIs : " << uris;
    localQueue->AppendUris(uris.count(), uris, mimeTypes);
}

void AppWindow::doSearch()
{
    searchString = searchEdit->text();

    qDebug() << "AppWindow::doSearch" << searchString << searchType;

//    if (searchEntry->text().isEmpty())
//        return;

    SearchResults *widget = static_cast < SearchResults * >(content);

    if (searchType == SearchArtist)
        return widget->showArtist(searchString);

    if (searchType == SearchAlbums)
        return widget->showAlbums(searchString);

    if (searchType == SearchSongs)
        return widget->showSongs(searchString);

    return widget->showAll(searchString);

}

void AppWindow::updateSearchType(int type)
{
    qDebug() << "AppWindow::updateSearchType " << type;
    searchType = (SearchType) type;
    doSearch();
}

void AppWindow::showSearchDialog()
{
    showSearch();
    bottomToolbarLayout->setPolicy(searchPolicy);
    searchEdit->clear();

    searchEdit->setFocus();
//    QEvent event(QEvent::RequestSoftwareInputPanel);
//    QApplication::sendEvent(searchEdit, &event);

    return;

    MDialog *dialog = new MDialog("Enter the search string", M::OkButton | M::CancelButton);
    MTextEdit *searchEdit = new MTextEdit(MTextEditModel::SingleLine, QString());

    dialog->setObjectName("DialogSearch");
    dialog->setCentralWidget(searchEdit);

//    dialog->appear(MSceneWindow::DestroyWhenDone);
    if (QMessageBox::Cancel != dialog->exec()) {
        qDebug() << "Search" << searchEdit->text();
        if(searchString.isEmpty())
            qDebug() << "searchString is" << searchString;
        searchString = searchEdit->text();

        showSearch();

        doSearch();
    }

}

MWidgetController *AppWindow::createTopToolbarHolder()
{
    MWidgetController *topToolbarHolder = new MWidgetController;
    topToolbarHolder->setViewType("background");
    topToolbarHolder->setObjectName("TopOverlayHolder");
    topToolbarHolder->setContentsMargins(0, 0, 0, 0);

    QGraphicsLinearLayout *topToolbarHolderLayout =
        new QGraphicsLinearLayout(Qt::Horizontal, topToolbarHolder);
    topToolbarHolderLayout->setSpacing(0);
    topToolbarHolderLayout->setContentsMargins(0, 0, 0, 0);
    topToolbarHolderLayout->addStretch();

    return topToolbarHolder;
}

void AppWindow::localeChanged()
{
    MLocale cur_locale;
    qDebug() << "AppWindow::localeChanged to" << cur_locale.name();
    //% "Search"
    searchAction->setText(qtTrId("meego_music_action_search"));
    //% "New Playlist"
    newplaylistAction->setText(qtTrId("meego_music_action_new_playlist"));
    //% "My Music"
    mymusicAction->setText(qtTrId("meego_music_action_my_music"));
    //% "Playlists"
    playlistsAction->setText(qtTrId("meego_music_action_playlists"));
    //% "Next:"
    nextButton->setText(qtTrId("meego_music_playbar_next"));
    //% "search"
    searchButton->setText(qtTrId("meego_music_button_search"));

    //% "Music player"
    mainPage->setTitle(qtTrId("meego_music_title"));
    if(artistPage)
        artistPage->setTitle(qtTrId("meego_music_title"));
    if(nowplayingPage)
        nowplayingPage->setTitle(qtTrId("meego_music_title"));
    if(playlistsPage)
        playlistsPage->setTitle(qtTrId("meego_music_title"));
    if(playlistPage)
        playlistPage->setTitle(qtTrId("meego_music_title"));
    if(searchPage)
        searchPage->setTitle(qtTrId("meego_music_title"));
}

void AppWindow::playAudio(const QString& audioPath)
{
    qDebug() << "AppWindow::playAudio" << audioPath;
}
