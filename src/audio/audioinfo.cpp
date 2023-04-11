#include "audioinfo.h"

#include <QtConcurrent/QtConcurrentMap>
#include <QtCore/QBuffer>
#include <QtCore/QDebug>
#include <QtCore/QFutureWatcher>
#include <QtCore/QTimer>
#include <QtGui/QImageReader>

#include "taglib/fileref.h"
#include "taglib/mpeg/id3v2/frames/attachedpictureframe.h"
#include "taglib/mpeg/id3v2/id3v2tag.h"
#include "taglib/mpeg/mpegfile.h"
#include "taglib/tag.h"
#include "tpropertymap.h"

#define TAGLIB_STR(QSTR) \
  TagLib::String((QSTR).toUtf8().data(), TagLib::String::UTF8)

bool AudioInfo::readAudioInfo(const QString &audioPath,
                              PlayContent *playContent, InfoOption infoOption) {
#if defined(Q_OS_WIN) || defined(Q_OS_WINDOWS)
  TagLib::FileRef f(reinterpret_cast<const wchar_t *>(audioPath.constData()));
#else
  TagLib::FileRef f(audioPath.toUtf8().constData());
#endif
  TagLib::ID3v2::Tag pf(f.file(), 0);
  if (infoOption != InfoOption::NoAlbumCover && !pf.isEmpty()) {
    const auto &frameListMap = pf.frameListMap();
    if (!frameListMap["APIC"].isEmpty()) {
      for (auto pic : frameListMap["APIC"]) {
        auto cover =
            reinterpret_cast<TagLib::ID3v2::AttachedPictureFrame *>(pic);
        const auto &vec = cover->picture().toBase64();
        auto img = new QImage();
        img->loadFromData(QByteArray::fromBase64(
            QByteArray::fromRawData(vec.data(), int(vec.size()))));
        playContent->coverList.append(new Cover{
            CoverType(cover->type()),
            cover->mimeType().toCString(true),
            img,
        });
      }
    }

    for (auto i : pf.frameList()) {
      if (QString(i->toString().toCString(true)).contains("[image/")) {
        const auto &p =
            reinterpret_cast<TagLib::ID3v2::AttachedPictureFrame *>(i);
        const auto &vector = p->picture().toBase64();
        const auto &arr = QByteArray::fromBase64(
            QByteArray::fromRawData(vector.data(), int(vector.size())));
        playContent->albumCover.loadFromData(arr);
      }
    }
  }

  if (playContent == nullptr) {
    qDebug() << "can not save audio info in a null PlayContent:" << audioPath;
    return false;
  }
  if (f.isNull() || !f.tag()) {
    qDebug() << "audio info not readable:" << audioPath;
    return false;
  }
  auto *tag = f.tag();

  playContent->title = tag->title().toCString(true);
  playContent->artist = tag->artist().toCString(true);
  playContent->albumTitle = tag->album().toCString(true);
  playContent->albumYear = int(tag->year());
  //  playContent->trackNumber = int(tag->track());
  playContent->genre = tag->genre().toCString(true);
  playContent->comment = tag->comment().toCString(true);

  // Replace empty info.
  if (playContent->title.isEmpty()) {
    playContent->title = playContent->contentName;
  }

  const auto &tags = f.file()->properties();
  playContent->albumArtist = tags["ALBUMARTIST"].toString().toCString(true);

  playContent->trackNumber = 0;
  playContent->albumTrackCount = 0;
  const auto &trackNumberString =
      QString(tags["TRACKNUMBER"].toString().toCString(true));
  if (!trackNumberString.isEmpty()) {
    const auto &trackNumberStringList = trackNumberString.split('/');
    if (trackNumberStringList.length() == 2) {
      playContent->trackNumber = trackNumberStringList[0].toInt();
      playContent->albumTrackCount = trackNumberStringList[1].toInt();
    } else {
      playContent->trackNumber = trackNumberString.toInt();
      playContent->albumTrackCount = 0;
    }
  }

  if (f.isNull() || !f.audioProperties()) {
    return false;
  }
  const auto *properties = f.audioProperties();
  playContent->bitRate = properties->bitrate();
  playContent->sampleRate = properties->sampleRate();
  playContent->channels = properties->channels();
  playContent->length = properties->lengthInSeconds();
  return true;
}

bool AudioInfo::writeAudioInfo(const QString &audioPath,
                               const PlayContent *playContent) {
  if (audioPath.isEmpty()) {
    qDebug()
        << "error writing audio info: intend to save audio info in empty path";
    return false;
  }
  if (playContent == nullptr) {
    qDebug() << "error writing audio info: intend to save audio info from a "
                "null play content";
    return false;
  }

  TagLib::ID3v2::FrameFactory::instance()->setDefaultTextEncoding(
      TagLib::String::UTF8);

  TagLib::MPEG::File f(audioPath.toUtf8().constData());

  if (!f.tag()) {
    qDebug() << "error writing audio info: audio file not readable:"
             << audioPath;
    return false;
  }
  f.ID3v2Tag()->setTitle(TAGLIB_STR(playContent->title));
  f.ID3v2Tag()->setArtist(TAGLIB_STR(playContent->artist));
  f.ID3v2Tag()->setAlbum(TAGLIB_STR(playContent->albumTitle));
  if (playContent->albumYear > 0) {
    f.ID3v2Tag()->setYear(playContent->albumYear);
  }
  f.ID3v2Tag()->setGenre(TAGLIB_STR(playContent->genre));
  f.ID3v2Tag()->setComment(TAGLIB_STR(playContent->comment));

  auto map = f.properties();
  map.replace("ALBUMARTIST",
              TagLib::StringList{TAGLIB_STR(playContent->albumArtist)});
  if (playContent->trackNumber > 0) {
    if (playContent->albumTrackCount > 0) {
      map.replace("TRACKNUMBER",
                  TagLib::StringList{TAGLIB_STR(QString("%1/%2").arg(
                      QString::number(playContent->trackNumber),
                      QString::number(playContent->albumTrackCount)))});
    } else {
      map.replace("TRACKNUMBER", TagLib::StringList{TAGLIB_STR(QString::number(
                                     playContent->trackNumber))});
    }
  }
  f.setProperties(map);
  f.save(TagLib::MPEG::File::ID3v2);
  return true;
}
