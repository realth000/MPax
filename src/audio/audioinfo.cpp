#include "audioinfo.h"

#include <iomanip>
#include <iostream>
using namespace std;
#include <QtCore/QDebug>

#include "taglib/fileref.h"
#include "taglib/tag.h"
#include "tpropertymap.h"

bool AudioInfo::readAudioInfo(const QString& audioPath) {
  TagLib::FileRef f(audioPath.toUtf8().constData());

  if (!f.isNull() && f.tag()) {
    TagLib::Tag* tag = f.tag();

    qDebug() << "-- TAG (basic) --";
    qDebug() << tag->title().toCString(true);
    qDebug() << tag->artist().toCString(true);
    qDebug() << tag->album().toCString(true);
    qDebug() << tag->year();
    qDebug() << tag->comment().toCString(true);
    qDebug() << tag->track();
    qDebug() << tag->genre().toCString(true);

    TagLib::PropertyMap tags = f.file()->properties();

    unsigned int longest = 0;
    for (TagLib::PropertyMap::ConstIterator i = tags.begin(); i != tags.end();
         ++i) {
      if (i->first.size() > longest) {
        longest = i->first.size();
      }
    }
    cout << "-- TAG (properties) --" << endl;
    for (TagLib::PropertyMap::ConstIterator i = tags.begin(); i != tags.end();
         ++i) {
      for (TagLib::StringList::ConstIterator j = i->second.begin();
           j != i->second.end(); ++j) {
        qDebug() << i->first.toCString(true)
                 << i->second.toString().toCString(true);
      }
    }
  }
  return true;
}
