#include "core/playcontent.h"

PlayContent::PlayContent(const QString& filePath) {
  const QFileInfo fileInfo(filePath);
  this->contentPath = fileInfo.absoluteFilePath();
  this->contentName = fileInfo.fileName();
}

QVariant PlayContent::value(const QString& headerName) const {
  // TODO: Need optimize
  if (headerName == QStringLiteral("ContentPath")) {
    return QVariant(contentPath);
  } else if (headerName == QStringLiteral("ContentName")) {
    return QVariant(contentName);
  } else if (headerName == QStringLiteral("ContentSize")) {
    return QVariant(contentSize);
  } else if (headerName == QStringLiteral("Artist")) {
    return QVariant(artist);
  } else if (headerName == QStringLiteral("Title")) {
    return QVariant(title);
  } else if (headerName == QStringLiteral("TrackNumber")) {
    return QVariant(trackNumber);
  } else if (headerName == QStringLiteral("AudioBitRate")) {
    return QVariant(audioBitRate);
  } else if (headerName == QStringLiteral("AlbumArtist")) {
    return QVariant(albumArtist);
  } else if (headerName == QStringLiteral("AlbumTitle")) {
    return QVariant(albumTitle);
  } else if (headerName == QStringLiteral("TrackCount")) {
    return QVariant(albumTrackCount);
  } else if (headerName == QStringLiteral("ThumbnailImage")) {
    return QVariant(albumCover);
  } else if (headerName == QStringLiteral("Year")) {
    return QVariant(albumYear);
  }
  return QVariant();
}
