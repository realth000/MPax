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

bool AudioInfo::readAudioInfo(const QString& audioPath,
                              PlayContent* playContent, InfoOption infoOption) {
  TagLib::FileRef f(audioPath.toUtf8().constData());
  TagLib::ID3v2::Tag pf(f.file(), 0);
  if (infoOption != InfoOption::NoAlbumCover && !pf.isEmpty()) {
    for (auto i : pf.frameList()) {
      if (QString(i->toString().toCString(true)).contains("[image/")) {
        auto p = reinterpret_cast<TagLib::ID3v2::AttachedPictureFrame*>(i);
        if (p != nullptr) {
          const TagLib::ByteVector vector = p->picture().toBase64();
          QByteArray arr = QByteArray::fromBase64(
              QByteArray::fromRawData(vector.data(), vector.size()));
          playContent->albumCover.loadFromData(arr);
        }
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
  TagLib::Tag* tag = f.tag();

  playContent->title = tag->title().toCString(true);
  playContent->artist = tag->artist().toCString(true);
  playContent->albumTitle = tag->album().toCString(true);
  playContent->albumYear = int(tag->year());
  playContent->trackNumber = int(tag->track());
  playContent->genre = tag->genre().toCString(true);
  playContent->comment = tag->comment().toCString(true);

  // Replace empty info.
  if (playContent->title.isEmpty()) {
    playContent->title = playContent->contentName;
  }

  TagLib::PropertyMap tags = f.file()->properties();
  playContent->albumArtist = tags["ALBUMARTIST"].toString().toCString(true);
  if (f.isNull() || !f.audioProperties()) {
    return false;
  }
  TagLib::AudioProperties* properties = f.audioProperties();
  playContent->duration = properties->lengthInSeconds();
  playContent->bitRate = properties->bitrate();
  playContent->sampleRate = properties->sampleRate();
  playContent->channels = properties->channels();
  playContent->length = properties->length();
  return true;
}

bool AudioInfo::writeAudioInfo(const QString& audioPath,
                               const PlayContent* playContent) {
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
  if (playContent->trackNumber > 0) {
    f.ID3v2Tag()->setTrack(playContent->trackNumber);
  }
  f.ID3v2Tag()->setGenre(TAGLIB_STR(playContent->genre));
  f.ID3v2Tag()->setComment(TAGLIB_STR(playContent->comment));

  TagLib::PropertyMap map = f.properties();
  map.replace("ALBUMARTIST",
              TagLib::StringList{TAGLIB_STR(playContent->albumArtist)});
  //  f.file()->setProperties(map);
  f.save(TagLib::MPEG::File::ID3v2);
  return true;
}

#if 0
bool AudioInfo::readAudioInfoList(PlayContentList* playContentList,
                                  AudioInfo::InfoOption infoOption) {
  QFutureWatcher<void>* watcher = new QFutureWatcher<void>;
  QTimer* timer = new QTimer;
  QElapsedTimer* t = new QElapsedTimer;
  connect(timer, &QTimer::timeout, this, [this, watcher, t, timer]() {
    emit this->reloadInfoStatusChanged(watcher->isFinished(),
                                       watcher->progressValue(), t->elapsed());
    if (watcher->isFinished()) {
      timer->stop();
      timer->deleteLater();
      t->invalidate();
      delete t;
      watcher->deleteLater();
    }
  });
  timer->start(100);
  t->start();
  watcher->setFuture(QtConcurrent::map(
      *playContentList, [&](PlayContent* content) -> PlayContent* {
        readAudioInfo(content->contentPath, content, infoOption);
        return content;
      }));
  return true;
}
#endif
