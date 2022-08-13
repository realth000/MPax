#ifndef MPAX_PARSER_H
#define MPAX_PARSER_H

#include <QtCore/QList>
#include <QtCore/QString>
#include <functional>

#include "analyzer.h"
#include "core/playcontent.h"

namespace SearchParser {
class Parser {
 public:
  explicit Parser();
  bool init(const QString &rawString, bool *ok, QString *errString);
  bool parse(const PlayContent *playContent,
             Qt::CaseSensitivity caseSensitivity) const;

 private:
  AST *m_ast;
};
}  // namespace SearchParser

#endif  // MPAX_PARSER_H
