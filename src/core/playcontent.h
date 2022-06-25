#ifndef PLAYCONTENT_H
#define PLAYCONTENT_H

#include <QtCore/QDate>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtGui/QImage>

class PlayContent {
 public:
  explicit PlayContent(const QString &filePath);
  QVariant value(const QString &headerName) const;
  QString contentPath;
  QString contentName;
  QString contentSize;
  QString artist;
  QString title;
  int trackNumber;
  int audioBitRate;
  QString albumArtist;
  QString albumTitle;
  QImage albumCover;
  int albumYear;
  int albumTrackCount;
};

typedef QList<PlayContent *> PlayContentList;

#endif  // PLAYCONTENT_H
