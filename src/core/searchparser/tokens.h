#ifndef MPAX_TOKENS_H
#define MPAX_TOKENS_H

#include <QtCore/QMap>
#include <QtCore/QStringList>

// clang-format off
static const QStringList keywords = {
    "HAS", "IS",
    "GREATER", "LESS",   "EQUAL",
    "MISSING", "PRESENT",
    "BEFORE", "AFTER", "SINCE", "DURING"
};
// clang-format on

static const QStringList operatorKeywords = {"AND", "OR"
#if 0
                                             ,"NOT"
#endif
};

static const QMap<QString, QString> metaKeywords =
    QMap<QString, QString>{{"title", "Title"},
                           {"artist", "Artist"},
                           {"album", "AlbumTitle"},
                           {"album_artist", "AlbumArtist"}
#if 0
                           ,
                           {"date", "date"},
                           {"track_number", "TrackNumber"},
                           {"track_total", "TrackTotal"},
                           {"disc_number", "DiscNumber"},
                           {"disc_total", "DiscTotal"},
                           {"codec", "codec"},
                           {"file_name", "FileName"},
                           {"file_name_ext", "FileNameExt"},
                           {"path", "FilePath"},
                           {"length", "Length"},
                           {"seconds", "Seconds"},
                           {"bitrate", "Bitrate"},
                           {"samplerate", "Samplerate"}
#endif
    };

#endif  // MPAX_TOKENS_H
