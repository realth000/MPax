#ifndef AUDIOSCANNER_H
#define AUDIOSCANNER_H

#include <QtCore/QFileInfo>

class AudioScanner
{
public:
    static QStringList scanAudioInDir(const QString &dirPath, const QStringList &audioFormat);
};

#endif // AUDIOSCANNER_H
