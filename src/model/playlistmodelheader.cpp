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
  return m_usedHeaderVector.count();
}

QString PlaylistModelHeader::header(const int& index) const {
  return m_headerVector[index].name;
}

void PlaylistModelHeader::setUsedHeader(const QString& header, bool used) {
  const QString name = m_headerTrans.key(header);
  if (name.isEmpty()) {
    return;
  }
  if (used) {
    for (auto& i : m_headerVector) {
      if (i.name == name) {
        i.used = true;
        m_usedHeaderVector.append(&i);
      }
    }
  } else {
    for (int i = 0; i < m_usedHeaderVector.length(); i++) {
      if (m_usedHeaderVector[i]->name == name) {
        m_usedHeaderVector[i]->used = false;
        m_usedHeaderVector.remove(i);
      }
    }
  }
}

QString PlaylistModelHeader::usedHeader(const int& index) const {
  if (m_usedHeaderVector.length() <= index) {
    return "";
  }
  return m_usedHeaderVector[index]->name;
}

PlaylistHeaderItem* PlaylistModelHeader::usedHeader(
    const QString& header) const {
  const auto t = m_headerTrans.key(header);
  if (t.isEmpty()) {
    return nullptr;
  }
  for (const auto& h : m_usedHeaderVector) {
    if (h->name == t) {
      return h;
    }
  }
  return nullptr;
}

PlaylistModelHeader::PlaylistModelHeader() : m_headerTrans(MODEL_ALL_HEADER) {
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
      if (i.value().toInt() > 0) {
        m_headerVector[j.value().toInt()].width = i.value().toInt();
      }
      m_headerVector[j.value().toInt()].index = j.value().toInt();
    }
  }
  for (auto& v : m_headerVector) {
    if (v.used) {
      m_usedHeaderVector.append(&v);
    }
  }
}

PlaylistModelHeader::~PlaylistModelHeader() {}

QVector<HeaderItem> PlaylistModelHeader::defaultHeaderVector() {
  return QVector<HeaderItem>{
      HeaderItem{"Title", 300, 0, true},
      HeaderItem{"Artist", 100, 1, true},
      HeaderItem{"AlbumTitle", 130, 2, true},
      HeaderItem{"ContentName", 100, 3, false},
      HeaderItem{"AlbumArtist", 100, 3, false},
      HeaderItem{"AlbumYear", 100, 3, false},
      HeaderItem{"AlbumTrackCount", 100, 3, false},
      HeaderItem{"TrackNumber", 100, 3, false},
      HeaderItem{"Length  ", 100, 3, false},
  };
}
void PlaylistModelHeader::updateSort(int logicalIndex, int oldVisualIndex,
                                     int newVisualIndex) {
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
  }
  saveConfig();
}

void PlaylistModelHeader::updateWidth(int logicalIndex, int oldSize,
                                      int newSize) {
  for (auto& h : m_headerVector) {
    if (h.index == logicalIndex) {
      h.width = newSize;
    }
  }
  saveConfig();
}

QVector<PlaylistHeaderItem> PlaylistModelHeader::headerVector() const {
  return m_headerVector;
}

void PlaylistModelHeader::saveConfig() {
  QMap<QString, QVariant> toSave;
  QMap<QString, QVariant> toSaveSort;
  for (auto& h : m_headerVector) {
    toSave.insert(h.name, h.width);
    toSaveSort.insert(h.name, h.index);
  }

  Config::AppConfig::getInstance()->setConfig(CONFIG_PLAYLIST_HEADER, toSave);
  Config::AppConfig::getInstance()->setConfig(CONFIG_PLAYLIST_HEADER_SORT,
                                              toSaveSort);
  Config::AppConfig::getInstance()->saveConfigDefer();
}
}  // namespace PLModel
