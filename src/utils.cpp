/*
 * Meego-handset-music is an audio player for MeeGo handset.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "searchengine.h"
#include "utils.h"

postLoadItems::postLoadItems()
{
    loadFinished = false;
}

void postLoadItems::run()
{
    items = new QVector < QStringList > ();
    SearchEngine::getAllMusicArtists(*items);
    loadFinished = true;
}

postLoadItems *postThread = NULL;
