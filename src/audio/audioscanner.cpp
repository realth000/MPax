#include "audioscanner.h"

#include <QtConcurrent/QtConcurrentRun>
#include <QtCore/QDebug>
#include <QtCore/QDirIterator>
#include <QtCore/QFutureWatcher>

#include "util/zawarudor.h"

void AudioScanner::scanDir(const QString &dirPath,
                           const QStringList &audioFormat) {
  m_timer.start(100);
  connect(&m_timer, &QTimer::timeout, this, [this]() {
    emit this->scanStatusChanged(false, m_audioList.length());
  });
  QFutureWatcher<void> *watcher = new QFutureWatcher<void>;
  watcher->setFuture(QtConcurrent::run(this, &AudioScanner::scanDirPrivate,
                                       dirPath, audioFormat));
  connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher]() {
    emit scanStatusChanged(true, m_audioList.length());
    m_timer.stop();
    watcher->deleteLater();
  });
}

QStringList AudioScanner::audioFileList() const { return m_audioList; }

void AudioScanner::scanDirPrivate(const QString &dirPath,
                                  const QStringList &audioFormat) {
  if (!QFileInfo::exists(dirPath) || !QFileInfo(dirPath).isDir()) {
    qDebug() << "invalid directory" << dirPath;
    m_audioList.clear();
  }
  QDirIterator it(
      dirPath,
      QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot,
      QDirIterator::Subdirectories);
  while (it.hasNext()) {
    it.next();
    if (it.fileInfo().isFile()) {
      for (const auto &f : audioFormat) {
        if (f == it.fileInfo().suffix()) {
          m_audioList.append(it.filePath());
        }
      }
    }
  }
}
