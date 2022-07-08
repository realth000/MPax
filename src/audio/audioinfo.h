#ifndef MPAX_AUDIOINFO_H
#define MPAX_AUDIOINFO_H

#include <QtCore/QString>

#include "core/playcontent.h"

class AudioInfo {
 public:
  static bool readAudioInfo(const QString &audioPath, PlayContent *playContent);
};

#endif  // MPAX_AUDIOINFO_H
