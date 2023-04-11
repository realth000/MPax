#ifndef MPAX_TOKENIZER_H
#define MPAX_TOKENIZER_H

#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtCore/QString>

namespace SearchParser {

enum TokenType : int {
  Word,
  Keyword,
  OpeKeyword,
  MetaKeyword,
  LeftParentheses,
  RightParentheses,
  Unknown = -1
};

class Token {
 public:
  Token() {
    type = TokenType::Unknown;
    content = "";
    start = -1;
    end = -1;
  };
  Token(TokenType t, int pos) {
    type = t;
    start = pos;
    end = pos;
  };
  int length() const {
    return (start < 0 || end < 0 || end < start) ? -1 : end - start + 1;
  };
  TokenType type;
  QString content;
  int start;
  int end;
};

typedef QList<Token> TokenList;

class Tokenizer {
 public:
  explicit Tokenizer();
  static TokenList tokenize(const QString &rawString, bool *ok,
                            QString *errString);

 private:
  QString m_rawString;
};

}  // namespace SearchParser

#endif  // MPAX_TOKENIZER_H
