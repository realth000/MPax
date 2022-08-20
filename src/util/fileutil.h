#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <QtCore/QString>

namespace Util {
void openFileInDir(const QString &filePath);

enum FileSizeLevel : int { B = 0, KB, MB, GB, TB };

qint64 toFileSize(const qint64 &fileSize, FileSizeLevel sizeLevel);

QString toFormatTime(const qint64 &seconds);
}  // namespace Util
#endif  // FILEUTIL_H
