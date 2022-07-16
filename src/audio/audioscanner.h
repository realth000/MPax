#ifndef AUDIOSCANNER_H
#define AUDIOSCANNER_H

#include <QtCore/QFileInfo>
#include <QtCore/QTimer>

class AudioScanner : public QObject {
  Q_OBJECT

 public:
  QStringList audioFileList() const;

 public slots:
  void scanDir(const QString &dirPath, const QStringList &audioFormat);

 signals:
  void scanStatusChanged(bool finished, qint64 count);

 private:
  QStringList m_audioList;
  QTimer m_timer;

  void scanDirPrivate(const QString &dirPath, const QStringList &audioFormat);
};

#endif  // AUDIOSCANNER_H
