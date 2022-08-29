#ifndef MPAX_PLAYLISTMODELHEADER_H
#define MPAX_PLAYLISTMODELHEADER_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPair>
#include <QtCore/QString>

namespace PLModel {
/**
 * PlaylistHeaderItem:
 * .first: header name
 * .second: header width
 */
struct PlaylistHeaderItem {
  QString name;
  int width;
  int index;
};

using HeaderItem = PlaylistHeaderItem;

class PlaylistModelHeader : public QObject {
  Q_OBJECT

 public:
  static PlaylistModelHeader *getInstance();
  PlaylistModelHeader(const PlaylistModelHeader &) = delete;
  PlaylistModelHeader &operator=(const PlaylistModelHeader &) = delete;

  int headerCount() const;
  int usedHeaderCount() const;
  QString header(const int &index) const;
  QString usedHeader(const int &index) const;

 public slots:
  void updateSort(int logicalIndex, int oldVisualIndex, int newVisualIndex);

 private:
  QList<PlaylistHeaderItem> m_headerList;
  QList<PlaylistHeaderItem> m_usedHeaderList;

  explicit PlaylistModelHeader();
  ~PlaylistModelHeader();
  static QList<HeaderItem> defaultHeaderList();
};
}  // namespace PLModel

#endif  // MPAX_PLAYLISTMODELHEADER_H
