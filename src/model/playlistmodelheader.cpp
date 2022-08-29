#include "playlistmodelheader.h"

#include <QDebug>

#include "config/appconfig.h"

namespace PLModel {
PlaylistModelHeader* PlaylistModelHeader::getInstance() {
  static PlaylistModelHeader playlistModelHeader;
  return &playlistModelHeader;
}

int PlaylistModelHeader::headerCount() const { return m_headerVector.count(); }

int PlaylistModelHeader::usedHeaderCount() const {
  return m_headerVector.count();
}

QString PlaylistModelHeader::header(const int& index) const {
  return m_headerVector[index].name;
}

QString PlaylistModelHeader::usedHeader(const int& index) const {
  return m_headerVector[index].name;
}

PlaylistModelHeader::PlaylistModelHeader() {
  // TODO: This should in somewhere else.
  //  PlaylistSql::getInstance();
  QMap<QString, QVariant> configHeader = Config::AppConfig::getInstance()
                                             ->config(CONFIG_PLAYLIST_HEADER)
                                             .value.toMap();
  QMap<QString, QVariant> configHeaderSort =
      Config::AppConfig::getInstance()
          ->config(CONFIG_PLAYLIST_HEADER_SORT)
          .value.toMap();

  if (configHeaderSort.isEmpty() || configHeader.isEmpty() ||
      configHeaderSort.count() != configHeader.count()) {
    m_headerVector = defaultHeaderVector();
  } else {
    m_headerVector.fill(PlaylistHeaderItem(), configHeader.size());
    for (auto i = configHeader.constBegin(), j = configHeaderSort.constBegin();
         i != configHeader.constEnd() && j != configHeaderSort.constEnd();
         i++, j++) {
      if (j.value().toInt() >= m_headerVector.length()) {
        m_headerVector = defaultHeaderVector();
        break;
      }
      m_headerVector[j.value().toInt()].name = i.key();
      m_headerVector[j.value().toInt()].width = i.value().toInt();
      m_headerVector[j.value().toInt()].index = j.value().toInt();
    }
  }
}

PlaylistModelHeader::~PlaylistModelHeader() {}

QVector<HeaderItem> PlaylistModelHeader::defaultHeaderVector() {
  return QVector<HeaderItem>{HeaderItem{"Title", 300, 0},
                             HeaderItem{"Artist", 100, 1},
                             HeaderItem{"AlbumTitle", 130, 2}};
}
void PlaylistModelHeader::updateSort(int logicalIndex, int oldVisualIndex,
                                     int newVisualIndex) {
  QMap<QString, QVariant> toSave;
  QMap<QString, QVariant> toSaveSort;
  for (auto& h : m_headerVector) {
    toSave.insert(h.name, h.width);
  }
  for (int i = 0; i < m_headerVector.length(); i++) {
    if (i == logicalIndex) {
      m_headerVector[i].index = newVisualIndex;
    } else if (oldVisualIndex < newVisualIndex &&
               oldVisualIndex < m_headerVector[i].index &&
               m_headerVector[i].index <= newVisualIndex) {
      m_headerVector[i].index--;
    } else if (newVisualIndex < oldVisualIndex &&
               newVisualIndex <= m_headerVector[i].index &&
               m_headerVector[i].index < oldVisualIndex) {
      m_headerVector[i].index++;
    }
    toSaveSort.insert(m_headerVector[i].name, m_headerVector[i].index);
  }
  Config::AppConfig::getInstance()->setConfig(CONFIG_PLAYLIST_HEADER, toSave);
  Config::AppConfig::getInstance()->setConfig(CONFIG_PLAYLIST_HEADER_SORT,
                                              toSaveSort);
  Config::AppConfig::getInstance()->saveConfigDefer();
}
}  // namespace PLModel
