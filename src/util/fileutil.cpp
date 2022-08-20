#include "fileutil.h"

#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices>

namespace Util {
void openFileInDir(const QString &filePath) {
#ifdef Q_OS_LINUX
  // Detect DE
  QProcess p;
  p.start("xdg-mime", {"query", "default", "inode/directory"});
  p.waitForFinished();
  const QString fileManagerType = p.readLine().simplified();
  // KDE
  if (fileManagerType == "org.kde.dolphin.desktop" &&
      QProcess::startDetached("dolphin", {"--select", filePath})) {
    return;
  }
  // GNOME
  if (fileManagerType == "org.gnome.Nautilus.desktop" &&
      QProcess::startDetached("nautilus", {filePath})) {
    return;
  }
  // UKUI
  if (fileManagerType == "peony.desktop" &&
      QProcess::startDetached("peony", {filePath})) {
    return;
  }
  // DDE
  if (fileManagerType == "dde-file-manager.desktop" &&
      QProcess::startDetached("dde-file-manager", {"--show-item", filePath})) {
    return;
  }
#endif
  // Default, can not select file.
  QDesktopServices::openUrl(
      QUrl("file://" + QFileInfo(filePath).absoluteDir().absolutePath()));
}

qint64 toFileSize(const qint64 &fileSize, FileSizeLevel sizeLevel) {
  qint64 size = fileSize;
  FileSizeLevel level = sizeLevel;
  while (size >= 1024 && level < FileSizeLevel::TB) {
    size /= 1024;
    switch (level) {
      case FileSizeLevel::B:
        level = FileSizeLevel::KB;
        break;
      case FileSizeLevel::KB:
        level = FileSizeLevel::MB;
        break;
      case FileSizeLevel::MB:
        level = FileSizeLevel::GB;
        break;
      case FileSizeLevel::GB:
        level = FileSizeLevel::TB;
        break;
    }
  }
  return size;
}
}  // namespace Util
