#include "parser.h"

#include <QtCore/QDebug>

namespace SearchParser {

namespace {

enum Mark {
  Percent = 0,  // %
  Parentheses,  // ()
  Escape        /* \ */
};

struct MarkPart {
  int start;
  int end;
};

typedef MarkPart PercentPart;
typedef MarkPart ParenthesesPart;
typedef MarkPart EscapePart;

}  // namespace

Parser::Parser() {}

bool Parser::init(const QString &rawString, bool *ok, QString *errString) {
  if (ok == nullptr || errString == nullptr) {
    qDebug() << "parse init error: null feedback not allowed in this step";
    return false;
  }

  Tokenizer tokenizer;
  TokenList tokenList = tokenizer.tokenize(rawString, ok, errString);
  if (!*ok) {
    return false;
  }
  Analyzer analyzer;
  AST ast = analyzer.analyze(tokenList, ok, errString);
  if (!*ok) {
    return false;
  }
  m_ast = ast;
  return true;
}

bool Parser::parse(const PlayContent *playContent) const {
  return m_ast.parse(playContent);
}
}  // namespace SearchParser