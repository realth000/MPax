#ifndef MPAX_PLAYLISTMODELHEADER_H
#define MPAX_PLAYLISTMODELHEADER_H

#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtCore/QString>

namespace PLModel {
/**
 * PlaylistHeaderItem:
 * .first: header name
 * .second: header width
 */
typedef QPair<QString, int> PlaylistHeaderItem;
using HeaderItem = PlaylistHeaderItem;

class PlaylistModelHeader {
 public:
  static PlaylistModelHeader *getInstance();
  PlaylistModelHeader(const PlaylistModelHeader &) = delete;
  PlaylistModelHeader &operator=(const PlaylistModelHeader &) = delete;

  int headerCount() const;
  int usedHeaderCount() const;
  QString header(const int &index) const;
  QString usedHeader(const int &index) const;

 private:
  QList<QPair<QString, int>> m_headerList;
  QList<QPair<QString, int>> m_usedHeaderList;

  explicit PlaylistModelHeader();
  ~PlaylistModelHeader();
  static QList<HeaderItem> defaultHeaderList();
};
}  // namespace PLModel

#endif  // MPAX_PLAYLISTMODELHEADER_H
