TARGET = meegomusic
TEMPLATE = app
target.path = /usr/bin

MOC_DIR = .moc
OBJECTS_DIR = .obj

QT += opengl dbus

CONFIG += link_pkgconfig \
    qt \
    meegotouch \
    qdbus \
    qttracker

PKGCONFIG += meegotouch

LIBS += -lqttracker

SOURCES += main.cpp \
  appwindow.cpp \
  artistlist.cpp \
  allartist.cpp \
  album.cpp \
  alphabetbar.cpp \
  crumbbar.cpp \
  nowplaying.cpp \
  playlist.cpp \
  playlists.cpp \
  searchbar.cpp \
  searchresults.cpp \
  albumitemcreator.cpp \
  albumslistmodel.cpp \
  searchengine.cpp \
  songslistmodel.cpp \
  songitemcreator.cpp \
  artistslistmodel.cpp \
  artistitemcreator.cpp \
  listwithurimodel.cpp \
  musiclayoutanimation.cpp \
  artistlayoutanimation.cpp \
  playlistsmodel.cpp \
  utils.cpp \
  listwithuri.cpp \
  $$DBUS_ADAPTOR_SOURCES

HEADERS += \
  appwindow.h \
  artistlist.h \
  artistitem.h \
  songitem.h \
  allartist.h \
  album.h \
  alphabetbar.h \
  crumbbar.h \
  nowplaying.h \
  playlists.h \
  playlist.h \
  searchbar.h \
  searchresults.h \
  albumitemcreator.h \
  albumslistmodel.h \
  searchengine.h \
  songslistmodel.h \
  songitemcreator.h \
  artistslistmodel.h \
  artistitemcreator.h \
  listwithurimodel.h \
  musiclayoutanimation.h \
  artistlayoutanimation.h \
  playlistsmodel.h \
  utils.h \
  listwithuri.h \
  $$DBUS_INTERFACE_HEADERS \
  $$DBUS_ADAPTOR_HEADERS

DBUS_INTERFACES += \
  interfaces/BognorQueue.xml

DBUS_ADAPTORS += interfaces/com.meego.music.xml


INSTALLS += target

MGEN_OUTDIR = .gen
mgen.output = $$GEN_DIR/gen_${QMAKE_FILE_BASE}data.cpp
mgen.clean += $$GEN_DIR/gen_*

