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
  return m_headerList[index].first;
}

QString PlaylistModelHeader::usedHeader(const int& index) const {
  return m_usedHeaderList[index].first;
}

PlaylistModelHeader::PlaylistModelHeader() {
  // TODO: This should in somewhere else.
  //  PlaylistSql::getInstance();
  m_headerList = defaultHeaderList();
  m_usedHeaderList = m_headerList;
}

PlaylistModelHeader::~PlaylistModelHeader() {}

QList<HeaderItem> PlaylistModelHeader::defaultHeaderList() {
  return QList<HeaderItem>{HeaderItem("Title", 300), HeaderItem("Artist", 100),
                           HeaderItem("AlbumTitle", 130)};
}
}  // namespace PLModel
