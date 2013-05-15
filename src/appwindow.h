/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <MApplicationWindow>
#include <MAction>
#include <QModelIndex>

#include "crumbbar.h"
#include "playlists.h"

class Album;
class AllArtist;
class ArtistList;
class MButton;
class MDialog;
class MLabel;
class MLayout;
class MLinearLayoutPolicy;
class MOverlay;
class MSeekBar;
class MTextEdit;
class MWidgetController;
class OrgMoblinBognorRegisQueueInterface;
class QDBusPendingCallWatcher;
class QGraphicsLayoutItem;
class Playlist;

#define PLAYLIST_SETTINGS_GROUP "MeeGoMusic_Playlists"

class M_EXPORT AppWindow:public MApplicationWindow {
    Q_OBJECT
public:
    AppWindow(QWidget * parent = 0);
    virtual ~ AppWindow();

    enum AppState {
        StateAllArtist = 0,
        StateArtist,
        StateAlbum,
        StateNowPlaying,
        StatePlaylists,
        StatePlaylist,
        StateSearch,
        StateMenuPlay,
        StateNULL
    };

    static AppWindow *instance();
    const QStringList & queue() const {
        return loadedUris;
    };
    enum AppState state() const {
        return appState;
    };

    enum SearchType {
        SearchAll = 0,
        SearchArtist,
        SearchAlbums,
        SearchSongs
    } searchType;

    void updateButton();

public slots:
    void play(QString uri = "", QString mimeType = "");
    void playSongs(QStringList uris, QStringList mimeTypes);
    void addToQueue(QStringList uris, QStringList mimeTypes);
    void removeFromQueue(int index, int count);
    void addToPlaylist(QVector < QStringList > &items);
    void showAlbum(QString nmmArtistName, QString nmmPerformer,
                   QString nmmMusicAlbum, QString nmmAlbumTitle);
    void showArtist(QString nmmArtistName, QString nmmPerformer);
    void backToArtist();
    void showNowPlaying();
    void showSearch();
    void showSearchDialog();

    void playSearchResults(QVector <QStringList> results);
    void loadSearchResults(QVector <QStringList> results);

    void showPlaylist(const QString & playlistName, const QStringList & uris);
    void loadPlaylist(QString playlist);
    void playPlaylist(QString playlist);

    void playAll();
    void playPrevious();
    void playNext();
    void stop();

    void doSearch();

    int index() {
        return currentIndex;
    };
    void localeChanged();

//    void playAudio(const QString& audioPath);

public Q_SLOTS:
    void indexChanged(int index);
    void itemMoved(int old_position, int new_position, int updated_index);
    void stateChanged(int state);
    void uriAdded(const QString & uri, int position);
    void uriRemoved(const QString & uri, int position);
    void positionChanged(double position);
    void sliderReleased();
    void sliderPressed();

    void showPlaylists();
    void showAllArtist();
    void showAllMusic();

    void playAudio(const QString& audioPath);
private Q_SLOTS:
    void listUrisFinished(QDBusPendingCallWatcher * watcher);
    void getIndexFinished(QDBusPendingCallWatcher * watcher);
    void getStateFinished(QDBusPendingCallWatcher * watcher);
    void getIndexMetadataFinished(QDBusPendingCallWatcher *watcher);
    void getNextMetadataFinished(QDBusPendingCallWatcher *watcher);
    void getDurationFinished(QDBusPendingCallWatcher *watcher);
    void toggleLoopMode();
    void toggleShuffleMode();
    void playlistSelected(QModelIndex index);
    void updateSearchType(int type);

signals:
    void queueIndexChanged(int index);
    void queueUriRemoved(int index);

private:
    enum AppState appState;

    enum _BognorQueueMode {
        BOGNOR_QUEUE_MODE_NORMAL,
        BOGNOR_QUEUE_MODE_REPEATING,
        BOGNOR_QUEUE_MODE_SINGLE_REPEATING,
        BOGNOR_QUEUE_MODE_SHUFFLE = 1 << 7,
        BOGNOR_QUEUE_LAST_MODE
    } BognorQueueMode;

    void setState(AppState state);
    void clearQueue();

    MButton *backButton;
    MButton *playButton;
    MButton *forwardButton;
    MButton *searchButton;
    MButton *nowPlayingButton;
    MButton *timeButton;
    MButton *nextButton;
    MButton *nextPlayingButton;
    MButton *shuffleButton;
    MButton *loopButton;
    bool isPlaying;
    bool userInitiatedStop;
    bool loopMode;
    bool shuffleMode;
    MSeekBar *seekBar;

    MButton *allMusicButton;
    MButton *playlistsButton;

    MAction *searchAction;
    MAction *newplaylistAction;
    MAction *mymusicAction;
    MAction *playlistsAction;

    MLayout *mainLayout;
    MLinearLayoutPolicy *policy;

    MWidgetController *container;
    MApplicationPage *mainPage;
    MApplicationPage *artistPage;
    MApplicationPage *albumPage;
    MApplicationPage *nowplayingPage;
    MApplicationPage *playlistsPage;
    MApplicationPage *playlistPage;
    MApplicationPage *searchPage;
    CrumbBar *crumbbar;

    MLayout *artistLayout;
    MLinearLayoutPolicy *artistPolicy;
    MLayout *bottomToolbarLayout;
    MLinearLayoutPolicy *playcontrolPolicy;
    MLinearLayoutPolicy *searchPolicy;

    QGraphicsLayoutItem *bar;
    AllArtist *contentAllArtist;
    ArtistList *contentArtist;
    Album *contentAlbum;
    Playlists *contentPlaylists;
    Playlist *contentPlaylist;
    QGraphicsLayoutItem *content;
    QGraphicsLayoutItem *toolbar;

    QString searchString;

    MWidgetController *bottomToolbar;

    MDialog *dialog;
    QString requestedPlaylist;

    OrgMoblinBognorRegisQueueInterface *localQueue;
    QStringList loadedUris;
    int currentIndex;
    int currentDuration;
    enum AppState origState;
    enum AppState playQueueState;
    bool seekBarIsPressed;

    MTextEdit *playlistEdit;
    MTextEdit *searchEdit;

    MWidgetController *createTopToolbarHolder();

    static AppWindow *mainWindowInstance;
};

#endif                          // APPWINDOW_H
