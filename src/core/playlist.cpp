#include "playlist.h"

#include <QDebug>
PlaylistInfo::PlaylistInfo() : m_info(new QMap<QString, QString>()) {}

PlaylistInfo::PlaylistInfo(const QMap<QString, QString>& info)
    : m_info(new QMap<QString, QString>(info)) {}

PlaylistInfo::~PlaylistInfo() {
  // delete m_info;
}

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

Playlist::~Playlist() {
  // FIXME: Here has potential memory leak, but delete will cause SIGSEGV,
  // Use QSharedPointer to avoid this.
  //  delete m_playListInfo;
  //  delete m_playContentList;
}

PlayContentList Playlist::content() const { return *m_playContentList; }

void Playlist::setContent(PlayContentList* list) {
  delete m_playContentList;
  m_playContentList = list;
}

PlaylistInfo Playlist::info() const { return *m_playListInfo; }

bool Playlist::contains(const QString& contentPath) const {
  for (auto c : *m_playContentList) {
    if (c->contentPath == contentPath) {
      return true;
    }
  }
  return false;
}

void Playlist::setInfo(const QString& infoName, const QString& infoValue) {
  m_playListInfo->setInfo(infoName, infoValue);
}

void Playlist::appendContent(PlayContent* playContent) {
  m_playContentList->append(playContent);
}

void Playlist::removeContentAt(const int& index) {
  m_playContentList->removeAt(index);
}

void Playlist::clearContent() { m_playContentList->clear(); }

PlayContent* Playlist::takeContent(int index) {
  if (m_playContentList->count() <= index) {
    return nullptr;
  }
  return m_playContentList->takeAt(index);
}

bool Playlist::insertContent(int index, PlayContent* playContent) {
  // Count can equal index, because will insert after the last item.
  if (index < 0 || m_playContentList->count() < index) {
    return false;
  }
  m_playContentList->insert(index, playContent);
  return true;
}
