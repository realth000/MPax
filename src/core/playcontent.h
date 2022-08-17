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
  QVariant value(const QString &propertyName) const;
  void setValue(const QString &name, const QVariant &value);
  QString contentPath;
  QString contentName;
  QString contentSize;
  QString artist;
  QString title;
  int trackNumber;
  int bitRate;
  QString albumArtist;
  QString albumTitle;
  QImage albumCover;
  int albumYear;
  int albumTrackCount;
  QString genre;
  QString comment;
  int sampleRate;
  int channels;
  int length;
  //  QMap<QString, QString> otherProperty;

  // private:
  //  QMap<QString, QVariant> property;
};

typedef QList<PlayContent *> PlayContentList;

#endif  // PLAYCONTENT_H
