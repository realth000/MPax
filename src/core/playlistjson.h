#ifndef MPAX_PLAYLISTJSON_H
#define MPAX_PLAYLISTJSON_H

#include "core/playcontent.h"
#include "core/playlist.h"

namespace PlaylistJson {

QString toJsonString(const Playlist &playlist);

QString toJsonString(const QList<Playlist> &playlist);

QList<Playlist> fromJsonString(const QString &filePath);

};  // namespace PlaylistJson

#endif  // MPAX_PLAYLISTJSON_H
