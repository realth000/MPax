#ifndef PLAYCONTENT_H
#define PLAYCONTENT_H

#include <QtCore/QDate>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtGui/QImage>

/*!
 * From taglib/taglib/mpeg/id3v2/frames/attachedpictureframe.h
 * This describes the function or content of the picture.
 */
enum CoverType {
  //! A type not enumerated below
  Other = 0x00,
  //! 32x32 PNG image that should be used as the file icon
  FileIcon = 0x01,
  //! File icon of a different size or format
  OtherFileIcon = 0x02,
  //! Front cover image of the album
  FrontCover = 0x03,
  //! Back cover image of the album
  BackCover = 0x04,
  //! Inside leaflet page of the album
  LeafletPage = 0x05,
  //! Image from the album itself
  Media = 0x06,
  //! Picture of the lead artist or soloist
  LeadArtist = 0x07,
  //! Picture of the artist or performer
  Artist = 0x08,
  //! Picture of the conductor
  Conductor = 0x09,
  //! Picture of the band or orchestra
  Band = 0x0A,
  //! Picture of the composer
  Composer = 0x0B,
  //! Picture of the lyricist or text writer
  Lyricist = 0x0C,
  //! Picture of the recording location or studio
  RecordingLocation = 0x0D,
  //! Picture of the artists during recording
  DuringRecording = 0x0E,
  //! Picture of the artists during performance
  DuringPerformance = 0x0F,
  //! Picture from a movie or video related to the track
  MovieScreenCapture = 0x10,
  //! Picture of a large, coloured fish
  ColouredFish = 0x11,
  //! Illustration related to the track
  Illustration = 0x12,
  //! Logo of the band or performer
  BandLogo = 0x13,
  //! Logo of the publisher (record company)
  PublisherLogo = 0x14
};

struct Cover {
  CoverType type;
  QString mimeType;
  QImage *image;
};

class PlayContent {
 public:
  explicit PlayContent(const QString &filePath);
  ~PlayContent();
  void reset();

  QVariant value(const QString &propertyName) const;
  void setValue(const QString &name, const QVariant &value);
  QString contentPath;
  QString contentName;
  qint64 contentSize;
  QString artist;
  QString title;
  int trackNumber;
  int bitRate;
  QString albumArtist;
  QString albumTitle;
  QImage albumCover;
  QList<Cover *> coverList;
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
