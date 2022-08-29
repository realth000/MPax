#include "playlistmodelheader.h"

namespace PLModel {
PlaylistModelHeader* PlaylistModelHeader::getInstance() {
  static PlaylistModelHeader playlistModelHeader;
  return &playlistModelHeader;
}

int PlaylistModelHeader::headerCount() const { return m_headerList.count(); }

int PlaylistModelHeader::usedHeaderCount() const {
  return m_usedHeaderList.count();
}

QString PlaylistModelHeader::header(const int& index) const {
  return m_headerList[index].name;
}

QString PlaylistModelHeader::usedHeader(const int& index) const {
  return m_usedHeaderList[index].name;
}

PlaylistModelHeader::PlaylistModelHeader() {
  // TODO: This should in somewhere else.
  //  PlaylistSql::getInstance();
  m_headerList = defaultHeaderList();
  m_usedHeaderList = m_headerList;
}

PlaylistModelHeader::~PlaylistModelHeader() {}

QList<HeaderItem> PlaylistModelHeader::defaultHeaderList() {
  return QList<HeaderItem>{HeaderItem{"Title", 300, 0},
                           HeaderItem{"Artist", 100, 1},
                           HeaderItem{"AlbumTitle", 130, 2}};
}
void PlaylistModelHeader::updateSort(int logicalIndex, int oldVisualIndex,
                                     int newVisualIndex) {
  for (int i = 0; i < m_usedHeaderList.length(); i++) {
    if (i == logicalIndex) {
      m_usedHeaderList[i].index = newVisualIndex;
    } else if (oldVisualIndex < newVisualIndex &&
               oldVisualIndex < m_usedHeaderList[i].index &&
               m_usedHeaderList[i].index <= newVisualIndex) {
      m_usedHeaderList[i].index--;
    } else if (newVisualIndex < oldVisualIndex &&
               newVisualIndex <= m_usedHeaderList[i].index &&
               m_usedHeaderList[i].index < oldVisualIndex) {
      m_usedHeaderList[i].index++;
    }
  }
}
}  // namespace PLModel
