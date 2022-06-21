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
    QString authorName;
    QString title;
    qint64 duration;
    qint64 size;
    int trackNumber;
    int audioBitRate;
    QString audioCodec;
    int sampleRate;
    QString albumArtist;
    QString albumTitle;
    QImage albumCover;
    int albumYear;
    QDate albumDate;
    int albumTrackCount;
};

inline QVariant PlayContent::value(const QString &headerName) const
{
    // TODO: Need optimize
    if (headerName == QStringLiteral("ContentPath")) {
        return QVariant(contentPath);
    } else if (headerName == QStringLiteral("ContentName")) {
        return QVariant(contentName);
    } else if (headerName == QStringLiteral("Author")) {
        return QVariant(authorName);
    } else if (headerName == QStringLiteral("Title")) {
        return QVariant(title);
    } else if (headerName == QStringLiteral("Duration")) {
        return QVariant(duration);
    } else if (headerName == QStringLiteral("Size")) {
        return QVariant(size);
    } else if (headerName == QStringLiteral("TrackNumber")) {
        return QVariant(trackNumber);
    } else if (headerName == QStringLiteral("AudioBitRate")) {
        return QVariant(audioBitRate);
    } else if (headerName == QStringLiteral("AudioCodec")) {
        return QVariant(audioCodec);
    } else if (headerName == QStringLiteral("SampleRate")) {
        return QVariant(sampleRate);
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
    } else if (headerName == QStringLiteral("Date")) {
        return QVariant(albumDate);
    }
    return QVariant();
}

#endif // PLAYCONTENT_H
