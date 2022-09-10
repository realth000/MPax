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
  return m_visibleHeaderVector.count();
}

void PlaylistModelHeader::setUsedHeader(const QString& header, bool used) {
  const QString name = m_headerTrans.key(header);
  qDebug() << "TRANS AME + " << name;
  if (name.isEmpty()) {
    return;
  }
  if (used) {
    for (auto& i : m_headerVector) {
      if (i.name == name) {
        i.used = true;
        m_visibleHeaderVector.append(&i);
      }
    }
  } else {
    for (int i = 0; i < m_visibleHeaderVector.length(); i++) {
      if (m_visibleHeaderVector[i]->name == name) {
        m_visibleHeaderVector[i]->used = false;
        m_visibleHeaderVector.remove(i);
      }
    }
  }
}

QString PlaylistModelHeader::usedHeader(const int& index) const {
  if (m_visibleHeaderVector.length() <= index) {
    return "";
  }
  return m_visibleHeaderVector[index]->name;
}

PlaylistHeaderItem PlaylistModelHeader::usedHeader(
    const QString& header) const {
  const auto t = m_headerTrans.key(header);
  if (t.isEmpty()) {
    return {};
  }
  for (auto& h : m_visibleHeaderVector) {
    if (h->name == t) {
      return *h;
    }
  }
  return {};
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
  auto configHeaderUsed = Config::AppConfig::getInstance()
                              ->config(CONFIG_PLAYLIST_HEADER_USED)
                              .value.toMap();

  if (configHeaderSort.isEmpty() || configHeader.isEmpty() ||
      configHeaderSort.count() != configHeader.count()) {
    m_headerVector = defaultHeaderVector();
  } else {
    for (auto i = configHeader.constBegin(); i != configHeader.constEnd();
         i++) {
      m_headerVector.push_back(PlaylistHeaderItem());
      m_headerVector.last().name = i.key();
      m_headerVector.last().width = i.value().toInt();
      m_headerVector.last().index = configHeaderSort[i.key()].toInt();
      m_headerVector.last().used = configHeaderUsed[i.key()].toBool();
    }
  }
  for (auto& h : m_headerVector) {
    if (h.used) {
      m_visibleHeaderVector.append(&h);
    }
  }
  std::stable_sort(m_visibleHeaderVector.begin(), m_visibleHeaderVector.end(),
                   [](PlaylistHeaderItem* i, PlaylistHeaderItem* j) -> bool {
                     return i->index < j->index;
                   });
}

PlaylistModelHeader::~PlaylistModelHeader() {}

QVector<HeaderItem> PlaylistModelHeader::defaultHeaderVector() {
  return QVector<HeaderItem>{
      HeaderItem{"Title", 300, 0, true},
      HeaderItem{"Artist", 100, 1, true},
      HeaderItem{"AlbumTitle", 130, 2, true},
      HeaderItem{"ContentName", 100, 3, false},
      HeaderItem{"AlbumArtist", 100, 4, false},
      HeaderItem{"AlbumYear", 100, 5, false},
      HeaderItem{"AlbumTrackCount", 100, 6, false},
      HeaderItem{"TrackNumber", 100, 7, false},
      HeaderItem{"Length", 100, 8, false},
  };
}
void PlaylistModelHeader::updateSort(int logicalIndex, int oldVisualIndex,
                                     int newVisualIndex) {
  for (int i = 0; i < m_visibleHeaderVector.length(); i++) {
    if (i == logicalIndex) {
      m_visibleHeaderVector[i]->index = newVisualIndex;
    } else if (oldVisualIndex < newVisualIndex &&
               oldVisualIndex < m_visibleHeaderVector[i]->index &&
               m_visibleHeaderVector[i]->index <= newVisualIndex) {
      m_visibleHeaderVector[i]->index--;
    } else if (newVisualIndex < oldVisualIndex &&
               newVisualIndex <= m_visibleHeaderVector[i]->index &&
               m_visibleHeaderVector[i]->index < oldVisualIndex) {
      m_visibleHeaderVector[i]->index++;
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
  QMap<QString, QVariant> toSaveUsed;
  for (auto& h : m_headerVector) {
    toSave.insert(h.name, h.width);
    toSaveSort.insert(h.name, h.index);
    toSaveUsed.insert(h.name, h.used);
  }

  Config::AppConfig::getInstance()->setConfig(CONFIG_PLAYLIST_HEADER, toSave);
  Config::AppConfig::getInstance()->setConfig(CONFIG_PLAYLIST_HEADER_SORT,
                                              toSaveSort);
  Config::AppConfig::getInstance()->setConfig(CONFIG_PLAYLIST_HEADER_USED,
                                              toSaveUsed);
  Config::AppConfig::getInstance()->saveConfigDefer();
}
}  // namespace PLModel
