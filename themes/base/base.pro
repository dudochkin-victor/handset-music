load(meegotouch_defines)
TEMPLATE = subdirs
TARGET = meegomusic
THEME_NAME = $$system(basename $$PWD)

OTHER_FILES += meegomusic.conf

# Theme files
theme.files = $$system(find ./* -type d)
theme.files += meegomusic.conf
theme.path = $$M_THEME_DIR/$$THEME_NAME/meegotouch/$$TARGET/
theme.CONFIG += no_check_exist

INSTALLS += theme
