#ifndef MPAX_PLAYLISTJSON_H
#define MPAX_PLAYLISTJSON_H

#include "core/playcontent.h"

namespace PlaylistJson {

QString toJsonString(const PlayContentList &playlist);

QString toJsonString(const QList<PlayContentList> &playlist);

QList<PlayContentList> fromJsonString(const QString &jsonString);

};  // namespace PlaylistJson

#endif  // MPAX_PLAYLISTJSON_H
