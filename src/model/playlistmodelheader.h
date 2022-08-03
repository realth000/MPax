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
  explicit PlaylistModelHeader(const QList<HeaderItem> &headerList);
  int headerCount() const;
  int usedHeaderCount() const;
  QString header(const int &index) const;
  QString usedHeader(const int &index) const;
  static QList<HeaderItem> defaultHeaderList();

 private:
  QList<QPair<QString, int>> m_headerList;
  QList<QPair<QString, int>> m_usedHeaderList;
};
}  // namespace PLModel

#endif  // MPAX_PLAYLISTMODELHEADER_H
