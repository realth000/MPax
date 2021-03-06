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
#include "taglib/tag.h"
#include "tpropertymap.h"

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

  // Replace empty info.
  if (playContent->title.isEmpty()) {
    playContent->title = playContent->contentName;
  }

  TagLib::PropertyMap tags = f.file()->properties();

  unsigned int longest = 0;
  for (TagLib::PropertyMap::ConstIterator i = tags.begin(); i != tags.end();
       ++i) {
    if (i->first.size() > longest) {
      longest = i->first.size();
    }
  }
  for (TagLib::PropertyMap::ConstIterator i = tags.begin(); i != tags.end();
       ++i) {
    for (TagLib::StringList::ConstIterator j = i->second.begin();
         j != i->second.end(); ++j) {
      const QString t(i->first.toCString(true));
      if (t == "ALBUMARTIST") {
        playContent->albumArtist = i->second.toString().toCString(true);
      }
    }
  }
  if (f.isNull() || !f.audioProperties()) {
    return false;
  }
  TagLib::AudioProperties* properties = f.audioProperties();
  playContent->audioBitRate = properties->bitrate();
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
