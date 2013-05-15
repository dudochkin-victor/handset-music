load(meegotouch_defines)
TEMPLATE = subdirs
SUBDIRS = src themes translations

translation.files = translations/*.qm
translation.path = /usr/share/l10n/meegotouch

desktop.files = data/*.desktop
desktop.path = /usr/share/applications

INSTALLS += desktop translation

