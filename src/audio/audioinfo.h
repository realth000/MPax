#ifndef MPAX_AUDIOINFO_H
#define MPAX_AUDIOINFO_H

#include <QtCore/QString>

class AudioInfo {
 public:
  static bool readAudioInfo(const QString &audioPath);
};

#endif  // MPAX_AUDIOINFO_H
