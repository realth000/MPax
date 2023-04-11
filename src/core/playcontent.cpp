#include "core/playcontent.h"

#include <QtCore/QDebug>

PlayContent::PlayContent(const QString &filePath)
#if 0
: property(QMap<QString, QVariant*>{{"ContentPath", &contentPath},
                                    {"ContentName", &contentName},
                                    {"ContentSize", &contentSize},
                                    {"Artist", &artist},
                                    {"Title", &title},
                                    {"TrackNumber", &trackNumber},
                                    {"BitRate", &bitRate},
                                    {"AlbumArtist", &albumArtist},
                                    {"AlbumTitle", &albumTitle},
                                    {"TrackCount", &albumTrackCount},
                                    {"Year", &albumYear},
                                    {"AlbumCover", &albumCover},
                                    {"Genre", &genre},
                                    {"Comment", &comment},
                                    {"SampleRate", &sampleRate},
                                    {"Channels", &channels},
                                    {"Length", &length}}
)
#endif
{
  const QFileInfo fileInfo(filePath);
  this->contentPath = fileInfo.absoluteFilePath();
  this->contentName = fileInfo.fileName();
  this->contentSize = fileInfo.size();
}

PlayContent::~PlayContent() {
  for (auto cover : coverList) {
    delete cover->image;
  }
}

QVariant PlayContent::value(const QString &propertyName) const {
  //  qDebug() << "get" << propertyName << property[propertyName];
  //  return property[propertyName];
  // TODO: Need optimize
  if (propertyName == QStringLiteral("ContentPath")) {
    return QVariant(contentPath);
  } else if (propertyName == QStringLiteral("ContentName")) {
    return QVariant(contentName);
  } else if (propertyName == QStringLiteral("ContentSize")) {
    return QVariant(contentSize);
  } else if (propertyName == QStringLiteral("Artist")) {
    return QVariant(artist);
  } else if (propertyName == QStringLiteral("Title")) {
    return QVariant(title);
  } else if (propertyName == QStringLiteral("TrackNumber")) {
    return QVariant(trackNumber);
  } else if (propertyName == QStringLiteral("BitRate")) {
    return QVariant(bitRate);
  } else if (propertyName == QStringLiteral("SampleRate")) {
    return QVariant(sampleRate);
  } else if (propertyName == QStringLiteral("AlbumTitle")) {
    return QVariant(albumTitle);
  } else if (propertyName == QStringLiteral("AlbumArtist")) {
    return QVariant(albumArtist);
  } else if (propertyName == QStringLiteral("AlbumYear")) {
    return QVariant(albumYear);
  } else if (propertyName == QStringLiteral("AlbumTrackCount")) {
    return QVariant(albumTrackCount);
  } else if (propertyName == QStringLiteral("TrackNumber")) {
    return QVariant(trackNumber);
  } else if (propertyName == QStringLiteral("AlbumCover")) {
    return QVariant(albumCover);
  } else if (propertyName == QStringLiteral("Genre")) {
    return QVariant(genre);
  } else if (propertyName == QStringLiteral("Comment")) {
    return QVariant(comment);
  } else if (propertyName == QStringLiteral("Channels")) {
    return QVariant(channels);
  } else if (propertyName == QStringLiteral("Length")) {
    return QVariant(length);
  }

  return QVariant();
}

void PlayContent::setValue(const QString &name, const QVariant &value) {}
