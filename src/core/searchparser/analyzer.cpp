#include "analyzer.h"

#include <QtCore/QDebug>

namespace SearchParser {
Analyzer::Analyzer() {}

AST Analyzer::analyze(const TokenList &tf, bool *ok, QString *errString) {
  if (ok == nullptr || errString == nullptr) {
    qDebug() << "analyze error: null feedback not allowed in this step";
    return AST();
  }
  for (auto &t : tf) {
    qDebug() << t.start << t.end << t.type << t.content;
  }
  qDebug() << "----------";
  return AST();
}
}  // namespace SearchParser