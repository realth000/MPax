#ifndef MPAX_ANALYZER_H
#define MPAX_ANALYZER_H

#include <QtCore/QMap>

#include "ast.h"
#include "tokenizer.h"

namespace SearchParser {

class Analyzer {
 public:
  explicit Analyzer();
  AST *analyze(const TokenList &tf, bool *ok, QString *errString);

 private:
  bool validate(const AST *a, bool *ok, QString *errString);
  bool isValidASTNode(const ASTNode *node, bool *ok, QString *errString) const;
};

}  // namespace SearchParser

#endif  // MPAX_ANALYZER_H
