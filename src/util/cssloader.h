#ifndef MPAX_CSSLOADER_H
#define MPAX_CSSLOADER_H

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QTextStream>

namespace util {
static QString loadCssFromFile(const QString &filePath) {
  if (!QFileInfo::exists(filePath)) {
    qDebug() << "css file not exists" << filePath;
    return QString();
  }
  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "can not read css file" << filePath;
    return QString();
  }
  QString ret = QTextStream(&file).readAll();
  file.close();
  return ret;
}
}  // namespace util

#endif  // MPAX_CSSLOADER_H
