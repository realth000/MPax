#ifndef MPAX_CSSLOADER_H
#define MPAX_CSSLOADER_H

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

namespace util {
static QString loadCssFromFile(const QStringList &filePathList) {
  QString cssStr;
  for (const auto &filePath : filePathList) {
    if (!QFileInfo::exists(filePath)) {
      qDebug() << "css file not exists" << filePath;
      continue;
    }
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
      qDebug() << "can not read css file" << filePath;
      continue;
    }
    cssStr += QTextStream(&file).readAll();
    file.close();
  }
  return cssStr;
}

static QString loadCssFromFile(const QString &filePath) {
  return util::loadCssFromFile(QStringList{filePath});
}
}  // namespace util

#endif  // MPAX_CSSLOADER_H
