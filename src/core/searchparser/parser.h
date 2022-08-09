#ifndef MPAX_PARSER_H
#define MPAX_PARSER_H

#include <QtCore/QList>
#include <QtCore/QString>
#include <functional>

namespace SearchParser {

QList<std::function<bool(QString)>> parse(const QString &rawString, bool *ok,
                                          QString *errString);
}  // namespace SearchParser

#endif  // MPAX_PARSER_H
