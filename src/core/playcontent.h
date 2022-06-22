#ifndef PLAYCONTENT_H
#define PLAYCONTENT_H

#include <QtCore/QDate>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtGui/QImage>

class PlayContent{
public:
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

inline QVariant PlayContent::value(const QString &headerName) const
{
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

#endif // PLAYCONTENT_H
