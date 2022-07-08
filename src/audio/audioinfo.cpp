#include "audioinfo.h"

#include <QtCore/QDebug>

#include "taglib/fileref.h"
#include "taglib/tag.h"
#include "tpropertymap.h"

bool AudioInfo::readAudioInfo(const QString& audioPath,
                              PlayContent* playContent) {
  TagLib::FileRef f(audioPath.toUtf8().constData());

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
      //      qDebug() << i->first.toCString(true)
      //               << i->second.toString().toCString(true);
    }
  }
  if (f.isNull() || !f.audioProperties()) {
    return false;
  }
  TagLib::AudioProperties* properties = f.audioProperties();
  playContent->audioBitRate = properties->bitrate();
  return true;
}
