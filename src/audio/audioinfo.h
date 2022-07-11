#ifndef MPAX_AUDIOINFO_H
#define MPAX_AUDIOINFO_H

#include <QtCore/QObject>
#include <QtCore/QString>

#include "core/playcontent.h"

class AudioInfo : QObject {
  Q_OBJECT
 public:
  enum class InfoOption : int { All = 0, NoAlbumCover = 1 };
  Q_ENUM(InfoOption)
  static bool readAudioInfo(const QString &audioPath, PlayContent *playContent,
                            InfoOption infoOption = InfoOption::All);
};

#endif  // MPAX_AUDIOINFO_H
