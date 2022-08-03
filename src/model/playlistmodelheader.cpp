#include "playlistmodelheader.h"

namespace PLModel {
PlaylistModelHeader::PlaylistModelHeader(const QList<HeaderItem>& headerList)
    : m_headerList(headerList) {
  // TODO: This should in somewhere else.
  //  PlaylistSql::getInstance();
  m_usedHeaderList = m_headerList;
}

int PlaylistModelHeader::headerCount() const { return m_headerList.count(); }

int PlaylistModelHeader::usedHeaderCount() const {
  return m_usedHeaderList.count();
}

QList<HeaderItem> PlaylistModelHeader::defaultHeaderList() {
  return QList<HeaderItem>{HeaderItem("Title", 300), HeaderItem("Artist", 100),
                           HeaderItem("AlbumTitle", 130)};
}

QString PlaylistModelHeader::header(const int& index) const {
  return m_headerList[index].first;
}

QString PlaylistModelHeader::usedHeader(const int& index) const {
  return m_usedHeaderList[index].first;
}
}  // namespace PLModel
