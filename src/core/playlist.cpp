#include "playlist.h"

PlaylistInfo::PlaylistInfo() : m_info(new QMap<QString, QString>()) {}

PlaylistInfo::PlaylistInfo(const QMap<QString, QString>& info)
    : m_info(new QMap<QString, QString>(info)) {}

const QMap<QString, QString>* PlaylistInfo::info() const { return m_info; }

QString PlaylistInfo::info(const QString& infoName) const {
  if (m_info == nullptr) {
    return QString();
  }
  return (*m_info)[infoName];
}

bool PlaylistInfo::setInfo(const QString& infoName, const QString& infoValue) {
  if (m_info == nullptr) {
    return false;
  }
  m_info->insert(infoName, infoValue);
  return true;
}

QMap<QString, QString> PlaylistInfo::defaultInfo() {
  return QMap<QString, QString>{{"Name", "default"}};
}

Playlist::Playlist() : m_playListInfo(nullptr), m_playContentList(nullptr) {}

Playlist::Playlist(const QMap<QString, QString>& info,
                   PlayContentList* playContentList)
    : m_playListInfo(new PlaylistInfo(info)),
      m_playContentList(playContentList) {}

Playlist::Playlist(PlaylistInfo* playlistInfo, PlayContentList* playContentList)
    : m_playListInfo(playlistInfo), m_playContentList(playContentList) {}

Playlist::Playlist(const PlaylistInfo& playlistInfo,
                   const PlayContentList& playContentList)
    : m_playListInfo(new PlaylistInfo(playlistInfo)),
      m_playContentList(new PlayContentList(playContentList)) {}

const PlayContentList* Playlist::content() const { return m_playContentList; }

const PlaylistInfo* Playlist::info() const { return m_playListInfo; }
