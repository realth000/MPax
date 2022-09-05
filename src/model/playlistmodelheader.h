#ifndef MPAX_PLAYLISTMODELHEADER_H
#define MPAX_PLAYLISTMODELHEADER_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QPair>
#include <QtCore/QString>
#include <QtCore/QVector>

namespace PLModel {

// clang-format off
#define MODEL_ALL_HEADER                                                       \
  QMap<QString, QString> {                                                     \
    {"Title", PLModel::PlaylistModelHeader::tr("Title")},                      \
    {"Artist", PLModel::PlaylistModelHeader::tr("Artist")},                    \
    {"AlbumTitle", PLModel::PlaylistModelHeader::tr("AlbumTitle")},            \
    {"ContentName", PLModel::PlaylistModelHeader::tr("ContentName")},          \
    {"AlbumArtist", PLModel::PlaylistModelHeader::tr("AlbumArtist")},          \
    {"AlbumYear", PLModel::PlaylistModelHeader::tr("AlbumYear")},              \
    {"AlbumTrackCount", PLModel::PlaylistModelHeader::tr("AlbumTrackCount")},  \
    {"TrackNumber", PLModel::PlaylistModelHeader::tr("TrackNumber")},          \
    {"Length", PLModel::PlaylistModelHeader::tr("Length")},                    \
  }
// clang-format on

/**
 * PlaylistHeaderItem:
 * .first: header name
 * .second: header width
 */
struct PlaylistHeaderItem {
  QString name;
  int width;
  int index;
  bool used;
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
  void setUsedHeader(const QString &header, bool used);
  QString usedHeader(const int &index) const;
  PlaylistHeaderItem *usedHeader(const QString &header) const;
  QVector<PlaylistHeaderItem> headerVector() const;

 public slots:
  void updateSort(int logicalIndex, int oldVisualIndex, int newVisualIndex);
  void updateWidth(int logicalIndex, int oldSize, int newSize);
  void saveConfig();

 private:
  QVector<PlaylistHeaderItem> m_headerVector;
  QVector<PlaylistHeaderItem *> m_usedHeaderVector;

  explicit PlaylistModelHeader();
  ~PlaylistModelHeader();
  static QVector<HeaderItem> defaultHeaderVector();
  QMap<QString, QString> m_headerTrans;
};
}  // namespace PLModel

#endif  // MPAX_PLAYLISTMODELHEADER_H
