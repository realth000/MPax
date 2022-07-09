#include "audioscanner.h"

#include <QtCore/QDebug>
#include <QtCore/QDirIterator>

QStringList AudioScanner::scanAudioInDir(const QString &dirPath,
                                         const QStringList &audioFormat) {
  if (!QFileInfo::exists(dirPath) || !QFileInfo(dirPath).isDir()) {
    qDebug() << "invalid directory" << dirPath;
    return QStringList();
  }
  QStringList allAudio;
  QDirIterator it(
      dirPath, QDir::Files | QDir::Dirs | QDir::Hidden | QDir::NoDotAndDotDot,
      QDirIterator::Subdirectories);
  while (it.hasNext()) {
    it.next();
    if (it.fileInfo().isFile()) {
      for (const auto &f : audioFormat) {
        if (f == it.fileInfo().suffix()) {
          allAudio.append(it.filePath());
        }
      }
    }
    if (it.fileInfo().isDir()) {
      allAudio.append(scanAudioInDir(it.filePath(), audioFormat));
    }
  }
  return allAudio;
}
