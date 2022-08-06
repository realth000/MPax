#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtGui/QDesktopServices>

namespace util {
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
}  // namespace util
#endif  // FILEUTIL_H
